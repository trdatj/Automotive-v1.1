
# Hướng Dẫn Kỹ Thuật Dự Án "Xây dựng mô hình Instrument Cluster tích hợp giao tiếp dữ liệu thời gian thực với vi điều khiển"

Tài liệu này cung cấp hướng dẫn chi tiết về cách thiết lập, biên dịch và chạy dự án "Xây dựng mô hình Instrument Cluster tích hợp giao tiếp dữ liệu thời gian thực với vi điều khiển", bao gồm cả việc cấu hình môi trường và giao tiếp với các thiết bị ngoại vi, cũng như vận hành hệ thống thị giác máy tính.


## 1. Tổng quan dự án

Dự án là một Hệ thống Giao diện Người-Máy (HMI) cho cụm đồng hồ ô tô, được xây dựng bằng Qt/QML và C++. Hệ thống này không chỉ hiển thị các thông số cơ bản của xe như tốc độ và vòng tua máy mà còn tích hợp các chức năng nâng cao như nhận diện biển báo tốc độ từ một hệ thống thị giác máy tính riêng biệt.
## 2. Các thành phần chính

Dự án bao gồm ba phần chính:

- HMI (Qt/QML & C++): Giao diện người dùng đồ họa hiển thị các thông số xe.

- Hệ thống Thị giác máy tính (Python): Nhận diện biển báo giao thông và gửi dữ liệu qua mạng.

- Hệ thống Điều khiển (ESP32): Mô phỏng ECU của xe, nhận lệnh từ HMI và điều khiển phần cứng thực tế.
## 3. Các thành phần và Giao thức truyền thông

Dự án này hoạt động dựa trên ba thành phần chính giao tiếp với nhau qua hai giao thức:

### 3.1 HMI (Qt/QML & C++)

- Chức năng: Hiển thị đồng hồ tốc độ, vòng tua, đèn báo hiệu, và các thông tin khác. Nhận dữ liệu từ ESP32 qua Serial và từ hệ thống thị giác qua TCP.

- Mã nguồn:

    - main.qml, Speedometer.qml, FuncIcon.qml, TempIcon.qml, StartWindows.qml: Các file QML xây dựng giao diện.

    - SerialManager.cpp, SerialManager.h: Module C++ quản lý kết nối Serial.

    - NetworkManager.cpp, NetworkManager.h: Module C++ tạo máy chủ TCP.

### 3.2 Hệ thống Điều khiển (ESP32)

- Chức năng: Mô phỏng một ECU (Electronic Control Unit). Điều khiển các đèn LED mô phỏng đèn xi nhan, đèn pha, và đọc dữ liệu từ các cảm biến (chiết áp, DHT11) để gửi về HMI.

- Mã nguồn: `ECU.ino` (mã Arduino cho ESP32).

- Giao tiếp: Gửi dữ liệu về HMI qua cổng Serial.

- `POT_VAL: <giá_trị>`: Giá trị từ chiết áp, dùng để cập nhật tốc độ và vòng tua.

- `Temperature: <giá_trị>`: Giá trị nhiệt độ từ cảm biến DHT11.

- `TURN_LEFT:ON`, `HAZARD:ON`, v.v.: Cập nhật trạng thái đèn báo hiệu khi các nút bấm vật lý được nhấn.

- `Car:Started!`: Tín hiệu khởi động xe.

### 3.3 Hệ thống Thị giác máy tính (Python)

- Chức năng: Sử dụng camera và mô hình học máy (TFLite) để nhận diện các biển báo tốc độ (40, 50, 60).

- Mã nguồn: rasp_demo.py.

- Giao tiếp: Gửi dữ liệu về HMI qua mạng TCP.

    - `SPEED_LIMIT:<value>`: Thông điệp chứa giá trị tốc độ đã nhận diện.
## 4. Hướng dẫn thiết lập chi tiết

### 4.1 Thiết lập HMI (Qt)

1. Cài đặt: Cài đặt Qt 5.15 trở lên và Qt Creator. Đảm bảo đã chọn các module cần thiết như Qt Quick và Qt SerialPort trong quá trình cài đặt.

2. Mở dự án: Mở file .pro của dự án bằng Qt Creator.

3. Cấu hình Serial: Mở file SerialManager.cpp. Thay đổi tên cổng serial trong hàm start() thành cổng của ESP32 (ví dụ: COM8 trên Windows hoặc /dev/ttyUSB0 trên Linux).

4. Chạy: Biên dịch và chạy dự án. Ứng dụng HMI sẽ khởi chạy và tự động lắng nghe kết nối serial và TCP.

### 4.2 Thiết lập Hệ thống Điều khiển (ESP32)

1. Phần cứng: Kết nối ESP32 với các linh kiện sau:

- Nút bấm: Kết nối các nút bấm vào các chân I/O được định nghĩa trong ECU.ino (BTN_START, BTN_DEN, BTN_PHA, v.v.).

- Đèn LED: Kết nối các đèn LED với các chân điều khiển (LED_XINHANTRAI, LED_DEN, v.v.).

- Chiết áp: Kết nối chiết áp vào chân BIENTRO.

- Cảm biến DHT11: Kết nối cảm biến vào chân DHT11_PIN.

2. Lập trình: Mở `ECU.ino` bằng Arduino IDE. Đảm bảo đã cài đặt các thư viện cần thiết như `DHT` và `RTClib`.

3. Nạp chương trình: Chọn đúng Board ESP32 Dev Module và cổng COM, sau đó nạp chương trình vào ESP32.

4. Vận hành: Sau khi nạp, ESP32 sẽ bắt đầu gửi dữ liệu cảm biến và trạng thái nút bấm về máy tính qua cổng serial.

### 4.3 Thiết lập Hệ thống Thị giác (Raspberry Pi)

1. Phần cứng: Cần một Raspberry Pi và một camera tương thích (picamera2).

2. Cài đặt:

- Cài đặt các thư viện Python: `pip install picamera2 opencv-python numpy tflite_runtime`.

- Tải mô hình TFLite (`traffic_sign_mobilenetv2_quant.tflite`) và đặt cùng thư mục với script.

3. Cấu hình mạng: Mở file `rasp_demo.py`. Thay đổi địa chỉ IP của biến `HOST` thành địa chỉ IP của máy tính đang chạy HMI. Cổng mặc định là `65432`.

4. Chạy script:

Bash

`python3 rasp_demo.py`

Script sẽ kết nối với HMI và bắt đầu nhận diện biển báo từ camera. Khi phát hiện, nó sẽ gửi chuỗi SPEED_LIMIT:<value> qua mạng TCP tới HMI.