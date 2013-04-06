#include <ArduinoUnit.h>
#include <Firmata.h>

TestSuite suite;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  suite.run();
}

void assertStringsEqual(Test& __test__, const char* expected, const String& actual)
{
  size_t expectedLength = strlen(expected);
  assertEquals(expectedLength, actual.length());
  for (size_t i = 0; i < expectedLength; i++)
  {
    assertEquals(expected[i], actual[i]);
  }
}

// test(setFirmwareVersionDoesNotLeakMemory)
// {
//   Firmata.setFirmwareVersion(1, 0);
//   int initialMemory = freeMemory();

//   Firmata.setFirmwareVersion(1, 0);

//   assertEquals(0, initialMemory - freeMemory());
  
//   Firmata.unsetFirmwareVersion();
// }

test(beginPrintsVersion)
{
  FakeStream stream;

  Firmata.begin(stream);

  char expected[] = 
  {
    REPORT_VERSION,
    FIRMATA_MAJOR_VERSION,
    FIRMATA_MINOR_VERSION,
    0
  };
  assertStringsEqual(__test__, expected, stream.bytesWritten());
}

void processMessage(const byte* message, size_t length)
{
  FakeStream stream;
  Firmata.begin(stream);

  for (size_t i = 0; i < length; i++)
  {
    stream.nextByte(message[i]);
    Firmata.processInput();
  }
}

byte _digitalPort;
int _digitalPortValue;
void writeToDigitalPort(byte port, int value)
{
  _digitalPort = port;
  _digitalPortValue = value;
}

void setupDigitalPort() {
  _digitalPort = 0;
  _digitalPortValue = 0;
}

test(processWriteDigital_0)
{
  setupDigitalPort();
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 0, 0 };
  processMessage(message, 3);

  assertEquals(0, _digitalPortValue);
}

test(processWriteDigital_127)
{
  setupDigitalPort();
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 127, 0 };
  processMessage(message, 3);

  assertEquals(127, _digitalPortValue);
}

test(processWriteDigital_128)
{
  setupDigitalPort();
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 0, 1 };
  processMessage(message, 3);

  assertEquals(128, _digitalPortValue);
}

test(processWriteLargestDigitalValue)
{
  setupDigitalPort();
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 0x7F, 0x7F };
  processMessage(message, 3);

  // Maximum of 14 bits can be set (B0011111111111111)
  assertEquals(0x3FFF, _digitalPortValue);
}

test(defaultDigitalWritePortIsZero)
{
  setupDigitalPort();
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 0, 0 };
  processMessage(message, 3);

  assertEquals(0, _digitalPort);
}

test(specifiedDigitalWritePort)
{
  setupDigitalPort();
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE + 1, 0, 0 };
  processMessage(message, 3);

  assertEquals(1, _digitalPort);
}

