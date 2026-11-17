#include <Arduino.h>

// ĐÁP ÁN BÀI 1 - BT2: Nháy LED theo tín hiệu SOS (··· ––– ···)

const int LED_PIN = 13;

const int CHAM      = 200;       // Nháy ngắn (dấu chấm)
const int GACH      = CHAM * 3;  // Nháy dài (dấu gạch) = 3 lần chấm
const int NGHI_KY_HIEU = CHAM;      // Nghỉ giữa các nháy trong cùng một chữ
const int NGHI_CHU  = CHAM * 3;  // Nghỉ giữa hai chữ
const int NGHI_TU   = CHAM * 7;  // Nghỉ trước khi lặp lại SOS

// Nháy LED một lần, sáng trong khoảng thoiGian ms
void nhay(int thoiGian) {
  digitalWrite(LED_PIN, HIGH);
  delay(thoiGian);
  digitalWrite(LED_PIN, LOW);
  delay(NGHI_KY_HIEU);
}

// Nháy 3 lần liên tiếp với cùng độ dài
void nhay3Lan(int thoiGian) {
  for (int i = 0; i < 3; i++) {  // Vòng lặp for: lặp 3 lần, i = 0, 1, 2
    nhay(thoiGian);
  }
  delay(NGHI_CHU);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  nhay3Lan(CHAM);  // S
  nhay3Lan(GACH);  // O
  nhay3Lan(CHAM);  // S
  delay(NGHI_TU);
}
