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
int32_t sece = 9;  // secundeneiner
int32_t secz = 5;  // secundenzehner
int32_t mine = 9;  // minuteneiner
int32_t minz = 5;  // minutenzehner

bool led_state = false;
volatile int freq = 1720;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

}

void loop() {
  // Clock
  uhr();
  // Display
  display_Clock();

}

// void uhr() {
//   act_time = millis();
//   if (act_time - old_time >= delay_time) {
//     old_time = act_time;
//     //digitalWrite(LED_BUILTIN, HIGH);
//     tone(8,freq,200);
//     sece = sece + 1;  // secundeneiner hochzählen
//     if (sece > 9) {
//       sece = 0;
//       secz = secz + 1;  // secundenzehner hochzählen
//       if (secz > 5) {
//         secz = 0;
//         sece = 0;
//         mine = mine + 1;
//         if (mine > 9) {
//           mine = 0;
//           minz = minz + 1;
//           if (minz > 5) {
//             minz = 0;
//           }
//         }
//       }
//     }
//   }
// }

void uhr() {
  act_time = millis();
  if (act_time - old_time >= delay_time) {
    old_time = act_time;
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state);
    //tone(8,freq,200);
    sece = sece - 1;  // secundeneiner runterzählen
    if (sece == -1) {
      sece = 9;
      //Serial.println(sece);
       secz = secz - 1;  // secundenzehner runterzählen
       if (secz < 0) {
         secz = 5;
         sece = 9;
         mine = mine - 1;
         if (mine < 0) {
           mine = 9;
           minz = minz - 1;
           if (minz < 0) {
             minz = 5;
           }
         }
       }
    }
  }
}

void display_Clock() {
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(3);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(10, 0);
  display.print(minz);
  display.print(mine);
  display.print(":");
  display.print(secz);
  display.print(sece);
  display.display();
}

