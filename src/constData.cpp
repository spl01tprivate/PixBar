/*
 * constData.cpp
 *
 *  Created on: 01-07-2022
 *      Author: Sploit
 */

// *** INCLUDES ***
#include <constData.h>

// *** Defines ***

// *** Variables ***
portMUX_TYPE timerStrobeMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool strobeInterruptLocal = false;

// *** Prototypes ***
void eepromToStruct(userDataStruct *ptrUserData);

// *** Functions ***
void eepromToStruct(userDataStruct *ptrUserData)
{
    EEPROM.begin(EEPROMSIZE);

    debugln("\n[EEPROM] Beginning data extraction!");

    uint8_t data;

    // Program
    data = EEPROM.read(programAdress);
    if (data >= DATA_MIN_PROGRAM && data <= DATA_MAX_PROGRAM)
        ptrUserData->program = data;
    else
        ptrUserData->program = DATA_MIN_PROGRAM;

    debugln("[EEPROM] Program: " + String(data));

    // Speed
    data = EEPROM.read(speedAdress);
    if (data >= DATA_MIN_SPEED && data <= DATA_MAX_SPEED)
        ptrUserData->speed = data;
    else
        ptrUserData->speed = DATA_MAX_SPEED / 2;

    debugln("[EEPROM] Speed: " + String(data));

    // Brtns
    data = EEPROM.read(brtnsAdress);
    if (data >= DATA_MIN_BRTNS && data <= DATA_MAX_BRTNS)
        ptrUserData->brtns = data;
    else
        ptrUserData->brtns = DATA_MAX_BRTNS;

    debugln("[EEPROM] Brtns: " + String(data));

    // FX Size
    data = EEPROM.read(fxsizeAdress);
    if (data >= DATA_MIN_FXSIZE && data <= DATA_MAX_FXSIZE)
        ptrUserData->fxSize = data;
    else
        ptrUserData->fxSize = DATA_MAX_FXSIZE / 2;

    debugln("[EEPROM] FXSize: " + String(data));

    // Operation Mode
    data = EEPROM.read(opModeAdress);
    if (data >= DATA_MIN_OPMODE && data <= DATA_MAX_OPMODE)
    {
        if (data == 0)
            ptrUserData->dmxOperation = true;
        else if (data == 1)
            ptrUserData->dmxOperation = false;
    }
    else
        ptrUserData->dmxOperation = true;

    debugln("[EEPROM] Operation Mode: " + String(data));

    // Color 1
    data = EEPROM.read(color1Adress);
    if (data >= DATA_MIN_COLOR && data <= DATA_MAX_COLOR)
        ptrUserData->colors[0] = data;
    else
        ptrUserData->colors[0] = DATA_MAX_COLOR;

    debugln("[EEPROM] Color 1: " + String(data));

    // Color 2
    data = EEPROM.read(color2Adress);
    if (data >= DATA_MIN_COLOR && data <= DATA_MAX_COLOR)
        ptrUserData->colors[1] = data;
    else
        ptrUserData->colors[1] = DATA_MAX_COLOR;

    debugln("[EEPROM] Color 2: " + String(data));

    // Color 3
    data = EEPROM.read(color3Adress);
    if (data >= DATA_MIN_COLOR && data <= DATA_MAX_COLOR)
        ptrUserData->colors[2] = data;
    else
        ptrUserData->colors[2] = DATA_MAX_COLOR;

    debugln("[EEPROM] Color 3: " + String(data));

    // DMX Adress
    uint16_t data_dmx;
    EEPROM.get(dmxAdrAdress, data_dmx);
    if (data_dmx >= DATA_MIN_DMXADR && data_dmx <= DATA_MAX_DMXADR)
        ptrUserData->dmxAdress = data_dmx;
    else
        ptrUserData->dmxAdress = DATA_MIN_DMXADR;

    debugln("[EEPROM] DMXAdress: " + String(data_dmx));

    debugln("[EEPROM] Finished data extraction!");
}

void IRAM_ATTR strobeTimer()
{
    portENTER_CRITICAL_ISR(&timerStrobeMux);
    strobeInterruptLocal = true;
    portEXIT_CRITICAL_ISR(&timerStrobeMux);
}

bool getStrobeInterrupt()
{
    return strobeInterruptLocal;
}

void setStrobeInterrupt(bool key)
{
    portENTER_CRITICAL(&timerStrobeMux);
    strobeInterruptLocal = key;
    portEXIT_CRITICAL(&timerStrobeMux);
}

void strobeTimerEnable(userDataStruct *ptrUserData)
{
    timerAttachInterrupt(ptrUserData->timerStrobe, &strobeTimer, true);
    if (ptrUserData->strobeTimerActive)
    {
        timerAlarmWrite(ptrUserData->timerStrobe, ptrUserData->strobeFreqTime, true); // 50ms --> 20Hz
        timerAlarmEnable(ptrUserData->timerStrobe);
    }
    else
        timerAlarmDisable(ptrUserData->timerStrobe);
}

void strobeTimerDisable(userDataStruct *ptrUserData)
{
    timerAlarmDisable(ptrUserData->timerStrobe);
    timerDetachInterrupt(ptrUserData->timerStrobe);
    delay(10);
}