#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include "include/page.h"
#include "include/secrets.h"

const uint16_t HTTP_PORT = 8080;
WebServer server(HTTP_PORT);
WebSocketsServer webSocket(81);

// WebSocket 이벤트 종류
enum StreamMessageType : uint8_t {
  MSG_START,
  MSG_STATUS,
  MSG_CHUNK,
  MSG_DONE,
  MSG_ERROR
};

// 큐를 통해 loop()에 전달할 메시지
struct StreamMessage {
  StreamMessageType type;
  char text[768];
};

QueueHandle_t streamQueue = nullptr;

// 요청은 한 번에 하나만 처리
String jobQuestion;
String jobImageUrl;
bool jobBusy = false;
bool firstContentLogged = false;


// 스트리밍 메시지를 큐에 넣습니다.
// 큐가 잠시 가득 차면 API task가 기다리고 loop()가 큐를 비웁니다.
bool enqueueMessage(StreamMessageType type, const String& text) {
  if (streamQueue == nullptr) {
    return false;
  }

  StreamMessage message;
  message.type = type;

  size_t length = text.length();

  if (length >= sizeof(message.text)) {
    length = sizeof(message.text) - 1;
  }

  if (length > 0) {
    memcpy(message.text, text.c_str(), length);
  }
  message.text[length] = '\0';

  if (xQueueSend(streamQueue, &message, pdMS_TO_TICKS(10000)) != pdTRUE) {
    Serial.printf("스트리밍 큐 전송 실패 (type=%u)\n", type);
    return false;
  }

  return true;
}


// UTF-8 문자를 중간에서 자르지 않고 텍스트 조각을 큐에 넣습니다.
bool enqueueTextChunks(const String& text) {
  size_t start = 0;
  const size_t maxChunkBytes = sizeof(StreamMessage::text) - 1;

  while (start < text.length()) {
    size_t end = start + maxChunkBytes;

    if (end > text.length()) {
      end = text.length();
    } else {
      // end 위치가 UTF-8 연속 바이트라면 문자의 시작까지 되돌립니다.
      while (end > start &&
             (static_cast<uint8_t>(text[end]) & 0xC0) == 0x80) {
        end--;
      }

      // 매우 긴 단일 문자 조각에 대한 안전장치
      if (end == start) {
        end = start + maxChunkBytes;
        if (end > text.length()) end = text.length();
      }
    }

    String part = text.substring(start, end);

    if (!enqueueMessage(MSG_CHUNK, part)) {
      return false;
    }

    start = end;
  }

  return true;
}


// SSE의 data: 한 줄을 처리합니다.
// 반환값: 0=계속, 1=[DONE], -1=오류
int processSseLine(String line) {
  line.trim();

  if (line.length() == 0) {
    return 0;
  }

  // SSE 주석/keepalive 행
  if (line.startsWith(":")) {
    return 0;
  }

  if (!line.startsWith("data:")) {
    return 0;
  }

  line.remove(0, 5);

  if (line.startsWith(" ")) {
    line.remove(0, 1);
  }

  line.trim();

  if (line == "[DONE]") {
    return 1;
  }

  DynamicJsonDocument eventDoc(4096);
  DeserializationError error = deserializeJson(eventDoc, line);

  if (error) {
    Serial.println("SSE JSON 파싱 실패:");
    Serial.println(line);
    return 0;
  }

  // 스트리밍 API 오류 응답 확인
  JsonVariant apiError = eventDoc["error"];
  if (!apiError.isNull()) {
    Serial.println("NVIDIA API가 스트림 오류 이벤트를 반환했습니다.");
    String errorText;
    serializeJson(apiError, errorText);
    enqueueMessage(MSG_ERROR, "NVIDIA API 오류: " + errorText);
    return -1;
  }

  // 응답 토큰만 보냅니다. reasoning_content는 화면에 표시하지 않습니다.
  JsonVariant content =
      eventDoc["choices"][0]["delta"]["content"];

  if (!content.isNull()) {
    String piece = content.as<String>();

    if (piece.length() > 0) {
      if (!firstContentLogged) {
        Serial.printf("NVIDIA 첫 응답 조각 수신 (%u bytes)\n", piece.length());
        firstContentLogged = true;
      }

      if (!enqueueTextChunks(piece)) {
        return -1;
      }
    }
  }

  return 0;
}


// NVIDIA API 스트리밍 요청 task
void apiTask(void* parameter) {
  Serial.println("NVIDIA API 작업 시작");

  String question = jobQuestion;
  String imageUrl = jobImageUrl;

  DynamicJsonDocument requestDoc(8192);

  requestDoc["model"] = NVIDIA_MODEL;
  requestDoc["temperature"] = 1;
  requestDoc["top_p"] = 0.95;
  requestDoc["max_tokens"] = 1024;
  requestDoc["stream"] = true;
  requestDoc["chat_template_kwargs"]["enable_thinking"] = false;

  JsonArray messages = requestDoc.createNestedArray("messages");
  JsonObject systemMessage = messages.createNestedObject();
  systemMessage["role"] = "system";
  systemMessage["content"] =
      "항상 한국어로만 답변하세요. 이미지 분석 결과와 설명도 한국어로 작성하세요. 영어 등 다른 언어로 답하지 마세요.";

  JsonObject message = messages.createNestedObject();
  message["role"] = "user";

  if (imageUrl.length() > 0) {
    JsonArray content = message.createNestedArray("content");

    JsonObject textPart = content.createNestedObject();
    textPart["type"] = "text";
    textPart["text"] = question;

    JsonObject imagePart = content.createNestedObject();
    imagePart["type"] = "image_url";
    imagePart["image_url"]["url"] = imageUrl;
  } else {
    message["content"] = question;
  }

  String payload;
  serializeJson(requestDoc, payload);

  WiFiClientSecure client;

  // 테스트용입니다. 실제 서비스에서는 CA 인증서 검증을 설정하세요.
  client.setInsecure();

  HTTPClient http;
  http.setConnectTimeout(20000);
  http.setTimeout(45000);

  // HTTP/1.0 요청으로 chunked transfer 응답 문제를 줄입니다.
  http.useHTTP10(true);

  if (!http.begin(client, NVIDIA_API_URL)) {
    Serial.println("NVIDIA HTTPS 연결 시작 실패");
    enqueueMessage(MSG_ERROR, "NVIDIA HTTPS 연결을 시작하지 못했습니다.");
    vTaskDelete(nullptr);
    return;
  }

  http.addHeader("Authorization", String("Bearer ") + NVIDIA_API_KEY);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "text/event-stream");

  Serial.printf(
    "NVIDIA API POST 전송 중 (Wi-Fi RSSI: %d dBm, heap: %u bytes)\n",
    WiFi.RSSI(),
    ESP.getFreeHeap()
  );
  unsigned long requestStartedAt = millis();
  int statusCode = http.POST(payload);

  Serial.printf(
    "NVIDIA HTTP 상태 코드: %d (응답 대기: %lu ms)\n",
    statusCode,
    millis() - requestStartedAt
  );

  if (statusCode <= 0) {
    String errorText = "NVIDIA 연결 오류: ";
    errorText += HTTPClient::errorToString(statusCode);
    http.end();
    enqueueMessage(MSG_ERROR, errorText);
    vTaskDelete(nullptr);
    return;
  }

  if (statusCode < 200 || statusCode >= 300) {
    String errorBody = http.getString();
    http.end();

    if (errorBody.length() > 1500) {
      errorBody.remove(1500);
      errorBody += "...";
    }

    String errorText = "API 요청 실패. HTTP 상태 코드: ";
    errorText += String(statusCode);
    errorText += "\n";
    errorText += errorBody;

    enqueueMessage(MSG_ERROR, errorText);
    vTaskDelete(nullptr);
    return;
  }

  WiFiClient* stream = http.getStreamPtr();

  if (stream == nullptr) {
    http.end();
    enqueueMessage(MSG_ERROR, "API 응답 스트림을 열지 못했습니다.");
    vTaskDelete(nullptr);
    return;
  }

  enqueueMessage(MSG_START, "");

  firstContentLogged = false;
  String line;
  line.reserve(512);

  bool receivedDone = false;
  bool streamError = false;
  unsigned long lastByteTime = millis();
  const unsigned long idleTimeoutMs = 180000;

  while (http.connected() || stream->available()) {
    while (stream->available()) {
      int value = stream->read();

      if (value < 0) {
        break;
      }

      lastByteTime = millis();

      if (value == '\n') {
        int result = processSseLine(line);
        line = "";

        if (result == 1) {
          receivedDone = true;
          break;
        }

        if (result < 0) {
          streamError = true;
          break;
        }
      } else if (value != '\r') {
        // 비정상적으로 긴 SSE 행으로 인한 메모리 증가를 제한
        if (line.length() < 8192) {
          line += static_cast<char>(value);
        } else {
          streamError = true;
          break;
        }
      }
    }

    if (receivedDone || streamError) {
      break;
    }

    if (millis() - lastByteTime > idleTimeoutMs) {
      streamError = true;
      break;
    }

    delay(2);
  }

  // 연결 종료 직전에 마지막 행에 개행이 없을 수 있습니다.
  if (!receivedDone && !streamError && line.length() > 0) {
    int result = processSseLine(line);
    receivedDone = (result == 1);
    streamError = (result < 0);
  }

  http.end();

  if (streamError) {
    Serial.println("NVIDIA 스트림 처리 오류");
    enqueueMessage(
      MSG_ERROR,
      "스트리밍 응답 처리 중 오류가 발생했습니다. 시리얼 모니터를 확인하세요."
    );
  } else if (receivedDone) {
    Serial.println("NVIDIA 스트림 완료 ([DONE])");
    enqueueMessage(MSG_DONE, "");
  } else {
    Serial.println("NVIDIA 스트림이 [DONE] 전에 종료됨");
    enqueueMessage(
      MSG_ERROR,
      "API 스트림이 [DONE] 이벤트 없이 종료되었습니다."
    );
  }

  vTaskDelete(nullptr);
}


// 브라우저 WebSocket 메시지 수신
void onWebSocketEvent(
  uint8_t clientNumber,
  WStype_t type,
  uint8_t* payload,
  size_t length
) {
  if (type != WStype_TEXT) {
    return;
  }

  String incoming;
  incoming.reserve(length + 1);

  for (size_t i = 0; i < length; i++) {
    incoming += static_cast<char>(payload[i]);
  }

  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, incoming);

  if (error) {
    enqueueMessage(MSG_ERROR, "브라우저 요청 JSON을 읽지 못했습니다.");
    return;
  }

  String action = doc["action"] | "";

  if (action != "ask") {
    return;
  }

  Serial.printf(
    "WebSocket 질문 수신 (image=%s)\n",
    doc["image"].as<String>().length() > 0 ? "yes" : "no"
  );

  if (jobBusy) {
    enqueueMessage(MSG_ERROR, "이미 질문을 처리 중입니다. 잠시 기다려 주세요.");
    return;
  }

  jobQuestion = doc["q"] | "";
  jobImageUrl = doc["image"] | "";

  jobQuestion.trim();
  jobImageUrl.trim();

  if (jobQuestion.length() == 0) {
    enqueueMessage(MSG_ERROR, "질문을 입력해 주세요.");
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    enqueueMessage(MSG_ERROR, "ESP32의 Wi-Fi 연결을 확인해 주세요.");
    return;
  }

  if (NVIDIA_API_KEY[0] == '\0' ||
      String(NVIDIA_API_KEY) == "YOUR_NEW_NVIDIA_API_KEY") {
    enqueueMessage(MSG_ERROR, "secrets.h에 NVIDIA API 키를 설정해 주세요.");
    return;
  }

  jobBusy = true;
  firstContentLogged = false;
  enqueueMessage(MSG_STATUS, "NVIDIA API에 요청 중입니다...");

  BaseType_t taskCreated = xTaskCreatePinnedToCore(
    apiTask,
    "NvidiaStreamTask",
    16384,
    nullptr,
    1,
    nullptr,
    1
  );

  if (taskCreated != pdPASS) {
    Serial.println("NVIDIA API 작업 생성 실패");
    jobBusy = false;
    enqueueMessage(MSG_ERROR, "API 작업을 시작하지 못했습니다. 메모리를 확인하세요.");
  }
}


// 웹페이지 제공
void handleRoot() {
  server.send_P(
    200,
    "text/html; charset=utf-8",
    PAGE
  );
}


void setup() {
  Serial.begin(115200);
  delay(500);

  // 스트리밍 메시지 큐: 꽉 차면 API task가 잠시 기다립니다.
  streamQueue = xQueueCreate(12, sizeof(StreamMessage));

  if (streamQueue == nullptr) {
    Serial.println("스트리밍 큐 생성 실패");
  }

  if (WIFI_SSID[0] == '\0' || WIFI_PASSWORD[0] == '\0') {
    Serial.println("secrets.h에 Wi-Fi 이름과 비밀번호를 설정해 주세요.");
    while (true) {
      delay(1000);
    }
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Wi-Fi 연결 중");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("웹페이지 IP 주소: http://");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32-ai")) {
    MDNS.addService("http", "tcp", HTTP_PORT);
    Serial.printf(
      "웹페이지 주소: http://esp32-ai.local:%u\n",
      static_cast<unsigned int>(HTTP_PORT)
    );
  } else {
    Serial.println("mDNS 시작 실패");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  Serial.printf(
    "웹서버 시작: HTTP %u, WebSocket 81\n",
    static_cast<unsigned int>(HTTP_PORT)
  );
}


void loop() {
  server.handleClient();
  webSocket.loop();

  // 큰 구조체를 loop 스택에 매번 만들지 않도록 static 사용
  static StreamMessage message;

  if (streamQueue != nullptr &&
      xQueueReceive(streamQueue, &message, 0) == pdTRUE) {

    DynamicJsonDocument outgoingDoc(1024);

    switch (message.type) {
      case MSG_START:
        outgoingDoc["type"] = "start";
        break;

      case MSG_STATUS:
        outgoingDoc["type"] = "status";
        outgoingDoc["text"] = message.text;
        break;

      case MSG_CHUNK:
        outgoingDoc["type"] = "chunk";
        outgoingDoc["text"] = message.text;
        break;

      case MSG_DONE:
        outgoingDoc["type"] = "done";
        jobBusy = false;
        break;

      case MSG_ERROR:
        outgoingDoc["type"] = "error";
        outgoingDoc["text"] = message.text;
        jobBusy = false;
        break;
    }

    String outgoingText;
    serializeJson(outgoingDoc, outgoingText);
    webSocket.broadcastTXT(outgoingText);
  }

  static unsigned long lastTelemetryAt = 0;
  unsigned long now = millis();

  if (now - lastTelemetryAt >= 5000) {
    DynamicJsonDocument telemetryDoc(512);
    bool wifiConnected = WiFi.status() == WL_CONNECTED;

    telemetryDoc["type"] = "telemetry";
    telemetryDoc["uptimeMs"] = now;
    telemetryDoc["wifiConnected"] = wifiConnected;
    telemetryDoc["wifiRssi"] = wifiConnected ? WiFi.RSSI() : 0;
    telemetryDoc["freeHeap"] = ESP.getFreeHeap();
    telemetryDoc["minFreeHeap"] = ESP.getMinFreeHeap();
    telemetryDoc["heapSize"] = ESP.getHeapSize();
    telemetryDoc["cpuMHz"] = ESP.getCpuFreqMHz();
    telemetryDoc["chipModel"] = ESP.getChipModel();
    telemetryDoc["chipCores"] = ESP.getChipCores();
    telemetryDoc["ip"] = WiFi.localIP().toString();
    telemetryDoc["httpPort"] = HTTP_PORT;
    telemetryDoc["apiBusy"] = jobBusy;

    String telemetryText;
    serializeJson(telemetryDoc, telemetryText);
    webSocket.broadcastTXT(telemetryText);
    lastTelemetryAt = now;
  }

  delay(2);
}