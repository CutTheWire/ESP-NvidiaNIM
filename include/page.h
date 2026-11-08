#pragma once
#include <Arduino.h>

const char PAGE[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="ko">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="color-scheme" content="dark">
<title>ESP32 AI Studio</title>

<style>
:root {
  --bg: #0b0e0c;
  --panel: #121713;
  --panel-2: #171e19;
  --line: #29332c;
  --text: #edf4ee;
  --muted: #9aa99d;
  --green: #76b900;
  --green-bright: #8bd600;
  --green-soft: rgba(118,185,0,.13);
  --red: #ff6b6b;
  --radius: 16px;
}

* { box-sizing: border-box; }

body {
  margin: 0;
  min-height: 100vh;
  color: var(--text);
  background:
    radial-gradient(ellipse at 50% -20%, rgba(118,185,0,.12), transparent 55%),
    var(--bg);
  font-family: Inter, "Segoe UI", Arial, sans-serif;
}

button, textarea, input { font: inherit; }

.shell {
  width: min(920px, 100%);
  margin: 0 auto;
  padding: 28px 20px 48px;
}

.topbar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 16px;
  padding-bottom: 28px;
  border-bottom: 1px solid var(--line);
}

.brand {
  display: flex;
  align-items: center;
  gap: 12px;
}

.brand-mark {
  display: grid;
  place-items: center;
  width: 42px;
  height: 42px;
  border-radius: 12px;
  background: var(--green);
  color: #10150b;
  font-weight: 900;
  font-size: 19px;
  box-shadow: 0 0 28px rgba(118,185,0,.22);
}

.brand-name {
  margin: 0;
  font-size: 16px;
  font-weight: 750;
  letter-spacing: .2px;
}

.brand-subtitle {
  margin-top: 4px;
  color: var(--muted);
  font-size: 12px;
}

.connection {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 8px 11px;
  border: 1px solid var(--line);
  border-radius: 999px;
  color: var(--muted);
  font-size: 12px;
  white-space: nowrap;
}

.connection-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: #707a72;
}

.connection.online {
  color: #c8f28a;
  border-color: rgba(118,185,0,.4);
  background: var(--green-soft);
}

.connection.online .connection-dot {
  background: var(--green-bright);
  box-shadow: 0 0 10px rgba(139,214,0,.7);
}

.hero {
  padding: 42px 0 25px;
}

.eyebrow {
  margin: 0 0 12px;
  color: var(--green-bright);
  font-size: 12px;
  font-weight: 750;
  letter-spacing: 1.8px;
  text-transform: uppercase;
}

h1 {
  margin: 0;
  font-size: clamp(28px, 5vw, 42px);
  line-height: 1.15;
  letter-spacing: -1.2px;
}

.hero-copy {
  max-width: 650px;
  margin: 14px 0 0;
  color: var(--muted);
  font-size: 15px;
  line-height: 1.7;
}

.card {
  border: 1px solid var(--line);
  border-radius: var(--radius);
  background: rgba(18,23,19,.94);
  box-shadow: 0 20px 60px rgba(0,0,0,.22);
}

.input-card {
  padding: 22px;
}

.field-label {
  display: block;
  margin: 0 0 9px;
  color: #dce6dd;
  font-size: 13px;
  font-weight: 700;
}

textarea, input[type="url"] {
  width: 100%;
  color: var(--text);
  border: 1px solid #303a33;
  border-radius: 11px;
  background: #0d110e;
  outline: none;
  transition: border-color .16s, box-shadow .16s;
}

textarea {
  min-height: 142px;
  padding: 14px 15px;
  resize: vertical;
  line-height: 1.55;
}

input[type="url"] {
  height: 46px;
  padding: 0 14px;
}

textarea::placeholder, input::placeholder {
  color: #657168;
}

textarea:focus, input[type="url"]:focus {
  border-color: var(--green);
  box-shadow: 0 0 0 3px rgba(118,185,0,.12);
}

.field-row {
  margin-top: 19px;
}

.help {
  margin: 8px 0 0;
  color: var(--muted);
  font-size: 12px;
  line-height: 1.55;
}

.preview {
  display: none;
  position: relative;
  margin-top: 13px;
  min-height: 100px;
  overflow: hidden;
  border: 1px solid var(--line);
  border-radius: 12px;
  background:
    linear-gradient(45deg, #151b16 25%, transparent 25%),
    linear-gradient(-45deg, #151b16 25%, transparent 25%),
    linear-gradient(45deg, transparent 75%, #151b16 75%),
    linear-gradient(-45deg, transparent 75%, #151b16 75%),
    #0d110e;
  background-size: 18px 18px;
  background-position: 0 0, 0 9px, 9px -9px, -9px 0;
}

.preview.visible {
  display: flex;
  align-items: center;
  justify-content: center;
}

.preview img {
  display: block;
  max-width: 100%;
  max-height: 340px;
  object-fit: contain;
}

.preview-message {
  padding: 18px;
  color: var(--muted);
  font-size: 13px;
  text-align: center;
}

.preview-error {
  color: #ff9a9a;
}

.actions {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  margin-top: 20px;
}

.privacy {
  color: var(--muted);
  font-size: 12px;
  line-height: 1.5;
}

.send-button {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  gap: 9px;
  min-width: 154px;
  height: 46px;
  padding: 0 18px;
  border: 0;
  border-radius: 10px;
  background: var(--green);
  color: #10150b;
  font-weight: 800;
  cursor: pointer;
  transition: transform .15s, background .15s, opacity .15s;
}

.send-button:hover:not(:disabled) {
  transform: translateY(-1px);
  background: var(--green-bright);
}

.send-button:disabled {
  opacity: .58;
  cursor: not-allowed;
}

.send-button svg {
  width: 17px;
  height: 17px;
}

.response-card {
  margin-top: 18px;
  overflow: hidden;
}

.response-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  padding: 17px 20px;
  border-bottom: 1px solid var(--line);
}

.response-title {
  display: flex;
  align-items: center;
  gap: 10px;
  margin: 0;
  font-size: 14px;
  font-weight: 750;
}

.response-icon {
  color: var(--green-bright);
  font-size: 17px;
}

.response-state {
  color: var(--muted);
  font-size: 12px;
}

.response-body {
  min-height: 112px;
  padding: 19px 20px 22px;
  color: #dce6dd;
  line-height: 1.7;
}

.markdown {
  overflow-wrap: anywhere;
}

.markdown > :first-child { margin-top: 0; }
.markdown > :last-child { margin-bottom: 0; }

.markdown h1, .markdown h2, .markdown h3 {
  margin: 1.3em 0 .55em;
  font-size: 1.15em;
  letter-spacing: 0;
}

.markdown p { margin: .7em 0; }

.markdown a {
  color: #a7df52;
}

.markdown ul, .markdown ol {
  padding-left: 1.5em;
}

.markdown blockquote {
  margin: 1em 0;
  padding: 2px 0 2px 14px;
  border-left: 3px solid var(--green);
  color: #b7c5b9;
}

.markdown code {
  padding: 2px 5px;
  border: 1px solid #303a33;
  border-radius: 5px;
  background: #0b100c;
  font-family: Consolas, monospace;
  font-size: .92em;
}

.markdown pre {
  overflow-x: auto;
  padding: 14px;
  border: 1px solid var(--line);
  border-radius: 10px;
  background: #0a0d0b;
}

.markdown pre code {
  padding: 0;
  border: 0;
  background: transparent;
}

.markdown table {
  display: block;
  max-width: 100%;
  overflow-x: auto;
  border-collapse: collapse;
}

.markdown th, .markdown td {
  padding: 7px 10px;
  border: 1px solid var(--line);
}

.markdown th {
  background: #1b241d;
}

.typing-cursor::after {
  display: inline-block;
  width: 7px;
  height: 1em;
  margin-left: 3px;
  vertical-align: -2px;
  background: var(--green-bright);
  content: "";
  animation: blink 1s steps(2, start) infinite;
}

@keyframes blink {
  to { visibility: hidden; }
}

.footer-note {
  margin: 15px 2px 0;
  color: #748077;
  font-size: 11px;
  line-height: 1.6;
}

.server-card {
  margin-bottom: 18px;
  padding: 20px;
}

.server-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  margin-bottom: 15px;
}

.server-title {
  margin: 0;
  font-size: 14px;
  font-weight: 750;
}

.server-live {
  display: inline-flex;
  align-items: center;
  gap: 7px;
  color: var(--muted);
  font-size: 11px;
  white-space: nowrap;
}

.server-live-dot {
  width: 7px;
  height: 7px;
  border-radius: 50%;
  background: #707a72;
}

.server-live.online {
  color: #c8f28a;
}

.server-live.online .server-live-dot {
  background: var(--green-bright);
  box-shadow: 0 0 9px rgba(139,214,0,.7);
}

.server-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 10px;
}

.server-metric {
  min-width: 0;
  padding: 13px;
  border: 1px solid var(--line);
  border-radius: 11px;
  background: #0e130f;
}

.server-metric-label {
  margin: 0;
  color: var(--muted);
  font-size: 11px;
}

.server-metric-value {
  margin: 8px 0 0;
  color: var(--text);
  font-size: 17px;
  font-weight: 750;
  overflow-wrap: anywhere;
}

.server-metric-detail {
  min-height: 15px;
  margin: 5px 0 0;
  color: #748077;
  font-size: 10px;
  overflow-wrap: anywhere;
}

@media (max-width: 600px) {
  .shell { padding: 18px 14px 32px; }
  .topbar { padding-bottom: 20px; }
  .hero { padding: 32px 0 20px; }
  .input-card { padding: 16px; }
  .server-card { padding: 16px; }
  .server-grid { grid-template-columns: repeat(2, minmax(0, 1fr)); }
  .actions {
    align-items: stretch;
    flex-direction: column;
  }
  .send-button { width: 100%; }
  .privacy { order: 2; }
  .response-header { padding: 15px 16px; }
  .response-body { padding: 16px; }
}
</style>
</head>

<body>
<div class="shell">
  <header class="topbar">
    <div class="brand">
      <div class="brand-mark" aria-hidden="true">N</div>
      <div>
        <p class="brand-name">ESP32 AI Studio</p>
        <div class="brand-subtitle">NVIDIA NIM API · Local interface</div>
      </div>
    </div>

    <div id="connection" class="connection" role="status">
      <span class="connection-dot"></span>
      <span id="connectionText">연결 중...</span>
    </div>
  </header>

  <main>
    <section class="hero">
      <p class="eyebrow">AI Inference</p>
      <h1>무엇을 도와드릴까요?</h1>
      <p class="hero-copy">
        질문을 보내면 NVIDIA API의 답변이 실시간으로 표시됩니다.
        이미지 URL을 입력하면 미리보기를 확인할 수 있습니다.
      </p>
    </section>

    <section class="card server-card" aria-labelledby="serverTitle">
      <div class="server-header">
        <h2 id="serverTitle" class="server-title">ESP32 서버 상태</h2>
        <div id="serverStatus" class="server-live" role="status">
          <span class="server-live-dot"></span>
          <span id="serverStatusText">정보 수신 대기 중</span>
        </div>
      </div>
      <div class="server-grid" aria-live="polite">
        <div class="server-metric">
          <p class="server-metric-label">디바이스</p>
          <p id="serverDevice" class="server-metric-value">ESP32</p>
          <p id="serverCores" class="server-metric-detail">칩 정보 수신 대기 중</p>
        </div>
        <div class="server-metric">
          <p class="server-metric-label">서버 가동 시간</p>
          <p id="serverUptime" class="server-metric-value">--</p>
          <p class="server-metric-detail">마지막 부팅 후 경과</p>
        </div>
        <div class="server-metric">
          <p class="server-metric-label">Wi-Fi 신호</p>
          <p id="wifiSignal" class="server-metric-value">--</p>
          <p id="wifiSignalDetail" class="server-metric-detail">신호 정보 수신 대기 중</p>
        </div>
        <div class="server-metric">
          <p class="server-metric-label">여유 힙 메모리</p>
          <p id="freeHeap" class="server-metric-value">--</p>
          <p id="freeHeapDetail" class="server-metric-detail">메모리 정보 수신 대기 중</p>
        </div>
        <div class="server-metric">
          <p class="server-metric-label">CPU 주파수</p>
          <p id="cpuFrequency" class="server-metric-value">--</p>
          <p class="server-metric-detail">현재 설정 주파수</p>
        </div>
        <div class="server-metric">
          <p class="server-metric-label">서버 접속 주소</p>
          <p id="serverAddress" class="server-metric-value">--</p>
          <p id="apiActivity" class="server-metric-detail">API 대기 중</p>
        </div>
      </div>
    </section>

    <section class="card input-card">
      <form id="questionForm">
        <label class="field-label" for="q">질문</label>
        <textarea id="q" maxlength="3000" required
          placeholder="질문을 입력하세요. Ctrl + Enter로 전송할 수 있습니다."></textarea>

        <div class="field-row">
          <label class="field-label" for="image">이미지 URL <span style="color:#8b978d;font-weight:400">(선택)</span></label>
          <input id="image" type="url" placeholder="https://example.com/image.jpg"
            autocomplete="url">
          <p class="help">
            입력한 URL의 미리보기를 표시합니다. AI가 이미지를 읽으려면 URL이 인터넷에서 공개적으로 접근 가능해야 합니다.
          </p>

          <div id="preview" class="preview" aria-live="polite">
            <div id="previewMessage" class="preview-message"></div>
            <img id="previewImage" alt="입력한 이미지 URL 미리보기" hidden>
          </div>
        </div>

        <div class="actions">
          <div class="privacy">질문과 답변은 ESP32에 저장되지 않습니다.</div>

          <button id="sendButton" class="send-button" type="submit" disabled>
            <svg viewBox="0 0 24 24" fill="none" aria-hidden="true">
              <path d="M4 12L20 4L14 20L11 13L4 12Z"
                stroke="currentColor" stroke-width="1.8"
                stroke-linejoin="round"/>
              <path d="M11 13L20 4"
                stroke="currentColor" stroke-width="1.8"/>
            </svg>
            <span>질문 보내기</span>
          </button>
        </div>
      </form>
    </section>

    <section class="card response-card">
      <div class="response-header">
        <h2 class="response-title">
          <span class="response-icon" aria-hidden="true">✦</span>
          응답
        </h2>
        <span id="responseState" class="response-state">대기 중</span>
      </div>
      <div id="answer" class="response-body markdown" aria-live="polite">
        질문을 입력하면 답변이 여기에 표시됩니다.
      </div>
    </section>

    <p class="footer-note">
      로컬 네트워크에서 동작하는 사용자 제작 인터페이스입니다. NVIDIA의 공식 제품 페이지가 아닙니다.
    </p>
  </main>
</div>

<script src="https://cdn.jsdelivr.net/npm/marked@15.0.7/marked.min.js"></script>
<script src="https://cdn.jsdelivr.net/npm/dompurify@3.2.6/dist/purify.min.js"></script>

<script>
const connectionBadge = document.getElementById("connection");
const connectionText = document.getElementById("connectionText");
const serverStatus = document.getElementById("serverStatus");
const serverStatusText = document.getElementById("serverStatusText");
const responseState = document.getElementById("responseState");
const answerElement = document.getElementById("answer");
const sendButton = document.getElementById("sendButton");
const form = document.getElementById("questionForm");
const questionInput = document.getElementById("q");
const imageInput = document.getElementById("image");
const preview = document.getElementById("preview");
const previewImage = document.getElementById("previewImage");
const previewMessage = document.getElementById("previewMessage");

let socket = null;
let reconnectTimer = null;
let waitingForAnswer = false;
let streamedText = "";

function renderAnswer(text, isStreaming) {
  if (window.marked && window.DOMPurify) {
    const html = marked.parse(text || "");
    answerElement.innerHTML = DOMPurify.sanitize(html);
  } else {
    answerElement.textContent = text || "";
  }

  answerElement.classList.toggle("typing-cursor", Boolean(isStreaming));
}

function setConnection(isOnline, message) {
  connectionText.textContent = message;
  connectionBadge.classList.toggle("online", isOnline);
  sendButton.disabled = !isOnline || waitingForAnswer;
}

function formatUptime(milliseconds) {
  const totalSeconds = Math.floor(milliseconds / 1000);
  const days = Math.floor(totalSeconds / 86400);
  const hours = Math.floor((totalSeconds % 86400) / 3600);
  const minutes = Math.floor((totalSeconds % 3600) / 60);
  const seconds = totalSeconds % 60;

  if (days > 0) return days + "일 " + hours + "시간";
  if (hours > 0) return hours + "시간 " + minutes + "분";
  if (minutes > 0) return minutes + "분 " + seconds + "초";
  return seconds + "초";
}

function formatMemory(bytes) {
  return Math.round(bytes / 1024) + " KB";
}

function updateServerStatus(message) {
  document.getElementById("serverUptime").textContent =
    formatUptime(message.uptimeMs);
  document.getElementById("cpuFrequency").textContent =
    message.cpuMHz + " MHz";
  document.getElementById("serverDevice").textContent =
    message.chipModel || "ESP32";
  document.getElementById("serverCores").textContent =
    message.chipCores + "코어 · ESP32";
  document.getElementById("freeHeap").textContent =
    formatMemory(message.freeHeap);
  document.getElementById("freeHeapDetail").textContent =
    "전체 " + formatMemory(message.heapSize) +
    " · 부팅 후 최저 " + formatMemory(message.minFreeHeap);
  document.getElementById("serverAddress").textContent =
    message.ip + ":" + message.httpPort;
  document.getElementById("apiActivity").textContent =
    message.apiBusy ? "NVIDIA API 요청 처리 중" : "NVIDIA API 대기 중";

  if (message.wifiConnected) {
    const rssi = message.wifiRssi;
    let quality = "약함";
    if (rssi >= -55) quality = "매우 좋음";
    else if (rssi >= -67) quality = "좋음";
    else if (rssi >= -75) quality = "보통";

    document.getElementById("wifiSignal").textContent = rssi + " dBm";
    document.getElementById("wifiSignalDetail").textContent =
      "Wi-Fi 연결됨 · " + quality;
  } else {
    document.getElementById("wifiSignal").textContent = "연결 끊김";
    document.getElementById("wifiSignalDetail").textContent =
      "Wi-Fi 연결을 확인해 주세요";
  }

  serverStatusText.textContent = "5초마다 실시간 갱신";
  serverStatus.classList.add("online");
}

function updatePreview() {
  const url = imageInput.value.trim();

  preview.classList.remove("visible");
  previewImage.hidden = true;
  previewImage.removeAttribute("src");
  previewMessage.textContent = "";
  previewMessage.classList.remove("preview-error");

  if (!url) return;

  let parsedUrl;

  try {
    parsedUrl = new URL(url);
  } catch (e) {
    preview.classList.add("visible");
    previewMessage.textContent = "이미지 URL 형식을 확인해 주세요.";
    previewMessage.classList.add("preview-error");
    return;
  }

  if (parsedUrl.protocol !== "http:" && parsedUrl.protocol !== "https:") {
    preview.classList.add("visible");
    previewMessage.textContent = "http:// 또는 https:// URL을 입력해 주세요.";
    previewMessage.classList.add("preview-error");
    return;
  }

  preview.classList.add("visible");
  previewMessage.textContent = "이미지 불러오는 중...";

  previewImage.onload = () => {
    previewMessage.textContent = "";
    previewImage.hidden = false;
  };

  previewImage.onerror = () => {
    previewImage.hidden = true;
    previewMessage.textContent =
      "미리보기를 불러오지 못했습니다. URL이 이미지 주소인지 확인해 주세요.";
    previewMessage.classList.add("preview-error");
  };

  previewImage.src = url;
}

imageInput.addEventListener("input", updatePreview);
imageInput.addEventListener("change", updatePreview);

function connectWebSocket() {
  if (socket && (
    socket.readyState === WebSocket.OPEN ||
    socket.readyState === WebSocket.CONNECTING
  )) {
    return;
  }

  const scheme = location.protocol === "https:" ? "wss://" : "ws://";
  socket = new WebSocket(scheme + location.hostname + ":81/");

  socket.onopen = () => {
    setConnection(true, "ESP32 연결됨");
    serverStatusText.textContent = "서버 정보 수신 대기 중";
    if (reconnectTimer) {
      clearTimeout(reconnectTimer);
      reconnectTimer = null;
    }
  };

  socket.onclose = () => {
    setConnection(false, "연결 끊김 · 재연결 중");
    serverStatus.classList.remove("online");
    serverStatusText.textContent = "서버 연결 끊김";
    if (!reconnectTimer) {
      reconnectTimer = setTimeout(() => {
        reconnectTimer = null;
        connectWebSocket();
      }, 2000);
    }
  };

  socket.onerror = () => {
    setConnection(false, "WebSocket 연결 오류");
    serverStatus.classList.remove("online");
    serverStatusText.textContent = "WebSocket 연결 오류";
  };

  socket.onmessage = (event) => {
    let message;

    try {
      message = JSON.parse(event.data);
    } catch (e) {
      responseState.textContent = "응답 오류";
      answerElement.textContent = "서버 응답을 해석하지 못했습니다.";
      return;
    }

    if (message.type === "telemetry") {
      updateServerStatus(message);
      return;
    }

    if (message.type === "start") {
      streamedText = "";
      responseState.textContent = "생성 중";
      renderAnswer("", true);
      return;
    }

    if (message.type === "status") {
      responseState.textContent = message.text || "처리 중";
      return;
    }

    if (message.type === "chunk") {
      streamedText += message.text || "";
      responseState.textContent = "실시간 생성 중";
      renderAnswer(streamedText, true);
      return;
    }

    if (message.type === "done") {
      responseState.textContent = "완료";
      renderAnswer(streamedText, false);
      waitingForAnswer = false;
      sendButton.querySelector("span").textContent = "질문 보내기";
      sendButton.disabled = !socket || socket.readyState !== WebSocket.OPEN;
      return;
    }

    if (message.type === "error") {
      const errorText = message.text || "요청 중 오류가 발생했습니다.";

      if (streamedText) {
        streamedText += "\n\n---\n\n" + errorText;
      } else {
        streamedText = errorText;
      }

      responseState.textContent = "오류";
      renderAnswer(streamedText, false);
      waitingForAnswer = false;
      sendButton.querySelector("span").textContent = "질문 보내기";
      sendButton.disabled = !socket || socket.readyState !== WebSocket.OPEN;
    }
  };
}

function sendQuestion() {
  if (!socket || socket.readyState !== WebSocket.OPEN) {
    answerElement.textContent = "ESP32 WebSocket 연결을 기다려 주세요.";
    responseState.textContent = "연결 필요";
    return;
  }

  if (waitingForAnswer) return;

  const question = questionInput.value.trim();
  const image = imageInput.value.trim();

  if (!question) {
    questionInput.focus();
    return;
  }

  waitingForAnswer = true;
  streamedText = "";
  responseState.textContent = "요청 전송 중";
  answerElement.textContent = "NVIDIA API에 연결하고 있습니다...";
  answerElement.classList.remove("typing-cursor");

  sendButton.disabled = true;
  sendButton.querySelector("span").textContent = "생성 중...";

  socket.send(JSON.stringify({
    action: "ask",
    q: question,
    image: image
  }));
}

form.addEventListener("submit", (event) => {
  event.preventDefault();
  sendQuestion();
});

questionInput.addEventListener("keydown", (event) => {
  if (event.key === "Enter" && (event.ctrlKey || event.metaKey)) {
    event.preventDefault();
    sendQuestion();
  }
});

connectWebSocket();
</script>
</body>
</html>
)rawliteral";