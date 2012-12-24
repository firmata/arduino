#ifndef OneWireScheduler_h
#define OneWireScheduler_h

#include "Arduino.h"
#include "OneWire.h"

#define ONEWIRE_DONE_COMMAND 0
#define ONEWIRE_SEARCH_COMMAND 1
#define ONEWIRE_SKIP_COMMAND 2
#define ONEWIRE_SELECT_COMMAND 3
#define ONEWIRE_WRITE_COMMAND 4
#define ONEWIRE_READ_COMMAND 5
#define ONEWIRE_RESET_COMMAND 6
#define ONEWIRE_DELAY_COMMAND 7
#define ONEWIRE_RESCHEDULE_COMMAND 8

struct ow_config
{
  byte pin;
  OneWire *ow;
  boolean power;
};

struct ow_task
{
  long time_ms;
  byte addr[8];
  ow_config *config;
  ow_task *nextTask;
  byte *currentcommand;
  byte *commands;
};

extern "C" {
// callback function types
//  typedef void (*callbackFunction)(byte, int);
  typedef void (*ow_callbackFunction)(byte,ow_task *,byte);
};

struct ow_command
{
  byte command;
};

struct ow_writecommand
{
  byte command;
  int numbytes;
  byte data[];
};

struct ow_readcommand
{
  byte command;
  int numbytes;
};

struct ow_delaycommand
{
  byte command;
  long time_ms;
};

class OneWireScheduler
{
  public:
  OneWireScheduler(ow_callbackFunction resultFn);
  
  void schedule(ow_config * config,byte *addr,int commandlen, byte *commands, long time_ms);

  void runTasks();

  void reset();

  private:
  ow_task *tasks;
  
  ow_callbackFunction resultFn;

  boolean execute(ow_task *task);

};

#endif
