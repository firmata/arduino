#include "Modmata.h"
#include "HardwareSerial.h"
using namespace modmata;

ModmataClass Modmata;

void ModmataClass::begin(void)
{
  mb.config(&Serial, 9600, SERIAL_8N1);
  mb.setSlaveId(1);
  
  for(word addr = 0; addr < TOTAL_PINS; addr++) {
    pinConfig[addr] = 0;
    pinState[addr] = false;
    mb.addHreg(addr);
    mb.addCoil(TOTAL_PINS+addr);
  }
}

// Returns register of address being written
word ModmataClass::update()
{
  word addr = mb.task();
  if(addr < TOTAL_PINS) {
    checkPinMode(addr);
  }
  else if(addr < 2*TOTAL_PINS) {
    checkDigitalWrite(addr);
  }
  return addr;
}

void ModmataClass::checkPinMode(word addr)
{
  word mode = mb.Hreg(addr);
  if(pinConfig[addr] != mode) {
    pinConfig[addr] = mode;
    pinMode(addr, mode);
  }
}

void ModmataClass::checkDigitalWrite(word addr)
{
  bool state = mb.Coil(addr);
  int pin = addr - TOTAL_PINS;
  if(pinState[pin] != state) {
    pinState[pin] = state;
    digitalWrite(pin, state);
  } 
}

