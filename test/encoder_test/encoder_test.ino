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

test(attachEncoder)
{
  int initial_memory = freeMemory();
  
  EncoderFirmata *encoder = new EncoderFirmata();
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  
  assertFalse(encoder->isEncoderAttached(encoderNum));
  
  encoder->attachEncoder(encoderNum, pin1, pin2);

  assertTrue(encoder->isEncoderAttached(encoderNum));
  
  delete encoder;
  assertEqual(0, initialMemory - freeMemory()); // no memory leak
}

test(detachEncoder)
{
  assertTestPass(attachEncoder);
  int initial_memory = freeMemory();
  
  EncoderFirmata *encoder = new EncoderFirmata();
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder->attachEncoder(encoderNum, pin1, pin2);

  encoder->detachEncoder(encoderNum);
  assertFalse(encoder->isEncoderAttached(encoderNum));
  
  delete encoder;
  assertEqual(0, initialMemory - freeMemory()); // no memory leak
}

test(handlePinModes)
{
  int initial_memory = freeMemory();
  
  EncoderFirmata *encoder = new EncoderFirmata();
  
  assertTrue(encoder->handlePinMode(2, INPUT));  // 2 is interrupt
  assertFalse(encoder->handlePinMode(1, INPUT)); // 1 is NOT interrupt
  
  delete encoder;
  assertEqual(0, initialMemory - freeMemory()); // no memory leak
}

test(reportEncoderPosition)
{
  assertTestPass(attachEncoder);
  int initial_memory = freeMemory();
  
  EncoderFirmata *encoder = new EncoderFirmata();
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder->attachEncoder(encoderNum, pin1, pin2);
  
  stream.flush();
  encoder->reportPosition(encoderNum);
  assertEqual(stream.bytesWritten().length(), 6);
  
  delete encoder;
  assertEqual(0, initialMemory - freeMemory()); // no memory leak
}

test(reportEncodersPositions)
{
  assertTestPass(attachEncoder);
  int initial_memory = freeMemory();
  
  EncoderFirmata *encoder = new EncoderFirmata();
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder->attachEncoder(encoderNum, pin1, pin2);
  
  stream.flush();
  encoder->reportPositions();
  assertEqual(stream.bytesWritten().length(), 9);
  
  delete encoder;
  assertEqual(0, initialMemory - freeMemory()); // no memory leak
}

test(enableAutomaticReports)
{
  int initial_memory = freeMemory();
  
  EncoderFirmata *encoder = new EncoderFirmata();
  assertFalse(encoder->isReportingEnabled());
  encoder->toggleAutoReport(true);
  assertTrue(encoder->isReportingEnabled());
  encoder->toggleAutoReport(false);
  assertFalse(encoder->isReportingEnabled());
  
  delete encoder;
  assertEqual(0, initialMemory - freeMemory()); // no memory leak
}

test(fullReport)
{
  assertTestPass(enableAutomaticReports);
  int initial_memory = freeMemory();
  
  EncoderFirmata *encoder = new EncoderFirmata();
  
  stream.flush();
  encoder->report();
  assertEqual(stream.bytesWritten().length(), 0); // reports disable
  
  encoder->toggleAutoReport(true);
  
  stream.flush();
  encoder->report();
  assertEqual(stream.bytesWritten().length(), 3); // reports enable
  
  delete encoder;
  assertEqual(0, initialMemory - freeMemory()); // no memory leak
}

test(resetEncoder)
{
  assertTestPass(attachEncoder);
  int initial_memory = freeMemory();
  
  EncoderFirmata *encoder = new EncoderFirmata();
  byte encoderNum = 0, pin1 = 2, pin2 = 3;
  encoder->attachEncoder(encoderNum, pin1, pin2);
  
  encoder->reset();
  assertFalse(encoder->isEncoderAttached(encoderNum));
  
  delete encoder;
  assertEqual(0, initialMemory - freeMemory()); // no memory leak
}

void loop()
{
  Test::run();
}
