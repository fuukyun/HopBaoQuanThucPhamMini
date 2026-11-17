# Chuẩn kiến thức – Hộp bảo quản thực phẩm mini di động

> Đề tài: **Nghiên cứu, thiết kế và chế tạo hộp bảo quản thực phẩm mini di động tích hợp giám sát và cảnh báo nhiệt độ**
> Cấu hình mẫu: hộp cách nhiệt 8–12 lít · Peltier TEC1-12706 · ESP32 · DS18B20 · LCD 1602 I2C · MOSFET DC · nguồn 12 V
>
> Cách dùng file này: học lần lượt từng chặng, tick `[x]` khi đã nắm được. Mỗi chặng có **"Chuẩn đạt"**: chỉ chuyển sang chặng sau khi làm được hết các yêu cầu trong phần đó.

---

## Mục đích và đối tượng phục vụ

### 1. Vấn đề cần giải quyết

Hộp giữ nhiệt thông thường (thùng xốp, thùng đá) chỉ **làm chậm** quá trình nóng lên. Nó không cho biết bên trong đang bao nhiêu độ và không báo khi thực phẩm bắt đầu nóng lên. Người dùng chỉ phát hiện khi đồ ăn đã hỏng.

### 2. Sản phẩm làm được gì

Một hộp nhỏ, xách tay được (khoảng 30 × 25 × 25 cm, 8–12 lít), có ba chức năng:

| Chức năng | Cách thực hiện |
|---|---|
| **Giữ mát** | Lớp cách nhiệt XPS/PU kết hợp sò lạnh Peltier |
| **Giám sát** | Cảm biến DS18B20 đo nhiệt độ, hiển thị liên tục trên LCD |
| **Cảnh báo** | Còi kêu khi nhiệt độ vượt 8 °C hoặc khi cảm biến hỏng |

### 3. Phục vụ ai, khi nào

- Đi dã ngoại, đi xa: mang đồ ăn, đồ uống, sữa cho trẻ nhỏ
- Giữ mát trong thời gian ngắn cho thuốc hoặc mẫu vật cần nhiệt độ thấp (chỉ khi đã kiểm chứng)
- Chở thực phẩm tươi quãng ngắn, ví dụ từ chợ về nhà
- Những nơi không có tủ lạnh nhưng có nguồn 12 V (ô tô, bộ nguồn DC)

> ⚠️ **Giới hạn:** đây là **mô hình thử nghiệm**, chưa phải thiết bị bảo quản thực phẩm đạt chuẩn. Không quảng cáo là thiết bị an toàn thực phẩm khi chưa kiểm chứng. Peltier có hiệu suất thấp, nên khả năng làm lạnh phụ thuộc vào nhiệt độ môi trường và khả năng tản nhiệt.

### 4. Mục đích giáo dục (cuộc thi KHKT cấp THCS)

Đề tài dành cho học sinh **lớp 8–9** dự thi **Khoa học Kỹ thuật**. Mục tiêu là vận dụng nhiều môn học cùng lúc:

| Lĩnh vực | Vận dụng |
|---|---|
| Vật lý | Truyền nhiệt, cách nhiệt, hiệu ứng Peltier, điện năng E = P × t |
| Điện tử | Cảm biến, MOSFET, nguồn DC, cầu chì |
| Tin học | Lập trình ESP32, điều khiển có độ trễ (hysteresis) |
| Công nghệ | Thiết kế, gia công, lắp ráp |
| Phương pháp khoa học | Giả thuyết, phân loại biến, thí nghiệm đối chứng, lặp lại 3 lần, xử lý số liệu |

> 💡 Cuộc thi KHKT chấm **quá trình nghiên cứu** chứ không chỉ sản phẩm. Học sinh phải đo, so sánh (hộp thường, hộp cách nhiệt, hộp cách nhiệt có Peltier) và kết luận từ **số liệu thật**, kể cả khi kết quả không như mong đợi.

### 5. Câu hỏi nghiên cứu

1. Vật liệu và độ dày cách nhiệt ảnh hưởng thế nào đến tốc độ tăng nhiệt?
2. Peltier giúp thay đổi nhiệt độ trong hộp ra sao trong điều kiện thử nghiệm?
3. Điều khiển có độ trễ có hạn chế được việc bật/tắt liên tục không?
4. Khối lượng và điện năng tiêu thụ có phù hợp với một sản phẩm di động không?

---

## Tiến độ tổng quan

| Chặng | Nội dung | Trạng thái |
|---|---|---|
| 1 | Vật lý: truyền nhiệt và nhiệt điện | ⬜ |
| 2 | Điện và điện tử cơ bản | ⬜ |
| 3 | Lập trình ESP32 | ⬜ |
| 4 | Ghép mạch điều khiển (chưa có Peltier) | ⬜ |
| 5 | Cơ khí và chế tạo hộp | ⬜ |
| 6 | Lắp Peltier (có người lớn kiểm tra) | ⬜ |
| 7 | Phương pháp nghiên cứu và thực nghiệm | ⬜ |
| 8 | Viết báo cáo và trình bày | ⬜ |
| ★ | An toàn (áp dụng xuyên suốt) | ⬜ |

Ký hiệu: ⬜ chưa học · 🟨 đang học · ✅ đạt chuẩn

---

## ★ Chuẩn an toàn (đọc trước tiên, áp dụng mọi chặng)

- [ ] Chỉ dùng **nguồn DC điện áp thấp** đã đóng kín, có bảo vệ quá dòng
- [ ] **Không** đấu điện lưới 220 V
- [ ] **Không** tự ghép cell pin lithium; nếu dùng pin thì chỉ dùng bộ pin thương mại có mạch bảo vệ
- [ ] **Không** cấp điện cho Peltier trực tiếp từ chân ESP32 hoặc qua relay nhỏ
- [ ] Tầng công suất Peltier (nguồn, MOSFET, cầu chì, dây) phải **được giáo viên/người lớn kiểm tra** trước khi cấp điện
- [ ] Tách khoang điện tử khỏi khoang thực phẩm, có biện pháp chống ẩm và chống nước ngưng
- [ ] Cẩn thận tản nhiệt mặt nóng (có thể gây bỏng) và dụng cụ cắt
- [ ] Dừng ngay nếu có mùi lạ, dây nóng bất thường hoặc quạt ngừng quay
- [ ] Không ăn thực phẩm dùng để thử nghiệm

**Chuẩn đạt:** Tự giải thích được vì sao có từng quy tắc trên.

---

## Chặng 1 – Vật lý: truyền nhiệt và nhiệt điện

### 1.1 Truyền nhiệt
- [ ] Ba hình thức truyền nhiệt: **dẫn nhiệt, đối lưu, bức xạ**, và mỗi hình thức có mặt ở đâu trong chiếc hộp
- [ ] Nhiệt lượng: Q = m·c·Δt (ý nghĩa của m, c, Δt)
- [ ] Vì sao mở nắp làm nhiệt độ trong hộp tăng nhanh (đối lưu, trao đổi không khí)
- [ ] Vì sao thực phẩm đặt vào (tải nhiệt) làm hộp lâu lạnh hơn

### 1.2 Vật liệu cách nhiệt
- [ ] Khái niệm **độ dẫn nhiệt**: vật dẫn nhiệt tốt và vật cách nhiệt
- [ ] So sánh XPS, PU, xốp EPS thường, không khí
- [ ] Ảnh hưởng của **độ dày lớp cách nhiệt** (khảo sát 20–40 mm)
- [ ] **Khe hở** và **cầu nhiệt** (vít, khung kim loại xuyên qua lớp cách nhiệt) làm giảm hiệu quả như thế nào
- [ ] Vai trò của gioăng kín ở nắp

### 1.3 Hiệu ứng Peltier
- [ ] Nguyên lý: dòng điện qua sò làm một mặt lạnh, một mặt nóng
- [ ] Vì sao **mặt nóng phải được tản nhiệt tốt**, nếu không mặt lạnh cũng không lạnh
- [ ] Peltier có hiệu suất thấp hơn tủ lạnh dùng máy nén
- [ ] Công suất làm lạnh giảm khi chênh lệch nhiệt độ giữa hai mặt tăng
- [ ] Đọc các thông số chính của TEC1-12706: Umax, Imax, Qmax, ΔTmax

### 1.4 Ngưng tụ
- [ ] Hơi nước gặp mặt lạnh sẽ đọng thành nước (điểm sương)
- [ ] Hệ quả: cần khay hứng nước và cách ly phần điện

**Chuẩn đạt:**
- [ ] Vẽ được sơ đồ chiếc hộp, chỉ ra các đường nhiệt đi vào
- [ ] Giải thích được bằng lời vì sao "chỉ cách nhiệt" khác với "cách nhiệt + Peltier"

---

## Chặng 2 – Điện và điện tử cơ bản

### 2.1 Đại lượng điện
- [ ] Điện áp U (V), cường độ dòng điện I (A), điện trở R (Ω)
- [ ] Định luật Ôm: U = I·R
- [ ] Công suất P = U·I, ví dụ Peltier 12 V × ~6 A ≈ 72 W
- [ ] Điện năng E = P·t (Wh), và vì sao khi tải bật/tắt **không** được lấy công suất danh định nhân với toàn bộ thời gian
- [ ] Mạch nối tiếp và mạch song song

### 2.2 Linh kiện
- [ ] **Điện trở kéo lên** (pull-up 4,7 kΩ cho DS18B20) dùng để làm gì
- [ ] **MOSFET** dùng như công tắc điện tử: chân điều khiển và chân tải; vì sao chọn MOSFET logic-level
- [ ] **Cầu chì DC**: chọn định mức theo dòng tải, có dự phòng
- [ ] **Mạch hạ áp DC-DC (buck)**: hạ 12 V xuống 5 V/3,3 V cho ESP32
- [ ] **Diode chống xung ngược** cho tải cảm (quạt, động cơ)
- [ ] Còi chủ động và còi thụ động khác nhau thế nào
- [ ] **Tiết diện dây** phải phù hợp với dòng điện

### 2.3 Mức logic
- [ ] ESP32 hoạt động ở **3,3 V**, chân GPIO không chịu được 5 V
- [ ] Vấn đề module LCD I2C kéo SDA/SCL lên 5 V, và cách xử lý (chuyển mức logic, bỏ trở kéo)
- [ ] Giới hạn dòng điện của một chân GPIO

### 2.4 Kỹ năng
- [ ] Đọc **datasheet** để tìm điện áp, dòng điện định mức và sơ đồ chân
- [ ] Dùng **đồng hồ vạn năng** đo điện áp, dòng điện, thông mạch
- [ ] Đọc sơ đồ nguyên lý và sơ đồ khối
- [ ] Sử dụng breadboard, dây cắm, hàn cơ bản (có giám sát)

**Chuẩn đạt:**
- [ ] Tự tính được dòng tổng và công suất tổng của hệ (Peltier + 2 quạt + ESP32 + LCD), từ đó chọn được nguồn và cầu chì
- [ ] Vẽ lại được sơ đồ đấu nối ở mục 5 của hồ sơ và giải thích từng đường dây

---

## Chặng 3 – Lập trình ESP32

### 3.1 Môi trường
- [ ] Cài Arduino IDE, cài board ESP32
- [ ] Cài thư viện: `OneWire`, `DallasTemperature`, `LiquidCrystal_I2C`
- [ ] Nạp chương trình, mở Serial Monitor (115200 baud)

### 3.2 C/C++ cơ bản
- [ ] Biến và kiểu dữ liệu: `int`, `float`, `bool`, `unsigned long`
- [ ] Hằng số: `const`, `#define`
- [ ] Câu lệnh điều kiện `if / else`, toán tử `&&`, `||`, `!`, toán tử ba ngôi `? :`
- [ ] Cấu trúc chương trình: `setup()` và `loop()`
- [ ] Dùng `millis()` để định thời thay cho `delay()`

### 3.3 Vào/ra và giao tiếp
- [ ] `pinMode`, `digitalWrite`
- [ ] **OneWire**: đọc nhiệt độ từ DS18B20 (`requestTemperatures`, `getTempCByIndex`)
- [ ] **I2C**: quét địa chỉ LCD (0x27 hoặc 0x3F), `setCursor`, `print`
- [ ] In dữ liệu ra Serial để ghi log

### 3.4 Logic điều khiển
- [ ] **Điều khiển ON/OFF có độ trễ (hysteresis)**: bật ≥ 5,5 °C, tắt ≤ 4,5 °C; vì sao không dùng một ngưỡng duy nhất
- [ ] Ngưỡng cảnh báo > 8 °C
- [ ] Phát hiện **lỗi cảm biến** (`DEVICE_DISCONNECTED_C`, giá trị vô lý) và khi đó phải tắt tải
- [ ] Nguyên tắc "an toàn khi lỗi": khi có lỗi thì tắt tải

### 3.5 Cải tiến (nâng cao, tùy chọn)
- [ ] Tránh nhấp nháy LCD: ghi đè bằng khoảng trắng thay cho `lcd.clear()` mỗi chu kỳ
- [ ] Cho còi kêu ngắt quãng thay cho kêu liên tục
- [ ] Thời gian bật/tắt tối thiểu để bảo vệ Peltier
- [ ] Ghi log nhiệt độ qua Serial sang file CSV để vẽ đồ thị
- [ ] Gửi dữ liệu lên điện thoại qua WiFi hoặc Bluetooth

**Chuẩn đạt:**
- [ ] Giải thích được từng dòng của chương trình mẫu ở mục 6
- [ ] Tự viết lại được phần hysteresis mà không nhìn code mẫu

---

## Chặng 4 – Ghép mạch điều khiển (chưa có Peltier)

Làm theo thứ tự, mỗi bước chạy được rồi mới làm bước tiếp theo:

- [ ] 4.1 ESP32 nhấp nháy đèn LED
- [ ] 4.2 Đọc DS18B20 và in nhiệt độ ra Serial
- [ ] 4.3 So sánh số đo với **nhiệt kế tham chiếu**, ghi lại sai số
- [ ] 4.4 Hiển thị nhiệt độ lên LCD
- [ ] 4.5 Dùng LED trên GPIO 25 thay cho Peltier, kiểm tra logic hysteresis (làm ấm hoặc làm lạnh cảm biến bằng tay hoặc cốc nước)
- [ ] 4.6 Kiểm tra còi cảnh báo trên GPIO 26
- [ ] 4.7 Rút dây cảm biến, xác nhận báo "LOI CAM BIEN" và tải bị tắt

**Chuẩn đạt:**
- [ ] Toàn bộ phần điều khiển chạy đúng với tải giả (LED)

---

## Chặng 5 – Cơ khí và chế tạo hộp

- [ ] Đọc và vẽ **bản vẽ có kích thước** (hình chiếu, mặt cắt)
- [ ] Tính dung tích hữu ích từ kích thước trong (mục tiêu 8–12 lít)
- [ ] Bố trí: khoang thực phẩm, cụm Peltier xuyên vách, khoang điện tử riêng, khay nước ngưng
- [ ] Cắt và ghép tấm cách nhiệt, xử lý kín khe hở, chờ keo đóng rắn
- [ ] Lớp lót khoang dễ vệ sinh; thực phẩm không tiếp xúc trực tiếp với xốp hay keo
- [ ] Lắp nắp có gioăng, bản lề, chốt
- [ ] Vị trí cảm biến **đại diện** cho nhiệt độ khoang: không chạm tấm lạnh, không đặt ngay trong luồng gió
- [ ] Phía nóng và quạt đặt ngoài hộp, thông thoáng, có lưới bảo vệ
- [ ] Đi dây gọn, cố định chống tuột; hoàn thiện tay xách và cạnh bảo vệ

**Chuẩn đạt:**
- [ ] Có bản vẽ hoàn chỉnh trước khi gia công
- [ ] Đo được nhiệt độ trong hộp khi **chỉ có cách nhiệt** (chưa lắp Peltier)

---

## Chặng 6 – Lắp Peltier (bắt buộc có người lớn kiểm tra)

- [ ] Xác định đúng **mặt nóng/mặt lạnh** của sò
- [ ] Lắp tản nhiệt hai mặt và keo tản nhiệt theo hướng dẫn nhà sản xuất
- [ ] Cách nhiệt kín quanh mép cụm Peltier
- [ ] Chọn MOSFET, dây, đầu nối, cầu chì theo dòng thực tế, có dự phòng
- [ ] Nguồn 12 V đủ dòng, có giới hạn dòng
- [ ] Ngắt nguồn, kiểm tra cực tính, thông mạch và nguy cơ chạm chập
- [ ] Giáo viên kiểm tra và đồng ý trước khi cấp điện lần đầu
- [ ] Chạy thử có giám sát: theo dõi nhiệt độ, dòng điện, nhiệt độ tản nhiệt, quạt, nước ngưng

**Chuẩn đạt:**
- [ ] Hệ chạy liên tục ít nhất 30 phút có giám sát, không có dấu hiệu bất thường

---

## Chặng 7 – Phương pháp nghiên cứu và thực nghiệm

### 7.1 Khái niệm
- [ ] Câu hỏi nghiên cứu và **giả thuyết**
- [ ] **Biến độc lập**: loại/độ dày cách nhiệt, bật/tắt Peltier, số lần mở nắp
- [ ] **Biến phụ thuộc**: nhiệt độ theo thời gian, thời gian đạt ngưỡng, điện năng, khối lượng
- [ ] **Biến kiểm soát**: dung tích, lượng nước thử, nhiệt độ ban đầu và môi trường, vị trí cảm biến, lịch mở nắp
- [ ] **Thí nghiệm đối chứng**: chỉ thay đổi một yếu tố mỗi lần
- [ ] Lặp lại **ít nhất 3 lần**, lấy trung bình; ghi cả kết quả không thuận lợi

### 7.2 Bốn thí nghiệm
- [ ] **A**: so sánh vật liệu và độ dày cách nhiệt (đo mỗi 5–10 phút, ΔT = T cuối − T đầu)
- [ ] **B**: chỉ cách nhiệt so với cách nhiệt + Peltier (đồ thị nhiệt độ và điện năng)
- [ ] **C**: ảnh hưởng của việc mở nắp (ví dụ 10 giây mỗi 15 phút)
- [ ] **D**: kiểm tra ngưỡng bật/tắt và còi cảnh báo

### 7.3 Xử lý số liệu
- [ ] Ghi bảng số liệu (theo mẫu ở mục 9 của hồ sơ)
- [ ] Tính trung bình, ΔT, chênh lệch giữa các lần đo
- [ ] Vẽ **đồ thị nhiệt độ theo thời gian** bằng Excel hoặc Google Sheets
- [ ] Đo điện năng bằng đồng hồ đo DC
- [ ] Nhận xét nguyên nhân sai số

**Chuẩn đạt:**
- [ ] Đủ số liệu cho 4 thí nghiệm, mỗi thí nghiệm 3 lần, có đồ thị

---

## Chặng 8 – Viết báo cáo và trình bày

- [ ] Cấu trúc báo cáo: tóm tắt, lý do, mục tiêu, giả thuyết, phương pháp, kết quả, thảo luận, kết luận, hướng phát triển
- [ ] Kết luận **dựa trên số liệu thật**, không phóng đại
- [ ] Nêu rõ giới hạn: không tuyên bố đạt chuẩn bảo quản thực phẩm khi chưa kiểm chứng
- [ ] Ảnh minh họa quá trình chế tạo và sản phẩm
- [ ] Poster hoặc slide trình bày
- [ ] Luyện trả lời câu hỏi của giám khảo (vì sao chọn Peltier, vì sao có hysteresis, sai số từ đâu, làm sao cải tiến...)

**Chuẩn đạt:**
- [ ] Trình bày trôi chảy trong 5–7 phút và trả lời được câu hỏi về mọi phần của sản phẩm

---

## Ghi chú học tập

> Ghi lại thắc mắc, link tài liệu và kết quả thử nghiệm ở đây.

| Ngày | Chặng | Nội dung đã học / làm | Vướng mắc |
|---|---|---|---|
| | | | |
