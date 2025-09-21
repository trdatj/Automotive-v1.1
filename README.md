
# Xây dựng mô hình Instrument Cluster tích hợp giao tiếp dữ liệu thời gian thực với vi điều khiển

Dự án này là một Giao diện Human Machine Interface (HMI) cho cụm đồng hồ ô tô, được phát triển bằng Qt/QML và C++. Nó được thiết kế để hiển thị dữ liệu xe theo thời gian thực và cho phép người dùng tương tác. Hệ thống giao tiếp với một bộ điều khiển chính qua cổng nối tiếp (serial) và với một hệ thống thị giác máy tính qua một máy chủ TCP.


## Tính năng

- Đồng hồ tốc độ và vòng tua máy: Hai đồng hồ kim để hiển thị tốc độ và RPM của xe. Các đồng hồ này có hiệu ứng kim quay động.

- Đèn xi nhan và đèn khẩn cấp: Các biểu tượng tương tác cho đèn xi nhan trái, phải và đèn khẩn cấp. Các biểu tượng nhấp nháy khi được kích hoạt. Lệnh từ người dùng khi nhấp vào các biểu tượng sẽ được gửi tới thiết bị đã kết nối. Hệ thống cũng xử lý các tín hiệu đến để cập nhật trạng thái giao diện người dùng.

- Hiển thị nhiệt độ: Hiển thị các biểu tượng cho nhiệt độ khoang cabin và nước làm mát. Giao diện của các biểu tượng sẽ thay đổi (ví dụ: màu sắc, hình ảnh nguồn) dựa trên trạng thái nhiệt độ (Bình thường, Cảnh báo, Nguy hiểm).

- Điều khiển đèn chiếu sáng: Các biểu tượng tương tác để bật/tắt đèn cốt (cos) và đèn pha (pha).

- Nhận diện giới hạn tốc độ: Tích hợp với một hệ thống thị giác máy tính qua kết nối TCP để hiển thị biển báo giới hạn tốc độ đã được nhận dạng trên bảng điều khiển.

- Chức năng khởi động/dừng: Màn hình khởi động với nút bấm để bắt đầu hiển thị bảng điều khiển chính, mô phỏng việc khởi động xe. Giao diện cũng có thể được khởi động hoặc dừng bởi một tín hiệu từ hệ thống xe đã kết nối.


## Công nghệ sử dụng

**Giao diện:** Qt/QML, C++

**Phần cứng:** Altium

**Module điều khiển:** C++, Arduino IDE

**Thị giác máy tính:** Mobinet v2, bài toán phân loại
## Cấu trúc dự án

- main.qml: File ứng dụng chính. Nó định nghĩa cửa sổ chính, quản lý các thành phần giao diện người dùng và xử lý giao tiếp với các đối tượng C++ backend thông qua Connections.

- Speedometer.qml: Một component QML có thể tái sử dụng để tạo các đồng hồ tốc độ và vòng tua máy động.

- FuncIcon.qml: Một component QML có thể tái sử dụng để tạo các biểu tượng chức năng tương tác (ví dụ: đèn pha, đèn xi nhan). Nó hỗ trợ các trạng thái checked và blinking.

- TempIcon.qml: Một component QML có thể tái sử dụng để hiển thị các biểu tượng nhiệt độ thay đổi giao diện dựa trên thuộc tính status.

- StartWindows.qml: Màn hình khởi động với một nút có hoạt ảnh để bắt đầu màn hình hiển thị chính.

- SerialManager.h & SerialManager.cpp: Module C++ chịu trách nhiệm giao tiếp nối tiếp. Nó thiết lập cổng, đọc dữ liệu đến và gửi lệnh.

- NetworkManager.h & NetworkManager.cpp: Module C++ để giao tiếp TCP. Nó hoạt động như một máy chủ, lắng nghe các kết nối đến và xử lý dữ liệu từ một client.


## Giao thức truyền thông

### Giao tiếp nối tiếp (C++ <-> ESP32)

Hệ thống sử dụng cổng nối tiếp (COM_X với X cổng sử dụng cho module USB to serial) để giao tiếp với vi điều khiển.

- Định dạng dữ liệu: Dữ liệu là một chuỗi có định dạng DEVICE:STATUS. Mỗi thông điệp kết thúc bằng một ký tự xuống dòng (\n).

- Dữ liệu gửi đi (tới ESP32):

    - TURN_LEFT:ON / TURN_LEFT:OFF

    - TURN_RIGHT:ON / TURN_RIGHT:OFF

    - HAZARD:ON / HAZARD:OFF

- Dữ liệu nhận được (từ ESP32):

    - Car:Started! / Car:Stopped!

    - TURN_LEFT:ON / TURN_LEFT:OFF

    - TURN_RIGHT:ON / TURN_RIGHT:OFF

    - HAZARD:ON / HAZARD:OFF

    - DEN_COS:ON / DEN_COS:OFF

    - DEN_PHA:ON / DEN_PHA:OFF

    - POT_VAL: <giá_trị> (giá trị là số nguyên từ 0-4095, dùng để điều khiển kim tốc độ và RPM)

    - Temperature: <giá_trị> (giá trị là số thực biểu thị nhiệt độ khoang cabin)

### Giao tiếp TCP (C++ <-> Hệ thống thị giác máy tính)

Hệ thống chạy một máy chủ TCP trên cổng 65432 để nhận thông tin giới hạn tốc độ từ một ứng dụng client.

- Định dạng dữ liệu: Một chuỗi có định dạng SPEED_LIMIT:<giá_trị> theo sau là một ký tự xuống dòng.

- Dữ liệu nhận được:

    - SPEED_LIMIT:40

    - SPEED_LIMIT:50

    - SPEED_LIMIT:60

Hệ thống sau đó sẽ cập nhật biển báo giới hạn tốc độ tương ứng trên bảng điều khiển.