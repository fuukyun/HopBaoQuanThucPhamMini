#include <Arduino.h>            // Bắt buộc trong PlatformIO
#include <OneWire.h>            // Giao tiếp 1 dây (OneWire)
#include <DallasTemperature.h>  // Đọc cảm biến DS18B20
#include <Wire.h>               // Giao tiếp I2C
#include <LiquidCrystal_I2C.h>  // Màn hình LCD 1602 qua I2C

// BÀI 5: millis() + điều khiển ON/OFF có độ trễ (hysteresis) + an toàn khi lỗi
// Ứng với bước 4.5 và 4.7 của đề tài. LED GPIO 25 đóng vai Peltier.

// ---------- Chân kết nối ----------
const int CHAN_CAM_BIEN = 4;    // DS18B20 chân DQ
const int CHAN_TAI      = 25;   // LED tải giả -> sau này là MOSFET -> Peltier
const int CHAN_COI      = 26;   // Còi cảnh báo
const int CHAN_SDA      = 21;   // I2C dữ liệu
const int CHAN_SCL      = 22;   // I2C xung nhịp

// ---------- Ngưỡng điều khiển ----------
const float NGUONG_BAT      = 5.5;   // T >= 5,5 °C -> BẬT Peltier
const float NGUONG_TAT      = 4.5;   // T <= 4,5 °C -> TẮT Peltier
const float NGUONG_CANH_BAO = 8.0;   // T >  8 °C   -> còi cảnh báo
const float NHIET_DO_MIN    = -30.0; // Ngoài khoảng này coi như số đo vô lý
const float NHIET_DO_MAX    = 70.0;

// ---------- Thời gian (ms) ----------
const unsigned long CHU_KY_DO      = 1000;  // Đo nhiệt độ mỗi 1 giây
const unsigned long COI_CHAM       = 500;   // Quá nhiệt: kêu/tắt mỗi 0,5 giây
const unsigned long COI_NHANH      = 150;   // Lỗi cảm biến: kêu/tắt mỗi 0,15 giây
const int           TAN_SO_COI     = 2000;  // Hz

OneWire oneWire(CHAN_CAM_BIEN);
DallasTemperature camBien(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- Trạng thái (nhớ giữa các vòng loop) ----------
bool peltierBat = false;     // Peltier đang bật hay tắt
bool loiCamBien = false;     // Cảm biến có đang lỗi không
bool quaNhiet   = false;     // Có đang vượt ngưỡng cảnh báo không
bool coiDangKeu = false;     // Còi đang ở nhịp kêu hay nhịp nghỉ

unsigned long lanDoTruoc  = 0;  // Mốc thời gian lần đo trước
unsigned long lanDoiCoi   = 0;  // Mốc thời gian lần đổi nhịp còi trước

// Ghi một dòng lên LCD, đệm khoảng trắng cho đủ 16 ký tự (không cần lcd.clear())
void inDong(int dong, const char *noiDung) {
  char dem[17];
  snprintf(dem, sizeof(dem), "%-16s", noiDung);
  lcd.setCursor(0, dong);
  lcd.print(dem);
}

// Số đo có đáng tin không? (mất kết nối trả về -127; số ngoài khoảng hợp lý cũng loại)
bool soDoHopLe(float t) {
  return t != DEVICE_DISCONNECTED_C && t > NHIET_DO_MIN && t < NHIET_DO_MAX;
}

// ===== VIỆC 1: Đo nhiệt độ, quyết định bật/tắt Peltier, cập nhật LCD =====
void doVaDieuKhien() {
  // Lấy kết quả của lệnh đo đã gửi ở lần trước, rồi gửi lệnh đo mới ngay
  float nhietDo = camBien.getTempCByIndex(0);
  camBien.requestTemperatures();  // Không chờ (xem setWaitForConversion trong setup)

  loiCamBien = !soDoHopLe(nhietDo);

  if (loiCamBien) {
    peltierBat = false;           // AN TOÀN KHI LỖI: không biết nhiệt độ thì tắt tải
    quaNhiet   = false;
  } else {
    // HYSTERESIS: hai ngưỡng khác nhau, vùng ở giữa thì GIỮ NGUYÊN trạng thái cũ
    if (nhietDo >= NGUONG_BAT) {
      peltierBat = true;
    } else if (nhietDo <= NGUONG_TAT) {
      peltierBat = false;
    }
    quaNhiet = nhietDo > NGUONG_CANH_BAO;
  }

  digitalWrite(CHAN_TAI, peltierBat ? HIGH : LOW);

  // Hiển thị
  char dong1[17];
  char dong2[17];
  if (loiCamBien) {
    inDong(0, "Nhiet do: --.-");
    inDong(1, "LOI CAM BIEN!");
    Serial.println("LOI CAM BIEN! -> Peltier TAT");
  } else {
    snprintf(dong1, sizeof(dong1), "Nhiet do:%5.1f%cC", nhietDo, 223);
    snprintf(dong2, sizeof(dong2), "Peltier:%s %s",
             peltierBat ? "BAT" : "TAT",
             quaNhiet ? ">8C!" : " OK ");
    inDong(0, dong1);
    inDong(1, dong2);

    Serial.print("T = ");
    Serial.print(nhietDo, 2);
    Serial.print(" *C | Peltier: ");
    Serial.print(peltierBat ? "BAT" : "TAT");
    Serial.println(quaNhiet ? " | CANH BAO" : "");
  }
}

// ===== VIỆC 2: Điều khiển còi kêu ngắt quãng =====
void dieuKhienCoi(unsigned long bayGio) {
  if (!loiCamBien && !quaNhiet) {  // Không có gì để báo
    noTone(CHAN_COI);
    coiDangKeu = false;
    return;
  }

  unsigned long nhip = loiCamBien ? COI_NHANH : COI_CHAM;  // Lỗi cảm biến kêu nhanh hơn

  if (bayGio - lanDoiCoi >= nhip) {  // Đến lúc đổi nhịp?
    lanDoiCoi = bayGio;
    coiDangKeu = !coiDangKeu;
    if (coiDangKeu) {
      tone(CHAN_COI, TAN_SO_COI);
    } else {
      noTone(CHAN_COI);
    }
  }
}

// setup() chạy MỘT lần khi bo khởi động
void setup() {
  Serial.begin(115200);
  pinMode(CHAN_TAI, OUTPUT);
  pinMode(CHAN_COI, OUTPUT);
  digitalWrite(CHAN_TAI, LOW);  // Khởi động ở trạng thái an toàn: tải TẮT

  camBien.begin();
  camBien.setWaitForConversion(false);  // requestTemperatures() trả về ngay, không chờ 750 ms
  camBien.requestTemperatures();        // Gửi lệnh đo đầu tiên

  Wire.begin(CHAN_SDA, CHAN_SCL);
  lcd.init();
  lcd.backlight();
  inDong(0, "Hop bao quan");
  inDong(1, "Dang khoi dong..");

  Serial.println("Bai 5: millis + hysteresis");
  lanDoTruoc = millis();  // Lần đo đầu tiên sẽ diễn ra sau CHU_KY_DO
}

// loop() lặp lại MÃI MÃI sau setup() - KHÔNG có delay(), chạy hàng nghìn lần mỗi giây
void loop() {
  unsigned long bayGio = millis();  // Số ms kể từ khi bo khởi động

  if (bayGio - lanDoTruoc >= CHU_KY_DO) {  // Đã đủ 1 giây kể từ lần đo trước?
    lanDoTruoc = bayGio;
    doVaDieuKhien();
  }

  dieuKhienCoi(bayGio);  // Còi được kiểm tra liên tục, không bị việc đo chặn lại
}
