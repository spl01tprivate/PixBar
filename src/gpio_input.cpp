/*
 * gpio_input.cpp
 *
 *  Created on: 26-06-2022
 *      Author: Sploit
 */

// *** INCLUDES ***
#include <gpio_input.h>

// *** Defines ***

// *** Variables ***

// *** Prototypes ***
void input_buttons(LiquidCrystal_I2C* lcd, userDataStruct *ptrUserData);

// *** Functions ***
void input_buttons(LiquidCrystal_I2C* lcd, userDataStruct *ptrUserData)
{
  static bool input_button1_pressed = false;
  static bool input_button2_pressed = false;
  static bool input_button3_pressed = false;
  static bool input_button4_pressed = false;
  static unsigned long i3_presstime = 0;
  static unsigned long i4_presstime = 0;
  static unsigned long i3_loprLastCmd = 0;
  static unsigned long i4_loprLastCmd = 0;

  if (!digitalRead(input_button1) && !input_button1_pressed)
  {
    debugln("[IN] Button 1 pressed!");
    input_button1_pressed = true;

    lcd_menu(lcd, 1, ptrUserData);
  }
  else if (digitalRead(input_button1))
    input_button1_pressed = false;

  if (!digitalRead(input_button2) && !input_button2_pressed)
  {
    debugln("[IN] Button 2 pressed!");
    input_button2_pressed = true;

    lcd_menu(lcd, 2, ptrUserData);
  }
  else if (digitalRead(input_button2))
    input_button2_pressed = false;

  if (!digitalRead(input_button3) && !input_button3_pressed)
  {
    debugln("[IN] Button 3 pressed!");
    input_button3_pressed = true;

    i3_presstime = millis();

    lcd_menu(lcd, 3, ptrUserData);
  }
  else if (digitalRead(input_button3))
    input_button3_pressed = false;

  if (!digitalRead(input_button4) && !input_button4_pressed)
  {
    debugln("[IN] Button 4 pressed!");
    input_button4_pressed = true;

    i4_presstime = millis();

    lcd_menu(lcd, 4, ptrUserData);
  }
  else if (digitalRead(input_button4))
    input_button4_pressed = false;

  // Check for long press

  if (input_button3_pressed && (millis() > i3_presstime + LONGPRESS) && (millis() > i3_loprLastCmd + LONGPRESSCMD))
  {
    i3_loprLastCmd = millis();
    lcd_menu(lcd, 3, ptrUserData);
  }

  if (input_button4_pressed && (millis() > i4_presstime + LONGPRESS) && (millis() > i4_loprLastCmd + LONGPRESSCMD))
  {
    i4_loprLastCmd = millis();
    lcd_menu(lcd, 4, ptrUserData);
  }
}