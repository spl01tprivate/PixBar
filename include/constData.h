#ifndef CONSTDATA_h
#define CONSTDATA_h

#include <EEPROM.h>

// Version
#define VERSION 1.0

// Debug
#define DEBUG 1 // leave 0 otherwise DMX cant be received

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// Channels
#define CHANNEL_COUNT 8

// User Data Variables Min Max
#define DATA_MIN_PROGRAM 1
#define DATA_MAX_PROGRAM 12
#define DATA_MIN_SPEED 1
#define DATA_MAX_SPEED 100
#define DATA_MIN_BRTNS 0
#define DATA_MAX_BRTNS 100
#define DATA_MIN_FXSIZE 1
#define DATA_MAX_FXSIZE 4
#define DATA_MIN_DMXADR 1
#define DATA_MAX_DMXADR 512
#define DATA_MIN_OPMODE 0
#define DATA_MAX_OPMODE 1
#define DATA_MIN_COLOR 1
#define DATA_MAX_COLOR 12

// Mapping boundaries
#define DATA_MAP_MIN_SPEED 0
#define DATA_MAP_MAX_SPEED 25000

// User Data
struct userDataStruct
{
  // Strobe Interrupt
  hw_timer_t *timerStrobe = NULL;
  bool strobeTimerActive = false;
  uint64_t strobeFreqTime = 1;
  portMUX_TYPE EEPROMMutex = portMUX_INITIALIZER_UNLOCKED;

  // User Data
  uint16_t dmxAdress = DATA_MIN_DMXADR;
  uint8_t dmxData[CHANNEL_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0};
  bool dmxDataChange[CHANNEL_COUNT] = {false, false, false, false, false, false, false, false};
  uint8_t program = DATA_MIN_PROGRAM;
  uint8_t speed = DATA_MAX_SPEED / 2;
  uint8_t brtns = DATA_MAX_BRTNS / 2;
  uint8_t fxSize = DATA_MAX_FXSIZE / 2;
  uint8_t colors[3] = {0, 0, 0};
  bool dmxOperation = true;
  bool changedOperation = false;
  bool changedOpMode = false;
  bool ledPrgStrobe = false;
  bool dmxInfoTab = false;
};

// EEPROM
void eepromToStruct(userDataStruct *ptrUserData);

// Strobe Interrupt & Timer
bool getStrobeInterrupt();
void setStrobeInterrupt(bool key);
void IRAM_ATTR strobeTimer(void);
void strobeTimerEnable(userDataStruct *ptrUserData);
void strobeTimerDisable(userDataStruct *ptrUserData);

// Button-Press Timings
#define LONGPRESS 350    // stores const time when longpress is recognized
#define LONGPRESSCMD 100 // stores const time between lcd func calls

// Pins
#define input_button1 4
#define input_button2 0
#define input_button3 2
#define input_button4 15

#define DMX_TX_PIN 17
#define DMX_RX_PIN 16
#define DMX_EN_PIN 33

#define LED_PIN 12

// WS2812 LEDs
#define LED_COUNT 236 // 4 * 59 LEDs

// EEPROM adresses
#define EEPROMSIZE 10
#define programAdress 0
#define speedAdress 1
#define brtnsAdress 2
#define fxsizeAdress 3
#define dmxAdrAdress 4 // 4 & 5 | 2 Byte
#define opModeAdress 6 
#define color1Adress 7
#define color2Adress 8
#define color3Adress 9

#endif