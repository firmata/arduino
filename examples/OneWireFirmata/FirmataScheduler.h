#ifndef FirmataScheduler_h
#define FirmataScheduler_h

#include "Arduino.h"
#include "Firmata.h"
#include "Encoder7Bit.h"

//sysex commands
#define SCHEDULER_REQUEST       0x7B // send a createtask/deletetask/addtotask/schedule/querytasks/querytask request to the scheduler
#define SCHEDULER_REPLY         0x7C // a reply to a querytasks/querytask-request from the scheduler

//subcommands
#define CREATE_FIRMATA_TASK 0
#define DELETE_FIRMATA_TASK 1
#define ADD_TO_FIRMATA_TASK 3
#define SCHEDULE_FIRMATA_TASK 4
#define QUERY_ALL_FIRMATA_TASKS 5
#define QUERY_FIRMATA_TASK 6
#define RESET_FIRMATA_TASKS 7
#define ERROR_FIRMATA_TASK 8

#define firmata_task_len(a)(sizeof(firmata_task)+(a)->len)

struct firmata_task
{
  byte id; //only 7bits used -> supports 127 tasks
  long time_ms;
  int len;
  firmata_task *nextTask;
  int pos;
  byte messages[];
};

class FirmataSchedulerClass
{
  public:
  
  FirmataSchedulerClass();
  void handleSchedulerRequest(byte subcommand, byte argc, byte*argv);
  void runTasks();
  void reset();
  void createTask(byte id,int len);
  void deleteTask(byte id);
  void addToTask(byte id, int len, byte *message);
  void schedule(byte id,long time_ms);
  void delayTask(long time_ms);
  void queryAllTasks();
  void queryTask(byte id);

  private:

  firmata_task *tasks;
  firmata_task *running;
  
  boolean execute(firmata_task *task);
  firmata_task *findTask(byte id);
  void reportTask(byte id, firmata_task *task, boolean error);
};

extern FirmataSchedulerClass FirmataScheduler;

#endif
