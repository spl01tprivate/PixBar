#ifndef DMX_h
#define DMX_h

#include <Arduino.h>
#include <constData.h>
#include <esp_dmx.h>

void dmx_init(uint16_t dmxAdress);

uint8_t dmx_update(userDataStruct* ptrUserData);

#endif