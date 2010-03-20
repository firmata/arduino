#include <Firmata.h>
#include <DmxSimple.h>

void dmxWriteCallback(byte pin, int value)
{
    // analogWrite is 10-bit, but DMX is 8-bit, so reduce the value
    DmxSimple.write(pin, value/4);
}

void setup()
{
    Firmata.setFirmwareVersion(0, 1);
    Firmata.attach(ANALOG_MESSAGE, dmxWriteCallback);
    Firmata.begin(57600);

    // Firmata only supports 16 analog pins, so limit the DMX output
    DmxSimple.maxChannel(16);
}

void loop()
{
    while(Firmata.available()) {
        Firmata.processInput();
    }
}
