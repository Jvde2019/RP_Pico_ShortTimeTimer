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
uint32_t cl_deltatime = 1000;  // 1000ms
uint32_t cl_oldtime;
uint32_t cl_acttime;
int32_t cl_sece = 9;  // secundeneiner
int32_t cl_secz = 5;  // secundenzehner
int32_t cl_mine = 9;  // minuteneiner
int32_t cl_minz = 5;  // minutenzehner

// Rotaryencoder variables
bool rt_mov = false;
volatile boolean rt_right = false;
volatile boolean rt_left = false;
volatile boolean rt_irdir = false;
volatile byte rt_ccw = 0;
volatile byte rt_cw = 0;
volatile byte rt_inc = 0;
volatile uint8_t rt_a = 0;
volatile uint8_t rt_b = 0;


// Button variables
bool bt_mov = false;
volatile boolean bt_irDir = false;
bool bt_press = false;
bool bt_releas = false;
bool bt_shortpress = false;
bool bt_longpress = false;
volatile uint8_t bt_c = 0;
uint32_t bt_timepressed = 0;
uint32_t bt_timereleased = 0;
uint32_t bt_deltatime = 0;

// Buzzer variables
bool bz_activ = false;
bool bz_state = false;
uint32_t bz_deltatime = 100;  // 100ms
uint32_t bz_oldtime;
uint32_t bz_acttime;
volatile int freq = 1720;


// Statecontrol variables
uint8_t state = 1;
bool run = false;

int digit = 4;
bool led_state = false;

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
  rt_mov = true;
  rt_a = digitalRead(6);
  rt_b = digitalRead(7);
  if (rt_a == rt_b) {
    rt_left = true;
  } else {
    rt_right = true;
  }
  if (rt_irdir) {
    attachInterrupt(6, rotaryMoved, RISING);
    rt_irdir = false;
  } else {
    attachInterrupt(6, rotaryMoved, FALLING);
    rt_irdir = true;
  }
}

// ISR Button
// detection of falling and raising edge of Buttonpin
void buttonEvent(){
  bt_mov = true;
  bt_c = digitalRead(11);
  if (bt_c == false) {
    bt_press = true;
  } else {
    bt_releas = true;
  }  
  if (bt_irDir) {
    attachInterrupt(11, buttonEvent, RISING);
    bt_irDir = false;
  } else {
    attachInterrupt(11, buttonEvent, FALLING);
    bt_irDir = true;
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
  //
  buzzer();
}


void buzzer() {
  if (bz_state){
    bz_acttime = millis();
    if(bz_acttime - bz_oldtime >= bz_deltatime) {
      bz_oldtime = bz_acttime;
      bz_activ = ! bz_activ;
    }  
  if (bz_activ) {tone(8,1720,50);}
  }
    
}

void countdown() {
  if (run == true){
    cl_acttime = millis();
    if (cl_acttime - cl_oldtime >= cl_deltatime){
      cl_oldtime = cl_acttime;
      //tone(8,freq,200);
      cl_sece --;           // secundeneiner runterzählen
      if (cl_sece == -1) {
        cl_sece = 9;
        cl_secz --;         // secundenzehner runterzählen
         if (cl_secz < 0) {
           cl_secz = 5;
           cl_sece = 9;
           cl_mine --;
           if (cl_mine < 0) {
             cl_mine = 9;
             cl_minz --;
             if (cl_minz < 0) {
               cl_minz = 5;
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
  if (digit == 3){display.setTextColor(BLACK, WHITE);}
  display.print(cl_minz);
  display.setTextColor(SSD1306_WHITE);
  if (digit == 2){display.setTextColor(BLACK, WHITE);}
  display.print(cl_mine);
  display.setTextColor(SSD1306_WHITE);
  display.print(":");
  if (digit == 1){display.setTextColor(BLACK, WHITE);}
  display.print(cl_secz);
  display.setTextColor(SSD1306_WHITE);
  if (digit == 0){display.setTextColor(BLACK, WHITE);}
  display.print(cl_sece);
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

void Eventhandling(){
  // looks for actions detected by ISR
  // in case of btpress time saved
  if (bt_press) {
    bt_timepressed = millis();
    bt_press = false;
    tone(8,1000,50);
  }
  
  // in case of bt releas delta T is calculated 
  // short- and longpress are determinated
  if (bt_releas) {
    bt_timereleased = millis();
    bt_releas = false;
    bt_deltatime = bt_timereleased - bt_timepressed;
    if (bt_deltatime < 200) {
      bt_shortpress = true;
      bt_longpress = false;
    }
    else if(bt_deltatime > 250) {
      bt_shortpress = false;
      bt_longpress = true;
    }
  }
}

void statecontrol(){
  switch(state){
    case 1:  // Clock stopped wait for Setting
    cl_sece = 9;
    cl_secz = 5;
    cl_mine = 9; 
    cl_minz = 5;
    if (bt_longpress){
      state = 2;
      bt_longpress = false;
      digit = 0;
      Serial.println(state);
    } 
    break;

    case 2:  // Clock stopped Setting sece digit 0 
    if (rt_right) {
      rt_right = false;
      cl_sece --;  // secundeneiner runterzählen
      if (cl_sece == -1) {cl_sece = 9;} 
    }
    if (rt_left) {
      rt_left = false;
      cl_sece ++;  // secundeneiner hochzählen
      if (cl_sece == 10) {cl_sece = 0;} 
    }
    if (bt_shortpress){
      state = 3;
      bt_shortpress = false;
      digit = 1;
      Serial.println(state);      
    }  
    break;

    case 3:  // Clock stopped Setting cl_secz digit 1
    if (rt_right) {
      rt_right = false;
      cl_secz --;  // secundenzehner runterzählen
      if (cl_secz < 0) {cl_secz = 5; }
    }
    if (rt_left) {
      rt_left = false;
      cl_secz ++;  // secundenzehner hochzählen
      if (cl_secz == 6) {cl_secz = 0;} 
    }     
    if (bt_shortpress){
      state = 4;
      bt_shortpress = false;
      digit = 2;
      Serial.println(state);      
    }  
    break;

    case 4:  // Clock stopped Setting cl_mine digit 2
    if (rt_right) {
      rt_right = false;
      cl_mine --;
      if (cl_mine < 0) {cl_mine = 9;}  
    } 
    if (rt_left) {
      rt_left = false;
      cl_mine ++;  // secundeneiner hochzählen
      if (cl_mine == 10) {cl_mine = 0;} 
    }             
    if (bt_shortpress){
      state = 5;
      bt_shortpress = false;
      digit = 3;
      Serial.println(state);      
    }    
    break;

    case 5:  // Clock stopped Setting cl_minz digit 3
    if (rt_right) {
      rt_right = false;
      cl_minz --;
      if (cl_minz < 0) {cl_minz = 5;} 
    }   
    if (rt_left) {
      rt_left = false;
      cl_minz ++;  // secundeneiner hochzählen
      if (cl_minz == 6) {cl_minz = 0;} 
    }          
    if (bt_shortpress){
      state = 6;
      bt_shortpress = false;
      digit = 4;
      Serial.println(state);      
    }    
    break;

    case 6:  // Clock running wait for Alarm reached
    run = true;
    if (bt_longpress){
      state = 1;
      run = false;
    }
    if (cl_sece == 0 && cl_secz == 0  && cl_mine == 0 && cl_minz == 0){
      run = false;
      bz_state = true;
      if (bt_shortpress){
        state = 1;
        bz_state = false;
        bz_activ = false;
      }
    }
  }
   
}
