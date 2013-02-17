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

class InMemoryStream : public Stream
{
public:
  virtual ~InMemoryStream() 
  {
  }

  size_t write(uint8_t val)
  {
    _bytesWritten += (char) val;

    return size_t(1);
  }

  void flush() 
  {
  }

  const String& bytesWritten() 
  {
    return _bytesWritten;
  }

  void nextByte(byte b)
  {
    _nextByte = b;
  }

  int available() 
  {
    return 1;
  }

  int read() 
  {
    return _nextByte;
  }

  int peek()
  {
    return _nextByte;
  }

private:
  String _bytesWritten;
  byte _nextByte;
};

void assertStringsEqual(Test& __test__, const char* expected, const String& actual)
{
  size_t expectedLength = strlen(expected);
  assertEquals(expectedLength, actual.length());
  for (size_t i = 0; i < strlen(expected); i++)
  {
    assertEquals(expected[i], actual[i]);
  }
}

test(beginPrintsVersion)
{
  InMemoryStream stream;

  Firmata.begin(stream);

  char expected[] = 
  {
    0xF9, // Version reporting identifier 
    2,    // Major version number
    3,    // Minor version number
    0
  };
  assertStringsEqual(__test__, expected, stream.bytesWritten());
}

void processMessage(const byte* message, size_t length)
{
  InMemoryStream stream;
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

test(processWriteDigital_0)
{
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 0, 0 };
  processMessage(message, 3);

  assertEquals(0, _digitalPortValue);
}

test(processWriteDigital_127)
{
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 127, 0 };
  processMessage(message, 3);

  assertEquals(127, _digitalPortValue);
}

test(processWriteDigitalStripsTopBit)
{
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, B11111111, 0 };
  processMessage(message, 3);

  assertEquals(B01111111, _digitalPortValue);
}

test(processWriteDigital_128)
{
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 0, 1 };
  processMessage(message, 3);

  assertEquals(128, _digitalPortValue);
}

test(processWriteLargestDigitalValue)
{
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 0x7F, 0x7F };
  processMessage(message, 3);

  // Maximum of 14 bits can be set (B0011111111111111)
  assertEquals(0x3FFF, _digitalPortValue);
}

test(defaultDigitalWritePortIsZero)
{
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE, 0, 0 };
  processMessage(message, 3);

  assertEquals(0, _digitalPort);
}

test(specifiedDigitalWritePort)
{
  Firmata.attach(DIGITAL_MESSAGE, writeToDigitalPort);
  
  byte message[] = { DIGITAL_MESSAGE + 1, 0, 0 };
  processMessage(message, 3);

  assertEquals(1, _digitalPort);
}

