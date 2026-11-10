#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SDA_PIN 8
#define SCL_PIN 9
#define BUTTON_PIN 4  // Nút bấm đổi trạng thái lập tức

// Danh sách hơn 15 trạng thái chuyển động & biểu cảm ngộ nghĩnh
enum State { 
  HAPPY,         // 0. Cười vui vẻ
  LOOK_AROUND,   // 1. Liếc qua lại trái phải tinh nghịch
  LOOK_UP_DOWN,  // 2. Nhìn lên nhìn xuống ngộ nghĩnh
  DOUBLE_BLINK,  // 3. Nháy mắt liên tục 2 lần
  SPARKLE,       // 4. Mắt cười tít hình cung (^ ^)
  LOVE,          // 5. Mắt trái tim lấp lánh ❤️
  SURPRISED,     // 6. Ngạc nhiên tròn xoe há miệng
  ANGRY,         // 7. Cáu kỉnh / Nghiến răng
  CRY,           // 8. Buồn mếu máo rớm lệ
  SLEEPING,      // 9. Ngủ gật khò khò (- -)
  PROUD,         // 10. Tự đắc nhếch mép
  DIZZY,         // 11. Mắt chóng mặt hình chữ X (X X)
  SQUINT,        // 12. Mắt nheo tít gian xảo
  WINK_LEFT,     // 13. Nháy mắt trái tinh nghịch
  WINK_RIGHT,    // 14. Nháy mắt phải tinh nghịch
  SHY            // 15. Xấu hổ má đỏ / cười thẹn thùng
};

State currentState = HAPPY;
unsigned long lastExpressionChange = 0;
unsigned long expressionInterval = 3000; 
unsigned long lastAnimUpdate = 0;
int animStep = 0;
bool buttonLastState = HIGH;

// --- Hàm vẽ Mắt cơ bản ---
void drawEye(int x, int y, int w, int h, int r) {
  display.fillRoundRect(x, y, w, h, r, SSD1306_WHITE);
}

void drawBlink(int x, int y, int w) {
  display.fillRect(x, y + 16, w, 5, SSD1306_WHITE);
}

// --- Hàm vẽ Miệng tương ứng với 16 trạng thái ---
void drawMouth(State state) {
  int mouthX = 56;
  int mouthY = 53;

  switch (state) {
    case HAPPY:
    case SPARKLE:
    case WINK_LEFT:
    case WINK_RIGHT:
      // Miệng cười mỉm
      display.drawFastHLine(mouthX - 6, mouthY, 13, SSD1306_WHITE);
      display.drawPixel(mouthX - 7, mouthY - 1, SSD1306_WHITE);
      display.drawPixel(mouthX + 7, mouthY - 1, SSD1306_WHITE);
      break;

    case LOVE:
    case SHY:
      // Miệng chúm chím nhỏ
      display.fillCircle(mouthX, mouthY, 3, SSD1306_WHITE);
      break;

    case SURPRISED:
    case DIZZY:
      // Miệng há to chữ O
      display.fillCircle(mouthX, mouthY, 5, SSD1306_BLACK);
      display.drawCircle(mouthX, mouthY, 5, SSD1306_WHITE);
      break;

    case ANGRY:
      // Miệng cáu kỉnh (zic-zac)
      display.drawLine(mouthX - 8, mouthY + 2, mouthX - 3, mouthY - 2, SSD1306_WHITE);
      display.drawLine(mouthX - 3, mouthY - 2, mouthX + 3, mouthY + 2, SSD1306_WHITE);
      display.drawLine(mouthX + 3, mouthY + 2, mouthX + 8, mouthY - 2, SSD1306_WHITE);
      break;

    case CRY:
      // Miệng mếu
      display.drawPixel(mouthX - 7, mouthY + 1, SSD1306_WHITE);
      display.drawFastHLine(mouthX - 6, mouthY, 13, SSD1306_WHITE);
      display.drawPixel(mouthX + 7, mouthY + 1, SSD1306_WHITE);
      break;

    case PROUD:
    case SQUINT:
      // Miệng nhếch mép
      display.drawLine(mouthX - 6, mouthY, mouthX + 6, mouthY - 2, SSD1306_WHITE);
      break;

    case SLEEPING:
      // Miệng khi ngủ
      display.drawFastHLine(mouthX - 3, mouthY, 7, SSD1306_WHITE);
      break;

    default:
      // Miệng ngang bình thường
      display.drawFastHLine(mouthX - 4, mouthY, 9, SSD1306_WHITE);
      break;
  }
}

// --- Hàm hiển thị khuôn mặt động hoàn chỉnh ---
void renderFace() {
  display.clearDisplay();

  int eyeWidth = 34;
  int eyeHeight = 38;
  int baseLeftX = 20;
  int baseRightX = 74;
  int baseEyeY = 10;

  int offsetX = 0;
  int offsetY = 0;

  // Xử lý chuyển động hoạt hoạ theo từng khung hình (animStep)
  if (currentState == LOOK_AROUND) {
    if (animStep % 4 == 0) offsetX = -7;
    else if (animStep % 4 == 2) offsetX = 7;
    else offsetX = 0;
  } else if (currentState == LOOK_UP_DOWN) {
    if (animStep % 2 == 0) offsetY = -5;
    else offsetY = 5;
  } else if (currentState == DOUBLE_BLINK) {
    if (animStep % 2 == 1) {
      drawBlink(baseLeftX, baseEyeY, eyeWidth);
      drawBlink(baseRightX, baseEyeY, eyeWidth);
      drawMouth(currentState);
      display.display();
      return;
    }
  }

  int leftX = baseLeftX + offsetX;
  int rightX = baseRightX + offsetX;
  int eyeY = baseEyeY + offsetY;

  // Vẽ mắt tương ứng với 16 trạng thái
  switch (currentState) {
    case HAPPY:
    case LOOK_AROUND:
    case LOOK_UP_DOWN:
    case DOUBLE_BLINK:
      drawEye(leftX, eyeY, eyeWidth, eyeHeight, 10);
      drawEye(rightX, eyeY, eyeWidth, eyeHeight, 10);
      break;

    case SPARKLE:
      drawEye(leftX, eyeY + 6, eyeWidth, eyeHeight - 6, 8);
      display.fillTriangle(leftX - 4, eyeY + 14, leftX + eyeWidth + 4, eyeY + 14, leftX + eyeWidth / 2, eyeY, SSD1306_BLACK);
      drawEye(rightX, eyeY + 6, eyeWidth, eyeHeight - 6, 8);
      display.fillTriangle(rightX - 4, eyeY + 14, rightX + eyeWidth + 4, eyeY + 14, rightX + eyeWidth / 2, eyeY, SSD1306_BLACK);
      break;

    case LOVE:
      display.fillCircle(leftX + 10, eyeY + 10, 9, SSD1306_WHITE);
      display.fillCircle(leftX + 26, eyeY + 10, 9, SSD1306_WHITE);
      display.fillTriangle(leftX, eyeY + 14, leftX + 36, eyeY + 14, leftX + 18, eyeY + 34, SSD1306_WHITE);
      display.fillCircle(rightX + 10, eyeY + 10, 9, SSD1306_WHITE);
      display.fillCircle(rightX + 26, eyeY + 10, 9, SSD1306_WHITE);
      display.fillTriangle(rightX, eyeY + 14, rightX + 36, eyeY + 14, rightX + 18, eyeY + 34, SSD1306_WHITE);
      break;

    case SURPRISED:
      display.fillCircle(leftX + 17, eyeY + 17, 16, SSD1306_WHITE);
      display.fillCircle(leftX + 17, eyeY + 17, 9, SSD1306_BLACK);
      display.fillCircle(rightX + 17, eyeY + 17, 16, SSD1306_WHITE);
      display.fillCircle(rightX + 17, eyeY + 17, 9, SSD1306_BLACK);
      break;

    case ANGRY:
      drawEye(leftX, eyeY, eyeWidth, eyeHeight, 8);
      display.fillTriangle(leftX - 5, eyeY - 4, leftX + eyeWidth + 5, eyeY - 4, leftX + eyeWidth + 5, eyeY + 14, SSD1306_BLACK);
      drawEye(rightX, eyeY, eyeWidth, eyeHeight, 8);
      display.fillTriangle(rightX - 5, eyeY - 4, rightX + eyeWidth + 5, eyeY - 4, rightX - 5, eyeY + 14, SSD1306_BLACK);
      break;

    case CRY:
      drawEye(leftX, eyeY, eyeWidth, eyeHeight, 8);
      display.fillTriangle(leftX - 4, eyeY - 4, leftX + eyeWidth + 4, eyeY - 4, leftX - 4, eyeY + 14, SSD1306_BLACK);
      display.fillCircle(leftX + 10, eyeY + 40, 3, SSD1306_WHITE);
      drawEye(rightX, eyeY, eyeWidth, eyeHeight, 8);
      display.fillTriangle(rightX - 4, eyeY - 4, rightX + eyeWidth + 4, eyeY - 4, rightX - 4, eyeY + 14, SSD1306_BLACK);
      display.fillCircle(rightX + 10, eyeY + 40, 3, SSD1306_WHITE);
      break;

    case SLEEPING:
      display.drawFastHLine(leftX, eyeY + 18, eyeWidth, SSD1306_WHITE);
      display.drawFastHLine(leftX, eyeY + 19, eyeWidth, SSD1306_WHITE);
      display.drawFastHLine(rightX, eyeY + 18, eyeWidth, SSD1306_WHITE);
      display.drawFastHLine(rightX, eyeY + 19, eyeWidth, SSD1306_WHITE);
      break;

    case PROUD:
      display.fillRoundRect(leftX, eyeY + 8, eyeWidth, 14, 6, SSD1306_WHITE);
      display.fillRoundRect(rightX, eyeY + 8, eyeWidth, 14, 6, SSD1306_WHITE);
      break;

    case DIZZY:
      display.drawLine(leftX, eyeY + 5, leftX + 30, eyeY + 35, SSD1306_WHITE);
      display.drawLine(leftX, eyeY + 35, leftX + 30, eyeY + 5, SSD1306_WHITE);
      display.fillRect(leftX + 6, eyeY + 12, 18, 12, SSD1306_BLACK);
      display.drawLine(rightX, eyeY + 5, rightX + 30, eyeY + 35, SSD1306_WHITE);
      display.drawLine(rightX, eyeY + 35, rightX + 30, eyeY + 5, SSD1306_WHITE);
      display.fillRect(rightX + 6, eyeY + 12, 18, 12, SSD1306_BLACK);
      break;

    case SQUINT:
      display.drawFastHLine(leftX, eyeY + 18, eyeWidth, SSD1306_WHITE);
      display.drawFastHLine(leftX, eyeY + 20, eyeWidth, SSD1306_WHITE);
      display.drawFastHLine(rightX, eyeY + 18, eyeWidth, SSD1306_WHITE);
      display.drawFastHLine(rightX, eyeY + 20, eyeWidth, SSD1306_WHITE);
      break;

    case WINK_LEFT:
      drawBlink(leftX, eyeY, eyeWidth); // Nháy mắt trái
      drawEye(rightX, eyeY, eyeWidth, eyeHeight, 10);
      break;

    case WINK_RIGHT:
      drawEye(leftX, eyeY, eyeWidth, eyeHeight, 10);
      drawBlink(rightX, eyeY, eyeWidth); // Nháy mắt phải
      break;

    case SHY:
      // Mắt cong thẹn thùng kèm hiệu ứng má hồng (chấm nhỏ hai bên)
      display.fillRoundRect(leftX, eyeY + 10, eyeWidth, 16, 6, SSD1306_WHITE);
      display.fillRoundRect(rightX, eyeY + 10, eyeWidth, 16, 6, SSD1306_WHITE);
      display.fillCircle(leftX - 4, eyeY + 28, 3, SSD1306_WHITE);
      display.fillCircle(rightX + eyeWidth + 4, eyeY + 28, 3, SSD1306_WHITE);
      break;
  }

  // Vẽ miệng tương ứng phía dưới
  drawMouth(currentState);
  display.display();
}

// --- Màn hình khởi động ---
void showBootScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(6, 12);
  display.println(F("DASAI"));
  display.setCursor(24, 34);
  display.println(F("MOCHI"));
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(28, 20);
  display.println(F("Created by"));
  display.setCursor(18, 36);
  display.println(F("HOANG DUONG"));
  display.display();
  delay(1800);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(SDA_PIN, SCL_PIN);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  showBootScreen();
  randomSeed(analogRead(A0));
  renderFace();
}

void loop() {
  unsigned long currentMillis = millis();

  // Nhấn nút để chuyển đổi qua lại 16 trạng thái liên tục
  bool buttonCurrentState = digitalRead(BUTTON_PIN);
  if (buttonCurrentState == LOW && buttonLastState == HIGH) {
    currentState = static_cast<State>((currentState + 1) % 16);
    renderFace();
    lastExpressionChange = currentMillis; 
    delay(200);
  }
  buttonLastState = buttonCurrentState;

  // Tự động đổi biểu cảm ngẫu nhiên sau mỗi 3 giây
  if (currentMillis - lastExpressionChange >= expressionInterval) {
    lastExpressionChange = currentMillis;
    expressionInterval = random(2500, 4500);
    currentState = static_cast<State>(random(0, 16)); // Chọn ngẫu nhiên trong 16 trạng thái
    renderFace();
  }

  // Cập nhật hoạt hoạ chuyển động mượt mà liên tục cho các trạng thái động
  if (currentMillis - lastAnimUpdate >= 300) {
    lastAnimUpdate = currentMillis;
    animStep++;
    if (currentState == LOOK_AROUND || currentState == LOOK_UP_DOWN || currentState == DOUBLE_BLINK) {
      renderFace();
    }
  }
}