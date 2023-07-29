/**************************************************************************
 * https://github.com/Jvde2019/RP_Pico_ShortTimeTimer/
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3C

// Clockvariables
uint32_t delay_time = 1000;  // 1000ms
uint32_t old_time;
uint32_t act_time;
uint32_t sece = 0;  // secundeneiner
uint32_t secz = 0;  // secundenzehner
uint32_t mine = 0;  // minuteneiner
uint32_t minz = 0;  // minutenzehner

volatile int freq = 1720;

void setup() {
  // put your setup code here, to run once:
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

}

void loop() {
  // put your main code here, to run repeatedly:
    // Clock
  uhr();

}

void uhr() {
  act_time = millis();
  if (act_time - old_time >= delay_time) {
    old_time = act_time;
    //digitalWrite(LED_BUILTIN, HIGH);
    tone(8,freq,200);
    sece = sece + 1;  // secundeneiner hochzählen
    if (sece > 9) {
      sece = 0;
      secz = secz + 1;  // secundenzehner hochzählen
      if (secz > 5) {
        secz = 0;
        sece = 0;
        mine = mine + 1;
        if (mine > 9) {
          mine = 0;
          minz = minz + 1;
          if (minz > 5) {
            minz = 0;
          }
        }
      }
    }
  }
}
