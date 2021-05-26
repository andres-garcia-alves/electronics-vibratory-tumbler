#include <TM1637Display.h>

const int PIN_BTN_INCREASE = 2;
const int PIN_BTN_DECREASE = 3;
const int PIN_BTN_START = 4;
const int PIN_BTN_STOP = 5;

const int PIN_BUZZER = 9;
const int PIN_RELE = 10;

const int PIN_DISPLAY_CLK = A0;
const int PIN_DISPLAY_DIO = A1;

const long int SCALE = 60; // 1 = seconds, 60 = minutes

// arrays that turns all segments on/off
const uint8_t segments_all_on[] = {0xff, 0xff, 0xff, 0xff};
const uint8_t segments_all_off[] = {0x00, 0x00, 0x00, 0x00};

// individual segments to spell 'HOLA' word
const uint8_t segments_hola[] = {
  SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,           // H
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_D | SEG_E | SEG_F,                           // L
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G    // A
};

// create display object of type TM1637Display
// e.g. #01: display.showNumberDec(24, false, 2, 0);
// e.g. #02: display.showNumberDecEx(countdown, 0b11100000, false, 4, 0);
// e.g. #03: display.setSegments(segments_celsius, 2, 2);
TM1637Display display = TM1637Display(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);

// variables to stores countdown timer
int countdown = 30;
bool countdown_enabled = false;
unsigned long int countdown_next_milis = 0;

// variables to store the push-buttons readed value
int btnIncreaseState = 0;
int btnDecreaseState = 0;
int btnStartState = 0;
int btnStopState = 0;

// functions declaration
int convertToTimelyNumber(int countNumber);
void setBeep(int frecuency = 450, int duration = 40);
void setRele(bool status);
void timeoutBlinkSecuence();


void setup() {

  // Serial.begin(9600);
  // Serial.println();

  pinMode(PIN_BTN_INCREASE, INPUT_PULLUP);
  pinMode(PIN_BTN_DECREASE, INPUT_PULLUP);
  pinMode(PIN_BTN_START, INPUT_PULLUP);
  pinMode(PIN_BTN_STOP, INPUT_PULLUP);
  
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RELE, OUTPUT);
    
  display.clear();
  display.setBrightness(2);
  delay(1000);
}


void loop() {

  // read the state of push-buttons
  btnIncreaseState = digitalRead(PIN_BTN_INCREASE);
  btnDecreaseState = digitalRead(PIN_BTN_DECREASE);  
  btnStartState = digitalRead(PIN_BTN_START);
  btnStopState = digitalRead(PIN_BTN_STOP);

  // check if the push-buttons are pressed
  if (btnIncreaseState == LOW) {
    countdown = countdown + 10;
    setBeep();
    delay(100);
  }
  
  if (btnDecreaseState == LOW) {
    countdown = countdown - 10;
    if (countdown < 0) { countdown = 0; }
    setBeep();
    delay(100);
  }
  
  if (btnStartState == LOW && countdown > 0) {
    countdown_next_milis = millis() + (SCALE * 1000);
    countdown_enabled = true;
    setBeep();
    setRele(true);
    delay(100);
  }
  
  if (btnStopState == LOW) {
    if (countdown_enabled == true) {
      countdown_enabled = false;
      setRele(false);
    } else { // countdown_enabled == false
      countdown = 0;
    }
    setBeep();
    delay(100);
  }

  // countdown calculations
  if (countdown_enabled == true && countdown > 0) {
    if (millis() > countdown_next_milis) {
      countdown_next_milis = countdown_next_milis + (SCALE * 1000);
      countdown = countdown - 1;
    }
  }

  if (countdown_enabled == true && countdown == 0) {
    countdown_enabled = false;
    setRele(false);
    timeoutBlinkSecuence();
  }

  // print count down
  // params: number to display, ':' simbol, fill with zeros, digits to use (right->left), position (0,1,2,3)
  int timelyCountdown = convertToTimelyNumber(countdown);
  display.showNumberDecEx(timelyCountdown, 0b11100000, false, 4, 0);
  delay(100);
}


int convertToTimelyNumber(int countNumber) {

  int hours = countNumber / 60;
  int remainder = countNumber % 60;  
  return hours * 100 + remainder;
}

void setBeep(int frecuency, int duration) {
  
  tone(PIN_BUZZER, frecuency); // send 450Hz sound signal
  delay(duration);
  noTone(PIN_BUZZER); // stop sound
}

void setRele(bool status) {
  
  if (status == true) {
    digitalWrite(PIN_RELE, HIGH);
  } else {
    digitalWrite(PIN_RELE, LOW);
  }
}

void timeoutBlinkSecuence() {
  
  for (int i = 0; i < 4; i++) {
    display.setSegments(segments_all_off, 4, 0);
    delay(300);
    display.showNumberDecEx(0, 0b11100000, false, 4, 0);
    setBeep();
    delay(300);
  }
}
