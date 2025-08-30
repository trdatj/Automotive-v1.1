
# Xây dựng mô hình Instrument Cluster, tích hợp giao tiếp dữ liệu thời gian thực với vi điều khiển


## 1. Công thức tính chiều rộng của ROW

Tổng chiều rộng của `mainRow` phải bằng tổng chiều rộng của tất cả các item con và tất cả các khoảng trống.

`Tổng chiều rộng mainRow = Chiều rộng panel 1 + spacing + Chiều rộng panel 2 + spacing + Chiều rộng panel 3`

=> `Tổng chiều rộng mainRow = 3 * Chiều rộng một panel + 2 * spacing`

=> `3 * Chiều rộng một panel = Tổng chiều rộng mainRow - 2 * spacing`

=> `Chiều rộng một panel` = (`Tổng chiều rộng mainRow` - `2 * spacing`) / 3

### 2. Ánh xạ giá trị biến trở (0-4095) sang dải góc quay của đồng hồ
Công thức ánh xạ tuyến tính:
mappedAngle = minAngle + ( (giá_trị_hiện_tại - min_giá_trị_ADC) / (max_giá_trị_ADC - min_giá_trị_ADC) ) * (max_góc - min_góc)
`var mappedAngle = minAngle + (potValue / 4095.0) * (maxAngle - minAngle);`