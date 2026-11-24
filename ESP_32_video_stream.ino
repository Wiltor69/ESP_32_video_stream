#include "esp_camera.h"   // ESP32 camera library
#include "FS.h"           // File system
#include "SD_MMC.h"       // SD card via SD/MMC interface

// --- AI-Thinker ESP32-CAM ---
#define PWDN_GPIO_NUM     32   // Power Down pin
#define RESET_GPIO_NUM    -1   // Reset pin, not connected
#define XCLK_GPIO_NUM      0   // XCLK pin for camera
#define SIOD_GPIO_NUM     26   // SDA pin for SCCB/I2C
#define SIOC_GPIO_NUM     27   // SCL pin for SCCB/I2C
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25   // Vertical sync pin
#define HREF_GPIO_NUM     23   // Horizontal reference pin
#define PCLK_GPIO_NUM     22   // Pixel clock pin

// --- Camera initialization function ---
void initCamera() {
  camera_config_t config;           // Camera configuration struct
  config.ledc_channel = LEDC_CHANNEL_0; // LEDC channel for XCLK
  config.ledc_timer = LEDC_TIMER_0;     // LEDC timer

  // --- Data pins ---
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  // --- Control pins ---
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  // --- Other settings ---
  config.xclk_freq_hz = 20000000;      // XCLK frequency
  config.pixel_format = PIXFORMAT_JPEG; // Image format
  config.frame_size = FRAMESIZE_VGA;    // Frame size
  config.jpeg_quality = 12;             // JPEG quality, 0-63, lower is better
  config.fb_count = 1;                  // Frame buffer count

  // --- Initialize camera ---
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed!"); // ПInitialization failed
    while (true);                           // Stop program
  }
}

void setup() {
  Serial.begin(115200);   // Start serial monitor

  // --- Initialize SD card ---
  if(!SD_MMC.begin()){    // Try to mount SD
    Serial.println("Card Mount Failed");
    return;               // Exit setup if failed
  }
  uint8_t cardType = SD_MMC.cardType(); // Get SD card type
  if(cardType == CARD_NONE){            // No SD card
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("SD card initialized");

  // --- Initialize camera ---
  initCamera();
  Serial.println("Camera ready!");
}

void loop() {
  // --- Capture frame ---
  camera_fb_t * fb = esp_camera_fb_get(); // Get frame buffer
  if(!fb){
    Serial.println("Camera capture failed"); // Capture failed
    delay(5000); // Retry in 5 seconds
    return;
  }

  // --- Create unique filename ---
  String path = "/photo_" + String(millis()) + ".jpg";

  // --- Write to SD card ---
  File file = SD_MMC.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode"); // Failed to open file
  } else {
    file.write(fb->buf, fb->len); // Write camera buffer to file
    Serial.print("Saved file to: ");
    Serial.println(path);
  }
  file.close();                // Close file
  esp_camera_fb_return(fb);    // Return frame buffer

  delay(5000);                 // Wait 5 seconds before next photo)
}