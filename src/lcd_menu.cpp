/*
 * lcd_menu.cpp
 *
 *  Created on: 24-06-2022
 *      Author: Sploit
 */

// *** INCLUDES ***
#include <lcd_menu.h>

// *** Defines ***
#define PAGEMAX 3
#define SUBPAGEMAX 6
#define LCD_UPDATE_RATE 200

// *** Variables ***
uint8_t layer = 0; // Change these values to start on certain page on power on
uint8_t page = 0;
uint8_t subPage = 0;

// *** Prototypes ***
void lcd_init(LiquidCrystal_I2C *lcd, userDataStruct *ptrUserData);
String lcd_color_string(uint8_t key);
void lcd_menu(LiquidCrystal_I2C *lcd, uint8_t key, userDataStruct *ptrUserData);
uint8_t lcd_update(LiquidCrystal_I2C *lcd, userDataStruct *ptrUserData);

// *** Functions ***
void lcd_init(LiquidCrystal_I2C *lcd, userDataStruct *ptrUserData)
{
    // Check for last operation mode and set corresponding page (dmx || program)
    if (!ptrUserData->dmxOperation)
        page = 1;

    lcd->init();
    lcd->clear();
    lcd_menu(lcd, 0, ptrUserData);
    lcd->backlight();
}

String lcd_color_string(uint8_t key)
{
    String retval = "ERROR";

    switch (key)
    {
    case 1:
        retval = "BLACK";
        break;
    case 2:
        retval = "RED";
        break;
    case 3:
        retval = "ORANGE";
        break;
    case 4:
        retval = "YELLOW";
        break;
    case 5:
        retval = "GREEN";
        break;
    case 6:
        retval = "CYAN";
        break;
    case 7:
        retval = "BLUE";
        break;
    case 8:
        retval = "PURPLE";
        break;
    case 9:
        retval = "MAGENTA";
        break;
    case 10:
        retval = "PINK";
        break;
    case 11:
        retval = "GRAY";
        break;
    case 12:
        retval = "WHITE";
        break;
    }

    return retval;
}

void lcd_menu(LiquidCrystal_I2C *lcd, uint8_t key, userDataStruct *ptrUserData)
{
    // Compute input

    // debugln("Old | Layer: " + String(layer) + " | Page: " + String(page) + " | SubPage: " + String(subPage));

    if (layer == 0)
    {
        if (page == 0)
        {
            switch (key)
            {
            case 0:
                ptrUserData->dmxOperation = true;
                ptrUserData->changedOperation = true;
                ptrUserData->changedOpMode = true;
                taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                EEPROM.write(opModeAdress, 0);
                EEPROM.commit();
                taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                break;
            case 1:
                layer++;
                if (!ptrUserData->dmxOperation)
                {
                    ptrUserData->dmxOperation = true;
                    ptrUserData->changedOperation = true;
                    ptrUserData->changedOpMode = true;
                }
                taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                EEPROM.write(opModeAdress, 0);
                EEPROM.commit();
                taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                break;
            case 2:
                layer++;
                if (!ptrUserData->dmxOperation)
                {
                    ptrUserData->dmxOperation = true;
                    ptrUserData->changedOperation = true;
                    ptrUserData->changedOpMode = true;
                }
                taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                EEPROM.write(opModeAdress, 0);
                EEPROM.commit();
                taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                break;
            case 3:
                page++;
                break;
            case 4:
                page = PAGEMAX;
                break;
            }
        }
        else if (page == 1)
        {
            switch (key)
            {
            case 0:
                ptrUserData->dmxOperation = false;
                ptrUserData->changedOperation = true;
                ptrUserData->changedOpMode = true;
                taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                EEPROM.write(opModeAdress, 1);
                EEPROM.commit();
                taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                break;
            case 1:
                layer++;
                if (ptrUserData->dmxOperation)
                {
                    ptrUserData->dmxOperation = false;
                    ptrUserData->changedOperation = true;
                    ptrUserData->changedOpMode = true;
                }
                taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                EEPROM.write(opModeAdress, 1);
                EEPROM.commit();
                taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                subPage = 0;
                break;
            case 2:
                layer++;
                if (ptrUserData->dmxOperation)
                {
                    ptrUserData->dmxOperation = false;
                    ptrUserData->changedOperation = true;
                    ptrUserData->changedOpMode = true;
                }
                taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                EEPROM.write(opModeAdress, 1);
                EEPROM.commit();
                taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                subPage = 0;
                break;
            case 3:
                page++;
                break;
            case 4:
                page--;
                break;
            }
        }
        else if (page == 2)
        {
            switch (key)
            {
            case 1:
                layer++;
                break;
            case 2:
                layer++;
                break;
            case 3:
                page++;
                break;
            case 4:
                page--;
                break;
            }
        }
        else if (page == 3)
        {
            switch (key)
            {
            case 1:
                layer++;
                ptrUserData->dmxInfoTab = true;
                break;
            case 2:
                layer++;
                ptrUserData->dmxInfoTab = true;
                break;
            case 3:
                page = 0;
                break;
            case 4:
                page--;
                break;
            }
        }
    }
    else if (layer == 1)
    {
        if (page == 0)
        {
            switch (key)
            {
            case 1:
                layer--;
                taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                EEPROM.put(dmxAdrAdress, ptrUserData->dmxAdress);
                EEPROM.commit();
                taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                break;
            case 2:
                layer--;
                taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                EEPROM.put(dmxAdrAdress, ptrUserData->dmxAdress);
                EEPROM.commit();
                taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                break;
            case 3:
                if (ptrUserData->dmxAdress < 512)
                    ptrUserData->dmxAdress++;
                else if (ptrUserData->dmxAdress >= 512)
                    ptrUserData->dmxAdress = 1;
                break;
            case 4:
                if (ptrUserData->dmxAdress > 1)
                    ptrUserData->dmxAdress--;
                else if (ptrUserData->dmxAdress <= 1)
                    ptrUserData->dmxAdress = 512;
                break;
            }
        }
        else if (page == 1)
        {
            if (subPage == 0)
            {
                switch (key)
                {
                case 1:
                    layer--;
                    break;
                case 2:
                    layer++;
                    break;
                case 3:
                    subPage++;
                    break;
                case 4:
                    subPage = SUBPAGEMAX;
                    break;
                }
            }
            else if (subPage == 1)
            {
                switch (key)
                {
                case 1:
                    layer--;
                    break;
                case 2:
                    layer++;
                    break;
                case 3:
                    subPage++;
                    break;
                case 4:
                    subPage--;
                    break;
                }
            }
            else if (subPage == 2)
            {
                switch (key)
                {
                case 1:
                    layer--;
                    break;
                case 2:
                    layer++;
                    break;
                case 3:
                    subPage++;
                    break;
                case 4:
                    subPage--;
                    break;
                }
            }
            else if (subPage == 3)
            {
                switch (key)
                {
                case 1:
                    layer--;
                    break;
                case 2:
                    layer++;
                    break;
                case 3:
                    subPage++;
                    break;
                case 4:
                    subPage--;
                    break;
                }
            }
            else if (subPage == 4)
            {
                switch (key)
                {
                case 1:
                    layer--;
                    break;
                case 2:
                    layer++;
                    break;
                case 3:
                    subPage++;
                    break;
                case 4:
                    subPage--;
                    break;
                }
            }
            else if (subPage == 5)
            {
                switch (key)
                {
                case 1:
                    layer--;
                    break;
                case 2:
                    layer++;
                    break;
                case 3:
                    subPage++;
                    break;
                case 4:
                    subPage--;
                    break;
                }
            }
            else if (subPage == 6)
            {
                switch (key)
                {
                case 1:
                    layer--;
                    break;
                case 2:
                    layer++;
                    break;
                case 3:
                    subPage = 0;
                    break;
                case 4:
                    subPage--;
                    break;
                }
            }
        }
        else if (page == 2)
        {
            switch (key)
            {
            case 1:
                layer--;
                break;
            case 2:
                layer--;
                break;
            case 3:
                break;
            case 4:
                break;
            }
        }
        else if (page == 3)
        {
            switch (key)
            {
            case 1:
                layer--;
                ptrUserData->dmxInfoTab = false;
                break;
            case 2:
                layer--;
                ptrUserData->dmxInfoTab = false;
                break;
            case 3:
                break;
            case 4:
                break;
            }
        }
    }
    else if (layer == 2)
    {
        if (page == 1)
        {
            if (subPage == 0)
            {
                switch (key)
                {
                case 1:
                    break;
                case 2:
                    layer--;
                    taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                    EEPROM.write(programAdress, ptrUserData->program);
                    EEPROM.commit();
                    taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                    break;
                case 3:
                    if (ptrUserData->program < DATA_MAX_PROGRAM)
                        ptrUserData->program++;
                    else if (ptrUserData->program >= DATA_MAX_PROGRAM)
                        ptrUserData->program = DATA_MIN_PROGRAM;
                    ptrUserData->changedOperation = true;
                    break;
                case 4:
                    if (ptrUserData->program > DATA_MIN_PROGRAM)
                        ptrUserData->program--;
                    else if (ptrUserData->program <= DATA_MIN_PROGRAM)
                        ptrUserData->program = DATA_MAX_PROGRAM;
                    ptrUserData->changedOperation = true;
                    break;
                }
            }
            else if (subPage == 1)
            {
                switch (key)
                {
                case 1:
                    break;
                case 2:
                    layer--;
                    taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                    EEPROM.write(speedAdress, ptrUserData->speed);
                    EEPROM.commit();
                    taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                    break;
                case 3:
                    if (ptrUserData->speed < DATA_MAX_SPEED)
                        ptrUserData->speed++;
                    else if (ptrUserData->speed >= DATA_MAX_SPEED)
                        ptrUserData->speed = DATA_MIN_SPEED;
                    ptrUserData->changedOperation = true;
                    break;
                case 4:
                    if (ptrUserData->speed > DATA_MIN_SPEED)
                        ptrUserData->speed--;
                    else if (ptrUserData->speed <= DATA_MIN_SPEED)
                        ptrUserData->speed = DATA_MAX_SPEED;
                    ptrUserData->changedOperation = true;
                    break;
                }
            }
            else if (subPage == 2)
            {
                switch (key)
                {
                case 1:
                    break;
                case 2:
                    layer--;
                    taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                    EEPROM.write(brtnsAdress, ptrUserData->brtns);
                    EEPROM.commit();
                    taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                    break;
                case 3:
                    if (ptrUserData->brtns < DATA_MAX_BRTNS)
                        ptrUserData->brtns++;
                    else if (ptrUserData->brtns >= DATA_MAX_BRTNS)
                        ptrUserData->brtns = DATA_MIN_BRTNS;
                    ptrUserData->changedOperation = true;
                    break;
                case 4:
                    if (ptrUserData->brtns > DATA_MIN_BRTNS)
                        ptrUserData->brtns--;
                    else if (ptrUserData->brtns <= DATA_MIN_BRTNS)
                        ptrUserData->brtns = DATA_MAX_BRTNS;
                    ptrUserData->changedOperation = true;
                    break;
                }
            }
            else if (subPage == 3)
            {
                switch (key)
                {
                case 1:
                    break;
                case 2:
                    layer--;
                    taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                    EEPROM.write(fxsizeAdress, ptrUserData->fxSize);
                    EEPROM.commit();
                    taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                    break;
                case 3:
                    if (ptrUserData->fxSize < DATA_MAX_FXSIZE)
                        ptrUserData->fxSize++;
                    else if (ptrUserData->fxSize >= DATA_MAX_FXSIZE)
                        ptrUserData->fxSize = DATA_MIN_FXSIZE;
                    ptrUserData->changedOperation = true;
                    break;
                case 4:
                    if (ptrUserData->fxSize > DATA_MIN_FXSIZE)
                        ptrUserData->fxSize--;
                    else if (ptrUserData->fxSize <= DATA_MIN_FXSIZE)
                        ptrUserData->fxSize = DATA_MAX_FXSIZE;
                    ptrUserData->changedOperation = true;
                    break;
                }
            }
            else if (subPage == 4)
            {
                switch (key)
                {
                case 1:
                    break;
                case 2:
                    layer--;
                    taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                    EEPROM.write(color1Adress, ptrUserData->colors[0]);
                    EEPROM.commit();
                    taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                    break;
                case 3:
                    if (ptrUserData->colors[0] < DATA_MAX_COLOR)
                        ptrUserData->colors[0]++;
                    else if (ptrUserData->colors[0] >= DATA_MAX_COLOR)
                        ptrUserData->colors[0] = DATA_MIN_COLOR;
                    ptrUserData->changedOperation = true;
                    break;
                case 4:
                    if (ptrUserData->colors[0] > DATA_MIN_COLOR)
                        ptrUserData->colors[0]--;
                    else if (ptrUserData->colors[0] <= DATA_MIN_COLOR)
                        ptrUserData->colors[0] = DATA_MAX_COLOR;
                    ptrUserData->changedOperation = true;
                    break;
                }
            }
            else if (subPage == 5)
            {
                switch (key)
                {
                case 1:
                    break;
                case 2:
                    layer--;
                    taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                    EEPROM.write(color2Adress, ptrUserData->colors[1]);
                    EEPROM.commit();
                    taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                    break;
                case 3:
                    if (ptrUserData->colors[1] < DATA_MAX_COLOR)
                        ptrUserData->colors[1]++;
                    else if (ptrUserData->colors[1] >= DATA_MAX_COLOR)
                        ptrUserData->colors[1] = DATA_MIN_COLOR;
                    ptrUserData->changedOperation = true;
                    break;
                case 4:
                    if (ptrUserData->colors[1] > DATA_MIN_COLOR)
                        ptrUserData->colors[1]--;
                    else if (ptrUserData->colors[1] <= DATA_MIN_COLOR)
                        ptrUserData->colors[1] = DATA_MAX_COLOR;
                    ptrUserData->changedOperation = true;
                    break;
                }
            }
            else if (subPage == 6)
            {
                switch (key)
                {
                case 1:
                    break;
                case 2:
                    layer--;
                    taskENTER_CRITICAL(&ptrUserData->EEPROMMutex);
                    EEPROM.write(color3Adress, ptrUserData->colors[2]);
                    EEPROM.commit();
                    taskEXIT_CRITICAL(&ptrUserData->EEPROMMutex);
                    break;
                case 3:
                    if (ptrUserData->colors[2] < DATA_MAX_COLOR)
                        ptrUserData->colors[2]++;
                    else if (ptrUserData->colors[2] >= DATA_MAX_COLOR)
                        ptrUserData->colors[2] = DATA_MIN_COLOR;
                    ptrUserData->changedOperation = true;
                    break;
                case 4:
                    if (ptrUserData->colors[2] > DATA_MIN_COLOR)
                        ptrUserData->colors[2]--;
                    else if (ptrUserData->colors[2] <= DATA_MIN_COLOR)
                        ptrUserData->colors[2] = DATA_MAX_COLOR;
                    ptrUserData->changedOperation = true;
                    break;
                }
            }
        }
    }

    // debugln("New | Layer: " + String(layer) + " | Page: " + String(page) + " | SubPage: " + String(subPage));

    // Display output

    if (layer == 0)
    {
        if (page == 0)
        {
            lcd->clear();
            lcd->print(" *** PixBar *** ");
            lcd->setCursor(0, 1);
            lcd->blink_off();
            lcd->print("DMX-Adress: " + String(ptrUserData->dmxAdress));
        }
        else if (page == 1)
        {
            lcd->clear();
            lcd->print(" *** PixBar *** ");
            lcd->setCursor(0, 1);
            lcd->print("Programs");
        }
        else if (page == 2)
        {
            lcd->clear();
            lcd->print(" *** PixBar *** ");
            lcd->setCursor(0, 1);
            lcd->print("Version");
        }
        else if (page == 3)
        {
            lcd->clear();
            lcd->print(" *** PixBar *** ");
            lcd->setCursor(0, 1);
            lcd->print("DMX-Info");
        }
    }
    else if (layer == 1)
    {
        if (page == 0)
        {
            lcd->clear();
            lcd->print(" *** PixBar *** ");
            lcd->setCursor(0, 1);
            lcd->blink_on();
            lcd->print("DMX-Adress: " + String(ptrUserData->dmxAdress));
        }
        else if (page == 1)
        {
            if (subPage == 0)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_off();
                lcd->print("Program: " + String(ptrUserData->program));
            }
            else if (subPage == 1)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_off();
                lcd->print("Speed: " + String(ptrUserData->speed));
            }
            else if (subPage == 2)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_off();
                lcd->print("Brtns: " + String(ptrUserData->brtns));
            }
            else if (subPage == 3)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_off();
                lcd->print("FXSize: " + String(ptrUserData->fxSize));
            }
            else if (subPage == 4)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_off();
                lcd->print("Col1: " + lcd_color_string(ptrUserData->colors[0]));
            }
            else if (subPage == 5)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_off();
                lcd->print("Col2: " + lcd_color_string(ptrUserData->colors[1]));
            }
            else if (subPage == 6)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_off();
                lcd->print("Col3: " + lcd_color_string(ptrUserData->colors[2]));
            }
        }
        else if (page == 2)
        {
            lcd->clear();
            lcd->print("V" + String(VERSION));
            lcd->setCursor(0, 1);
            lcd->print("by Sploit");
        }
        else if (page == 3)
        {
            lcd->clear();
            for (int i = 0; i < 4; i++)
            {
                lcd->print(String(ptrUserData->dmxData[i]) + " ");
            }

            lcd->setCursor(0, 1);

            for (int i = 4; i < 7; i++)
            {
                lcd->print(String(ptrUserData->dmxData[i]) + " ");
            }
        }
    }
    else if (layer == 2)
    {
        if (page == 1)
        {
            if (subPage == 0)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_on();
                lcd->print("Program: " + String(ptrUserData->program));
            }
            else if (subPage == 1)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_on();
                lcd->print("Speed: " + String(ptrUserData->speed));
            }
            else if (subPage == 2)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_on();
                lcd->print("Brtns: " + String(ptrUserData->brtns));
            }
            else if (subPage == 3)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_on();
                lcd->print("FXSize: " + String(ptrUserData->fxSize));
            }
            else if (subPage == 4)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_on();
                lcd->print("Col1: " + lcd_color_string(ptrUserData->colors[0]));
            }
            else if (subPage == 5)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_on();
                lcd->print("Col2: " + lcd_color_string(ptrUserData->colors[1]));
            }
            else if (subPage == 6)
            {
                lcd->clear();
                lcd->print("*** Programs ***");
                lcd->setCursor(0, 1);
                lcd->blink_on();
                lcd->print("Col3: " + lcd_color_string(ptrUserData->colors[2]));
            }
        }
    }
}

uint8_t lcd_update(LiquidCrystal_I2C *lcd, userDataStruct *ptrUserData)
{
    static unsigned long updIntv = 0;

    if (millis() < updIntv + LCD_UPDATE_RATE)
        return 0;
    else
        updIntv = millis();

    if (layer == 1 && page == 3)
    {
        lcd_menu(lcd, 0, ptrUserData);
    }

    return 1;
}