# Hộp bảo quản thực phẩm mini – Giám sát và cảnh báo nhiệt độ

Chương trình điều khiển cho đề tài KHKT THCS: hộp bảo quản thực phẩm mini dùng sò lạnh Peltier, tự giám sát nhiệt độ và cảnh báo khi thực phẩm có nguy cơ hỏng. Chạy thử trên trình mô phỏng Wokwi.

## Công nghệ

- **Vi điều khiển:** ESP32 (Arduino framework)
- **Công cụ:** VS Code, PlatformIO, Wokwi Simulator
- **Linh kiện:** DS18B20, LCD 1602 I2C, còi, MOSFET + Peltier TEC1-12706
- **Thư viện:** OneWire, DallasTemperature, LiquidCrystal_I2C, WiFi, WebServer

## Chức năng

- Đo nhiệt độ mỗi giây, hiển thị lên LCD
- Tự bật/tắt Peltier có độ trễ (bật ≥ 5,5 °C, tắt ≤ 4,5 °C), giữ mỗi trạng thái tối thiểu 30 giây
- Còi cảnh báo khi nhiệt độ > 8 °C hoặc khi cảm biến lỗi
- Tự tắt Peltier khi mất cảm biến (an toàn khi lỗi)
- Ghi log CSV qua Serial để vẽ đồ thị
- Xem nhiệt độ trên điện thoại qua WiFi
