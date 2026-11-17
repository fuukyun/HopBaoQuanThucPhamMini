#include <Arduino.h>            // Bắt buộc trong PlatformIO
#include <OneWire.h>            // Giao tiếp 1 dây (OneWire)
#include <DallasTemperature.h>  // Đọc cảm biến DS18B20

// BÀI 3: ESP32 nháy LED (bước 4.1) + đọc DS18B20 in ra Serial (bước 4.2)

const int CHAN_CAM_BIEN = 4;   // DS18B20 chân DQ nối GPIO 4
const int CHAN_TAI      = 25;  // LED tải giả, sau này thay bằng MOSFET -> Peltier

OneWire oneWire(CHAN_CAM_BIEN);        // Tạo "đường dây" OneWire trên GPIO 4
DallasTemperature camBien(&oneWire);   // Gắn cảm biến DS18B20 vào đường dây đó

bool ledSang = false;  // Nhớ trạng thái LED giữa các vòng loop()

// setup() chạy MỘT lần khi bo khởi động
void setup() {
  Serial.begin(115200);
  pinMode(CHAN_TAI, OUTPUT);
  camBien.begin();  // Khởi động thư viện, tìm cảm biến trên dây

  Serial.print("So cam bien tim thay: ");
  Serial.println(camBien.getDeviceCount());
  Serial.println("Bai 3: ESP32 + DS18B20");
}

// loop() lặp lại MÃI MÃI sau setup()
void loop() {
  // 4.1 - Đảo trạng thái LED mỗi vòng
  ledSang = !ledSang;
  digitalWrite(CHAN_TAI, ledSang ? HIGH : LOW);

  // 4.2 - Đọc nhiệt độ: ra lệnh đo, rồi lấy kết quả
  camBien.requestTemperatures();               // Bảo cảm biến đo (mất ~750 ms)
  float nhietDo = camBien.getTempCByIndex(0);  // Lấy kết quả cảm biến số 0

  if (nhietDo == DEVICE_DISCONNECTED_C) {      // Thư viện trả về -127 khi mất kết nối
    Serial.println("LOI CAM BIEN!");
  } else {
    Serial.print("Nhiet do: ");
    Serial.print(nhietDo, 2);  // In 2 chữ số sau dấu phẩy
    Serial.println(" *C");
  }

  delay(1000);
}
