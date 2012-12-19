#include <OneWire.h>
#include <Boards.h>
#include <Firmata.h>

struct ow_pin_config {
  byte pin;
  OneWire* ow;
  boolean power;
};

struct ow_report_config {
  byte addr[8];
  byte preReadCommand;
  long readDelay;
  byte readCommand;
  int  numBytes;
  ow_pin_config *pinConfig;
};

void readAndReportOWData(ow_report_config *owConfig);
