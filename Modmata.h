#include "Boards.h"
#include "ModbusSerial.h"

namespace modmata {

class ModmataClass
{
  public:
    void begin();
    word update();

  private:
    ModbusSerial mb;
    word pinConfig[TOTAL_PINS];
    bool pinState[TOTAL_PINS];
    void checkPinMode(word addr);
    void checkDigitalWrite(word addr);

};

}

extern modmata::ModmataClass Modmata;
