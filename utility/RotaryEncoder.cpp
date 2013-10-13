/*
  RotaryEncoder.cpp - Firmata library
  Copyright 2011 Michael Schwager (aka, "GreyGnome")
  Copyright (C) 2013 Alan Yorinks. All rights reserved.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  For further information see: http://code.google.com/p/adaencoder/
                               http://code.google.com/p/oopinchangeint/
*/


#include "RotaryEncoder.h"


# define cli()  __asm__ __volatile__ ("cli" ::)
# define sei()  __asm__ __volatile__ ("sei" ::)

extern PCintPort portB;
extern PCintPort portC;
extern PCintPort portD;
#ifdef __USE_PORT_JK
extern PCintPort portJ;
extern PCintPort portK;
#endif

uint8_t PCintPort::curr=0;
#ifndef NO_PORTB_PINCHANGES
// port PB==2  (from Arduino.h, Arduino version 1.0)
PCintPort portB=PCintPort(0,PCMSK0);
#endif

#ifndef NO_PORTC_PINCHANGES
// port PC==3  (also in pins_arduino.c, Arduino version 022)
PCintPort portC=PCintPort(1,PCMSK1);
#endif

#ifndef NO_PORTD_PINCHANGES
PCintPort portD=PCintPort(2,PCMSK2); // port PD==4
#endif

#ifdef __USE_PORT_JK

#ifndef NO_PORTJ_PINCHANGES
PCintPort portJ=PCintPort(10,1,PCMSK1); // port PJ==10
#endif

#ifndef NO_PORTK_PINCHANGES
PCintPort portK=PCintPort(11,2,PCMSK2); // port PK==11
#endif

#endif

static PCintPort *lookupPortNumToPort( int portNum ) {
    PCintPort *port = NULL;

    switch (portNum) {
#ifndef NO_PORTB_PINCHANGES
    case 2:
        port=&portB;
        break;
#endif
#ifndef NO_PORTC_PINCHANGES
    case 3:
        port=&portC;
        break;
#endif
#ifndef NO_PORTD_PINCHANGES
    case 4:
        port=&portD;
        break;
#endif
#ifdef __USE_PORT_JK

#ifndef NO_PORTJ_PINCHANGES
    case 10:
        port=&portJ;
        break;
#endif

#ifndef NO_PORTK_PINCHANGES
    case 11:
        port=&portK;
        break;
#endif

#endif
    }

    return port;
}

void PCintPort::enable(PCintPin* p, CallBackInterface* cbIface, uint8_t mode) {
    // Enable the pin for interrupts by adding to the PCMSKx register.
    // ...The final steps; at this point the interrupt is enabled on this pin.
    p->mode=mode;
    p->pinCallBack=cbIface;
    portPCMask |= p->mask;
    if ((p->mode == RISING) || (p->mode == CHANGE)) portRisingPins |= p->mask;
    if ((p->mode == FALLING) || (p->mode == CHANGE)) portFallingPins |= p->mask;
    PCICR |= PCICRbit;
}

int8_t PCintPort::addPin(uint8_t arduinoPin,
                         CallBackInterface* cbIface, uint8_t mode)
{
    PCintPin* tmp;
    uint8_t bitmask=digitalPinToBitMask(arduinoPin);

    // Add to linked list, starting with firstPin
    if (firstPin != NULL) {
        tmp=firstPin;
        do {
            if (tmp->mask == bitmask) { enable(tmp, cbIface, mode); return(0); }
            if (tmp->next == NULL) break;
            tmp=tmp->next;
        } while (true);
    }

    // Create pin p:  fill in the data
    PCintPin* p=new PCintPin;
    if (p == NULL) return(-1);
    p->mode = mode;
    p->next=NULL;
    p->mask = bitmask; // the mask
    // ...Pin created

    if (firstPin == NULL) firstPin=p;
    else tmp->next=p;

    enable(p, cbIface, mode);
    return(1);
}

/*
 * attach an interrupt to a specific pin using pin change interrupts.
 */
int8_t PCintPort::attachInterrupt(uint8_t arduinoPin,
                                  CallBackInterface* cbIface, int mode)
{
    PCintPort *port;
    uint8_t portNum = digitalPinToPort(arduinoPin);
    if ((portNum == NOT_A_PORT) || (cbIface == NULL)) return(-1);

    port=lookupPortNumToPort(portNum);
    // Added by GreyGnome... must set the initial value of lastPinView
    // for it to be correct on the 1st interrupt.
    // ...but even then, how do you define "correct"?
    // Ultimately, the user must specify (not provisioned for yet).

    port->lastPinView=port->portInputReg;

    // map pin to PCIR register
    return(port->addPin(arduinoPin,cbIface,mode));
}

void PCintPort::detachInterrupt(uint8_t arduinoPin)
{
    PCintPort *port;
    PCintPin* current;
    uint8_t mask;

    uint8_t portNum = digitalPinToPort(arduinoPin);
    if (portNum == NOT_A_PORT) return;
    port=lookupPortNumToPort(portNum);
    mask=digitalPinToBitMask(arduinoPin);
    current=port->firstPin;
    while (current) {
        if (current->mask == mask) { // found the target
            uint8_t oldSREG = SREG;
            cli(); // disable interrupts
            port->portPCMask &= ~mask; // disable the mask entry.
            if (port->portPCMask == 0) PCICR &= ~(port->PCICRbit);
            port->portRisingPins &= ~mask; port->portFallingPins &= ~mask;
            SREG = oldSREG; // Restore register; reenables interrupts
            return;
        }
        current=current->next;
    }
}

// common code for isr handler. "port" is the PCINT number.
// there isn't really a good way to back-map ports and masks to pins.
void PCintPort::PCint() {
    uint8_t thisChangedPin, changedPins;
#ifndef DISABLE_PCINT_MULTI_SERVICE
    uint8_t pcifr;
    do {
#endif
        // get the pin states for the indicated port.
        //uint8_t changedPins = PCintPort::curr ^ lastPinView;
        //lastPinView = PCintPort::curr;
        //changedPins &= portPCMask;
        // NEW

        changedPins=(PCintPort::curr ^ lastPinView) &
                ((portRisingPins & PCintPort::curr) |
                 ( portFallingPins & ~PCintPort::curr));
        lastPinView = PCintPort::curr;

        PCintPin* p = firstPin;
        while (p) {
            if (p->mask & changedPins) { // a changed bit
                // Trigger interrupt if mode is CHANGE, or if mode is RISING
                // andthe bit is currently high, or if mode is
                // FALLING and bit is low.
#ifndef NO_PIN_STATE
                p->state=PCintPort::curr & p->mask ? HIGH : LOW;
#endif
                (*(p->pinCallBack)).cbmethod();
                //}
            }
            //changedPins ^= p->mask;  // MIKE:  Check on this optimization.
            //if (!changedPins) break;
            p=p->next;
        }
#ifndef DISABLE_PCINT_MULTI_SERVICE
        pcifr = PCIFR & PCICRbit;
        if (pcifr == 0) break;
        PCIFR |= pcifr;	// clear the interrupt if we will process it
        //(no effect if bit is zero)
        PCintPort::curr=portInputReg;
    } while (true);
#endif
}

#ifndef NO_PORTB_PINCHANGES
ISR(PCINT0_vect) {
    PCintPort::curr = portB.portInputReg;
    portB.PCint();
}
#endif

#ifndef NO_PORTC_PINCHANGES
ISR(PCINT1_vect) {
    PCintPort::curr = portC.portInputReg;
    portC.PCint();
}
#endif

#ifndef NO_PORTD_PINCHANGES
ISR(PCINT2_vect) {
    PCintPort::curr = portD.portInputReg;
    portD.PCint();
}
#endif

#ifdef __USE_PORT_JK
#ifndef NO_PORTJ_PINCHANGES
ISR(PCINT1_vect) {
#ifdef PINMODE
    PCintPort::s_PORT='J';
#endif
    PCintPort::curr = portJ.portInputReg;
    portJ.PCint();
}
#endif

#ifndef NO_PORTK_PINCHANGES
ISR(PCINT2_vect){
#ifdef PINMODE
    PCintPort::s_PORT='K';
#endif
    PCintPort::curr = portK.portInputReg;
    portK.PCint();
}
#endif

#endif // __USE_PORT_JK



RotaryEncoder *firstEncoder=NULL;
RotaryEncoder *currentEncoder=NULL;
RotaryEncoder *RotaryEncoder::getFirstEncoder() {
    return firstEncoder;
}

/*
 * RotaryEncoder
 * Arguments:
 * - id: a single char that identifies this encoder
 * - pinA: The pin on the Arduino that one side of the encoder plugs into.
 *   Turning in this direction means we're
 *   turning clockwise.
 * - pinB: The Arduino pin connected to the encoder; this is the
 *  counterclockwise direction.
 *
 * The pins can be any of the digital pins 0-13, or any of the
 * analog pins A0-A5 (aka, 14-19).  You can specify the
 * analog pins as A0, A1, A2, ... etc.
 *
 * The pins must be paired in the same port, as described above.
 * In summary this means that the two pins should
 * together be grouped within a single port; ie, if you connect
 * pinA to digital pin 9, you must connect pinB to
 * digital pin 8 or 10-13.  See this table:
 * Arduino Pins		PORT
 * ------------		----
 * Digital 0-7		D
 * Digital 8-13		B
 * Analog  0-5		C	(== digital pins 14-19)
 */

void RotaryEncoder::addEncoder(char _id, uint8_t _pinA, uint8_t _pinB)
{
    RotaryEncoder *tmpencoder;

    id=_id;
    pinA=_pinA;
    pinB=_pinB;


    // error checking
    if (pinA == pinB)
        return;  // No! silly
    if (pinA < 8 && pinB > 7)
        return; // No! different ports
    if ((pinA > 7 && pinA < 14) && (pinB < 8 || pinB > 13))
        return; // No! different ports
    if (pinA > 13 && pinB < 14)
        return; // No! different ports
    if (pinA > 19 || pinB > 19)
        return; // No! out of band

    turning=0;
    clicks=0;

    /* ADD TO THE LIST HERE */
    if (firstEncoder==NULL) { firstEncoder=this; }
    else {
        tmpencoder=firstEncoder;
        while (tmpencoder->next != NULL) tmpencoder=tmpencoder->next;
        tmpencoder->next=this;
    }
    this->next=NULL;


    port=portInputRegister(digitalPinToPort(pinA));
    if (port == NOT_A_PIN) { return; }

    // ** A **
    bitA=digitalPinToBitMask(pinA);
    uint8_t timerA=digitalPinToTimer(pinA);
    //    if (timerA != NOT_ON_TIMER) turnOffPWM(timerA);

    // ** B **
    bitB=digitalPinToBitMask(pinB);
    uint8_t timerB=digitalPinToTimer(pinB);
    //    if (timerB != NOT_ON_TIMER) turnOffPWM(timerB);


    // ** INTERRUPT **
    attachInterrupt(pinA, pinB);
}

void RotaryEncoder::attachInterrupt(uint8_t pinA, uint8_t pinB) {

    PCintPort::attachInterrupt(pinA, this, CHANGE);
    PCintPort::attachInterrupt(pinB, this, CHANGE);

}


void RotaryEncoder::cbmethod() {
    uint8_t stateA;
    uint8_t stateB;
    uint8_t portState;

    portState=*this->port;
    stateA=portState & bitA;
    stateB=portState & bitB;

    if (stateA && stateB ) { // the detent. If we're turning, mark it.
        if (turning) {
            clicks+=direction;
        }
        turning=0; direction=0;		// reset counters.
        return;
    }
    if (stateA == 0 && stateB == 0) { // The 1/2way point.
        //Flag that we've reached it.
        turning=1;
        return;
    }
    if (turning == 0) { // Either stateA!=0 or stateB!=0.
        // We are just starting to turn, so this will
        //indicate direction
        if (stateA) { direction=1; // CCW
            return;
        };                                     // CCW.
        if (stateB) { direction=-1; // CW
            return;
        };                                      // CW.
    }
}

/*
 * GEt Next Indicated Encoder - gets the next encoder with non-zero clicks
 * Gets the next encoder that has been turned, as indicated by a
 * non-zero "clicks" variable.
 *
 * Returns:
 * A pointer to the encoder found.
 *
 */
RotaryEncoder *RotaryEncoder::genie() {

    //	if (currentEncoder == NULL)
    currentEncoder=firstEncoder;
    while (currentEncoder != NULL) {
        if (currentEncoder->clicks) {
            return currentEncoder;
        }
        currentEncoder=currentEncoder->next;
    }
    return NULL;
}

char RotaryEncoder::getID() {
    return id;
}

int8_t RotaryEncoder::getClicks() {
    return clicks;
}

/*
 * query() returns the clicks, but additionally updates the clicks variable.
 */
int8_t RotaryEncoder::query() {
    int8_t tmpclicks;
    tmpclicks=clicks;
    if (clicks < 0) clicks++;
    if (clicks > 0) clicks--;
    return tmpclicks;
}
