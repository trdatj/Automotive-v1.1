#include <DHT.h>
#include <Wire.h>
#include "RTClib.h"

// ================== KHAI BÁO CHÂN ==================
#define RELAY 17
#define BTN_START 33
#define BTN_DEN 35
#define BTN_PHA 34
#define BTN_HAZARD 32
#define BTN_XINHANTRAI 19
#define BTN_XINHANPHAI 5

#define LED_DEN 26
#define LED_PHA 14
#define LED_XINHANTRAI 25
#define LED_XINHANPHAI 27

#define BIENTRO 4
#define DHT11_PIN 23
#define DHTTYPE DHT11

// ================== KHAI BÁO BIẾN ==================
DHT dht(DHT11_PIN, DHTTYPE);
RTC_DS1307 rtc;

// Trạng thái hệ thống
bool carStarted = false;
bool denThuong = false;
bool denPha = false;
bool xinhanTrai = false;
bool xinhanPhai = false;
bool hazardMode = false;
bool prevXinhanTrai = false;  // Lưu trạng thái xin han trước khi bật hazard
bool prevXinhanPhai = false;

// Biến đo lường
float temperature = 0;
float humidity = 0;
float speed = 0;
DateTime currentTime;

// Task handles
TaskHandle_t taskStartHandle;
TaskHandle_t taskDHT11Handle;
TaskHandle_t taskSpeedHandle;
TaskHandle_t taskDS1307Handle;
TaskHandle_t taskDenThuongHandle;
TaskHandle_t taskDenPhaHandle;
TaskHandle_t taskXinhanHandle;
TaskHandle_t taskHazardHandle;
TaskHandle_t taskCMDHandle;


// Semaphores
SemaphoreHandle_t xMutex;

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);

  // Khởi tạo pins
  pinMode(RELAY, OUTPUT);
  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_DEN, INPUT_PULLUP);
  pinMode(BTN_PHA, INPUT_PULLUP);
  pinMode(BTN_HAZARD, INPUT_PULLUP);
  pinMode(BTN_XINHANTRAI, INPUT_PULLUP);
  pinMode(BTN_XINHANPHAI, INPUT_PULLUP);

  pinMode(LED_DEN, OUTPUT);
  pinMode(LED_PHA, OUTPUT);
  pinMode(LED_XINHANTRAI, OUTPUT);
  pinMode(LED_XINHANPHAI, OUTPUT);

  pinMode(BIENTRO, INPUT);

  // Tắt tất cả đèn ban đầu
  digitalWrite(LED_DEN, LOW);
  digitalWrite(LED_PHA, LOW);
  digitalWrite(LED_XINHANTRAI, LOW);
  digitalWrite(LED_XINHANPHAI, LOW);
  digitalWrite(RELAY, LOW);

  // Tạo mutex
  xMutex = xSemaphoreCreateMutex();

  // Tạo task START (luôn chạy để kiểm tra nút khởi động)
  xTaskCreate(taskStart, "START_TASK", 2048, NULL, 2, &taskStartHandle);

  Serial.println("System initialized. Press START button to start the car.");
}

void loop() {
  // FreeRTOS sẽ quản lý tất cả, không cần code trong loop
  vTaskDelay(portMAX_DELAY);
}

// ================== TASK FUNCTIONS ==================

void taskStart(void *pvParameters) {
  bool lastBtnState = HIGH;
  bool currentBtnState;

  while (1) {
    currentBtnState = digitalRead(BTN_START);

    // Phát hiện nhấn nút (falling edge)
    if (lastBtnState == HIGH && currentBtnState == LOW) {
      vTaskDelay(50 / portTICK_PERIOD_MS);  // Debounce

      if (!carStarted) {
        // Khởi động xe
        carStarted = true;
        digitalWrite(RELAY, HIGH);

        // Khởi tạo sensors
        dht.begin();
        if (!rtc.begin()) {
          Serial.println("Couldn't find RTC");
        }
        if (!rtc.isrunning()) {
          Serial.println("RTC is NOT running, setting time!");
          rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        }

        // Tạo các tasks khác
        xTaskCreate(taskDHT11, "DHT11_TASK", 2048, NULL, 1, &taskDHT11Handle);
        xTaskCreate(taskSpeed, "SPEED_TASK", 2048, NULL, 1, &taskSpeedHandle);
        xTaskCreate(taskDS1307, "DS1307_TASK", 2048, NULL, 1, &taskDS1307Handle);
        xTaskCreate(taskDenThuong, "DEN_THUONG_TASK", 2048, NULL, 1, &taskDenThuongHandle);
        xTaskCreate(taskDenPha, "DEN_PHA_TASK", 2048, NULL, 1, &taskDenPhaHandle);
        xTaskCreate(taskXinhan, "XINHAN_TASK", 2048, NULL, 1, &taskXinhanHandle);
        xTaskCreate(taskHazard, "HAZARD_TASK", 2048, NULL, 1, &taskHazardHandle);
        xTaskCreate(taskReadCMD, "HAZARD_TASK", 2048, NULL, 1, &taskCMDHandle);


        Serial.println("Car:Started!");
      } else {
        // Tắt xe
        carStarted = false;
        digitalWrite(RELAY, LOW);

        // Tắt tất cả đèn
        digitalWrite(LED_DEN, LOW);
        digitalWrite(LED_PHA, LOW);
        digitalWrite(LED_XINHANTRAI, LOW);
        digitalWrite(LED_XINHANPHAI, LOW);

        // Reset trạng thái
        denThuong = false;
        denPha = false;
        xinhanTrai = false;
        xinhanPhai = false;
        hazardMode = false;

        // Xóa các tasks
        if (taskDHT11Handle != NULL) vTaskDelete(taskDHT11Handle);
        if (taskSpeedHandle != NULL) vTaskDelete(taskSpeedHandle);
        if (taskDS1307Handle != NULL) vTaskDelete(taskDS1307Handle);
        if (taskDenThuongHandle != NULL) vTaskDelete(taskDenThuongHandle);
        if (taskDenPhaHandle != NULL) vTaskDelete(taskDenPhaHandle);
        if (taskXinhanHandle != NULL) vTaskDelete(taskXinhanHandle);
        if (taskHazardHandle != NULL) vTaskDelete(taskHazardHandle);

        Serial.println("Car:Stopped!");
      }
    }

    lastBtnState = currentBtnState;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void taskDHT11(void *pvParameters) {
  while (1) {
    if (carStarted) {
      if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        temperature = dht.readTemperature();

        if (!isnan(temperature)) {
          Serial.printf("Temperature:%.1f\n", temperature);
        }
        xSemaphoreGive(xMutex);
      }
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void taskSpeed(void *pvParameters) {
  while (1) {
    if (carStarted) {
      int adcValue = analogRead(BIENTRO);
      // speed = map(adcValue, 0, 4095, 0, 200);  // Chuyển đổi sang km/h (0-200)

      if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        Serial.printf("POT_VAL:%d\n", adcValue);
        xSemaphoreGive(xMutex);
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void taskDS1307(void *pvParameters) {
  while (1) {
    if (carStarted) {
      if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        currentTime = rtc.now() + TimeSpan(0, 6, 30, 20);
        Serial.printf("DATE/TIME:%02d-%02d-%04d %02d:%02d:%02d\n",
                        currentTime.day(), currentTime.month(), currentTime.year(),
                        currentTime.hour(), currentTime.minute(), currentTime.second());
        xSemaphoreGive(xMutex);
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void taskDenThuong(void *pvParameters) {
  bool lastBtnState = HIGH;
  bool currentBtnState;

  while (1) {
    if (carStarted) {
      currentBtnState = digitalRead(BTN_DEN);

      if (lastBtnState == HIGH && currentBtnState == LOW) {
        vTaskDelay(50 / portTICK_PERIOD_MS);  // Debounce

        denThuong = !denThuong;
        digitalWrite(LED_DEN, denThuong ? HIGH : LOW);

        // Nếu tắt đèn thường thì tắt luôn đèn pha
        if (!denThuong && denPha) {
          denPha = false;
          digitalWrite(LED_PHA, LOW);
        }

        Serial.printf("DEN_COS:%s\n", denThuong ? "ON" : "OFF");
      }

      lastBtnState = currentBtnState;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void taskDenPha(void *pvParameters) {
  bool lastBtnState = HIGH;
  bool currentBtnState;

  while (1) {
    if (carStarted) {
      currentBtnState = digitalRead(BTN_PHA);

      if (lastBtnState == HIGH && currentBtnState == LOW) {
        vTaskDelay(50 / portTICK_PERIOD_MS);  // Debounce

        // Chỉ bật đèn pha khi đèn thường đã bật
        if (denThuong) {
          denPha = !denPha;
          digitalWrite(LED_PHA, denPha ? HIGH : LOW);
          Serial.printf("DEN_PHA:%s\n", denPha ? "ON" : "OFF");
        }
      }

      lastBtnState = currentBtnState;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void taskXinhan(void *pvParameters) {
  bool lastBtnTrai = HIGH, lastBtnPhai = HIGH;
  bool currentBtnTrai, currentBtnPhai;
  unsigned long lastBlinkTime = 0;
  bool blinkState = false;

  while (1) {
    if (carStarted && !hazardMode) {
      currentBtnTrai = digitalRead(BTN_XINHANTRAI);
      currentBtnPhai = digitalRead(BTN_XINHANPHAI);

      // Xử lý nút xi nhan trái
      if (lastBtnTrai == HIGH && currentBtnTrai == LOW) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        if (xinhanTrai) {
          xinhanTrai = false;
          Serial.println("TURN_LEFT:OFF");
        } else {
          xinhanTrai = true;
          xinhanPhai = false;  // Tắt xi nhan phải
          Serial.println("TURN_LEFT:ON");
        }
      }

      // Xử lý nút xi nhan phải
      if (lastBtnPhai == HIGH && currentBtnPhai == LOW) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        if (xinhanPhai) {
          xinhanPhai = false;
          Serial.println("TURN_RIGHT:OFF");
        } else {
          xinhanPhai = true;
          xinhanTrai = false;  // Tắt xi nhan trái
          Serial.println("TURN_RIGHT:ON");
        }
      }

      // Xử lý nhấp nháy
      if (millis() - lastBlinkTime >= 500) {
        blinkState = !blinkState;
        lastBlinkTime = millis();

        if (xinhanTrai) {
          digitalWrite(LED_XINHANTRAI, blinkState ? HIGH : LOW);
          digitalWrite(LED_XINHANPHAI, LOW);
        } else if (xinhanPhai) {
          digitalWrite(LED_XINHANPHAI, blinkState ? HIGH : LOW);
          digitalWrite(LED_XINHANTRAI, LOW);
        } else {
          digitalWrite(LED_XINHANTRAI, LOW);
          digitalWrite(LED_XINHANPHAI, LOW);
        }
      }

      lastBtnTrai = currentBtnTrai;
      lastBtnPhai = currentBtnPhai;
    } else if (!hazardMode) {
      // Tắt xi nhan khi xe chưa khởi động
      digitalWrite(LED_XINHANTRAI, LOW);
      digitalWrite(LED_XINHANPHAI, LOW);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void taskHazard(void *pvParameters) {
  bool lastBtnState = HIGH;
  bool currentBtnState;
  unsigned long lastBlinkTime = 0;
  bool blinkState = false;

  while (1) {
    if (carStarted) {
      currentBtnState = digitalRead(BTN_HAZARD);

      if (lastBtnState == HIGH && currentBtnState == LOW) {
        vTaskDelay(50 / portTICK_PERIOD_MS);  // Debounce

        if (!hazardMode) {
          // Lưu trạng thái xi nhan hiện tại
          prevXinhanTrai = xinhanTrai;
          prevXinhanPhai = xinhanPhai;

          hazardMode = true;
          xinhanTrai = false;
          xinhanPhai = false;
          Serial.println("HAZARD:ON");
        } else {
          hazardMode = false;

          // Khôi phục trạng thái xi nhan trước đó
          xinhanTrai = prevXinhanTrai;
          xinhanPhai = prevXinhanPhai;
          Serial.println("HAZARD:OFF");
        }
      }

      // Xử lý nhấp nháy hazard
      if (hazardMode) {
        if (millis() - lastBlinkTime >= 500) {
          blinkState = !blinkState;
          lastBlinkTime = millis();

          digitalWrite(LED_XINHANTRAI, blinkState ? HIGH : LOW);
          digitalWrite(LED_XINHANPHAI, blinkState ? HIGH : LOW);
        }
      }

      lastBtnState = currentBtnState;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void taskReadCMD(void *pvParameters) {
  while (1) {
    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim();
      cmd.toUpperCase();  // Chuyển về chữ hoa để dễ xử lý

      Serial.println("Received command: " + cmd);

      // Xử lý lệnh xi nhan trái
      if (cmd == "TURN_LEFT:ON" || cmd == "LEFT:ON" || cmd == "TRAI:ON") {
        if (carStarted && !hazardMode) {
          xinhanTrai = true;
          xinhanPhai = false;
          Serial.println("✓ Xi nhan trái BẬT");
        } else {
          Serial.println("✗ Không thể bật xi nhan (xe chưa khởi động hoặc đang bật hazard)");
        }
      } else if (cmd == "TURN_LEFT:OFF" || cmd == "LEFT:OFF" || cmd == "TRAI:OFF") {
        xinhanTrai = false;
        Serial.println("✓ Xi nhan trái TẮT");
      }

      // Xử lý lệnh xi nhan phải
      else if (cmd == "TURN_RIGHT:ON" || cmd == "RIGHT:ON" || cmd == "PHAI:ON") {
        if (carStarted && !hazardMode) {
          xinhanPhai = true;
          xinhanTrai = false;
          Serial.println("✓ Xi nhan phải BẬT");
        } else {
          Serial.println("✗ Không thể bật xi nhan (xe chưa khởi động hoặc đang bật hazard)");
        }
      } else if (cmd == "TURN_RIGHT:OFF" || cmd == "RIGHT:OFF" || cmd == "PHAI:OFF") {
        xinhanPhai = false;
        Serial.println("✓ Xi nhan phải TẮT");
      }

      // Xử lý lệnh hazard
      else if (cmd == "HAZARD:ON" || cmd == "HAZARD") {
        if (carStarted) {
          // Lưu trạng thái xi nhan hiện tại
          prevXinhanTrai = xinhanTrai;
          prevXinhanPhai = xinhanPhai;
          hazardMode = true;
          xinhanTrai = false;
          xinhanPhai = false;
          Serial.println("✓ Chế độ Hazard BẬT");
        } else {
          Serial.println("✗ Xe chưa khởi động");
        }
      } else if (cmd == "HAZARD:OFF") {
        hazardMode = false;
        // Khôi phục trạng thái xi nhan trước đó
        xinhanTrai = prevXinhanTrai;
        xinhanPhai = prevXinhanPhai;
        Serial.println("✓ Chế độ Hazard TẮT");
      }

      // Xử lý lệnh đèn thường
      else if (cmd == "DEN_COS:ON" || cmd == "DEN:ON" || cmd == "COS:ON") {
        if (carStarted) {
          denThuong = true;
          digitalWrite(LED_DEN, HIGH);
          Serial.println("✓ Đèn thường BẬT");
        } else {
          Serial.println("✗ Xe chưa khởi động");
        }
      } else if (cmd == "DEN_COS:OFF" || cmd == "DEN:OFF" || cmd == "COS:OFF") {
        denThuong = false;
        digitalWrite(LED_DEN, LOW);
        // Tắt luôn đèn pha nếu đang bật
        if (denPha) {
          denPha = false;
          digitalWrite(LED_PHA, LOW);
          Serial.println("✓ Đèn thường và đèn pha đã TẮT");
        } else {
          Serial.println("✓ Đèn thường TẮT");
        }
      }

      // Xử lý lệnh đèn pha
      else if (cmd == "DEN_PHA:ON" || cmd == "PHA:ON" || cmd == "HIGH_BEAM:ON") {
        if (carStarted && denThuong) {
          denPha = true;
          digitalWrite(LED_PHA, HIGH);
          Serial.println("✓ Đèn pha BẬT");
        } else if (!carStarted) {
          Serial.println("✗ Xe chưa khởi động");
        } else {
          Serial.println("✗ Phải bật đèn thường trước khi bật đèn pha");
        }
      } else if (cmd == "DEN_PHA:OFF" || cmd == "PHA:OFF" || cmd == "HIGH_BEAM:OFF") {
        denPha = false;
        digitalWrite(LED_PHA, LOW);
        Serial.println("✓ Đèn pha TẮT");
      }

      // Lệnh trạng thái
      else if (cmd == "STATUS" || cmd == "TRANGTHAI") {
        Serial.println("========== TRẠNG THÁI HỆ THỐNG ==========");
        Serial.println("Xe: " + String(carStarted ? "KHỞI ĐỘNG" : "TẮT"));
        if (carStarted) {
          Serial.println("Đèn thường: " + String(denThuong ? "BẬT" : "TẮT"));
          Serial.println("Đèn pha: " + String(denPha ? "BẬT" : "TẮT"));
          Serial.println("Xi nhan trái: " + String(xinhanTrai ? "BẬT" : "TẮT"));
          Serial.println("Xi nhan phải: " + String(xinhanPhai ? "BẬT" : "TẮT"));
          Serial.println("Hazard: " + String(hazardMode ? "BẬT" : "TẮT"));
        }
        Serial.println("========================================");
      }

      // Lệnh help
      else if (cmd == "HELP" || cmd == "H" || cmd == "?") {
        Serial.println("========== DANH SÁCH LỆNH ==========");
        Serial.println("Xi nhan:");
        Serial.println("  TRAI:ON / LEFT:ON - Bật xi nhan trái");
        Serial.println("  TRAI:OFF / LEFT:OFF - Tắt xi nhan trái");
        Serial.println("  PHAI:ON / RIGHT:ON - Bật xi nhan phải");
        Serial.println("  PHAI:OFF / RIGHT:OFF - Tắt xi nhan phải");
        Serial.println("");
        Serial.println("Hazard:");
        Serial.println("  HAZARD:ON / HAZARD - Bật hazard");
        Serial.println("  HAZARD:OFF - Tắt hazard");
        Serial.println("");
        Serial.println("Đèn:");
        Serial.println("  DEN:ON / LIGHT:ON - Bật đèn thường");
        Serial.println("  DEN:OFF / LIGHT:OFF - Tắt đèn thường");
        Serial.println("  PHA:ON / HIGH_BEAM:ON - Bật đèn pha");
        Serial.println("  PHA:OFF / HIGH_BEAM:OFF - Tắt đèn pha");
        Serial.println("");
        Serial.println("Khác:");
        Serial.println("  STATUS / TRANGTHAI - Xem trạng thái");
        Serial.println("  HELP / H / ? - Hiện danh sách lệnh");
        Serial.println("===================================");
      }

      // Lệnh không hợp lệ
      else if (cmd.length() > 0) {
        Serial.println("✗ Lệnh không hợp lệ: " + cmd);
        Serial.println("Gõ 'HELP' để xem danh sách lệnh");
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));  // Giảm delay để phản hồi nhanh hơn
  }
}