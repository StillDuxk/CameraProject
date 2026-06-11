
extern int led_duty;
const int FLASH_BRIGHTNESS = 120;
void enable_led(bool en);

#define FLASH_BUTTON 12

bool flashEnabled = false;
bool lastFlashButtonState = HIGH;
bool lastButtonState = HIGH;
int photoNumber;
#define SHUTTER_BUTTON 13
#include "FS.h"
#include "SD_MMC.h"
#include "esp_camera.h"


// ===========================
// Select camera model in board_config.h
// ===========================
#include "board_config.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "iPhone";
const char *password = "NopeIAmNotGiving";

void startCameraServer();
void setupLedFlash();

int getNextPhotoNumber() {

    int num = 1;

    char path[32];

    while(true) {

        snprintf(path, sizeof(path),
                 "/IMG%04d.JPG",
                 num);

        if(!SD_MMC.exists(path)) {
            return num;
        }

        num++;
    }
}

void savePhotoButton() {

    camera_fb_t *fb;

    // Throw away one frame
    // Throw away one old frame
    fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);

    // Turn flash on if enabled
    if(flashEnabled){

      led_duty = FLASH_BRIGHTNESS;

      enable_led(true);

      delay(150);
    } 

    // Capture fresh frame
    fb = esp_camera_fb_get();
    if(!fb){
        Serial.println("Capture failed");
        return;
    }
    
    char path[32];
    
    snprintf(path,
             sizeof(path),
             "/IMG%04d.JPG",
             photoNumber);

    File file = SD_MMC.open(path, FILE_WRITE);

    if(file){

        file.write(fb->buf, fb->len);

        file.close();

        Serial.print("Saved: ");
        Serial.println(path);
        photoNumber++;


    } else {

        Serial.println("Save failed");
    }

    if(flashEnabled){
      enable_led(false);
    }

    esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);
  pinMode(SHUTTER_BUTTON, INPUT_PULLUP);
  pinMode(FLASH_BUTTON, INPUT_PULLUP);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_XGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 1;
      config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_XGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 1;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

    // =========================
  // SD CARD INIT
  // =========================

  if(!SD_MMC.begin("/sdcard", true)){
      Serial.println("SD Card Mount Failed");
      return;
  }

  uint8_t cardType = SD_MMC.cardType();

  if(cardType == CARD_NONE){
      Serial.println("No SD Card attached");
      return;
  }

  Serial.println("SD Card initialized");
  photoNumber = getNextPhotoNumber();

  Serial.print("Starting at image ");
  Serial.println(photoNumber);

  sensor_t *s = esp_camera_sensor_get();
  
  s->set_brightness(s, -1);
  s->set_contrast(s, 2);
  s->set_saturation(s, 1);
  s->set_sharpness(s, 2);
  s->set_denoise(s, 1);
  s->set_gainceiling(s, (gainceiling_t)2);
  s->set_quality(s, 10);
  s->set_whitebal(s, 1);
  s->set_awb_gain(s, 1);
  s->set_exposure_ctrl(s, 1);
  s->set_ae_level(s, -2);
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_XGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash();
#endif
  Serial.println("Camera Ready");
}

void loop() {

    bool currentButtonState = digitalRead(SHUTTER_BUTTON);
    bool currentFlashButtonState = digitalRead(FLASH_BUTTON);

    // SHUTTER BUTTON

    if(currentButtonState == LOW && lastButtonState == HIGH){

        Serial.println("BUTTON PRESSED");

        savePhotoButton();
    }

    // FLASH BUTTON

    if(currentFlashButtonState == LOW && lastFlashButtonState == HIGH){

      Serial.println("FLASH BUTTON EVENT");

      flashEnabled = !flashEnabled;

      if(flashEnabled){
        Serial.println("FLASH ON");

      } else {

        Serial.println("FLASH OFF");
      }

      while(digitalRead(FLASH_BUTTON) == LOW){
        delay(10);
      }

      delay(50);
    }

    lastButtonState = currentButtonState;
    lastFlashButtonState = currentFlashButtonState;
}
