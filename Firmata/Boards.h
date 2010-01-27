/* Boards.h - Hardware Abstraction Layer for Firmata library */

#ifndef Firmata_Boards_h
#define Firmata_Boards_h

#include <WProgram.h>	// for digitalRead, digitalWrite, etc
//#include <Servo.h>      // for MAX_SERVOS
#ifndef MAX_SERVOS
#define MAX_SERVOS 0
#endif

// Hardware Abstraction Layer
//
// TODO: write documentation here.....

// TOTAL_PINS

// TOTAL_ANALOG_PINS

// VERSION_BLINK_PIN



/*==============================================================================
 * Board Specific Configuration
 *============================================================================*/

// Arduino Duemilanove, Diecimila, and NG
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              24 // 14 digital + 2 unused + 8 analog
#define VERSION_BLINK_PIN       13
#define IS_PIN_DIGITAL(p)       (((p) >= 2 && (p) <= 13) || ((p) >= 16 && (p) <= 21))
#define IS_PIN_ANALOG(p)        ((p) >= 16 && (p) <= 23)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         ((p) >= 2 && (p) <= 13 && (p) - 2 < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define PIN_TO_DIGITAL(p)       (((p) < 16) ? (p) : (p) - 2)
#define PIN_TO_ANALOG(p)        ((p) - 16)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)
#define ARDUINO_PINOUT_OPTIMIZE 1


// old Arduinos
#elif defined(__AVR_ATmega8__)
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              22 // 14 digital + 2 unused + 6 analog
#define VERSION_BLINK_PIN       13
#define IS_PIN_DIGITAL(p)       (((p) >= 2 && (p) <= 13) || ((p) >= 16 && (p) <= 21))
#define IS_PIN_ANALOG(p)        ((p) >= 16 && (p) <= 21)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         ((p) >= 2 && (p) <= 13 && (p) - 2 < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define PIN_TO_DIGITAL(p)       (((p) < 16) ? (p) : (p) - 2)
#define PIN_TO_ANALOG(p)        ((p) - 16)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)
#define ARDUINO_PINOUT_OPTIMIZE 1


// Arduino Mega
#elif defined(__AVR_ATmega1280__)
#define TOTAL_ANALOG_PINS       16
#define TOTAL_PINS              70 // 54 digital + 16 analog
#define VERSION_BLINK_PIN       13
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 54 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         ((p) >= 2 && (p) - 2 < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 54)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)


// Wiring
#elif defined(__AVR_ATmega128__)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              51
#define VERSION_BLINK_PIN       48
// TODO: hardware abstraction for wiring board


// Teensy 1.0
#elif defined(__AVR_AT90USB162__)
#define TOTAL_ANALOG_PINS       0
#define TOTAL_PINS              21 // 21 digital + no analog
#define VERSION_BLINK_PIN       6
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        (0)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (0)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy 2.0
#elif defined(__AVR_ATmega32U4__)
#define TOTAL_ANALOG_PINS       12
#define TOTAL_PINS              25 // 11 digital + 12 analog
#define VERSION_BLINK_PIN       11
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 11 && (p) <= 22)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (((p)<22)?21-(p):11)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy++ 1.0 and 2.0
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              46 // 38 digital + 8 analog
#define VERSION_BLINK_PIN       6
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 38 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 38)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Sanguino
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              32 // 24 digital + 8 analog
#define VERSION_BLINK_PIN       0
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 24 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 24)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)


// Illuminato
#elif defined(__AVR_ATmega645__)
#define TOTAL_ANALOG_PINS       6
#define TOTAL_PINS              42 // 36 digital + 6 analog
#define VERSION_BLINK_PIN       13
#define IS_PIN_DIGITAL(p)       ((p) >= 2 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 36 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 36)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         ((p) - 2)


// anything else
#else
#error "Please edit Boards.h with a hardware abstraction for this board"
#endif


/*==============================================================================
 * readPort() - Read an 8 bit port
 *============================================================================*/

static inline unsigned char readPort(byte) __attribute__((always_inline, unused));
static inline unsigned char readPort(byte port)
{
#if defined(ARDUINO_PINOUT_OPTIMIZE)
	if (port == 0) return PIND & B11111100; // ignore Rx/Tx 0/1
	if (port == 1) return PINB & B00111111; // pins 8-13 (14,15 are disabled for the crystal)
	if (port == 2) return PINC;
	return 0;
#else
	unsigned char out=0, pin=port*8;
	if (IS_PIN_DIGITAL(pin+0) && digitalRead(PIN_TO_DIGITAL(pin+0))) out |= 0x01;
	if (IS_PIN_DIGITAL(pin+1) && digitalRead(PIN_TO_DIGITAL(pin+1))) out |= 0x02;
	if (IS_PIN_DIGITAL(pin+2) && digitalRead(PIN_TO_DIGITAL(pin+2))) out |= 0x04;
	if (IS_PIN_DIGITAL(pin+3) && digitalRead(PIN_TO_DIGITAL(pin+3))) out |= 0x08;
	if (IS_PIN_DIGITAL(pin+4) && digitalRead(PIN_TO_DIGITAL(pin+4))) out |= 0x10;
	if (IS_PIN_DIGITAL(pin+5) && digitalRead(PIN_TO_DIGITAL(pin+5))) out |= 0x20;
	if (IS_PIN_DIGITAL(pin+6) && digitalRead(PIN_TO_DIGITAL(pin+6))) out |= 0x40;
	if (IS_PIN_DIGITAL(pin+7) && digitalRead(PIN_TO_DIGITAL(pin+7))) out |= 0x80;
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
		bitmask = bitmask & 0xFC;  // Tx & Rx pins
		cli();
		PORTD = (PORTD & ~bitmask) | (bitmask & value);
		sei();
	} else if (port == 1) {
		cli();
		PORTB = (PORTB & ~bitmask) | (bitmask & value);
		sei();
	} else if (port == 2) {
		cli();
		PORTC = (PORTC & ~bitmask) | (bitmask & value);
		sei();
	}
#else
	byte pin=port*8;
	if ((bitmask & 0x01)) digitalWrite(PIN_TO_DIGITAL(pin+0), (value & 0x01));
	if ((bitmask & 0x02)) digitalWrite(PIN_TO_DIGITAL(pin+1), (value & 0x02));
	if ((bitmask & 0x04)) digitalWrite(PIN_TO_DIGITAL(pin+2), (value & 0x04));
	if ((bitmask & 0x08)) digitalWrite(PIN_TO_DIGITAL(pin+3), (value & 0x08));
	if ((bitmask & 0x10)) digitalWrite(PIN_TO_DIGITAL(pin+4), (value & 0x10));
	if ((bitmask & 0x20)) digitalWrite(PIN_TO_DIGITAL(pin+5), (value & 0x20));
	if ((bitmask & 0x40)) digitalWrite(PIN_TO_DIGITAL(pin+6), (value & 0x40));
	if ((bitmask & 0x80)) digitalWrite(PIN_TO_DIGITAL(pin+7), (value & 0x80));
#endif
}




#ifndef TOTAL_PORTS
#define TOTAL_PORTS             ((TOTAL_PINS + 7) / 8)
#endif


#endif /* Firmata_Boards_h */

