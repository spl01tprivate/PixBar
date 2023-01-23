/*
 * led.cpp
 *
 *  Created on: 26-06-2022
 *      Author: Sploit
 */

// *** INCLUDES ***
#include <led.h>

// *** Defines ***
#define ANIMATION_RATE 1

// *** Variables ***
uint8_t dmxDataColors[3] = {0, 0, 0};

bool strobeState = false;

// *** Prototypes ***
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
void led_prg_9(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t colors[3]);
void led_prg_10(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t colors[3]);
void led_prg_11(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t colors[3]);
void led_prg_12(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t colors[3]);

// *** Functions ***
/*!
   @brief   Initialises strobeTimer & ws2812fx system.
            Links ESP32 RMT peripheral to pixel library.
   @return
   @note
*/
void led_init(WS2812FX *ws2812fx, void (*led_init_show)(), userDataStruct *ptrUserData)
{
    // Strobe interrupt
    ptrUserData->timerStrobe = timerBegin(0, 80, true); // Prescaler 80 (ESP32 80MHz)
    strobeTimerEnable(ptrUserData);

    ws2812fx->init();
    ws2812fx->setBrightness(255);
    rmt_tx_int(RMT_CHANNEL_0, ws2812fx->getPin());
    led_init_show();
    ws2812fx->clear();
    ws2812fx->start();
}

/*!
   @brief   Checks which operation mode is active and executes dmx or progam function.
            Also handels strobe functions, which is called if timer set specific flag.
            Executes ws2812fx animation service if program is active in dmx mode.
   @return
   @note
*/
void led_update(WS2812FX *ws2812fx, userDataStruct *ptrUserData)
{
    if (ptrUserData->dmxOperation)
        led_dmx(ws2812fx, ptrUserData);
    else
        led_program(ws2812fx, ptrUserData);

    if (getStrobeInterrupt())
        led_strobe(ws2812fx, ptrUserData);

    if (ptrUserData->dmxData[3] > 0 && !strobeState)
        ws2812fx->service();
}

/*!
   @brief   Computes dmx values and outputs pixel animations.
            Checks for color, brtns, strobe parameters.
   @return  0 blocked execution | 1 successful execution
   @note
*/
uint8_t led_dmx(WS2812FX *ws2812fx, userDataStruct *ptrUserData)
{
    static unsigned long lastExec = 0;
    static bool strobeFinished = false;
    bool led_changes_applied = false;
    static uint8_t prg_last_speed = 0;
    static uint8_t prg_last_brtns = 0;
    static uint8_t prg_last_fxsize = 0;
    static uint8_t prg_last_color1 = 0;
    static uint8_t prg_last_color2 = 0;
    static uint8_t prg_last_color3 = 0;
    static uint32_t colors[3] = {0, 0, 0};
    uint8_t opMode = 0;

    // Check for active strobe from led program 8
    if (ptrUserData->ledPrgStrobe && ptrUserData->changedOperation)
    {
        led_prg_8_stop(ptrUserData);
    }

    // Check for elapsed time
    if (millis() < lastExec + ANIMATION_RATE)
    {
        if (!ptrUserData->strobeTimerActive || strobeState)
            return 0;
    }
    else
        lastExec = millis();

    // Prepare to do changes if strobe mode active, but not "blackouting"
    if (ptrUserData->dmxData[5] > 0 && !strobeState && ptrUserData->strobeTimerActive || strobeFinished)
    {
        if (strobeFinished)
            strobeFinished = false;

        ptrUserData->dmxDataChange[6] = true;
    }

    // Compute changes
    if (ptrUserData->dmxData[3] == 0)
    {
        // Compute these only if program is not active - conflict with setSegment and fill methods
        // Colors
        if ((ptrUserData->dmxDataChange[0] || ptrUserData->dmxDataChange[1] || ptrUserData->dmxDataChange[2]) && !ptrUserData->dmxDataChange[6] && !strobeState) // Only colors
        {
            if (ptrUserData->dmxDataChange[0])
                led_calc_color(ptrUserData->dmxData[6], ptrUserData->dmxData[0], 0);
            if (ptrUserData->dmxDataChange[1])
                led_calc_color(ptrUserData->dmxData[6], ptrUserData->dmxData[1], 1);
            if (ptrUserData->dmxDataChange[2])
                led_calc_color(ptrUserData->dmxData[6], ptrUserData->dmxData[2], 2);

            ws2812fx->fill(ws2812fx->Color(dmxDataColors[0], dmxDataColors[1], dmxDataColors[2]), 0, LED_COUNT);
            led_changes_applied = true;
        }
        // Dimmer + colors
        if ((ptrUserData->dmxDataChange[6] && !strobeState) || ptrUserData->changedOperation)
        {
            if (ptrUserData->changedOperation)
            {
                ptrUserData->changedOperation = false;
                ws2812fx->setBrightness(255);
            }

            for (int i = 0; i < 3; i++)
            {
                led_calc_color(ptrUserData->dmxData[6], ptrUserData->dmxData[i], i);
            }

            ws2812fx->fill(ws2812fx->Color(dmxDataColors[0], dmxDataColors[1], dmxDataColors[2]), 0, LED_COUNT);
            led_changes_applied = true;
        }
    }
    // Strobe
    if (ptrUserData->dmxDataChange[5])
    {
        if (ptrUserData->dmxData[5] > 0)
        {
            // Calculate strobe frequency
            uint8_t hzMapping;

            hzMapping = map(ptrUserData->dmxData[5], 1, 255, 1, 20); // DMX 1 | 1 Hz // DMX 255 | 20 Hz
            ptrUserData->strobeFreqTime = (1000 / hzMapping) * 1000; // Result in micsec

            timerAlarmWrite(ptrUserData->timerStrobe, ptrUserData->strobeFreqTime, true); // 50ms --> 20Hz
            timerAlarmEnable(ptrUserData->timerStrobe);
            ptrUserData->strobeTimerActive = true;

            debugln("[LED] Enabled Strobe");
        }
        else
        {
            // Disable ISR / Timer
            timerAlarmDisable(ptrUserData->timerStrobe);
            ptrUserData->strobeTimerActive = false;

            // Reset vars
            strobeState = false;
            setStrobeInterrupt(false);

            // FLush current pixel data again
            strobeFinished = true;
            led_dmx(ws2812fx, ptrUserData);

            debugln("[LED] Disabled Strobe");
        }
    }
    // Programs
    if (ptrUserData->dmxDataChange[3])
    {
        ptrUserData->dmxDataChange[3] = false;

        if (ptrUserData->dmxData[3] > 0 && ptrUserData->dmxData[3] < 64)
        {
            led_prg_9(ws2812fx, ptrUserData, opMode, colors);
        }
        else if (ptrUserData->dmxData[3] >= 64 && ptrUserData->dmxData[3] < 128)
        {
            led_prg_10(ws2812fx, ptrUserData, opMode, colors);
        }
        else if (ptrUserData->dmxData[3] >= 128 && ptrUserData->dmxData[3] < 192)
        {
            led_prg_11(ws2812fx, ptrUserData, opMode, colors);
        }
        else if (ptrUserData->dmxData[3] >= 192)
        {
            led_prg_12(ws2812fx, ptrUserData, opMode, colors);
        }
    }
    // Speed & brtns handlers during program dmx mode
    if (ptrUserData->dmxData[3] > 0)
    {
        // Speed
        if (ptrUserData->dmxData[4] != prg_last_speed || ptrUserData->changedOpMode)
        {
            prg_last_speed = ptrUserData->dmxData[4];
            ws2812fx->setSpeed(led_mappedSpeed(ptrUserData, opMode));
        }
        // Brtns
        if (ptrUserData->dmxData[6] != prg_last_brtns || ptrUserData->changedOpMode)
        {
            prg_last_brtns = ptrUserData->dmxData[6];
            ws2812fx->setBrightness(ptrUserData->dmxData[6]);
        }
        // FXSize
        if (ptrUserData->dmxData[7] != prg_last_fxsize || ptrUserData->changedOpMode)
        {
            prg_last_fxsize = ptrUserData->dmxData[7];
            ws2812fx->setOptions(0, led_mappedFXSize(ptrUserData, opMode));
        }
        // Color 1
        if (ptrUserData->dmxData[0] != prg_last_color1 || ptrUserData->changedOpMode)
        {
            prg_last_color1 = ptrUserData->dmxData[0];
            colors[0] = led_mappedColor_dmx(ptrUserData->dmxData[0]);
            ws2812fx->setColors(0, colors);
        }
        // Color 2
        if (ptrUserData->dmxData[1] != prg_last_color2 || ptrUserData->changedOpMode)
        {
            prg_last_color2 = ptrUserData->dmxData[1];
            colors[1] = led_mappedColor_dmx(ptrUserData->dmxData[1]);
            ws2812fx->setColors(0, colors);
        }
        // Color 3
        if (ptrUserData->dmxData[2] != prg_last_color3 || ptrUserData->changedOpMode)
        {
            prg_last_color3 = ptrUserData->dmxData[2];
            colors[2] = led_mappedColor_dmx(ptrUserData->dmxData[2]);
            ws2812fx->setColors(0, colors);
        }
    }

    if (ptrUserData->changedOpMode)
        ptrUserData->changedOpMode = false;

    if (led_changes_applied)
        ws2812fx->show();

    return 1;
}

/*!
   @brief   Calculates true color value corresponding to current brtns.
            Stores true color value in array.
   @return  True color value
   @note
*/
uint8_t led_calc_color(uint8_t brtns, uint8_t colorVal, uint8_t colorKey)
{
    float brtnsCoef = brtns / 255.0;
    float trueColor = colorVal * brtnsCoef;

    dmxDataColors[colorKey] = (uint8_t)trueColor;

    return dmxDataColors[colorKey];
}

/*!
   @brief   Maps user friendly brtns value to program range 0 100 -> 0 255
   @return  Newly mapped brtns value
   @note
*/
uint8_t led_mappedBrtns(userDataStruct *ptrUserData)
{
    uint8_t mappedBrtns = map(ptrUserData->brtns, DATA_MIN_BRTNS, DATA_MAX_BRTNS, 0, 255);

    return mappedBrtns;
}

/*!
   @brief   Maps user friendly speed value to program range 0 100 -> DATA_MAP_MIN_SPEED DATA_MAP_MAX_SPEED
   @return  Newly mapped speed value
   @note
*/
uint16_t led_mappedSpeed(userDataStruct *ptrUserData, uint8_t opMode)
{
    uint16_t mappedSpeed;
    if (opMode == 0)
    {
        // DMX
        mappedSpeed = map(ptrUserData->dmxData[4], 0, 255, DATA_MAP_MAX_SPEED, DATA_MAP_MIN_SPEED);
    }
    else if (opMode == 1)
    {
        // Program
        mappedSpeed = map(ptrUserData->speed, DATA_MIN_SPEED, DATA_MAX_SPEED, DATA_MAP_MAX_SPEED, DATA_MAP_MIN_SPEED);
    }

    return mappedSpeed;
}

/*!
   @brief   Maps user friendly fxsize value to program range 1 4 -> FX_SIZE FADE_SIZE...
   @return  Newly mapped fxsize value
   @note
*/
uint8_t led_mappedFXSize(userDataStruct *ptrUserData, uint8_t opMode)
{
    uint8_t mappedFXSize;
    uint8_t mappedSourceVal;

    if (opMode == 0)
    {
        if (ptrUserData->dmxData[7] == 255)
            mappedSourceVal = 4;
        else
            mappedSourceVal = map(ptrUserData->dmxData[7], 0, 255, 1, 5);
    }
    else if (opMode == 1)
    {
        mappedSourceVal = ptrUserData->fxSize;
    }

    switch (mappedSourceVal)
    {
    case 1:
        mappedFXSize = SIZE_SMALL + FADE_XFAST;
        break;
    case 2:
        mappedFXSize = SIZE_MEDIUM + FADE_MEDIUM;
        break;
    case 3:
        mappedFXSize = SIZE_LARGE + FADE_XSLOW;
        break;
    case 4:
        mappedFXSize = SIZE_XLARGE + FADE_GLACIAL;
        break;
    default:
        mappedFXSize = SIZE_SMALL + FADE_XFAST;
        break;
    }

    return mappedFXSize;
}

/*!
   @brief   Maps dmx input data to corresponding color value (0 - 255).
   @return  32 bit color value
   @note
*/
uint32_t led_mappedColor_dmx(uint8_t dmxData)
{
    uint32_t retval;

    if (dmxData >= 0 && dmxData < 21)
    {
        retval = BLACK;
    }
    else if (dmxData >= 21 && dmxData < 42)
    {
        retval = RED;
    }
    else if (dmxData >= 42 && dmxData < 63)
    {
        retval = ORANGE;
    }
    else if (dmxData >= 63 && dmxData < 85)
    {
        retval = YELLOW;
    }
    else if (dmxData >= 85 && dmxData < 106)
    {
        retval = GREEN;
    }
    else if (dmxData >= 106 && dmxData < 127)
    {
        retval = CYAN;
    }
    else if (dmxData >= 127 && dmxData < 148)
    {
        retval = BLUE;
    }
    else if (dmxData >= 148 && dmxData < 170)
    {
        retval = PURPLE;
    }
    else if (dmxData >= 170 && dmxData < 191)
    {
        retval = MAGENTA;
    }
    else if (dmxData >= 191 && dmxData < 212)
    {
        retval = PINK;
    }
    else if (dmxData >= 212 && dmxData < 233)
    {
        retval = GRAY;
    }
    else if (dmxData >= 233)
    {
        retval = WHITE;
    }

    return retval;
}

/*!
   @brief   Maps user input data to corresponding color value (1 - 12).
   @return  32 bit color value
   @note
*/
uint32_t led_mappedColor_prg(uint8_t key)
{
    uint32_t retval;

    switch (key)
    {
    case 1:
        retval = BLACK;
        break;
    case 2:
        retval = RED;
        break;
    case 3:
        retval = ORANGE;
        break;
    case 4:
        retval = YELLOW;
        break;
    case 5:
        retval = GREEN;
        break;
    case 6:
        retval = CYAN;
        break;
    case 7:
        retval = BLUE;
        break;
    case 8:
        retval = PURPLE;
        break;
    case 9:
        retval = MAGENTA;
        break;
    case 10:
        retval = PINK;
        break;
    case 11:
        retval = GRAY;
        break;
    case 12:
        retval = WHITE;
        break;

    default:
        retval = WHITE;
        break;
    }

    return retval;
}

/*!
   @brief   Called if strobeTimer-flag was set.
            Resets flag & inverts strobeState & clears pixels in dmx and program operation.
            But only fills pixels white with corresponding brtns in program operation (only white strobe atm).
   @return
   @note
*/
void led_strobe(WS2812FX *ws2812fx, userDataStruct *ptrUserData)
{
    // Reset ISR flag
    setStrobeInterrupt(false);

    // Set strobe state
    strobeState = !strobeState;

    if (strobeState)
    {
        ws2812fx->fill(0, 0, LED_COUNT);
        ws2812fx->show();
    }
    else if (!strobeState && !ptrUserData->dmxOperation)
    {
        float brtnsCoef = ptrUserData->brtns / 100.0;
        float trueColor = 255 * brtnsCoef;

        ws2812fx->fill(ws2812fx->Color(trueColor, trueColor, trueColor), 0, LED_COUNT);
        ws2812fx->show();
    }
}

/*!
   @brief   Checks for changes in userData to switch to new program or set new brtns / speed / fxSize.
            WS2812FX service is called in here to handle effect animation.
   @return
   @note
*/
void led_program(WS2812FX *ws2812fx, userDataStruct *ptrUserData)
{
    static uint32_t colors[3] = {0, 0, 0};
    static uint8_t lastProgram = ptrUserData->program + 1;
    static uint8_t lastSpeed = ptrUserData->speed + 1;
    static uint8_t lastBrtns = ptrUserData->brtns + 1;
    static uint8_t lastfxSize = ptrUserData->fxSize + 1;
    static uint8_t lastColors[3] = {ptrUserData->colors[0] + 1, ptrUserData->colors[1] + 1, ptrUserData->colors[2] + 1};

    if (ptrUserData->changedOperation)
    {
        ptrUserData->changedOperation = false;

        if (ptrUserData->ledPrgStrobe && ptrUserData->program != 8)
        {
            led_prg_8_stop(ptrUserData);
        }
        // Colors
        bool changedColor = false;
        for (int i = 0; i < 3; i++)
        {
            if ((ptrUserData->colors[i] != lastColors[i] && !ptrUserData->ledPrgStrobe) || ptrUserData->changedOpMode)
            {
                lastColors[i] = ptrUserData->colors[i];
                colors[i] = led_mappedColor_prg(ptrUserData->colors[i]);
                changedColor = true;
            }
        }
        if (changedColor && !ptrUserData->changedOpMode)
            ws2812fx->setColors(0, colors);
        // Program
        if (ptrUserData->changedOpMode || ptrUserData->program != lastProgram || (ptrUserData->program == 8 && (ptrUserData->brtns != lastBrtns || ptrUserData->speed != lastSpeed)))
        {
            lastProgram = ptrUserData->program;

            uint8_t opMode = 1; // 1 = Program mode | 0 = DMX mode, for mapping functions sothat they know which data source they should access (userData or DMX)

            switch (ptrUserData->program)
            {
            case 1:
                led_prg_1(ws2812fx);
                break;
            case 2:
                led_prg_2(ws2812fx);
                break;
            case 3:
                led_prg_3(ws2812fx);
                break;
            case 4:
                led_prg_4(ws2812fx);
                break;
            case 5:
                led_prg_5(ws2812fx);
                break;
            case 6:
                led_prg_6(ws2812fx);
                break;
            case 7:
                led_prg_7(ws2812fx);
                break;
            case 8:
                if (!ptrUserData->ledPrgStrobe)
                    led_prg_8(ws2812fx, ptrUserData);
                if (ptrUserData->speed != lastSpeed)
                {
                    lastSpeed = ptrUserData->speed;
                    led_prg_8(ws2812fx, ptrUserData);
                }
                else if (ptrUserData->brtns != lastBrtns)
                {
                    lastBrtns = ptrUserData->brtns;
                    led_prg_8(ws2812fx, ptrUserData);
                }
                break;
            case 9:
                lastSpeed = ptrUserData->speed;
                lastfxSize = ptrUserData->fxSize;
                led_prg_9(ws2812fx, ptrUserData, opMode, colors);
                break;
            case 10:
                lastSpeed = ptrUserData->speed;
                lastfxSize = ptrUserData->fxSize;
                led_prg_10(ws2812fx, ptrUserData, opMode, colors);
                break;
            case 11:
                lastSpeed = ptrUserData->speed;
                lastfxSize = ptrUserData->fxSize;
                led_prg_11(ws2812fx, ptrUserData, opMode, colors);
                break;
            case 12:
                lastSpeed = ptrUserData->speed;
                lastfxSize = ptrUserData->fxSize;
                led_prg_12(ws2812fx, ptrUserData, opMode, colors);
                break;

            default:
                break;
            }
        }
        // Brtns
        if ((ptrUserData->brtns != lastBrtns && !ptrUserData->ledPrgStrobe) || ptrUserData->changedOpMode)
        {
            lastBrtns = ptrUserData->brtns;
            ws2812fx->setBrightness(led_mappedBrtns(ptrUserData));
        }
        // Speed
        if ((ptrUserData->speed != lastSpeed && !ptrUserData->ledPrgStrobe) || ptrUserData->changedOpMode)
        {
            lastSpeed = ptrUserData->speed;
            ws2812fx->setSpeed(led_mappedSpeed(ptrUserData, 1));
        }
        // FXSize
        if ((ptrUserData->fxSize != lastfxSize && !ptrUserData->ledPrgStrobe) || ptrUserData->changedOpMode)
        {
            lastfxSize = ptrUserData->fxSize;
            ws2812fx->setOptions(0, led_mappedFXSize(ptrUserData, 1));
        }

        if (ptrUserData->changedOpMode)
            ptrUserData->changedOpMode = false;
    }

    if (!ptrUserData->ledPrgStrobe)
        ws2812fx->service();
}

void led_prg_1(WS2812FX *ws2812fx)
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_STATIC, RED);
}

void led_prg_2(WS2812FX *ws2812fx)
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_STATIC, YELLOW);
}

void led_prg_3(WS2812FX *ws2812fx)
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_STATIC, GREEN);
}

void led_prg_4(WS2812FX *ws2812fx)
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_STATIC, CYAN);
}

void led_prg_5(WS2812FX *ws2812fx)
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_STATIC, BLUE);
}

void led_prg_6(WS2812FX *ws2812fx)
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_STATIC, PURPLE);
}

void led_prg_7(WS2812FX *ws2812fx)
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_STATIC, WHITE);
}

/*!
   @brief   Program 8 White Strobe.
            Resets WS2812FX mode to static.
            Starts strobeTimer with corresponding frequency calculated from userData speed.
   @return
   @note
*/
void led_prg_8(WS2812FX *ws2812fx, userDataStruct *ptrUserData)
{
    led_prg_8_stop(ptrUserData);

    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_STATIC);
    ws2812fx->setBrightness(255);

    ptrUserData->ledPrgStrobe = true;

    // Calculate strobe frequency
    uint8_t hzMapping;

    hzMapping = map(ptrUserData->speed, 1, 100, 1, 20);      // DMX 1 | 1 Hz // DMX 255 | 20 Hz
    ptrUserData->strobeFreqTime = (1000 / hzMapping) * 1000; // Result in micsec

    timerAlarmWrite(ptrUserData->timerStrobe, ptrUserData->strobeFreqTime, true); // 50ms --> 20Hz
    timerAlarmEnable(ptrUserData->timerStrobe);
    ptrUserData->strobeTimerActive = true;

    debugln("[LED-Prg] Enabled Strobe");
}

/*!
   @brief   Stop Program 8 White Strobe.
            Resets and disables strobeTimer & vars.
   @return
   @note
*/
void led_prg_8_stop(userDataStruct *ptrUserData)
{
    ptrUserData->ledPrgStrobe = false;

    // Disable ISR / Timer
    timerAlarmDisable(ptrUserData->timerStrobe);
    ptrUserData->strobeTimerActive = false;

    // Reset vars
    strobeState = false;
    setStrobeInterrupt(false);

    debugln("[LED-Prg] Disabled Strobe");
}

void led_prg_9(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t colors[3])
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_COMET, colors, led_mappedSpeed(ptrUserData, opMode), led_mappedFXSize(ptrUserData, opMode));
}

void led_prg_10(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t colors[3])
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_TWINKLE_FADE, colors, led_mappedSpeed(ptrUserData, opMode), led_mappedFXSize(ptrUserData, opMode));
}

void led_prg_11(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t colors[3])
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_RAINBOW, colors, led_mappedSpeed(ptrUserData, opMode), led_mappedFXSize(ptrUserData, opMode));
}

void led_prg_12(WS2812FX *ws2812fx, userDataStruct *ptrUserData, uint8_t opMode, uint32_t colors[3])
{
    ws2812fx->setSegment(0, 0, LED_COUNT - 1, FX_MODE_RUNNING_LIGHTS, colors, led_mappedSpeed(ptrUserData, opMode), led_mappedFXSize(ptrUserData, opMode));
}