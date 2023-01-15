#include "Boards.h"
#include "modbus-arduino/ModbusSerial.h"

namespace modmata {

class ModmataClass
{
  public:
    void begin();
    void update();

  private:
    ModbusSerial mb;
    uint8_t pinConfig[TOTAL_PINS];
    int pinState[TOTAL_PINS];
    void checkPinMode();
    void checkDigitalWrite();

};

}

extern modmata::ModmataClass Modmata;
