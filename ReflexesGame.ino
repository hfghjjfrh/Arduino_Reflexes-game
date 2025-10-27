const int inputPins[]  = {36, 39, 34, 35, 32, 33, 25, 26, 27, 14};
const int outputPins[] = {22, 1, 3, 21, 17, 16, 4, 0, 2, 15};
const int pinCount = sizeof(inputPins) / sizeof(inputPins[0]);

const int startBtn1 = 36;
const int startBtn2 = 39;
const int buzzer = 23;  // 正解時のみ点灯

int targetIndex = -1;
bool gameRunning = false;
unsigned long ledOnTime = 0;
const unsigned long ledDuration = 500;      // LED点灯時間1.5秒
const unsigned long gameDuration = 30000;    // ゲーム30秒
unsigned long gameStartTime = 0;
int score = 0;

// ボタンの立ち上がり判定用
bool lastButtonState[pinCount] = {0};

void setup() {
  for (int i = 0; i < pinCount; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);
    if (inputPins[i] == 34 || inputPins[i] == 35 || inputPins[i] == 36 || inputPins[i] == 39)
      pinMode(inputPins[i], INPUT);        // 内部プルアップなし
    else
      pinMode(inputPins[i], INPUT_PULLUP); // 内部プルアップ有効
  }

  pinMode(buzzer, OUTPUT);
  noTone(buzzer);

  showWaitingAnimation();
}

void loop() {
  if (!gameRunning) {
    waitForStart();
    return;
  }

  // ゲーム終了判定
  if (millis() - gameStartTime >= gameDuration) {
    showScore();
    gameRunning = false;
    showWaitingAnimation();
    return;
  }

  // ボタンチェック（立ち上がりのみ）
  for (int i = 0; i < pinCount; i++) {
    bool currentState = digitalRead(inputPins[i]) == LOW;
    if (currentState && !lastButtonState[i]) {  // 押された瞬間
      if (i == targetIndex) {
        // 正解時
        score++;
        tone(buzzer,1000,200);
        delay(200);
        noTone(buzzer);
        // ターゲットLEDを一定時間表示
        delay(ledDuration);
      }
      // 間違いの場合は何もしない
      nextRound();
    }
    lastButtonState[i] = currentState;
  }

  // タイムアウト判定（押さなかった場合）
  if (millis() - ledOnTime > ledDuration) {
    nextRound();
  }
}

// スタート待機
void waitForStart() {
  if (digitalRead(startBtn1) == LOW && digitalRead(startBtn2) == LOW) {
    blinkAllLEDs(2, 100);
    delay(500);
    gameRunning = true;
    score = 0;
    gameStartTime = millis();
    nextRound();
  } else {
    showWaitingAnimation();
  }
}

// 次のターゲットLEDをランダム表示
void nextRound() {
  for (int i = 0; i < pinCount; i++) digitalWrite(outputPins[i], LOW);
  targetIndex = random(pinCount);
  digitalWrite(outputPins[targetIndex], HIGH);
  ledOnTime = millis();
}

// 全LED点滅（演出用）
void blinkAllLEDs(int times, int interval) {
  for (int t = 0; t < times; t++) {
    for (int i = 0; i < pinCount; i++) digitalWrite(outputPins[i], HIGH);
    delay(interval);
    for (int i = 0; i < pinCount; i++) digitalWrite(outputPins[i], LOW);
    delay(interval);
  }
}

// 待機アニメーション
void showWaitingAnimation() {
  static int idx = 0;
  static unsigned long prev = 0;
  if (millis() - prev > 150) {
    for (int i = 0; i < pinCount; i++) digitalWrite(outputPins[i], LOW);
    digitalWrite(outputPins[idx], HIGH);
    idx = (idx + 1) % pinCount;
    prev = millis();
  }
}

// スコア表示（補助LEDで点滅）
void showScore() {
  for (int i = 0; i < score; i++) {
    tone(buzzer,2000,150);
    delay(200);
    noTone(buzzer);
    delay(200);
  }
}
