from picamera2 import Picamera2
import cv2
import numpy as np
import tflite_runtime.interpreter as tflite
import time

# ========== 1. Cấu hình ==========
FRAME_WIDTH = 320
FRAME_HEIGHT = 180

CLASS_NAMES = {
    0: "Cấm rẽ phải",
    1: "Cấm rẽ trái",
    2: "Stop",
}

HOST = '192.168.1.60'  # Thay thế bằng địa chỉ IP của máy tính Windows của bạn
PORT = 65432

# ========== 2. Tham số lọc ==========
CONFIDENCE_THRESHOLD = 0.85
MIN_SIGN_AREA = 1500
ASPECT_RATIO = (0.7, 1.5)
RED_PIXEL_RATIO = 0.3
MIN_CIRCULARITY = 0.6

SIGN_HOLD_TIME = 2  # giây
DISPLAY_GUI = True   # Bật GUI để xem trực tiếp

# ========== 3. Khởi tạo camera ==========
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(
    main={"format": 'RGB888', "size": (FRAME_WIDTH, FRAME_HEIGHT)},
    controls={"FrameDurationLimits": (50000, 50000)}
))
picam2.start()
time.sleep(1)  # Cho camera ổn định

# ========== 4. Load mô hình TFLite ==========
interpreter = tflite.Interpreter(model_path="traffic_sign_mobilenetv2_quant.tflite")
interpreter.allocate_tensors()
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# ========== 5. Hàm hỗ trợ ==========
def calculate_circularity(contour):
    perimeter = cv2.arcLength(contour, True)
    if perimeter == 0:
        return 0
    area = cv2.contourArea(contour)
    return (4 * np.pi * area) / (perimeter ** 2)

def advanced_color_filter(roi):
    hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
    lower_red1 = np.array([0, 120, 70])
    upper_red1 = np.array([10, 255, 255])
    lower_red2 = np.array([165, 120, 70])
    upper_red2 = np.array([180, 255, 255])
    mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask2 = cv2.inRange(hsv, lower_red2, upper_red2)
    red_mask = cv2.bitwise_or(mask1, mask2)
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5,5))
    red_mask = cv2.morphologyEx(red_mask, cv2.MORPH_CLOSE, kernel)
    red_mask = cv2.morphologyEx(red_mask, cv2.MORPH_OPEN, kernel)
    return red_mask

def is_valid_sign(contour, frame, predicted_class=None):
    area = cv2.contourArea(contour)
    if area < MIN_SIGN_AREA:
        return False
    x, y, w, h = cv2.boundingRect(contour)
    aspect_ratio = w / float(h)
    if not ASPECT_RATIO[0] <= aspect_ratio <= ASPECT_RATIO[1]:
        return False
    roi = frame[y:y+h, x:x+w]
    red_mask = advanced_color_filter(roi)
    red_ratio = np.sum(red_mask > 0) / float(red_mask.size)
    if red_ratio < RED_PIXEL_RATIO:
        return False
    circularity = calculate_circularity(contour)
    if predicted_class is not None and CLASS_NAMES.get(predicted_class, "") == "Stop" and circularity < MIN_CIRCULARITY:
        return False
    return True

# ========== 6. Nhận diện biển báo ==========
def detect_signs(frame, current_time):
    global last_sign_id, last_sign_time

    red_mask = advanced_color_filter(frame)
    contours, _ = cv2.findContours(red_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    for cnt in contours:
        if not is_valid_sign(cnt, frame):
            continue
        x, y, w, h = cv2.boundingRect(cnt)
        roi = frame[y:y+h, x:x+w]
        img = cv2.resize(roi, (192, 192))
        img_array = np.array(img, dtype=np.float32) / 255.0
        img_array = np.expand_dims(img_array, axis=0)
        interpreter.set_tensor(input_details[0]['index'], img_array)
        interpreter.invoke()
        predictions = interpreter.get_tensor(output_details[0]['index'])
        class_id = np.argmax(predictions)
        confidence = np.max(predictions)

        if confidence > CONFIDENCE_THRESHOLD and is_valid_sign(cnt, frame, class_id):
            if class_id != last_sign_id or (current_time - last_sign_time) > SIGN_HOLD_TIME:
                last_sign_id = class_id
                last_sign_time = current_time

                if DISPLAY_GUI:
                    sign_name = CLASS_NAMES.get(class_id, f"Unknown (ID: {class_id})")
                    display_text = f"{sign_name} - {confidence:.2f}"
                    cv2.drawContours(frame, [cnt], -1, (0, 255, 0), 2)
                    cv2.putText(frame, display_text, (x, y-10),
                                cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
            return True
    return False

# ========== 7. Vòng lặp chính ==========
last_sign_id = None
last_sign_time = 0

try:
    while True:
        frame = picam2.capture_array()
        current_time = time.time()

        detect_signs(frame, current_time)

        if DISPLAY_GUI:
            cv2.imshow("Traffic Sign Detection", frame)
            if cv2.waitKey(1) == ord('q'):
                break
finally:
    if DISPLAY_GUI:
        cv2.destroyAllWindows()
