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

// Rotaryencoder variables
volatile boolean irDir = false;
volatile byte ccw = 0;
volatile byte cw = 0;
volatile byte inc = 0;
bool rm = false;
volatile uint8_t a = 0;
volatile uint8_t b = 0;
volatile boolean right = false;
volatile boolean left = false;
bool buttonPress = false;

bool led_state = false;
volatile int freq = 1720;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  // Rotary Button INPUT_PULLUP GPIO 11 !!
  pinMode(11, INPUT_PULLUP);
  //pinMode(11, INPUT);
  attachInterrupt(11, buttonPressed, FALLING);
  // can be CHANGE or LOW or RISING or FALLING or HIGH
  // Rotary ChanA INPUT_PULLUP GPIO 6 !!
  pinMode(6, INPUT_PULLUP);
  //pinMode(6, INPUT);
  attachInterrupt(6, rotaryMoved, CHANGE);
  // can be CHANGE or LOW or RISING or FALLING or HIGH
  // Rotary ChanB INPUT_PULLUP GPIO 7 !!
  pinMode(7, INPUT_PULLUP);
  //pinMode(7, INPUT);
  pinMode(8, OUTPUT_8MA);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

}

// ISR Rotary
void rotaryMoved() {
  // ISR for DDM427 See Datasheet https://hopt-schuler.com/sites/default/files/medien/dokumente/2022-11/miniature_2bit_encoder_427_2022.pdf
  rm = true;
  a = digitalRead(6);
  b = digitalRead(7);
  if (a == b) {
    left = true;
  } else {
    right = true;
  }
  if (irDir) {
    attachInterrupt(6, rotaryMoved, RISING);
    irDir = false;
  } else {
    attachInterrupt(6, rotaryMoved, FALLING);
    irDir = true;
  }
}

// ISR Button
void buttonPressed() {
  buttonPress = true;
}
void loop() {
  // Clock
  countdown();
  // Display
  display_Clock();

}

void countdown() {
  act_time = millis();
  if (act_time - old_time >= delay_time) {
    old_time = act_time;
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state);
    //tone(8,freq,200);
    sece = sece - 1;  // secundeneiner runterzählen
    if (sece == -1) {
      sece = 9;
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



void Eventhandling(){
  // Take action if a new command received from the encoder
  if (left) {
    left = false;
    ccw++;
    inc++;
    // switch(page){
    //   case 1:
    //   menuitem--;
    //   if (menuitem == 0) { menuitem = 4; }
    //   break;

    //   case 4:
    //   menuitem4--;
    //   if (menuitem4 == 0) { menuitem4 = 3; }
    //   if (menuitem4 == 1) { freq = freq+10;}
    //   if (menuitem4 == 2) { freq = freq+10;}
    //  } 
   }

  if (right) {
    right = false;
    cw++;
    inc--;
    // switch(page){
    //   case 1:
    //   menuitem++;
    //   if (menuitem == 5) { menuitem = 1; }
    //   break;

    //   case 4:
    //   menuitem4++;
    //   if (menuitem4 == 4) { menuitem4 = 1; }      
    //   if (menuitem4 == 1) { freq = freq-10; }
    //   if (menuitem4 == 1) { freq = freq-10; }
    // }
  }

  if (buttonPress) {
    buttonPress = false;
    // switch(page){
    //   case 1:
    //   switch(menuitem){
    //     case 1:
    //     page = 4;
    //     break;

    //     case 2:
    //     page = 2;
    //     cw = 0;
    //     ccw = 0;
    //     inc = 0;
    //     break;

    //     case 3:
    //     led_state = !led_state;
    //     digitalWrite(LED_BUILTIN, led_state);
    //     break;

    //     case 4:
    //     page = 3;
    //     break;
    //   } 

    //   break;

    //   case 2:
    //   page = 1;
    //   break;

    //   case 3:
    //   page = 1;
    //   break;

    //   case 4:
    //   if (menuitem4 == 3){page = 1;}
    //   break;
    //}
  }
}

