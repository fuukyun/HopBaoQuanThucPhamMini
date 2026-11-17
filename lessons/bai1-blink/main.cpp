#include <Arduino.h>  // Bắt buộc trong PlatformIO (Arduino IDE tự thêm ngầm)

const int LED_DO   = 13;  // LED đỏ nối chân 13
const int LED_XANH = 12;  // LED xanh nối chân 12
const int THOI_GIAN = 500; // Thời gian sáng mỗi LED (ms)

// setup() chạy MỘT lần khi bo khởi động
void setup() {
  pinMode(LED_DO, OUTPUT);    // Mỗi chân dùng làm ngõ ra đều phải khai báo
  pinMode(LED_XANH, OUTPUT);
  Serial.begin(9600);         // Mở cổng Serial để in thông tin
  Serial.println("Xin chao! Hai LED nhay luan phien...");
}

// loop() lặp lại MÃI MÃI sau setup()
void loop() {
  // Pha 1: đỏ sáng, xanh tắt
  digitalWrite(LED_DO, HIGH);
  digitalWrite(LED_XANH, LOW);
  Serial.println("DO: BAT  | XANH: TAT");
  delay(THOI_GIAN);

  // Pha 2: đỏ tắt, xanh sáng
  digitalWrite(LED_DO, LOW);
  digitalWrite(LED_XANH, HIGH);
  Serial.println("DO: TAT  | XANH: BAT");
  delay(THOI_GIAN);
}
