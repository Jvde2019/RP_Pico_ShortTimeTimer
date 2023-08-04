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
bool rm = false;
volatile boolean right = false;
volatile boolean left = false;
volatile boolean rtirDir = false;
volatile byte ccw = 0;
volatile byte cw = 0;
volatile byte inc = 0;
volatile uint8_t a = 0;
volatile uint8_t b = 0;


// Button variables
volatile boolean btirDir = false;
bool bevt = false;
bool buttonPress = false;
bool buttonReleas = false;
bool shortpress = false;
bool longpress = false;
volatile uint8_t c = 0;
uint32_t buttonTimePressed = 0;
uint32_t buttonTimeReleased = 0;
uint32_t btntm = 0;

// Statecontrol
uint8_t state = 1;

int digit = 0;
bool run = false;
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
  attachInterrupt(11, buttonEvent, CHANGE);
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
  if (rtirDir) {
    attachInterrupt(6, rotaryMoved, RISING);
    rtirDir = false;
  } else {
    attachInterrupt(6, rotaryMoved, FALLING);
    rtirDir = true;
  }
}

// ISR Button
// detection of falling and raising edge of Buttonpin
void buttonEvent(){
  bevt = true;
  c = digitalRead(11);
  if (c == false) {
    buttonPress = true;
  } else {
    buttonReleas = true;
  }  
  if (btirDir) {
    attachInterrupt(11, buttonEvent, RISING);
    btirDir = false;
  } else {
    attachInterrupt(11, buttonEvent, FALLING);
    btirDir = true;
  }
}

void loop() {
  // Events Checken
  Eventhandling();
  // Clock
  countdown();
  // Display
  display_Clock();
  // State control
  statecontrol();
}

void countdown() {
  if (run == true){
    act_time = millis();
    if (act_time - old_time >= delay_time) {
      old_time = act_time;
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
}

void display_Clock() {
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(3);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(10, 0);
  if (run == false && digit == 3){display.setTextColor(BLACK, WHITE);}
  display.print(minz);
  display.setTextColor(SSD1306_WHITE);
  if (run == false && digit == 2){display.setTextColor(BLACK, WHITE);}
  display.print(mine);
  display.setTextColor(SSD1306_WHITE);
  display.print(":");
  if (run == false && digit == 1){display.setTextColor(BLACK, WHITE);}
  display.print(secz);
  display.setTextColor(SSD1306_WHITE);
  if (run == false && digit == 0){display.setTextColor(BLACK, WHITE);}
  display.print(sece);
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

void Eventhandling(){
  // Take action if a new command received from the encoder
  // if (left) {
  //   left = false;
  //   ccw++;
  //   inc++;
  //   if (run == false){
  //     digit--;
  //     if (digit < 0) {digit = 3;}
  //   }
  // }

  // if (right) {
  //   right = false;
  //   cw++;
  //   inc--;
  //   if (run == false){
  //     digit++;
  //     if (digit > 3) {digit = 0;}
  //   }
  // }

  if (buttonPress) {
    buttonTimePressed = millis();
    buttonPress = false;
    tone(8,1000,50);
  }

  if (buttonReleas) {
    buttonTimeReleased = millis();
    buttonReleas = false;
    btntm = buttonTimeReleased - buttonTimePressed;
    if (btntm < 200) {
      shortpress = true;
      longpress = false;
    }
    else if(btntm > 250) {
      shortpress = false;
      longpress = true;
    }
  }

  // if (shortpress){
  //   shortpress = false;
  //   led_state = !led_state;
  //   digitalWrite(LED_BUILTIN, led_state);
  // }

  // if (longpress){
  //   longpress = false;
  //   digit = 0;
  //   run = !run;
  // }
}

void statecontrol(){
  switch(state){
    case 1:  // Clock stopped wait for Setting
    if (longpress){
      state = 2;
      longpress = false;
      digit = 0;
      Serial.println(state);
    } 
    break;
    case 2:  // Clock stopped Setting digit 0  
    if (shortpress){
      state = 3;
      shortpress = false;
      Serial.println(state);
    }
    break;
    case 3:  // Clock stopped Setting sece
    if (right) {
      right = false;
      sece--;  // secundeneiner runterzählen
      if (sece == -1) {sece = 9;} 
    }
    if (shortpress){
      state = 4;
      shortpress = false;
      digit = 1;
      Serial.println(state);      
    }  
    break;
    case 4:  // Clock stopped Setting secz
    if (right) {
      right = false;
      secz--;  // secundenzehner runterzählen
      if (secz < 0) {secz = 5; }
    }     
    if (shortpress){
      state = 5;
      shortpress = false;
      digit = 2;
      Serial.println(state);      
    }  
    break;
    case 5:  // Clock stopped Setting mine
    if (right) {
      right = false;
      mine--;
      if (mine < 0) {mine = 9;}  
    }          
    if (shortpress){
      state = 6;
      shortpress = false;
      digit = 3;
      Serial.println(state);      
    }    
    break;
    case 6:  // Clock stopped Setting minz
    if (right) {
      right = false;
      minz--;
      if (minz < 0) {minz = 5;} 
    }           
    if (shortpress){
      state = 7;
      shortpress = false;
      digit = 4;
      Serial.println(state);      
    }    
    break;
    case 7:  // Clock running wait for Setting
    run = true;
    if (longpress){
      state = 1;
      run = false;
    }
    if (sece == 0 && secz == 0  && mine == 0 && minz == 0){
      run = false;
      tone(8,1000,50);

    }
    break;
    case 8:  // Clock stopped wait for Setting
    break;
    case 9:  // Clock stopped wait for Setting
    break;
  }
   
}
