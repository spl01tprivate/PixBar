/*
Project: PixBar
Start-Date: 24.06.2022
Author: Marius Gerstner
Description: >-


Bugs:

T/D:

*/

//***** Includes *****
#include <Arduino.h>
#include <lcd_menu.h>
#include <dmx.h>
#include <gpio_input.h>
#include <led.h>

//***** Defines *****
// TX Data
#define TRANSMISSION_INTERVAL 60000 // milliseconds

//***** Variables & Objects *****
// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal_I2C *ptrLCD;

// LED Parameters
// Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WS2812FX *ptrws2812fx;

// User Data Struct & Pointer
userDataStruct *ptrUserData, userData;

//***** Prototypes *****
void setup(void);
void loop(void);
void handlers(void);

// WS2812FX helper functions
void rmtShow()
{
  uint8_t *pixels = ws2812fx.getPixels();
  uint16_t numBytes = ws2812fx.getNumBytes() + 1;
  rmt_write_sample(RMT_CHANNEL_0, pixels, numBytes, false); // channel 0
}

void led_init_rmt()
{
  ws2812fx.setCustomShow(rmtShow);
}

//***** Setup *****
void setup()
{
  // Serial Initialisation
  if (DEBUG)
    Serial.begin(115200);

  debugln("\n[PixBar] Starting programm ~ by spl01t*#7");
  debugln("[PixBar] You are running version " + String(VERSION) + "!");

  // Pinmodes
  pinMode(input_button1, INPUT_PULLUP);
  pinMode(input_button2, INPUT_PULLUP);
  pinMode(input_button3, INPUT_PULLUP);
  pinMode(input_button4, INPUT_PULLUP);

  // User Data Pointer
  ptrUserData = &userData;

  // LCD Pointer
  ptrLCD = &lcd;

  // EEPROM
  eepromToStruct(ptrUserData);

  // WS2812FX Pointer
  ptrws2812fx = &ws2812fx;

  // Init
  lcd_init(ptrLCD, ptrUserData);
  dmx_init(ptrUserData->dmxAdress);
  led_init(ptrws2812fx, led_init_rmt, ptrUserData);

  debugln("\n[PixBar] Initialization completed...starting programm loop!\n");
}

//***** Main Loop *****
void loop()
{
  input_buttons(ptrLCD, ptrUserData);

  dmx_update(ptrUserData);

  lcd_update(ptrLCD, ptrUserData);

  led_update(ptrws2812fx, ptrUserData);

  handlers();
}

//***** Functions *****
void handlers()
{
  yield();
}