#include <Arduino.h>  // Bắt buộc trong PlatformIO (Arduino IDE tự thêm ngầm)

// BÀI 2 - PHẦN 1: Nhấn giữ nút thì LED sáng, thả ra thì LED tắt

const int NUT_NHAN = 2;   // Nút nhấn nối chân 2 (chân còn lại nối GND)
const int LED_DO   = 13;  // LED đỏ nối chân 13

// setup() chạy MỘT lần khi bo khởi động
void setup() {
  pinMode(NUT_NHAN, INPUT_PULLUP);  // Ngõ vào + bật điện trở kéo lên bên trong chip
  pinMode(LED_DO, OUTPUT);
  Serial.begin(9600);
  Serial.println("Bai 2: Nhan nut de bat LED");
}

// loop() lặp lại MÃI MÃI sau setup()
void loop() {
  int trangThai = digitalRead(NUT_NHAN);  // Đọc chân 2: HIGH hoặc LOW

  // Với INPUT_PULLUP: KHÔNG nhấn = HIGH, ĐANG nhấn = LOW (logic ngược!)
  if (trangThai == LOW) {
    digitalWrite(LED_DO, HIGH);
  } else {
    digitalWrite(LED_DO, LOW);
  }

  Serial.println(trangThai == LOW ? "Dang nhan" : "Tha ra");
  delay(100);  // Chậm lại cho dễ đọc Serial
}
