#include <Arduino.h>
#include <Wire.h>

// ĐÁP ÁN BÀI 4 - BT1: Quét địa chỉ I2C (mục 3.3)
// Dùng khi LCD thật chỉ sáng đèn nền mà không hiện chữ -> kiểm tra 0x27 hay 0x3F

const int CHAN_SDA = 21;
const int CHAN_SCL = 22;

void setup() {
  Serial.begin(115200);
  Wire.begin(CHAN_SDA, CHAN_SCL);
  delay(500);

  Serial.println("Dang quet I2C...");
  int soThietBi = 0;

  for (int diaChi = 1; diaChi < 127; diaChi++) {
    Wire.beginTransmission(diaChi);       // "Gõ cửa" địa chỉ này
    if (Wire.endTransmission() == 0) {    // 0 = có thiết bị trả lời
      Serial.printf("Tim thay thiet bi tai dia chi 0x%02X\n", diaChi);
      soThietBi++;
    }
  }

  Serial.printf("Xong. Tong cong %d thiet bi.\n", soThietBi);
}

void loop() {
}
