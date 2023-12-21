/*
  Boards.h - Hardware Abstraction Layer for Firmata library
  Copyright (c) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2009-2017 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2023 Jens B. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated December 17th, 2023
*/

#ifndef Firmata_Boards_h
#define Firmata_Boards_h

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"  // for digitalRead, digitalWrite, etc
#else
#include "WProgram.h"
#endif

// Normally Servo.h must be included before Firmata.h (which then includes
// this file).  If Servo.h wasn't included, this allows the code to still
// compile, but without support for any Servos.  Hopefully that's what the
// user intended by not including Servo.h
#ifndef MAX_SERVOS
  #define MAX_SERVOS 0
#endif

/*
    Firmata Hardware Abstraction Layer

Firmata is built on top of the hardware abstraction functions of Arduino,
specifically digitalWrite, digitalRead, analogWrite, analogRead, and
pinMode.  While these functions offer simple integer pin numbers, Firmata
needs more information than is provided by Arduino.  This file provides
all other hardware specific details.  To make Firmata support a new board,
only this file should require editing.

The key concept is every "pin" implemented by Firmata may be mapped to
any pin as implemented by Arduino.  Usually a simple 1-to-1 mapping is
best, but such mapping should not be assumed.  This hardware abstraction
layer allows Firmata to implement any number of pins which map onto the
Arduino implemented pins in almost any arbitrary way.


General Constants:

These constants provide basic information Firmata requires.

TOTAL_PINS: The total number of pins Firmata implemented by Firmata.
    Usually this will match the number of pins the Arduino functions
    implement, including any pins pins capable of analog or digital.
    However, Firmata may implement any number of pins.  For example,
    on Arduino Mini with 8 analog inputs, 6 of these may be used
    for digital functions, and 2 are analog only.  On such boards,
    Firmata can implement more pins than Arduino's pinMode()
    function, in order to accommodate those special pins.  The
    Firmata protocol supports a maximum of 128 pins, so this
    constant must not exceed 128.

TOTAL_ANALOG_PINS: The total number of analog input pins implemented.
    The Firmata protocol allows up to 16 analog inputs, accessed
    using offsets 0 to 15.  Because Firmata presents the analog
    inputs using different offsets than the actual pin numbers
    (a legacy of Arduino's analogRead function, and the way the
    analog input capable pins are physically labeled on all
    Arduino boards), the total number of analog input signals
    must be specified.  16 is the maximum.

VERSION_BLINK_PIN: When Firmata starts up, it will blink the version
    number.  This constant is the Arduino pin number where a
    LED is connected.


Pin Mapping Macros:

These macros provide the mapping between pins as implemented by
Firmata protocol and the actual pin numbers used by the Arduino
functions.  Even though such mappings are often simple, pin
numbers received by Firmata protocol should always be used as
input to these macros, and the result of the macro should be
used with with any Arduino function.

When Firmata is extended to support a new pin mode or feature,
a pair of macros should be added and used for all hardware
access.  For simple 1:1 mapping, these macros add no actual
overhead, yet their consistent use allows source code which
uses them consistently to be easily adapted to all other boards
with different requirements.

IS_PIN_XXXX(pin): The IS_PIN macros resolve to true or non-zero
    if a pin as implemented by Firmata corresponds to a pin
    that actually implements the named feature.

PIN_TO_XXXX(pin): The PIN_TO macros translate pin numbers as
    implemented by Firmata to the pin numbers needed as inputs
    to the Arduino functions.  The corresponding IS_PIN macro
    should always be tested before using a PIN_TO macro, so
    these macros only need to handle valid Firmata pin
    numbers for the named feature.


Port Access Inline Funtions:

For efficiency, Firmata protocol provides access to digital
input and output pins grouped by 8 bit ports.  When these
groups of 8 correspond to actual 8 bit ports as implemented
by the hardware, these inline functions can provide high
speed direct port access.  Otherwise, a default implementation
using 8 calls to digitalWrite or digitalRead is used.

When porting Firmata to a new board, it is recommended to
use the default functions first and focus only on the constants
and macros above.  When those are working, if optimized port
access is desired, these inline functions may be extended.
The recommended approach defines a symbol indicating which
optimization to use, and then conditional complication is
used within these functions.

readPort(port, bitmask):  Read an 8 bit port, returning the value.
   port:    The port number, Firmata pins port*8 to port*8+7
   bitmask: The actual pins to read, indicated by 1 bits.

writePort(port, value, bitmask):  Write an 8 bit port.
   port:    The port number, Firmata pins port*8 to port*8+7
   value:   The 8 bit value to write
   bitmask: The actual pins to write, indicated by 1 bits.
*/

/*==============================================================================
 * Board Specific Configuration
 *============================================================================*/

#ifndef digitalPinHasPWM
#define digitalPinHasPWM(p)     IS_PIN_DIGITAL(p)
#endif

#undef IS_PIN_INTERRUPT
#if defined(digitalPinToInterrupt) && defined(NOT_AN_INTERRUPT)
#define IS_PIN_INTERRUPT(p)     (digitalPinToInterrupt(p) > NOT_AN_INTERRUPT)
#else
#define IS_PIN_INTERRUPT(p)     (0)
#endif

// Arduino Duemilanove, Diecimila, and NG
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#if defined(NUM_ANALOG_INPUTS) && NUM_ANALOG_INPUTS == 6
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              20 // 14 digital + 6 analog
#else
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              22 // 14 digital + 8 analog
#endif
#define VERSION_BLINK_PIN       13
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) <= 19)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) < 14 + TOTAL_ANALOG_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) - 2 < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)
#define ARDUINO_PINOUT_OPTIMIZE 1


// Wiring (and board)
#elif defined(WIRING)
#define VERSION_BLINK_PIN       WLED
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= FIRST_ANALOG_PIN && (p) < (FIRST_ANALOG_PIN+TOTAL_ANALOG_PINS))
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == SDA || (p) == SCL)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - FIRST_ANALOG_PIN)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// old Arduinos
#elif defined(__AVR_ATmega8__)
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              20 // 14 digital + 6 analog
#define VERSION_BLINK_PIN       13
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) <= 19)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) <= 19)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) - 2 < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)
#define ARDUINO_PINOUT_OPTIMIZE 1


// Arduino Mega
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define TOTAL_ANALOG_PINS       16
#define TOTAL_PINS              70 // 54 digital + 16 analog
#define VERSION_BLINK_PIN       13
#define PIN_SERIAL1_RX          19
#define PIN_SERIAL1_TX          18
#define PIN_SERIAL2_RX          17
#define PIN_SERIAL2_TX          16
#define PIN_SERIAL3_RX          15
#define PIN_SERIAL3_TX          14
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 54 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 2 && (p) - 2 < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 20 || (p) == 21)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) > 13 && (p) < 20)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 54)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)

#elif defined(AVR_NANO_EVERY) || defined(ARDUINO_NANO_EVERY) || defined(ARDUINO_AVR_NANO_EVERY)
#define TOTAL_ANALOG_PINS 8
#define TOTAL_PINS 24 // 14 digital + 8 analog + 2 i2c
#define IS_PIN_DIGITAL(p) ((p) >= 2 && (p) <= 21) // TBD if pins 0 and 1 are usable
#define IS_PIN_ANALOG(p) ((p) >= 14 && (p) < 14 + TOTAL_ANALOG_PINS)
#define IS_PIN_PWM(p) digitalPinHasPWM(p)
#define IS_PIN_SERVO(p) (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p) ((p) == PIN_WIRE_SDA || (p) == PIN_WIRE_SCL) // SDA = 22, SCL = 23
#define IS_PIN_SPI(p) ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p) (p)
#define PIN_TO_ANALOG(p) ((p) - 14)
#define PIN_TO_PWM(p) PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p) (p) // deprecated since v2.4

// Arduino UNO WiFi rev2 (ATMega 4809)
#elif defined(__AVR_ATmega4809__)
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              20 // 14 digital + 6 analog + /* 3 SPI (unexported, on ISP header) */
#define VERSION_BLINK_PIN       25
#define PIN_SERIAL1_RX          0
#define PIN_SERIAL1_TX          1
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (p)
#define IS_PIN_I2C(p)           ((p) == SDA || (p) == SCL)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 0 || (p) == 1)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (p) - 14
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)

// Arduino DUE
#elif defined(__SAM3X8E__)
#define TOTAL_ANALOG_PINS       12
#define TOTAL_PINS              66 // 54 digital + 12 analog
#define VERSION_BLINK_PIN       13
#define PIN_SERIAL1_RX          19
#define PIN_SERIAL1_TX          18
#define PIN_SERIAL2_RX          17
#define PIN_SERIAL2_TX          16
#define PIN_SERIAL3_RX          15
#define PIN_SERIAL3_TX          14
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 54 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 2 && (p) - 2 < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 20 || (p) == 21) // 70 71
#define IS_PIN_SERIAL(p)        ((p) > 13 && (p) < 20)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 54)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)


// Arduino Nano 33 IoT
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              22 // 14 Digital + 8 Analog
#define IS_PIN_DIGITAL(p)       ((p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) > 13 && (p) < 14 + TOTAL_ANALOG_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == PIN_WIRE_SDA || (p) == PIN_WIRE_SCL)
#define IS_PIN_SPI(p)           ((p) == PIN_SPI_SS || (p) == PIN_SPI_MOSI || (p) == PIN_SPI_MISO || (p) == PIN_SPI_SCK)
#define IS_PIN_SERIAL(p)        ((p) == PIN_SERIAL1_RX || (p) == PIN_SERIAL1_TX) //defined in variant.h  RX = 0 TX = 1
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4


// Arduino Nano 33 BLE
#elif defined(ARDUINO_ARDUINO_NANO33BLE)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              22 // 14 Digital + 8 Analog
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) < 14 + TOTAL_ANALOG_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == PIN_WIRE_SDA || (p) == PIN_WIRE_SCL) // SDA = 18, SCL = 19
#define IS_PIN_SPI(p)           ((p) == PIN_SPI_SS || (p) == PIN_SPI_MOSI || (p) == PIN_SPI_MISO || (p) == PIN_SPI_SCK)
#define IS_PIN_SERIAL(p)        ((p) == PIN_SERIAL_RX || (p) == PIN_SERIAL_TX) //defined in pins_arduino.h  RX = 1 TX = 0
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4


// Arduino/Genuino MKR1000 or MKR1010
#elif defined(ARDUINO_SAMD_MKR1000)	|| defined(ARDUINO_SAMD_MKRWIFI1010)
#define TOTAL_ANALOG_PINS       7
#define TOTAL_PINS              22 // 8 digital + 3 spi + 2 i2c + 2 uart + 7 analog
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 21)
#define IS_PIN_ANALOG(p)        ((p) >= 15 && (p) < 15 + TOTAL_ANALOG_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == 11 || (p) == 12) // SDA = 11, SCL = 12
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == PIN_SERIAL1_RX || (p) == PIN_SERIAL1_TX) //defined in variant.h  RX = 13, TX = 14
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 15)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4


// Arduino MKRZero
#elif defined(ARDUINO_SAMD_MKRZERO)
#define TOTAL_ANALOG_PINS       7
#define TOTAL_PINS              34 // 8 digital + 3 spi + 2 i2c + 2 uart + 7 analog + 3 usb + 1 aref + 5 sd + 1 bottom pad + 1 led + 1 battery adc
#define IS_PIN_DIGITAL(p)       (((p) >= 0 && (p) <= 21) || (p) == 32)
#define IS_PIN_ANALOG(p)        (((p) >= 15 && (p) < 15 + TOTAL_ANALOG_PINS) || (p) == 33)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == 11 || (p) == 12) // SDA = 11, SCL = 12
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == PIN_SERIAL1_RX || (p) == PIN_SERIAL1_TX) //defined in variant.h  RX = 13, TX = 14
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 15)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4

// Arduino MKRFox1200
#elif defined(ARDUINO_SAMD_MKRFox1200)
#define TOTAL_ANALOG_PINS       7
#define TOTAL_PINS              33 // 8 digital + 3 spi + 2 i2c + 2 uart + 7 analog + 3 usb + 1 aref + 5 sd + 1 bottom pad + 1 battery adc
#define IS_PIN_DIGITAL(p)       (((p) >= 0 && (p) <= 21))
#define IS_PIN_ANALOG(p)        (((p) >= 15 && (p) < 15 + TOTAL_ANALOG_PINS) || (p) == 32)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == 11 || (p) == 12) // SDA = 11, SCL = 12
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == PIN_SERIAL1_RX || (p) == PIN_SERIAL1_TX) //defined in variant.h  RX = 13, TX = 14
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 15)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4

// Arduino MKR WAN 1300
#elif defined(ARDUINO_SAMD_MKRWAN1300)
#define TOTAL_ANALOG_PINS       7
#define TOTAL_PINS              33
#define IS_PIN_DIGITAL(p)       (((p) >= 0 && (p) <= 21))
#define IS_PIN_ANALOG(p)        (((p) >= 15 && (p) < 15 + TOTAL_ANALOG_PINS) || (p) == 32)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == 11 || (p) == 12) // SDA = 11, SCL = 12
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == PIN_SERIAL1_RX || (p) == PIN_SERIAL1_TX) //defined in variant.h  RX = 13, TX = 14
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 15)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4

// Arduino MKR GSM 1400
#elif defined(ARDUINO_SAMD_MKRGSM1400)
#define TOTAL_ANALOG_PINS       7
#define TOTAL_PINS              33
#define IS_PIN_DIGITAL(p)       (((p) >= 0 && (p) <= 21))
#define IS_PIN_ANALOG(p)        (((p) >= 15 && (p) < 15 + TOTAL_ANALOG_PINS) || (p) == 32)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == 11 || (p) == 12) // SDA = 11, SCL = 12
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == PIN_SERIAL1_RX || (p) == PIN_SERIAL1_TX) //defined in variant.h  RX = 13, TX = 14
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 15)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4

// Arduino Zero
// Note this will work with an Arduino Zero Pro, but not with an Arduino M0 Pro
// Arduino M0 Pro does not properly map pins to the board labeled pin numbers
#elif defined(_VARIANT_ARDUINO_ZERO_)
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              25 // 14 digital + 6 analog + 2 i2c + 3 spi
#define TOTAL_PORTS             3  // set when TOTAL_PINS > num digitial I/O pins
#define VERSION_BLINK_PIN       LED_BUILTIN
//#define PIN_SERIAL1_RX          0 // already defined in zero core variant.h
//#define PIN_SERIAL1_TX          1 // already defined in zero core variant.h
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 19)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) < 14 + TOTAL_ANALOG_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == 20 || (p) == 21) // SDA = 20, SCL = 21
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK) // SS = A2
#define IS_PIN_SERIAL(p)        ((p) == 0 || (p) == 1)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4

// Arduino Primo
#elif defined(ARDUINO_PRIMO)
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              22 //14 digital + 6 analog + 2 i2c
#define VERSION_BLINK_PIN       LED_BUILTIN
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) < 20)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) < 20)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS+2)
#define IS_PIN_I2C(p)           ((p) == PIN_WIRE_SDA || (p) == PIN_WIRE_SCL) // SDA = 20, SCL = 21
#define IS_PIN_SPI(p)           ((p) == SS || (p)== MOSI || (p) == MISO || (p == SCK)) // 10, 11, 12, 13
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)

// Arduino 101
#elif defined(_VARIANT_ARDUINO_101_X_)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_INPUTS
#define TOTAL_PINS              NUM_DIGITAL_PINS // 15 digital (including ATN pin) + 6 analog
#define VERSION_BLINK_PIN       LED_BUILTIN
#define PIN_SERIAL1_RX          0
#define PIN_SERIAL1_TX          1
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 20)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) < 14 + TOTAL_ANALOG_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p) // 3, 5, 6, 9
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == SDA || (p) == SCL) // SDA = 18, SCL = 19
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 0 || (p) == 1)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4

// Arduino UNO R4 Minima and Wifi
// The pinout is the same as for the classical UNO R3
#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
#if defined(NUM_ANALOG_INPUTS) && NUM_ANALOG_INPUTS == 6
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              20 // 14 digital + 6 analog
#else
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              22 // 14 digital + 8 analog
#endif
// These have conflicting(?) definitions in the core for this CPU
#undef IS_PIN_PWM
#undef IS_PIN_ANALOG
#define VERSION_BLINK_PIN       13
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) <= 19)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) < 14 + TOTAL_ANALOG_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) - 2 < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)

// Teensy 1.0
#elif defined(__AVR_AT90USB162__)
#define TOTAL_ANALOG_PINS       0
#define TOTAL_PINS              21 // 21 digital + no analog
#define VERSION_BLINK_PIN       6
#define PIN_SERIAL1_RX          2
#define PIN_SERIAL1_TX          3
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        (0)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 2 || (p) == 3)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (0)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy 2.0
#elif defined(__AVR_ATmega32U4__) && defined(CORE_TEENSY)
#define TOTAL_ANALOG_PINS       12
#define TOTAL_PINS              25 // 11 digital + 12 analog
#define VERSION_BLINK_PIN       11
#define PIN_SERIAL1_RX          7
#define PIN_SERIAL1_TX          8
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 11 && (p) <= 22)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 5 || (p) == 6)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 7 || (p) == 8)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (((p) < 22) ? 21 - (p) : 11)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy 3.5 and 3.6
// reference: https://github.com/PaulStoffregen/cores/blob/master/teensy3/pins_arduino.h
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define TOTAL_ANALOG_PINS       27 // 3.5 has 27 and 3.6 has 25
#define TOTAL_PINS              70 // 43 digital + 21 analog-digital + 6 analog (64-69)
#define VERSION_BLINK_PIN       13
#define PIN_SERIAL1_RX          0
#define PIN_SERIAL1_TX          1
#define PIN_SERIAL2_RX          9
#define PIN_SERIAL2_TX          10
#define PIN_SERIAL3_RX          7
#define PIN_SERIAL3_TX          8
#define PIN_SERIAL4_RX          31
#define PIN_SERIAL4_TX          32
#define PIN_SERIAL5_RX          34
#define PIN_SERIAL5_TX          33
#define PIN_SERIAL6_RX          47
#define PIN_SERIAL6_TX          48
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 63)
#define IS_PIN_ANALOG(p)        (((p) >= 14 && (p) <= 23) || ((p) >= 31 && (p) <= 39) || ((p) >= 49 && (p) <= 50) || ((p) >= 64 && (p) <= 69))
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define IS_PIN_SERIAL(p)        (((p) > 6 && (p) < 11) || ((p) == 0 || (p) == 1) || ((p) > 30 && (p) < 35) || ((p) == 47 || (p) == 48))
#define PIN_TO_DIGITAL(p)       (p)
// A0-A9 = D14-D23; A12-A20 = D31-D39; A23-A24 = D49-D50; A10-A11 = D64-D65; A21-A22 = D66-D67; A25-A26 = D68-D69
#define PIN_TO_ANALOG(p)        (((p) <= 23) ? (p) - 14 : (((p) <= 39) ? (p) - 19 : (((p) <= 50) ? (p) - 26 : (((p) <= 65) ? (p) - 55 : (((p) <= 67) ? (p) - 45 : (p) - 43)))))
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy 3.0, 3.1 and 3.2
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
#define TOTAL_ANALOG_PINS       14
#define TOTAL_PINS              38 // 24 digital + 10 analog-digital + 4 analog
#define VERSION_BLINK_PIN       13
#define PIN_SERIAL1_RX          0
#define PIN_SERIAL1_TX          1
#define PIN_SERIAL2_RX          9
#define PIN_SERIAL2_TX          10
#define PIN_SERIAL3_RX          7
#define PIN_SERIAL3_TX          8
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 33)
#define IS_PIN_ANALOG(p)        (((p) >= 14 && (p) <= 23) || ((p) >= 34 && (p) <= 38))
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define IS_PIN_SERIAL(p)        (((p) > 6 && (p) < 11) || ((p) == 0 || (p) == 1))
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (((p) <= 23) ? (p) - 14 : (p) - 24)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy-LC
#elif defined(__MKL26Z64__)
#define TOTAL_ANALOG_PINS       13
#define TOTAL_PINS              27 // 27 digital + 13 analog-digital
#define VERSION_BLINK_PIN       13
#define PIN_SERIAL1_RX          0
#define PIN_SERIAL1_TX          1
#define PIN_SERIAL2_RX          9
#define PIN_SERIAL2_TX          10
#define PIN_SERIAL3_RX          7
#define PIN_SERIAL3_TX          8
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 26)
#define IS_PIN_ANALOG(p)        ((p) >= 14)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define IS_PIN_SERIAL(p)        (((p) > 6 && (p) < 11) || ((p) == 0 || (p) == 1))
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy++ 1.0 and 2.0
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              46 // 38 digital + 8 analog
#define VERSION_BLINK_PIN       6
#define PIN_SERIAL1_RX          2
#define PIN_SERIAL1_TX          3
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 38 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 0 || (p) == 1)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 2 || (p) == 3)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 38)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)

// Teensy 4.0 and Teensy 4.1
#elif defined(__IMXRT1062__)
#if !defined(ARDUINO_TEENSY40) && !defined(ARDUINO_TEENSY41)
  #warning Assuming ARDUINO_TEENSY40. Please #define ARDUINO_TEENSY40 or ARDUINO_TEENSY41.
  #define ARDUINO_TEENSY40
#endif
#if defined(ARDUINO_TEENSY40)
  #define TOTAL_PINS              40
  #define TOTAL_ANALOG_PINS       14
  #define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) <= 27)
  #define PIN_TO_ANALOG(p)        ((p) - 14)
#elif defined(ARDUINO_TEENSY41)
  #define TOTAL_PINS              55
  #define TOTAL_ANALOG_PINS       18
  #define IS_PIN_ANALOG(p)        (((p) >= 14 && (p) <= 27) || ((p) >= 38 && (p) <= 41))
  #define PIN_TO_ANALOG(p)        (((p) <= 27) ? ((p) - 14 ) : ((p) - 24))
#endif
#define VERSION_BLINK_PIN       13
#define PIN_SERIAL1_RX          0
#define PIN_SERIAL1_TX          1
#define PIN_SERIAL2_RX          7
#define PIN_SERIAL2_TX          8
#define PIN_SERIAL3_RX          15
#define PIN_SERIAL3_TX          14
#define PIN_SERIAL4_RX          16
#define PIN_SERIAL4_TX          17
#define PIN_SERIAL5_RX          21
#define PIN_SERIAL5_TX          20
#define PIN_SERIAL6_RX          25
#define PIN_SERIAL6_TX          24
#define PIN_SERIAL7_RX          28
#define PIN_SERIAL7_TX          29
#if defined(ARDUINO_TEENSY40)
  #define IS_PIN_SERIAL(p)        (((p) == PIN_SERIAL1_RX) || \
                                   ((p) == PIN_SERIAL1_TX) || \
                                   ((p) == PIN_SERIAL2_RX) || \
                                   ((p) == PIN_SERIAL2_TX) || \
                                   ((p) == PIN_SERIAL3_RX) || \
                                   ((p) == PIN_SERIAL3_TX) || \
                                   ((p) == PIN_SERIAL4_RX) || \
                                   ((p) == PIN_SERIAL4_TX) || \
                                   ((p) == PIN_SERIAL5_RX) || \
                                   ((p) == PIN_SERIAL5_TX) || \
                                   ((p) == PIN_SERIAL6_RX) || \
                                   ((p) == PIN_SERIAL6_TX) || \
                                   ((p) == PIN_SERIAL7_RX) || \
                                   ((p) == PIN_SERIAL7_TX))
  #define IS_PIN_PWM(p)           (((p) >= 0 && (p) <= 16) || \
                                   ((p) == 18) || \
                                   ((p) == 19) || \
                                   ((p) >= 22 && (p) <= 25) || \
                                   ((p) == 28) || \
                                   ((p) == 29) || \
                                   ((p) >= 33 && (p) <= 39))
#elif defined(ARDUINO_TEENSY41)
  #define PIN_SERIAL8_RX          34
  #define PIN_SERIAL8_TX          35
  #define IS_PIN_SERIAL(p)         (((p) == PIN_SERIAL1_RX) || \
                                    ((p) == PIN_SERIAL1_TX) || \
                                    ((p) == PIN_SERIAL2_RX) || \
                                    ((p) == PIN_SERIAL2_TX) || \
                                    ((p) == PIN_SERIAL3_RX) || \
                                    ((p) == PIN_SERIAL3_TX) || \
                                    ((p) == PIN_SERIAL4_RX) || \
                                    ((p) == PIN_SERIAL4_TX) || \
                                    ((p) == PIN_SERIAL5_RX) || \
                                    ((p) == PIN_SERIAL5_TX) || \
                                    ((p) == PIN_SERIAL6_RX) || \
                                    ((p) == PIN_SERIAL6_TX) || \
                                    ((p) == PIN_SERIAL7_RX) || \
                                    ((p) == PIN_SERIAL7_TX) ||\
                                    ((p) == PIN_SERIAL8_RX) || \
                                    ((p) == PIN_SERIAL8_TX))
  #define IS_PIN_PWM(p)           (((p) >= 0 && (p) <= 15)  || \
                                   ((p) == 18) || \
                                   ((p) == 19) || \
                                   ((p) >= 22 && (p) <= 25) || \
                                   ((p) == 28) || \
                                   ((p) == 29) || \
                                   ((p) == 33) || \
                                   ((p) == 36) || \
                                   ((p) == 37) || \
                                   ((p) >= 42 && (p) <= 47) || \
                                   ((p) == 51) || \
                                   ((p) == 54))
#endif
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)


// Leonardo
#elif defined(__AVR_ATmega32U4__)
#define TOTAL_ANALOG_PINS       12
#define TOTAL_PINS              30 // 14 digital + 12 analog + 4 SPI (D14-D17 on ISP header)
#define VERSION_BLINK_PIN       13
#define PIN_SERIAL1_RX          0
#define PIN_SERIAL1_TX          1
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 18 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           ((p) == 3 || (p) == 5 || (p) == 6 || (p) == 9 || (p) == 10 || (p) == 11 || (p) == 13)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 2 || (p) == 3)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 0 || (p) == 1)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (p) - 18
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Intel Galileo Board (gen 1 and 2) and Intel Edison
#elif defined(ARDUINO_LINUX)
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              20 // 14 digital + 6 analog
#define VERSION_BLINK_PIN       13
#define PIN_SERIAL1_RX          0
#define PIN_SERIAL1_TX          1
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) <= 19)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) <= 19)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) - 2 < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == SDA || (p) == SCL)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 0 || (p) == 1)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)


// RedBearLab BLE Nano with factory switch settings (S1 - S10)
#elif defined(BLE_NANO)
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              15 // 9 digital + 3 analog
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) <= 14)
#define IS_PIN_ANALOG(p)        ((p) == 8 || (p) == 9 || (p) == 10 || (p) == 11 || (p) == 12 || (p) == 14) //A0~A5
#define IS_PIN_PWM(p)           ((p) == 3 || (p) == 5 || (p) == 6)
#define IS_PIN_SERVO(p)         ((p) >= 2 && (p) <= 7)
#define IS_PIN_I2C(p)           ((p) == SDA || (p) == SCL)
#define IS_PIN_SPI(p)           ((p) == CS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 8)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Sanguino
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              32 // 24 digital + 8 analog
#define VERSION_BLINK_PIN       0
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 24 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 16 || (p) == 17)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 24)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)

// Sanguino/Melzi, e.g. Creality Ender-3
#elif defined(__AVR_ATmega1284P__)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              32
#define VERSION_BLINK_PIN       13
#define PIN_SERIAL1_RX          8 //PD0
#define PIN_SERIAL1_TX          9 //PD1
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 24 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           ((p) == 3 || (p) == 4 || (p) == 6 || (p) == 7 || (p) == 12 || (p) == 13 || (p) == 14 || (p) == 15)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 16 || (p) == 17)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 8 || (p) == 9)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (p) - 24
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Illuminato
#elif defined(__AVR_ATmega645__)
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              42 // 36 digital + 6 analog
#define VERSION_BLINK_PIN       13
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 36 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 4 || (p) == 5)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 36)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)


// Pic32 chipKIT FubarinoSD
#elif defined(_BOARD_FUBARINO_SD_)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_PINS  // 15
#define TOTAL_PINS              NUM_DIGITAL_PINS // 45, All pins can be digital
#define MAX_SERVOS              NUM_DIGITAL_PINS
#define VERSION_BLINK_PIN       PIN_LED1
#define IS_PIN_DIGITAL(p)       1
#define IS_PIN_ANALOG(p)        ((p) >= 30 && (p) <= 44)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 1 || (p) == 2)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (14 - (p - 30))
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)


// Pic32 chipKIT FubarinoMini
// Note, FubarinoMini analog pin 20 will not function in Firmata as analog input due to limitation in analog mapping
#elif defined(_BOARD_FUBARINO_MINI_)
#define TOTAL_ANALOG_PINS       14 // We have to fake this because of the poor analog pin mapping planning in FubarinoMini
#define TOTAL_PINS              NUM_DIGITAL_PINS // 33
#define MAX_SERVOS              NUM_DIGITAL_PINS
#define VERSION_BLINK_PIN       PIN_LED1
#define IS_PIN_DIGITAL(p)       ((p) != 14 && (p) != 15 && (p) != 31 && (p) != 32)
#define IS_PIN_ANALOG(p)        ((p) == 0 || ((p) >= 3 && (p) <= 13))
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 25 || (p) == 26)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (p)
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)


// Pic32 chipKIT UNO32
#elif defined(_BOARD_UNO_) && defined(__PIC32)  // NOTE: no _BOARD_UNO32_ to use
#define TOTAL_ANALOG_PINS       NUM_ANALOG_PINS // 12
#define TOTAL_PINS              NUM_DIGITAL_PINS // 47 All pins can be digital
#define MAX_SERVOS              NUM_DIGITAL_PINS // All pins can be servo with SoftPWMservo
#define VERSION_BLINK_PIN       PIN_LED1
#define IS_PIN_DIGITAL(p)       ((p) >= 2)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) <= 25)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 45 || (p) == 46)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)


// Pic32 chipKIT DP32
#elif defined(_BOARD_DP32_)
#define TOTAL_ANALOG_PINS       15  // Really only has 9, but have to override because of mistake in variant file
#define TOTAL_PINS              NUM_DIGITAL_PINS // 19
#define MAX_SERVOS              NUM_DIGITAL_PINS // All pins can be servo with SoftPWMservo
#define VERSION_BLINK_PIN       PIN_LED1
#define IS_PIN_DIGITAL(p)       (((p) != 1) && ((p) != 4) && ((p) != 5) && ((p) != 15) && ((p) != 16))
#define IS_PIN_ANALOG(p)        ((p) >= 6 && (p) <= 14)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 2 || (p) == 3)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (p)
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)


// Pic32 chipKIT uC32
#elif defined(_BOARD_UC32_)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_PINS  // 12
#define TOTAL_PINS              NUM_DIGITAL_PINS // 47 All pins can be digital
#define MAX_SERVOS              NUM_DIGITAL_PINS // All pins can be servo with SoftPWMservo
#define VERSION_BLINK_PIN       PIN_LED1
#define IS_PIN_DIGITAL(p)       ((p) >= 2)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) <= 25)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 45 || (p) == 46)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)


// Pic32 chipKIT WF32
#elif defined(_BOARD_WF32_)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_PINS
#define TOTAL_PINS              NUM_DIGITAL_PINS
#define MAX_SERVOS              NUM_DIGITAL_PINS
#define VERSION_BLINK_PIN       PIN_LED1
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) <= 49)     // Accounts for SD and WiFi dedicated pins
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) <= 25)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 34 || (p) == 35)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)


// Pic32 chipKIT WiFire
#elif defined(_BOARD_WIFIRE_)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_PINS  // 14
#define TOTAL_PINS              NUM_DIGITAL_PINS // 71
#define MAX_SERVOS              NUM_DIGITAL_PINS
#define VERSION_BLINK_PIN       PIN_LED1
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) <= 47)     // Accounts for SD and WiFi dedicated pins
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) <= 25)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 34 || (p) == 35)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) <= 25 ? ((p) - 14) : (p) - 36)
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)


// Pic32 chipKIT MAX32
#elif defined(_BOARD_MEGA_) && defined(__PIC32)  // NOTE: no _BOARD_MAX32_ to use
#define TOTAL_ANALOG_PINS       NUM_ANALOG_PINS  // 16
#define TOTAL_PINS              NUM_DIGITAL_PINS // 87
#define MAX_SERVOS              NUM_DIGITAL_PINS
#define VERSION_BLINK_PIN       PIN_LED1
#define IS_PIN_DIGITAL(p)       ((p) >= 2)
#define IS_PIN_ANALOG(p)        ((p) >= 54 && (p) <= 69)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 34 || (p) == 35)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 54)
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)


// Pic32 chipKIT Pi
#elif defined(_BOARD_CHIPKIT_PI_)
#define TOTAL_ANALOG_PINS       16
#define TOTAL_PINS              NUM_DIGITAL_PINS // 19
#define MAX_SERVOS              NUM_DIGITAL_PINS
#define VERSION_BLINK_PIN       PIN_LED1
#define IS_PIN_DIGITAL(p)       (((p) >= 2) && ((p) <= 3) || (((p) >= 8) && ((p) <= 13)) || (((p) >= 14) && ((p) <= 17)))
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) <= 17)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 16 || (p) == 17)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) <= 15 ? (p) - 14 : (p) - 12)
//#define PIN_TO_ANALOG(p)        (((p) <= 16) ? ((p) - 14) : ((p) - 16))
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)

// Pinoccio Scout
// Note: digital pins 9-16 are usable but not labeled on the board numerically.
// SS=9, MOSI=10, MISO=11, SCK=12, RX1=13, TX1=14, SCL=15, SDA=16
#elif defined(ARDUINO_PINOCCIO)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              NUM_DIGITAL_PINS // 32
#define VERSION_BLINK_PIN       23
#define PIN_SERIAL1_RX          13
#define PIN_SERIAL1_TX          14
#define IS_PIN_DIGITAL(p)       (((p) >= 2) && ((p) <= 16)) || (((p) >= 24) && ((p) <= 31))
#define IS_PIN_ANALOG(p)        ((p) >= 24 && (p) <= 31)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == SCL || (p) == SDA)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 13 || (p) == 14)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 24)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)

// ESP8266
// note: boot mode GPIOs 0, 2 and 15 can be used as outputs, GPIOs 6-11 are in use for flash IO
#elif defined(ESP8266)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_INPUTS
#define TOTAL_PINS              A0 + NUM_ANALOG_INPUTS
#define PIN_SERIAL_RX           3
#define PIN_SERIAL_TX           1
#define IS_PIN_DIGITAL(p)       (((p) >= 0 && (p) <= 5) || ((p) >= 12 && (p) < A0))
#define IS_PIN_ANALOG(p)        ((p) >= A0 && (p) < A0 + NUM_ANALOG_INPUTS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == SDA || (p) == SCL)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_INTERRUPT(p)     (digitalPinToInterrupt(p) > NOT_AN_INTERRUPT)
#define IS_PIN_SERIAL(p)        ((p) == PIN_SERIAL_RX || (p) == PIN_SERIAL_TX)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - A0)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)
#define DEFAULT_PWM_RESOLUTION  10

// XIAO ESP32C3
// note: Firmata pin numbering schema is by ESP32 GPIO -> IS_XXX checks GPIO number (Ax = Dx, Dx to GPIOy)
#elif defined(ARDUINO_XIAO_ESP32C3)
#define TOTAL_ANALOG_PINS       (A2 + 1)          // (max GPIOx + 1), there are 4 physical analog pins but only 3 are supported by ESP32 SDK 2.0.14 via ADC1
#define TOTAL_PINS              NUM_DIGITAL_PINS  // (max GPIOx + 1), there are 11 physical pins
#define PIN_SERIAL_RX           RX
#define PIN_SERIAL_TX           TX
#define IS_PIN_DIGITAL(p)       (((p) >= D0 && (p) <= D10) || (p) == D6 || (p) == D7)
#define IS_PIN_ANALOG(p)        ((p) >= A0 && (p) <= A2)
#define IS_PIN_PWM(p)           0
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && MAX_SERVOS > 0)
#define IS_PIN_I2C(p)           ((p) == SDA || (p) == SCL)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_INTERRUPT(p)     (digitalPinToInterrupt(p) > NOT_AN_INTERRUPT)
#define IS_PIN_SERIAL(p)        ((p) == PIN_SERIAL_RX || (p) == PIN_SERIAL_TX)
#define PIN_TO_DIGITAL(p)       ((p) < 6? D0 + (p) : ((p) < 8? D6 + 6 - (p) : (p))) // Dx to GPIOy
#define PIN_TO_ANALOG(p)        (p)                                                 // FIRMATAx to GPIOy
#define PIN_TO_PWM(p)           127                                                 // @TODO ESP32 SDK does not support analogWrite()
#define PIN_TO_SERVO(p)         127                                                 // @TODO ESP32 SDK does not support servos

#define DEFAULT_PWM_RESOLUTION   8  // see esp32-hal-led.c, analog_resolution
#define DEFAULT_ANALOG_RESOLUTION 12  // see esp32-hal-adc.h, analogSetWidth()

// STM32 based boards
#elif defined(ARDUINO_ARCH_STM32)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_INPUTS
#define TOTAL_PINS              NUM_DIGITAL_PINS
#define TOTAL_PORTS             MAX_NB_PORT
#ifdef LED_BUILTIN
#define VERSION_BLINK_PIN       LED_BUILTIN
#endif
// PIN_SERIALY_RX/TX defined in the variant.h
#define IS_PIN_DIGITAL(p)       (digitalPinIsValid(p) && !pinIsSerial(p))
#if !defined(STM32_CORE_VERSION) || (STM32_CORE_VERSION  < 0x01080000)
#define IS_PIN_ANALOG(p)        ((p >= A0) && (p < (A0 + TOTAL_ANALOG_PINS)) && !pinIsSerial(p))
#else
#define IS_PIN_ANALOG(p)        (pinIsAnalogInput(p) && !pinIsSerial(p))
#endif
#define IS_PIN_PWM(p)           (IS_PIN_DIGITAL(p) && digitalPinHasPWM(p))
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p))
#define IS_PIN_I2C(p)           (IS_PIN_DIGITAL(p) && digitalPinHasI2C(p))
#define IS_PIN_SPI(p)           (IS_PIN_DIGITAL(p) && digitalPinHasSPI(p))
#define IS_PIN_INTERRUPT(p)     (IS_PIN_DIGITAL(p) && (digitalPinToInterrupt(p) > NOT_AN_INTERRUPT)))
#define IS_PIN_SERIAL(p)        (digitalPinHasSerial(p) && !pinIsSerial(p))
#define PIN_TO_DIGITAL(p)       (p)
#if !defined(STM32_CORE_VERSION) || (STM32_CORE_VERSION  < 0x01080000)
#define PIN_TO_ANALOG(p)        (p-A0)
#else
#define PIN_TO_ANALOG(p)        (digitalPinToAnalogInput(p))
#endif
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)
#define DEFAULT_PWM_RESOLUTION  PWM_RESOLUTION

// Adafruit Bluefruit nRF52 boards
#elif defined(ARDUINO_NRF52_ADAFRUIT)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_INPUTS
#define TOTAL_PINS              NUM_DIGITAL_PINS
#define VERSION_BLINK_PIN       LED_BUILTIN
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) == PIN_A0 || (p) == PIN_A1 || (p) == PIN_A2  || (p) == PIN_A3 || \
                                 (p) == PIN_A4 || (p) == PIN_A5 || (p) == PIN_A6  || (p) == PIN_A7)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == PIN_WIRE_SDA || (p) == PIN_WIRE_SCL)
#define IS_PIN_SPI(p)           ((p) == SS || (p)== MOSI || (p) == MISO || (p == SCK))
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ( ((p) == PIN_A0) ? 0 : ((p) == PIN_A1) ? 1 : ((p) == PIN_A2) ? 2 : ((p) == PIN_A3) ? 3 : \
                                  ((p) == PIN_A4) ? 4 : ((p) == PIN_A5) ? 5 : ((p) == PIN_A6) ? 6 : ((p) == PIN_A7) ? 7 : (127))
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)

// SPRESENSE
#elif defined(ARDUINO_ARCH_SPRESENSE)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_INPUTS
#define TOTAL_PINS              NUM_DIGITAL_PINS + 4 + NUM_ANALOG_INPUTS // + 4 built-in led
#define VERSION_BLINK_PIN       LED_BUILTIN
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < (NUM_DIGITAL_PINS + 4))
#define IS_PIN_ANALOG(p)        ((p) >= (TOTAL_PINS - NUM_ANALOG_INPUTS) && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           ((p) == 6 || (p) == 5 || (p) == 9 || (p) == 3)
#define IS_PIN_SERVO(p)         ((p) < NUM_DIGITAL_PINS)
#define IS_PIN_I2C(p)           ((p) == SDA || (p) == SCL)
#define IS_PIN_SPI(p)           ((p) == 10 || (p) == 11 || (p) == 12 || (p) == 13)
#define PIN_TO_DIGITAL(p)       (((p) < NUM_DIGITAL_PINS) ? (p) : (_LED_PIN((p) - NUM_DIGITAL_PINS)))
#define PIN_TO_ANALOG(p)        ((p) - (TOTAL_PINS - NUM_ANALOG_INPUTS))
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)
#define analogRead(p)           analogRead(_ANALOG_PIN(p)) // wrap function for analogRead()

// Robo HAT MM1
#elif defined(ROBOTICSMASTERS_ROBOHATMM1_M4)
#define TOTAL_ANALOG_PINS       7
#define TOTAL_PINS              46 // 14 digital + 7 analog + 4 i2c + 6 spi + 4 serial
#define TOTAL_PORTS             3  // set when TOTAL_PINS > num digitial I/O pins
#define VERSION_BLINK_PIN       LED_BUILTIN
//#define PIN_SERIAL1_RX          0 // already defined in zero core variant.h
//#define PIN_SERIAL1_TX          1 // already defined in zero core variant.h
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 13)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) < 14 + TOTAL_ANALOG_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) < MAX_SERVOS) // deprecated since v2.4
#define IS_PIN_I2C(p)           ((p) == 21 || (p) == 22) // SDA = 21, SCL = 21
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK) // SS = A2
#define IS_PIN_SERIAL(p)        ((p) == 0 || (p) == 1)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) // deprecated since v2.4

//Arduino Uno Wifi Rev2
#elif defined(__AVR_ATmega4809__)
#define TOTAL_ANALOG_PINS       NUM_ANALOG_INPUTS //6
#define TOTAL_PINS              41 // 14 digital + 6 analog + 6 reserved + 10 internal used + 2 I2C + 3 SPI
#define TOTAL_PORTS             3
#define VERSION_BLINK_PIN       LED_BUILTIN //25
#define PIN_SERIAL1_RX          0
#define PIN_SERIAL1_TX          1
#define PIN_SERIAL2_RX          23
#define PIN_SERIAL2_TX          24
#define PIN_SERIAL0_RX          26
#define PIN_SERIAL0_TX          27
#define IS_PIN_DIGITAL(p)       (((p) >= 0 && (p) < 20) || (p) == 25)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) < 19)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 20 || (p) == 21)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define IS_PIN_SERIAL(p)        ((p) == 23 || (p) == 24 || (p) == 26 || (p) == 27)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 14)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// anything else
#else
#error "Please edit Boards.h with a hardware abstraction for this board"
#endif

// as long this is not defined for all boards:
#ifndef IS_PIN_SPI
#define IS_PIN_SPI(p)           0
#endif

#ifndef IS_PIN_SERIAL
#define IS_PIN_SERIAL(p)        0
#endif

#ifndef DEFAULT_PWM_RESOLUTION
#define DEFAULT_PWM_RESOLUTION  8
#endif

/*==============================================================================
 * readPort() - Read an 8 bit port
 *============================================================================*/

static inline unsigned char readPort(byte, byte) __attribute__((always_inline, unused));
static inline unsigned char readPort(byte port, byte bitmask)
{
#if defined(ARDUINO_PINOUT_OPTIMIZE)
  if (port == 0) return (PIND & 0xFC) & bitmask; // ignore Rx/Tx 0/1
  if (port == 1) return ((PINB & 0x3F) | ((PINC & 0x03) << 6)) & bitmask;
  if (port == 2) return ((PINC & 0x3C) >> 2) & bitmask;
  return 0;
#else
  unsigned char out = 0, pin = port * 8;
  if (IS_PIN_DIGITAL(pin + 0) && (bitmask & 0x01) && digitalRead(PIN_TO_DIGITAL(pin + 0))) out |= 0x01;
  if (IS_PIN_DIGITAL(pin + 1) && (bitmask & 0x02) && digitalRead(PIN_TO_DIGITAL(pin + 1))) out |= 0x02;
  if (IS_PIN_DIGITAL(pin + 2) && (bitmask & 0x04) && digitalRead(PIN_TO_DIGITAL(pin + 2))) out |= 0x04;
  if (IS_PIN_DIGITAL(pin + 3) && (bitmask & 0x08) && digitalRead(PIN_TO_DIGITAL(pin + 3))) out |= 0x08;
  if (IS_PIN_DIGITAL(pin + 4) && (bitmask & 0x10) && digitalRead(PIN_TO_DIGITAL(pin + 4))) out |= 0x10;
  if (IS_PIN_DIGITAL(pin + 5) && (bitmask & 0x20) && digitalRead(PIN_TO_DIGITAL(pin + 5))) out |= 0x20;
  if (IS_PIN_DIGITAL(pin + 6) && (bitmask & 0x40) && digitalRead(PIN_TO_DIGITAL(pin + 6))) out |= 0x40;
  if (IS_PIN_DIGITAL(pin + 7) && (bitmask & 0x80) && digitalRead(PIN_TO_DIGITAL(pin + 7))) out |= 0x80;
  return out;
#endif
}

/*==============================================================================
 * writePort() - Write an 8 bit port, only touch pins specified by a bitmask
 *============================================================================*/

static inline unsigned char writePort(byte, byte, byte) __attribute__((always_inline, unused));
static inline unsigned char writePort(byte port, byte value, byte bitmask)
{
#if defined(ARDUINO_PINOUT_OPTIMIZE)
  if (port == 0) {
    bitmask = bitmask & 0xFC;  // do not touch Tx & Rx pins
    byte valD = value & bitmask;
    byte maskD = ~bitmask;
    cli();
    PORTD = (PORTD & maskD) | valD;
    sei();
  } else if (port == 1) {
    byte valB = (value & bitmask) & 0x3F;
    byte valC = (value & bitmask) >> 6;
    byte maskB = ~(bitmask & 0x3F);
    byte maskC = ~((bitmask & 0xC0) >> 6);
    cli();
    PORTB = (PORTB & maskB) | valB;
    PORTC = (PORTC & maskC) | valC;
    sei();
  } else if (port == 2) {
    bitmask = bitmask & 0x0F;
    byte valC = (value & bitmask) << 2;
    byte maskC = ~(bitmask << 2);
    cli();
    PORTC = (PORTC & maskC) | valC;
    sei();
  }
  return 1;
#else
  byte pin = port * 8;
  if ((bitmask & 0x01)) digitalWrite(PIN_TO_DIGITAL(pin + 0), (value & 0x01));
  if ((bitmask & 0x02)) digitalWrite(PIN_TO_DIGITAL(pin + 1), (value & 0x02));
  if ((bitmask & 0x04)) digitalWrite(PIN_TO_DIGITAL(pin + 2), (value & 0x04));
  if ((bitmask & 0x08)) digitalWrite(PIN_TO_DIGITAL(pin + 3), (value & 0x08));
  if ((bitmask & 0x10)) digitalWrite(PIN_TO_DIGITAL(pin + 4), (value & 0x10));
  if ((bitmask & 0x20)) digitalWrite(PIN_TO_DIGITAL(pin + 5), (value & 0x20));
  if ((bitmask & 0x40)) digitalWrite(PIN_TO_DIGITAL(pin + 6), (value & 0x40));
  if ((bitmask & 0x80)) digitalWrite(PIN_TO_DIGITAL(pin + 7), (value & 0x80));
  return 1;
#endif
}




#ifndef TOTAL_PORTS
#define TOTAL_PORTS             ((TOTAL_PINS + 7) / 8)
#endif


#endif /* Firmata_Boards_h */
