#include <Arduino.h>            // Bắt buộc trong PlatformIO
#include <OneWire.h>            // Giao tiếp 1 dây (OneWire)
#include <DallasTemperature.h>  // Đọc cảm biến DS18B20
#include <Wire.h>               // Giao tiếp I2C
#include <LiquidCrystal_I2C.h>  // Màn hình LCD 1602 qua I2C
#include <WiFi.h>               // Kết nối WiFi (có sẵn trong ESP32)
#include <WebServer.h>          // Máy chủ web nhỏ chạy trên ESP32

// BÀI 7: Xem nhiệt độ trên điện thoại qua WiFi (mục 3.5)
// Đã gộp: bài 5 (hysteresis, an toàn khi lỗi) + BT bài 5 (thời gian tối thiểu,
// đếm số lần bật) + log CSV qua Serial (BT3 bài 3 / bài 6)

// ---------- Chân kết nối ----------
const int CHAN_CAM_BIEN = 4;    // DS18B20 chân DQ
const int CHAN_TAI      = 25;   // LED tải giả -> sau này là MOSFET -> Peltier
const int CHAN_COI      = 26;   // Còi cảnh báo
const int CHAN_SDA      = 21;   // I2C dữ liệu
const int CHAN_SCL      = 22;   // I2C xung nhịp

// ---------- WiFi ----------
// Trên Wokwi dùng mạng ảo "Wokwi-GUEST" (không mật khẩu, kênh 6).
// Khi nạp vào ESP32 thật: đổi thành tên và mật khẩu WiFi nhà em.
const char *TEN_WIFI     = "Wokwi-GUEST";
const char *MAT_KHAU_WIFI = "";

// ---------- Ngưỡng điều khiển ----------
const float NGUONG_BAT      = 5.5;   // T >= 5,5 °C -> BẬT Peltier
const float NGUONG_TAT      = 4.5;   // T <= 4,5 °C -> TẮT Peltier
const float NGUONG_CANH_BAO = 8.0;   // T >  8 °C   -> còi cảnh báo
const float NHIET_DO_MIN    = -30.0; // Ngoài khoảng này coi như số đo vô lý
const float NHIET_DO_MAX    = 70.0;

// ---------- Thời gian (ms) ----------
const unsigned long CHU_KY_DO          = 1000;   // Đo nhiệt độ mỗi 1 giây
const unsigned long THOI_GIAN_TOI_THIEU = 30000; // Peltier bật/tắt phải giữ ít nhất 30 giây
const unsigned long COI_CHAM           = 500;    // Quá nhiệt: kêu/tắt mỗi 0,5 giây
const unsigned long COI_NHANH          = 150;    // Lỗi cảm biến: kêu/tắt mỗi 0,15 giây
const int           TAN_SO_COI         = 2000;   // Hz

OneWire oneWire(CHAN_CAM_BIEN);
DallasTemperature camBien(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WebServer mayChu(80);  // Máy chủ web ở cổng 80 (cổng mặc định của trình duyệt)

// ---------- Trạng thái (nhớ giữa các vòng loop) ----------
float nhietDoHienTai = 0;    // Số đo gần nhất (để trang web đọc)
bool peltierBat = false;     // Peltier đang bật hay tắt
bool loiCamBien = false;     // Cảm biến có đang lỗi không
bool quaNhiet   = false;     // Có đang vượt ngưỡng cảnh báo không
bool coiDangKeu = false;     // Còi đang ở nhịp kêu hay nhịp nghỉ
bool daBaoIP    = false;     // Đã in địa chỉ IP ra Serial chưa
unsigned int soLanBat = 0;   // Số lần Peltier chuyển TẮT -> BẬT (câu hỏi nghiên cứu 3)

unsigned long lanDoTruoc = 0;  // Mốc thời gian lần đo trước
unsigned long lanDoiCoi  = 0;  // Mốc thời gian lần đổi nhịp còi trước
unsigned long lanDoiTai  = 0;  // Mốc thời gian lần Peltier đổi trạng thái gần nhất

// Trang web gửi cho điện thoại. Trang tự hỏi "/du-lieu" mỗi 2 giây để cập nhật số.
const char TRANG_WEB[] = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Hộp bảo quản</title>
<style>
  body { font-family: sans-serif; text-align: center; background: #eef4f8; margin: 0; padding: 16px; }
  .the { background: white; border-radius: 12px; padding: 16px; margin: 12px auto; max-width: 360px; }
  .so { font-size: 56px; font-weight: bold; }
  .ok { color: #1a7f37; } .canh-bao { color: #c62828; }
</style>
</head>
<body>
  <h2>❄️ Hộp bảo quản mini</h2>
  <div class="the"><div>Nhiệt độ trong hộp</div><div class="so" id="t">--.-</div></div>
  <div class="the">
    <div>Trạng thái: <b id="tt">...</b></div>
    <div>Peltier: <b id="p">...</b></div>
    <div>Số lần bật Peltier: <b id="n">0</b></div>
    <div>Thời gian chạy: <b id="g">0</b> giây</div>
  </div>
<script>
async function capNhat() {
  try {
    const d = await (await fetch('/du-lieu')).json();
    document.getElementById('t').textContent = d.loi ? '--.-' : d.t.toFixed(1) + ' °C';
    const tt = document.getElementById('tt');
    tt.textContent = d.loi ? 'LỖI CẢM BIẾN' : (d.quaNhiet ? 'CẢNH BÁO > 8 °C' : 'Bình thường');
    tt.className = (d.loi || d.quaNhiet) ? 'canh-bao' : 'ok';
    document.getElementById('p').textContent = d.peltier ? 'BẬT' : 'TẮT';
    document.getElementById('n').textContent = d.soLanBat;
    document.getElementById('g').textContent = d.giay;
  } catch (e) {
    document.getElementById('tt').textContent = 'Mất kết nối';
  }
}
capNhat();
setInterval(capNhat, 2000);
</script>
</body>
</html>
)rawliteral";

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

// Đổi trạng thái Peltier (chỉ làm khi thật sự khác trạng thái cũ)
void datTai(bool bat) {
  if (bat == peltierBat) return;
  peltierBat = bat;
  lanDoiTai = millis();
  if (bat) soLanBat++;
  digitalWrite(CHAN_TAI, bat ? HIGH : LOW);
}

// ===== VIỆC 1: Đo nhiệt độ, quyết định bật/tắt Peltier, cập nhật LCD, ghi log =====
void doVaDieuKhien() {
  float nhietDo = camBien.getTempCByIndex(0);
  camBien.requestTemperatures();  // Không chờ (xem setWaitForConversion trong setup)

  loiCamBien = !soDoHopLe(nhietDo);

  if (loiCamBien) {
    datTai(false);     // AN TOÀN KHI LỖI: tắt NGAY, không cần chờ thời gian tối thiểu
    quaNhiet = false;
  } else {
    nhietDoHienTai = nhietDo;

    // HYSTERESIS: quyết định "muốn" bật hay tắt
    bool muonBat = peltierBat;
    if (nhietDo >= NGUONG_BAT) {
      muonBat = true;
    } else if (nhietDo <= NGUONG_TAT) {
      muonBat = false;
    }

    // THỜI GIAN TỐI THIỂU: chỉ được đổi khi trạng thái cũ đã giữ đủ lâu
    // (lần bật đầu tiên sau khi khởi động thì không phải chờ)
    bool duThoiGian = (soLanBat == 0) || (millis() - lanDoiTai >= THOI_GIAN_TOI_THIEU);
    if (muonBat != peltierBat && duThoiGian) {
      datTai(muonBat);
    }

    quaNhiet = nhietDo > NGUONG_CANH_BAO;
  }

  // Hiển thị LCD
  char dong1[17];
  char dong2[17];
  if (loiCamBien) {
    inDong(0, "Nhiet do: --.-");
    inDong(1, "LOI CAM BIEN!");
  } else {
    snprintf(dong1, sizeof(dong1), "Nhiet do:%5.1f%cC", nhietDo, 223);
    snprintf(dong2, sizeof(dong2), "Peltier:%s %s",
             peltierBat ? "BAT" : "TAT",
             quaNhiet ? ">8C!" : " OK ");
    inDong(0, dong1);
    inDong(1, dong2);
  }

  // Log CSV: giay,nhiet_do,peltier,canh_bao,so_lan_bat
  // canh_bao: 0 = bình thường, 1 = quá nhiệt, 2 = lỗi cảm biến
  int maCanhBao = loiCamBien ? 2 : (quaNhiet ? 1 : 0);
  Serial.printf("%lu,", millis() / 1000);
  if (!loiCamBien) {
    Serial.print(nhietDo, 2);  // Khi lỗi thì để trống ô này (Excel hiểu là thiếu số liệu)
  }
  Serial.printf(",%d,%d,%u", peltierBat ? 1 : 0, maCanhBao, soLanBat);
  Serial.println();  // Xuống dòng chuẩn "\r\n" (chỉ "\n" sẽ làm log bị xếp bậc thang)
}

// ===== VIỆC 2: Điều khiển còi kêu ngắt quãng =====
void dieuKhienCoi(unsigned long bayGio) {
  if (!loiCamBien && !quaNhiet) {
    noTone(CHAN_COI);
    coiDangKeu = false;
    return;
  }

  unsigned long nhip = loiCamBien ? COI_NHANH : COI_CHAM;

  if (bayGio - lanDoiCoi >= nhip) {
    lanDoiCoi = bayGio;
    coiDangKeu = !coiDangKeu;
    if (coiDangKeu) {
      tone(CHAN_COI, TAN_SO_COI);
    } else {
      noTone(CHAN_COI);
    }
  }
}

// ===== VIỆC 3: Trả lời trình duyệt =====
void guiTrangChu() {
  mayChu.send(200, "text/html; charset=utf-8", TRANG_WEB);
}

void guiDuLieu() {
  char json[160];
  snprintf(json, sizeof(json),
           "{\"t\":%.2f,\"loi\":%s,\"quaNhiet\":%s,\"peltier\":%s,\"soLanBat\":%u,\"giay\":%lu}",
           nhietDoHienTai,
           loiCamBien ? "true" : "false",
           quaNhiet ? "true" : "false",
           peltierBat ? "true" : "false",
           soLanBat,
           millis() / 1000);
  mayChu.send(200, "application/json", json);
}

// ===== VIỆC 4: Báo địa chỉ IP một lần khi WiFi vừa kết nối =====
void kiemTraWiFi() {
  if (!daBaoIP && WiFi.status() == WL_CONNECTED) {
    daBaoIP = true;
    Serial.print("# WiFi da ket noi. Mo trinh duyet: http://");  // Dòng bắt đầu bằng # là ghi chú,
    Serial.println(WiFi.localIP());                              // không phải số liệu CSV
  }
}

// setup() chạy MỘT lần khi bo khởi động
void setup() {
  Serial.begin(115200);
  pinMode(CHAN_TAI, OUTPUT);
  pinMode(CHAN_COI, OUTPUT);
  digitalWrite(CHAN_TAI, LOW);  // Khởi động ở trạng thái an toàn: tải TẮT

  camBien.begin();
  camBien.setWaitForConversion(false);
  camBien.requestTemperatures();

  Wire.begin(CHAN_SDA, CHAN_SCL);
  lcd.init();
  lcd.backlight();
  inDong(0, "Hop bao quan");
  inDong(1, "Dang khoi dong..");

  // WiFi: chỉ ra lệnh kết nối rồi đi tiếp, KHÔNG đứng chờ.
  // Mất WiFi thì hộp vẫn phải giữ lạnh và cảnh báo bình thường!
  WiFi.mode(WIFI_STA);
  WiFi.begin(TEN_WIFI, MAT_KHAU_WIFI, 6);  // Kênh 6 giúp Wokwi kết nối nhanh hơn

  mayChu.on("/", guiTrangChu);         // Địa chỉ "/" -> trang web
  mayChu.on("/du-lieu", guiDuLieu);    // Địa chỉ "/du-lieu" -> số liệu dạng JSON
  mayChu.begin();

  Serial.println("# Bai 7: WiFi + web");
  Serial.println("giay,nhiet_do,peltier,canh_bao,so_lan_bat");  // Dòng tiêu đề CSV
  lanDoTruoc = millis();
}

// loop() lặp lại MÃI MÃI sau setup() - KHÔNG có delay()
void loop() {
  unsigned long bayGio = millis();

  if (bayGio - lanDoTruoc >= CHU_KY_DO) {
    lanDoTruoc = bayGio;
    doVaDieuKhien();
  }

  dieuKhienCoi(bayGio);
  mayChu.handleClient();  // Có ai đang mở trang web không? Có thì trả lời
  kiemTraWiFi();
}
