#include "Modmata.h"
#include "HardwareSerial.h"
using namespace modmata;

ModmataClass Modmata;

void ModmataClass::begin(void)
{
  mb.config(&Serial, 9600, SERIAL_8N1);
  mb.setSlaveId(1);

  for(int addr = 0; addr < TOTAL_PINS; addr++) {
    mb.addHreg(addr);
  }
  for(int addr = 0; addr < TOTAL_PINS; addr++) {
    mb.addCoil(TOTAL_PINS+addr);
  }
}

void ModmataClass::update()
{
  mb.task();
  checkPinMode();
  checkDigitalWrite();
}

void ModmataClass::checkPinMode()
{
  // TODO: Simplify this so that it only updates whenever a pinMode message is recieved,
  // rather than repeatedly checking every address
  for(int addr = 0; addr < TOTAL_PINS; addr++) {
    uint8_t mode = mb.Hreg(addr);
    if(pinConfig[addr] != mode) {
      pinConfig[addr] = mode;
      pinMode(addr, mode);
    }
  }
}

void ModmataClass::checkDigitalWrite()
{
  // TODO: Simplify to only check when digitalWrite message is recieved
  for(int addr = 0; addr < TOTAL_PINS; addr++) {
    int state = mb.Coil(TOTAL_PINS+addr);
    if(pinState[addr] != state) {
      pinState[addr] = state;
      digitalWrite(addr, state);
    }
  }
}

