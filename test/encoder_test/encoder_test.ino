/*
 * To run this test suite, you must first install to your Arduino/libraries/ directory : 
 * the ArduinoUnit library to your Arduino/libraries/ directory. 
 * available  here: https://github.com/mmurdoch/arduinounit (2.0 or greater.)
 */
#include <Wire.h>
#include <Servo.h>
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

void loop()
{
  Test::run();
}

test(fullTest)
{
  Serial.print(F("BT mem.: "));
  Serial.println(freeMemory());
  
  EncoderFirmata encoder;

  assertTrue(encoder.handlePinMode(2, INPUT));  // 2 is interrupt
  assertFalse(encoder.handlePinMode(1, INPUT)); // 1 is NOT intterrupt

  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  
  byte attachEncoderMessage[] = { ENCODER_ATTACH, encoderNum, pin1, pin2 };
  byte detachEncoderMessage[] = { ENCODER_DETACH, encoderNum };
  
  assertFalse(encoder.isEncoderAttached(encoderNum));

  assertTrue(encoder.handleSysex(ENCODER_DATA, 0, attachEncoderMessage));

  assertTrue(encoder.isEncoderAttached(encoderNum));

  assertTrue(encoder.handleSysex(ENCODER_DATA, 0, detachEncoderMessage));
  assertFalse(encoder.isEncoderAttached(encoderNum));

  encoder.handleSysex(ENCODER_DATA, 0, attachEncoderMessage);
  assertTrue(encoder.isEncoderAttached(encoderNum));
  //report encoder position
  byte reportPosition[] = {ENCODER_REPORT_POSITION, encoderNum};
  stream.flush();
  assertTrue(encoder.handleSysex(ENCODER_DATA, 0, reportPosition));
  assertEqual(stream.bytesWritten().length(), 6);

  byte reportPositions[] = {ENCODER_REPORT_POSITIONS};
  stream.flush();
  assertTrue(encoder.handleSysex(ENCODER_DATA, 0, reportPositions));
  assertEqual(stream.bytesWritten().length(), 9);


  // check that auto-report is disable by default
  assertFalse(encoder.autoReport);
  stream.flush();
  encoder.report(); 
  assertTrue(stream.bytesWritten().length()==0);

  
  //  simulate logical reset
  encoder.reset();
  assertFalse(encoder.isEncoderAttached(encoderNum));

  // Enable auto reports
  byte enableReportsMessage[] = {ENCODER_REPORT_AUTO, 0x01};
  assertTrue(encoder.handleSysex(ENCODER_DATA, 0, enableReportsMessage));
  assertTrue(encoder.autoReport);
  stream.flush();
  encoder.report();
  assertEqual(stream.bytesWritten().length(), 3); // i.e : no encoder attached, message is empty (expt sysex start/end and ENCODER_DATA message)

  Serial.print(F("AT mem.: "));
  Serial.println(freeMemory());
}
