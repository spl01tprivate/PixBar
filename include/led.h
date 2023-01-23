#ifndef LED_h
#define LED_h

#include <Arduino.h>
#include <constData.h>
#include <WS2812FX.h>
#include "ESP32_RMT_Driver.h"

void IRAM_ATTR strobeTimer(void);

void led_init(WS2812FX *ws2812fx, void (*led_init_show)(), userDataStruct *ptrUserData);

void led_update(WS2812FX *ws2812fx, userDataStruct *ptrUserData);

uint8_t led_dmx(WS2812FX *ws2812fx, userDataStruct *ptrUserData);

uint8_t led_calc_color(uint8_t brtns, uint8_t colorVal, uint8_t colorKey);

uint8_t led_mappedBrtns(userDataStruct *ptrUserData);

uint16_t led_mappedSpeed(userDataStruct *ptrUserData, uint8_t opMode);

uint8_t led_mappedFXSize(userDataStruct *ptrUserData, uint8_t opMode);

uint32_t led_mappedColor_dmx(uint8_t dmxData);

uint32_t led_mappedColor_prg(uint8_t key);

void led_strobe(WS2812FX *ws2812fx, userDataStruct *ptrUserData);

void led_program(WS2812FX *ws2812fx, userDataStruct *ptrUserData);

void led_prg_1(WS2812FX *ws2812fx);
void led_prg_2(WS2812FX *ws2812fx);
void led_prg_3(WS2812FX *ws2812fx);
void led_prg_4(WS2812FX *ws2812fx);
void led_prg_5(WS2812FX *ws2812fx);
void led_prg_6(WS2812FX *ws2812fx);
void led_prg_7(WS2812FX *ws2812fx);
void led_prg_8(WS2812FX *ws2812fx, userDataStruct *ptrUserData);
void led_prg_8_stop(userDataStruct *ptrUserData);
void led_prg_9(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t *colors[3]);
void led_prg_10(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t *colors[3]);
void led_prg_11(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t *colors[3]);
void led_prg_12(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t *colors[3]);

#endif