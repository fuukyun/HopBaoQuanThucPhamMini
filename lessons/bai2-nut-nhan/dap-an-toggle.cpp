#include <Arduino.h>

// ĐÁP ÁN BÀI 2 - BT2: Nhấn một lần bật, nhấn lần nữa tắt (toggle) + chống dội phím

const int NUT_NHAN = 2;
const int LED_DO   = 13;
const int CHONG_DOI = 50;  // ms chờ cho tiếp điểm hết nảy

bool denBat = false;          // Khai báo NGOÀI loop() để nhớ giữa các vòng
int trangThaiTruoc = HIGH;    // Lần đọc trước (HIGH = không nhấn, vì INPUT_PULLUP)

void setup() {
  pinMode(NUT_NHAN, INPUT_PULLUP);
  pinMode(LED_DO, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int trangThai = digitalRead(NUT_NHAN);

  if (trangThai != trangThaiTruoc) {     // Có thay đổi -> có thể là nhấn thật, có thể là dội
    delay(CHONG_DOI);                    // Chờ tiếp điểm ổn định
    trangThai = digitalRead(NUT_NHAN);   // Đọc lại cho chắc

    if (trangThai != trangThaiTruoc) {   // Vẫn khác -> thay đổi thật
      if (trangThai == LOW) {            // Chỉ đổi đèn ở cạnh HIGH -> LOW (lúc vừa nhấn xuống)
        denBat = !denBat;
        digitalWrite(LED_DO, denBat ? HIGH : LOW);
        Serial.println(denBat ? "Den BAT" : "Den TAT");
      }
      trangThaiTruoc = trangThai;
    }
  }
}
