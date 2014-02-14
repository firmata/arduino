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

  assertTrue((initialMemory - freeMemory()) < 1 );
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
  
  assertTrue(encoder.handlePinMode(2, INPUT));  // 2 is interrupt
  assertFalse(encoder.handlePinMode(1, INPUT)); // 1 is NOT interrupt
}

test(reportEncoderPosition)
{
  assertTestPass(attachEncoder);
  
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);
  
  stream.flush();
  encoder.reportPosition(encoderNum);
  assertEqual(stream.bytesWritten().length(), 6);
}

test(handeReportEncoderPositionMessage)
{
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);
  
  stream.flush();
  byte message[]={ENCODER_REPORT_POSITION, encoderNum};
  encoder.handleSysex(ENCODER_DATA, 2, message);
  assertEqual(stream.bytesWritten().length(), 6);
}

test(reportEncodersPositions)
{
  assertTestPass(attachEncoder);
  
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);
  
  stream.flush();
  encoder.reportPositions();
  assertEqual(stream.bytesWritten().length(), 9);
}

test(handleReportEncodersPositionsMessage)
{
  
  EncoderFirmata encoder;
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);
  
  stream.flush();
  byte message[]={ENCODER_REPORT_POSITIONS};
  encoder.handleSysex(ENCODER_DATA, 1, message);
  assertEqual(stream.bytesWritten().length(), 9);
}

test(enableAutomaticReports)
{
  EncoderFirmata encoder;
  assertFalse(encoder.isReportingEnabled());
  encoder.toggleAutoReport(true);
  assertTrue(encoder.isReportingEnabled());
  encoder.toggleAutoReport(false);
  assertFalse(encoder.isReportingEnabled());
}

test(handleEnableAutomaticReportsMessage)
{
  EncoderFirmata encoder;
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
  
  stream.flush();
  encoder.report();
  assertEqual(stream.bytesWritten().length(), 0); // reports disable
  
  encoder.toggleAutoReport(true);
  
  stream.flush();
  encoder.report();
  assertEqual(stream.bytesWritten().length(), 0); // no encoder attached


  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder.attachEncoder(encoderNum, pin1, pin2);

  stream.flush();
  encoder.report();
  assertEqual(stream.bytesWritten().length(), 6); // 1 encoder attached
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
