/*
  RotaryEncoder.h - Firmata library
  Copyright 2011 Michael Schwager (aka, "GreyGnome")
  Copyright (C) 2013 Alan Yorinks. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either version 
  3 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
  
  For further information see: http://code.google.com/p/adaencoder/
                               http://code.google.com/p/oopinchangeint/
*/


#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
  #include <new.h>
#else
  #include <pins_arduino.h>
  #include <wiring.h>
#endif

#include <wiring_private.h>
#include "stddef.h"

#define OOPCIVERSION 1030

#undef DEBUG
/*
* Theory: all IO pins on Atmega168 are covered by Pin Change Interrupts.
* The PCINT corresponding to the pin must be enabled and masked, and
* an ISR routine provided.  Since PCINTs are per port, not per pin, the ISR
* must use some logic to actually implement a per-pin interrupt service.
*/

/* Pin to interrupt map:
* D0-D7 = PCINT 16-23 = PCIR2 = PD = PCIE2 = pcmsk2
* D8-D13 = PCINT 0-5 = PCIR0 = PB = PCIE0 = pcmsk0
* A0-A5 (D14-D19) = PCINT 8-13 = PCIR1 = PC = PCIE1 = pcmsk1
*/


#undef  INLINE_PCINT
#define INLINE_PCINT
// Thanks to cserveny...@gmail.com for MEGA support!
#if defined __AVR_ATmega2560__ || defined __AVR_ATmega1280__ || defined __AVR_ATmega1281__ || defined __AVR_ATmega2561__ || defined __AVR_ATmega640__
    #define __USE_PORT_JK
    // Mega does not have PORTC or D
    #define NO_PORTC_PINCHANGES
    #define NO_PORTD_PINCHANGES
    #if ((defined(NO_PORTB_PINCHANGES) && defined(NO_PORTJ_PINCHANGES)) || \
            (defined(NO_PORTJ_PINCHANGES) && defined(NO_PORTK_PINCHANGES)) || \
            (defined(NO_PORTK_PINCHANGES) && defined(NO_PORTB_PINCHANGES)))
        #define INLINE_PCINT inline
    #endif
#else
    #if ((defined(NO_PORTB_PINCHANGES) && defined(NO_PORTC_PINCHANGES)) || \
            (defined(NO_PORTC_PINCHANGES) && defined(NO_PORTD_PINCHANGES)) || \
            (defined(NO_PORTD_PINCHANGES) && defined(NO_PORTB_PINCHANGES)))
        #define INLINE_PCINT inline
    #endif
#endif


class CallBackInterface
{
   public:

     CallBackInterface() {};

     virtual void cbmethod() {
     };

};

/************************************************************************************/

class PCintPort {
public:
	PCintPort(int index,volatile uint8_t& maskReg) :
	portInputReg(*portInputRegister(index + 2)),
	portPCMask(maskReg),
	firstPin(NULL),
	PCICRbit(1 << index),
	portRisingPins(0),
	portFallingPins(0)
	{ }
	volatile	uint8_t&		portInputReg;
	// cbIface should be an object instantiated from a subclass of CallBackInterface
	static		int8_t attachInterrupt(uint8_t pin, CallBackInterface* cbIface, int mode);
	static		void detachInterrupt(uint8_t pin);
	INLINE_PCINT void PCint();
	static uint8_t curr;

protected:
	class PCintPin {
	public:
		PCintPin() :
		mode(0) {}
		CallBackInterface* 	pinCallBack;
		uint8_t 	mode;
		uint8_t		mask;
		#ifndef NO_PIN_STATE
		uint8_t		state;
		#endif
		PCintPin* next;
	};
	void		enable(PCintPin* pin, CallBackInterface* cbIface, uint8_t mode);
	int8_t		addPin(uint8_t arduinoPin,CallBackInterface* cbIface, uint8_t mode);
	void		delPin(uint8_t mask);
	volatile	uint8_t&		portPCMask;
	const		uint8_t			PCICRbit;
	volatile	uint8_t	portRisingPins;
	volatile	uint8_t	portFallingPins;
	volatile	uint8_t		lastPinView;
	PCintPin*	firstPin;
};

/********************************************************************************/


class RotaryEncoder : public CallBackInterface {
 public:
	RotaryEncoder(char _id, uint8_t _pinA, uint8_t _pinB) {
    	/* 
     	 * Add a new encoder 
     	 * Params :
     	 * pinA			CW pin  (Arduino pin number)
     	 * pinB			CCW pin
	 	 *
     	 */
		addEncoder(_id, _pinA, _pinB);
		
	}
	int8_t query(); // BUG under pre-0.7 versions (had uint8_t)
	int8_t getClicks(); // BUG under pre-0.7 versions (had uint8_t)
	char getID();

	static RotaryEncoder *getFirstEncoder();

	static RotaryEncoder *genie();
	static RotaryEncoder *genie(int8_t *clicks, char *id); // GEt Next Indicated Encoder - gets the next encoder with non-zero clicks

 private:

	void addEncoder(char _id, uint8_t _pinA, uint8_t _pinB);
	void attachInterrupt(uint8_t _pinA, uint8_t _pinB);
	void cbmethod();
	static void debugMessage();

    volatile uint8_t* port;

    uint8_t pinA, bitA;
    uint8_t pinB, bitB;
    uint8_t turning;    // Flag to keep track of turning state
    int8_t clicks;      // Counter to indicate cumulative clicks in either direction
    int8_t direction;   // indicator

    char id;

	RotaryEncoder *next;
};

#endif	//RotaryEncoder.h
