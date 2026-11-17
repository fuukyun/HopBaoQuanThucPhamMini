#include <Arduino.h>

// ĐÁP ÁN BÀI 2 - BT3: Nút 1 (chân 2) -> nháy NHANH, nút 2 (chân 3) -> nháy CHẬM
// Dùng millis() để vừa nháy LED vừa đọc nút liên tục (delay() sẽ làm "điếc" nút nhấn)
// Mạch: thêm nút thứ hai vào diagram.json, một chân nối uno:3, chân chéo đối diện nối GND

const int NUT_NHANH = 2;
const int NUT_CHAM  = 3;
const int LED_DO    = 13;

unsigned long chuKy = 500;      // Nhịp nháy hiện tại (ms), mặc định trung bình
unsigned long lanDoiTruoc = 0;  // Mốc thời gian lần đổi trạng thái LED trước
bool ledSang = false;

void setup() {
  pinMode(NUT_NHANH, INPUT_PULLUP);
  pinMode(NUT_CHAM, INPUT_PULLUP);
  pinMode(LED_DO, OUTPUT);
}

void loop() {
  if (digitalRead(NUT_NHANH) == LOW) chuKy = 100;
  if (digitalRead(NUT_CHAM) == LOW)  chuKy = 1000;

  if (millis() - lanDoiTruoc >= chuKy) {
    lanDoiTruoc = millis();
    ledSang = !ledSang;
    digitalWrite(LED_DO, ledSang ? HIGH : LOW);
  }
}
