  /* ATTiny85 fan speed control using a simple Button
   * 
   * This script runs a fan at various speeds using just one button to step through by 11 steps. 
   * 
   * By default, the lowest it goes is 45%. Some fans will run at high speed
   * when given a PWM duty cycle that's too low; I want to avoid that in the
   * demo. 45% seems like a safe minimum in my testing but other fans may go
   * lower. Test fan was a FoxConn PVB120G12H-PO1 pulled from a Dell Server.
   * 
   * I took the demo project from https://github.com/swvincent/arduino-fan-controller
   * wich comes from the original project by Marcelo Aquino: https://github.com/marceloaqno
   * That program controls the speed using a potentionmeter.
   * 
   * Copyright (C) 2018  Scott W. Vincent
   * Copyright (C) 2014  Marcelo Aquino
   *
   * This program is free software; you can redistribute it and/or modify
   * it under the terms of the GNU General Public License as published by
   * the Free Software Foundation; either version 2 of the License, or
   * (at your option) any later version.
   *
   * This program is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   * GNU General Public License for more details.
   *
   * You should have received a copy of the GNU General Public License along
   * with this program; if not, write to the Free Software Foundation, Inc.,
   * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
   * 
   * Hookup guide:
   * 
   *                         ATtiny85
   *                      -------u-------
   *  RST - A0 - (D 5) --| 1 PB5   VCC 8 |-- +5V
   *                     |               |
   *        A3 - (D 3) --| 2 PB3   PB2 7 |-- (D 2)
   *                     |               | 
   *        A2 - (D 4) --| 3 PB4   PB1 6 |-- (D 1) - PWM --> Fan Blue wire
   *                     |               |      
   *              Gnd ---| 4 GND   PB0 5 |-- (D 0) - Button --> to cycle through speed settings
   *                     -----------------
   * 
   * It's very important to remember connecting both ground leads if running a seperate 12V and 5V PSU. Otherwise the PWM Pin doesn't know which level its at.
   */
   
  // normal delay() won't work anymore because we are changing Timer1 behavior
  // Adds delay_ms and delay_us functions
#include <util/delay.h>    // Adds delay_ms and delay_us functions
 // Clock at 8mHz
#define F_CPU 8000000         // This is used by delay.h library
const int buttonPin = 0; 
const int PWM_PIN = 1;        // Only works with Pin 1(PB1)
const int LOWER_BOUND = 72;   // 45% mimimum (45 * (160 / 100))
int buttonState = 0;
int pwmValue = LOWER_BOUND;
 
void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(PWM_PIN, OUTPUT); 
  // Phase Correct PWM Mode, no Prescaler
  // PWM on Pin 1(PB1)
  // 8Mhz / 160 / 2 = 25Khz
  TCCR0A = _BV(COM0B1) | _BV(WGM00);
  TCCR0B = _BV(WGM02) | _BV(CS00); 
  // Set MAX
  OCR0A = 160;  // TOP - DO NOT CHANGE, SETS PWM PULSE RATE
  // Run fan at 100% speed for at least 2 seconds to ensure it starts.
  // Intel spec says run at 30% speed max on startup but this
  // is easier. You can change the initial value of 0CR0B if
  // you want it to be "correct".
  OCR0B = 160;    // duty cycle for Pin 1(PB1)
  _delay_ms(2000); // wait 2 sec.
}

void loop() {
  buttonState = digitalRead(buttonPin);
  //if button pressed and pwmValue < than max, increase pwm by 4 steps
  if (buttonState == LOW) {
    if (pwmValue < 160) {        
      _delay_ms(50);   
      OCR0B = pwmValue;
      _delay_ms(50);
      pwmValue += 4;
    }
   }
  if (buttonState == HIGH) {
    _delay_ms(50);      // if button released / not pressed - wait 50mS for debounce and then check again
  }
  //if pwmValue is max (160 or more), reset to lowest speed
  if( pwmValue >= 160){
    pwmValue = LOWER_BOUND;
    OCR0B = pwmValue;
    _delay_ms(50);
  }
}