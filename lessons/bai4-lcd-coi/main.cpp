#include <Arduino.h>            // Bắt buộc trong PlatformIO
#include <OneWire.h>            // Giao tiếp 1 dây (OneWire)
#include <DallasTemperature.h>  // Đọc cảm biến DS18B20
#include <Wire.h>               // Giao tiếp I2C
#include <LiquidCrystal_I2C.h>  // Màn hình LCD 1602 qua I2C

// BÀI 4: Hiển thị nhiệt độ lên LCD (bước 4.4) + còi cảnh báo (bước 4.6)

const int CHAN_CAM_BIEN = 4;    // DS18B20 chân DQ
const int CHAN_COI      = 26;   // Còi cảnh báo
const int CHAN_SDA      = 21;   // I2C dữ liệu
const int CHAN_SCL      = 22;   // I2C xung nhịp

const float NGUONG_CANH_BAO = 8.0;  // Quá 8 °C thì cảnh báo
const int   TAN_SO_COI      = 2000; // Hz

OneWire oneWire(CHAN_CAM_BIEN);
DallasTemperature camBien(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Địa chỉ 0x27, 16 cột, 2 dòng

// Ghi một dòng lên LCD, tự thêm khoảng trắng cho đủ 16 ký tự
// -> ghi đè chữ cũ mà không cần lcd.clear() (tránh nhấp nháy)
void inDong(int dong, const char *noiDung) {
  char dem[17];                              // 16 ký tự + 1 ký tự kết thúc chuỗi
  snprintf(dem, sizeof(dem), "%-16s", noiDung);  // %-16s: canh trái, đệm cho đủ 16
  lcd.setCursor(0, dong);                    // Cột 0, dòng 0 hoặc 1
  lcd.print(dem);
}

// setup() chạy MỘT lần khi bo khởi động
void setup() {
  Serial.begin(115200);
  pinMode(CHAN_COI, OUTPUT);
  camBien.begin();

  Wire.begin(CHAN_SDA, CHAN_SCL);  // Khởi động I2C trên chân 21, 22
  lcd.init();
  lcd.backlight();                 // Bật đèn nền

  inDong(0, "Hop bao quan");
  inDong(1, "Dang khoi dong..");
  Serial.println("Bai 4: LCD + coi canh bao");
  delay(1500);
}

// loop() lặp lại MÃI MÃI sau setup()
void loop() {
  camBien.requestTemperatures();
  float nhietDo = camBien.getTempCByIndex(0);

  char dong1[17];
  bool coiKeu;

  if (nhietDo == DEVICE_DISCONNECTED_C) {
    inDong(0, "Nhiet do: --.-");
    inDong(1, "LOI CAM BIEN!");
    Serial.println("LOI CAM BIEN!");
    coiKeu = true;                       // Lỗi cảm biến cũng phải báo
  } else {
    snprintf(dong1, sizeof(dong1), "Nhiet do:%5.1f%cC", nhietDo, 223);  // 223 = ký hiệu độ trên LCD
    inDong(0, dong1);

    coiKeu = nhietDo > NGUONG_CANH_BAO;
    inDong(1, coiKeu ? "CANH BAO: >8 C" : "Trang thai: OK");

    Serial.print("Nhiet do: ");
    Serial.print(nhietDo, 2);
    Serial.println(coiKeu ? " *C  -> CANH BAO" : " *C");
  }

  if (coiKeu) {
    tone(CHAN_COI, TAN_SO_COI);  // Phát âm thanh liên tục
  } else {
    noTone(CHAN_COI);            // Tắt âm thanh
  }

  delay(1000);
}
