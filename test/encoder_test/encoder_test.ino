/*
 * To run this test suite, you must first install to your Arduino/libraries/ directory : 
 * the ArduinoUnit library to your Arduino/libraries/ directory. 
 * available  here: https://github.com/mmurdoch/arduinounit (2.0 or greater.)
 */

//* Due to arduino issue, Wire and Servo libraries need to be included. Should be fixed in later versions
#include <Wire.h>
#include <Servo.h>
//*/
#include <ArduinoUnit.h>
#include "Firmata.h"
#include "utility/EncoderFirmata.h" 

// overide interrupt macro (for unit tests only)
#define IS_PIN_INTERRUPT(p)   ((p) == 2 || (p) == 3) 

FakeStream stream;

void setup()
{
  Firmata.begin(stream); // use fake stream
  
  Serial.begin(57600); // must match port configuration
}



test(attachEncoder)
{  
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  
  assertFalse(encoder.isEncoderAttached(encoderNum));
  
  encoder.attachEncoder(encoderNum, pin1, pin2);

  assertTrue(encoder.isEncoderAttached(encoderNum));
}

test(requiredMemoryPerInstance)
{  
  assertTestPass(attachEncoder);
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  int initialMemory = freeMemory();
  encoder.attachEncoder(encoderNum, pin1, pin2);
  int afterInitializationMemory = freeMemory();
  int requiredMemory = initialMemory - afterInitializationMemory;
  Serial.print(requiredMemory, DEC);
  Serial.println(" mem req. per instance");
  assertTrue(requiredMemory < 16);
}

test(handleAttachEncoderMessage)
{  
  assertTestPass(attachEncoder);
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  
  assertFalse(encoder.isEncoderAttached(encoderNum));
  byte message[]={ENCODER_ATTACH, encoderNum, pin1, pin2};
  encoder.handleSysex(ENCODER_DATA, 4, message);

  assertTrue(encoder.isEncoderAttached(encoderNum));
}

test(detachEncoder)
{
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);

  encoder.detachEncoder(encoderNum);
  assertFalse(encoder.isEncoderAttached(encoderNum));
}

test(handeDetachEncoderMessage)
{
  
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);

  byte message[]={ENCODER_DETACH, encoderNum};
  encoder.handleSysex(ENCODER_DATA, 2, message);
  assertFalse(encoder.isEncoderAttached(encoderNum));
}


test(handlePinModes)
{
  EncoderFirmata encoder;
  
  assertTrue(encoder.handlePinMode(2, ENCODER));  // 2 is interrupt
  assertFalse(encoder.handlePinMode(1, ENCODER)); // 1 is NOT interrupt
}

test(reportEncoderPosition)
{
  assertTestPass(attachEncoder);
  
  EncoderFirmata encoder;
  byte encoderNum = 1, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);
  
  stream.reset();
  encoder.reportPosition(encoderNum);
  assertEqual(stream.bytesWritten().length(), 8);
}

test(handeReportEncoderPositionMessage)
{
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);
  
  stream.reset();
  byte message[]={ENCODER_REPORT_POSITION, encoderNum};
  encoder.handleSysex(ENCODER_DATA, 2, message);
  assertEqual(stream.bytesWritten().length(), 8);
}

test(reportEncodersPositions)
{
  assertTestPass(attachEncoder);
  
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);
  
  stream.reset();
  encoder.reportPositions();
  assertEqual(stream.bytesWritten().length(), 8);
}

test(handleReportEncodersPositionsMessage)
{
  
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);
  
  stream.reset();
  byte message[]={ENCODER_REPORT_POSITIONS};
  encoder.handleSysex(ENCODER_DATA, 1, message);
  assertEqual(stream.bytesWritten().length(), 8);
}

test(enableAutomaticReports)
{
  EncoderFirmata encoder;
  assertTrue(encoder.isReportingEnabled());
  encoder.toggleAutoReport(1);
  assertTrue(encoder.isReportingEnabled());
  encoder.toggleAutoReport(2);
  assertTrue(encoder.isReportingEnabled());
  encoder.toggleAutoReport(0);
  assertFalse(encoder.isReportingEnabled());
}

test(handleEnableAutomaticReportsMessage)
{
  EncoderFirmata encoder;
  encoder.toggleAutoReport(0);
  assertFalse(encoder.isReportingEnabled());

  byte enableMessage[]={ENCODER_REPORT_AUTO, 0x01};
  encoder.handleSysex(ENCODER_DATA, 2, enableMessage);

  assertTrue(encoder.isReportingEnabled());

  byte disableMessage[]={ENCODER_REPORT_AUTO, 0x00};
  encoder.handleSysex(ENCODER_DATA, 2, disableMessage);

  assertFalse(encoder.isReportingEnabled());
}

test(fullReport)
{
  assertTestPass(enableAutomaticReports);
  
  EncoderFirmata encoder;
  
  stream.reset();
  encoder.report();
  assertEqual(stream.bytesWritten().length(), 0); // reports disable
  
  encoder.toggleAutoReport(1);
  
  stream.reset();
  encoder.report();
  assertEqual(stream.bytesWritten().length(), 0); // no encoder attached


  byte pin1 = 2, pin2 = 3;
  encoder.attachEncoder(0, pin1, pin2);

  stream.reset();
  encoder.report();
  assertEqual(stream.bytesWritten().length(), 8); // 1 encoder attached

  encoder.attachEncoder(1, pin1, pin2);

  stream.reset();
  encoder.report();
  assertEqual(stream.bytesWritten().length(), 13); // 2 encoders attached
}

test(resetEncoder)
{
  assertTestPass(attachEncoder);
  
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);
  
  encoder.reset();
  assertFalse(encoder.isEncoderAttached(encoderNum));
}


void loop()
{
  Test::run();
}
