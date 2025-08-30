bool last_btn_den = HIGH;
bool last_btn_pha = HIGH;
bool last_btn_haza = HIGH;
bool last_btn_trai = HIGH;
bool last_btn_phai = HIGH;
bool last_btn_start = HIGH;

int led_send = 13;     // LED báo hiệu đang gửi dữ liệu (TX)
int led_receive = 14;  // LED báo hiệu đã nhận lệnh (RX)
int led_den = 25;
int led_pha = 32;
int led_xiNhanTrai = 26;
int led_xiNhanPhai = 33;

const int button_den = 2;
const int button_pha = 4;
const int button_haza = 16;
const int button_xinnhantrai = 17;
const int button_xinnhanphai = 5;
const int button_start = 18;
const int POT_PIN = 35;

int gtcambien;
int last_gtcambien = -1;

bool wasLeftBlinkingBeforeHazard = false;
bool wasRightBlinkingBeforeHazard = false;

bool state_den = false;
bool state_pha = false;
bool state_haza = false;
bool state_xinhantrai = false;
bool state_xinhanphai = false;
bool state_engine_running = false;

bool hazaBlinkState = false;
bool traiBlinkState = false;
bool phaiBlinkState = false;

TaskHandle_t TaskReadButtons;
TaskHandle_t TaskReadCMD;
TaskHandle_t TaskReadAnalog;
TaskHandle_t TaskControlLights;

//Task để đọc các lệnh từ Serial
void readCMDTask(void *pvParameters) {
  while (1) {
    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim();  // Loại bỏ khoảng trắng ở đầu và cuối chuỗi

      if (cmd == "TURN_LEFT:ON") {
        state_xinhantrai = true;
        state_xinhanphai = false;
        // state_haza = false;
      } else if (cmd == "TURN_LEFT:OFF") {
        state_xinhantrai = false;
      } else if (cmd == "TURN_RIGHT:ON") {
        state_xinhanphai = true;
        state_xinhantrai = false;
        // state_haza = false;
      } else if (cmd == "TURN_RIGHT:OFF") {
        state_xinhanphai = false;
      } else if (cmd == "HAZARD:ON") {
        wasLeftBlinkingBeforeHazard = state_xinhantrai;
        wasRightBlinkingBeforeHazard = state_xinhanphai;

        state_haza = true;
        state_xinhantrai = false;
        state_xinhanphai = false;
      } else if (cmd == "HAZARD:OFF") {
        state_haza = false;
        
        if (wasLeftBlinkingBeforeHazard) {
          state_xinhantrai = true;
          Serial.println("TURN_LEFT:ON");
        }
        if (wasRightBlinkingBeforeHazard) {
          state_xinhanphai = true;
          Serial.println("TURN_RIGHT:ON");
        }
        wasLeftBlinkingBeforeHazard = false;
        wasRightBlinkingBeforeHazard = false;
      } else if (cmd == "DEN_COS:ON") {
        state_den = true;
      } else if (cmd == "DEN_COS:OFF") {
        state_den = false;
        state_pha = false;  // Tắt đèn pha nếu đèn cos tắt
      } else if (cmd == "DEN_PHA:ON") {
        // Chỉ bật pha nếu cos đang bật
        if (state_den) state_pha = true;
      } else if (cmd == "DEN_PHA:OFF") {
        state_pha = false;
      } else if (cmd == "ENGINE:START") {
        state_engine_running = true;
        Serial.println("ENGINE_STATUS:RUNNING");  // Báo trạng thái động cơ
      } else if (cmd == "ENGINE:STOP") {
        state_engine_running = false;
        Serial.println("ENGINE_STATUS:STOPPED");  // Báo trạng thái động cơ
      }

      Serial.println(cmd);
      digitalWrite(led_receive, HIGH);  // Báo hiệu đã nhận lệnh
      vTaskDelay(pdMS_TO_TICKS(50));
      digitalWrite(led_receive, LOW);
    }
    vTaskDelay(pdMS_TO_TICKS(50));  // Tránh task chạy quá nhanh
  }
}

// Task để đọc trạng thái các nút nhấn vật lý và gửi qua Serial
void readButtonTask(void *pvParameters) {
  while (1) {
    bool current_den = digitalRead(button_den);
    bool current_pha = digitalRead(button_pha);
    bool current_haza = digitalRead(button_haza);
    bool current_trai = digitalRead(button_xinnhantrai);
    bool current_phai = digitalRead(button_xinnhanphai);
    bool current_button_start = digitalRead(button_start);

    // Xử lý nút Đèn Cos
    if (last_btn_den == HIGH && current_den == LOW) {
      state_den = !state_den;
      Serial.print("DEN_COS:");
      Serial.println(state_den ? "ON" : "OFF");
      if (!state_den) state_pha = false;  // Tắt pha nếu cos tắt
    }
    // Xử lý nút Đèn Pha
    if (last_btn_pha == HIGH && current_pha == LOW) {
      if (state_den) {  // Chỉ bật pha nếu cos đang bật
        state_pha = !state_pha;
        Serial.print("DEN_PHA:");
        Serial.println(state_pha ? "ON" : "OFF");
      } else {
        Serial.println("CANNOT_PHA:COS_OFF");
      }
    }
    // Xử lý nút Hazard
    if (last_btn_haza == HIGH && current_haza == LOW) {
      state_haza = !state_haza;
      Serial.print("HAZARD:");
      Serial.println(state_haza ? "ON" : "OFF");
      if (state_haza) {
        wasLeftBlinkingBeforeHazard = state_xinhantrai;
        wasRightBlinkingBeforeHazard = state_xinhanphai;
        state_xinhantrai = false;
        state_xinhanphai = false;
      } else {
        if (wasLeftBlinkingBeforeHazard) {
          state_xinhantrai = true;
        }
        if (wasRightBlinkingBeforeHazard) {
          state_xinhanphai = true;
        }
        wasLeftBlinkingBeforeHazard = false;
        wasRightBlinkingBeforeHazard = false;
      }
    }
    // Xử lý nút Xi nhan trái
    if (last_btn_trai == HIGH && current_trai == LOW) {
      if (!state_haza) {  // Chỉ xử lý nếu hazard không bật
        state_xinhantrai = !state_xinhantrai;
        Serial.print("TURN_LEFT:");
        Serial.println(state_xinhantrai ? "ON" : "OFF");
        if (state_xinhantrai) {
          state_xinhanphai = false;  // Tắt xi nhan phải nếu bật trái
          Serial.println("TURN_RIGHT:OFF");
        }
      } else {
        Serial.println("CANNOT_TURN_LEFT:HAZARD_ON");
      }
    }
    // Xử lý nút Xi nhan phải
    if (last_btn_phai == HIGH && current_phai == LOW) {
      if (!state_haza) {  // Chỉ xử lý nếu hazard không bật
        state_xinhanphai = !state_xinhanphai;
        Serial.print("TURN_RIGHT:");
        Serial.println(state_xinhanphai ? "ON" : "OFF");
        if (state_xinhanphai) {
          state_xinhantrai = false;  // Tắt xi nhan trái nếu bật phải
          Serial.println("TURN_LEFT:OFF");
        }
      } else {
        Serial.println("CANNOT_TURN_RIGHT:HAZARD_ON");
      }
    }
    // Xử lý nút Start/Stop Engine
    if (last_btn_start == HIGH && current_button_start == LOW) {
      state_engine_running = !state_engine_running;
      Serial.print("ENGINE_STATUS:");
      Serial.println(state_engine_running ? "RUNNING" : "STOPPED");
    }

    // Cập nhật trạng thái cuối cùng của các nút
    last_btn_den = current_den;    // Đã giữ lại tên biến gốc
    last_btn_pha = current_pha;    // Đã giữ lại tên biến gốc
    last_btn_haza = current_haza;  // Đã giữ lại tên biến gốc
    last_btn_trai = current_trai;  // Đã giữ lại tên biến gốc
    last_btn_phai = current_phai;  // Đã giữ lại tên biến gốc
    last_btn_start = current_button_start;

    vTaskDelay(pdMS_TO_TICKS(50));  // Tạm dừng 50ms để chống nhiễu
  }
}

// Task để đọc giá trị analog từ biến trở (cho tốc độ)
void readPotentiometerTask(void *pvParameters) {
  const int NUM_READINGS = 10;  // Số lượng lần đọc để lấy trung bình
  int readings[NUM_READINGS];   // Mảng để lưu trữ các lần đọc
  int readIndex = 0;            // Vị trí hiện tại trong mảng
  long total = 0;               // Tổng các giá trị đọc
  int averageReading = 0;       // Giá trị trung bình

  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = 0;
  }

  while (1) {
    total = total - readings[readIndex];
    readings[readIndex] = analogRead(POT_PIN);
    total = total + readings[readIndex];
    readIndex = (readIndex + 1) % NUM_READINGS;

    averageReading = total / NUM_READINGS;
    gtcambien = averageReading;

    if (gtcambien != last_gtcambien) {
      Serial.print("POT_VAL:");
      Serial.println(gtcambien);
      digitalWrite(led_send, HIGH);
      vTaskDelay(pdMS_TO_TICKS(50));
      digitalWrite(led_send, LOW);

      last_gtcambien = gtcambien;
    }

    vTaskDelay(pdMS_TO_TICKS(100));  // Đọc mỗi 100ms
  }
}

// Task điều khiển đèn vật lý (cos/pha/xi nhan/hazard) dựa trên trạng thái
void controlLightsTask(void *pvParameters) {
  while (1) {
    // Điều khiển đèn cos và pha
    digitalWrite(led_den, state_den ? HIGH : LOW);
    digitalWrite(led_pha, state_pha ? HIGH : LOW);

    // Điều khiển xi nhan và hazard
    if (state_haza) {  // Nếu hazard đang bật
      hazaBlinkState = !hazaBlinkState;
      digitalWrite(led_xiNhanTrai, hazaBlinkState ? HIGH : LOW);
      digitalWrite(led_xiNhanPhai, hazaBlinkState ? HIGH : LOW);
    } else {  // Nếu hazard tắt, xử lý xi nhan riêng lẻ
      // Kiểm tra xem xi nhan trái có đang bật không và nhấp nháy
      if (state_xinhantrai) {
        traiBlinkState = !traiBlinkState;  // Đảo trạng thái nhấp nháy
        digitalWrite(led_xiNhanTrai, traiBlinkState ? HIGH : LOW);
      } else {
        digitalWrite(led_xiNhanTrai, LOW);  // Đảm bảo tắt LED nếu xi nhan tắt
        traiBlinkState = false;             // Reset trạng thái nhấp nháy
      }

      // Kiểm tra xem xi nhan phải có đang bật không và nhấp nháy
      if (state_xinhanphai) {
        phaiBlinkState = !phaiBlinkState;  // Đảo trạng thái nhấp nháy
        digitalWrite(led_xiNhanPhai, phaiBlinkState ? HIGH : LOW);
      } else {
        digitalWrite(led_xiNhanPhai, LOW);  // Đảm bảo tắt LED nếu xi nhan tắt
        phaiBlinkState = false;             // Reset trạng thái nhấp nháy
      }
    }
    vTaskDelay(pdMS_TO_TICKS(500));  // Tốc độ nhấp nháy cho đèn (300ms on/off)
  }
}

void setup() {
  Serial.begin(115200);

  // Cấu hình các chân GPIO (output)
  pinMode(led_send, OUTPUT);
  pinMode(led_receive, OUTPUT);
  pinMode(led_den, OUTPUT);
  pinMode(led_pha, OUTPUT);
  pinMode(led_xiNhanTrai, OUTPUT);
  pinMode(led_xiNhanPhai, OUTPUT);

  // Cấu hình các chân GPIO (input_pullup)
  pinMode(button_den, INPUT_PULLUP);
  pinMode(button_pha, INPUT_PULLUP);
  pinMode(button_haza, INPUT_PULLUP);
  pinMode(button_xinnhantrai, INPUT_PULLUP);
  pinMode(button_xinnhanphai, INPUT_PULLUP);
  pinMode(button_start, INPUT_PULLUP);

  // Không cần pinMode cho chân ADC, analogRead sẽ tự cấu hình

  // Tạo các FreeRTOS tasks
  xTaskCreate(readCMDTask, "ReadCMD", 4096, NULL, 2, &TaskReadCMD);  // Ưu tiên cao hơn cho việc nhận lệnh
  xTaskCreate(readButtonTask, "ReadButton", 2048, NULL, 1, &TaskReadButtons);
  xTaskCreate(readPotentiometerTask, "ReadPot", 2048, NULL, 1, &TaskReadAnalog);
  // Đã loại bỏ: xTaskCreate(readDHTTask, "ReadDHT", 2048, NULL, 1, &TaskReadDHT);
  xTaskCreate(controlLightsTask, "ControlLights", 2048, NULL, 1, &TaskControlLights);

  // Bật/tắt LED để kiểm tra xem ESP32 đã khởi động và tạo task thành công
  digitalWrite(led_send, HIGH);
  digitalWrite(led_receive, HIGH);
  delay(1000);
  digitalWrite(led_send, LOW);
  digitalWrite(led_receive, LOW);
}

void loop() {
  // Hàm loop trống rỗng vì tất cả logic đã được chuyển vào các FreeRTOS tasks
  // và chúng tự chạy độc lập.
  vTaskDelay(pdMS_TO_TICKS(10));  // Chỉ để giữ cho loop() không hoàn toàn trống rỗng
}