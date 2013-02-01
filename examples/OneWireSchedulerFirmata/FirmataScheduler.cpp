#include "Arduino.h"
#include "Encoder7Bit.h"
#include "FirmataScheduler.h"
//#include <new.h>

FirmataSchedulerClass::FirmataSchedulerClass() {
  tasks = NULL;
  running = NULL;
}

void FirmataSchedulerClass::handleSchedulerRequest(byte subcommand, byte argc, byte*argv) {
  switch(subcommand) {
  case CREATE_FIRMATA_TASK: 
    {
      if (argc==3) {
        createTask(argv[0],argv[1] | argv[2]<<7);
      }
      break;
    }
  case DELETE_FIRMATA_TASK: 
    {
      if (argc==1) {
        deleteTask(argv[0]);
      }
      break;
    }
  case ADD_TO_FIRMATA_TASK: 
    {
      if (argc>1) {
        int len = num7BitOutbytes(argc-1);
        Encoder7Bit.readBinary(len,argv+1, argv+1); //decode inplace
        addToTask(argv[0],len,argv+1); //addToTask copies data...
      }
      break;
    }
  case DELAY_FIRMATA_TASK:
    {
      if (argc==5) {
        Encoder7Bit.readBinary(4,argv,argv); //decode inplace
        delayTask(*(long*)((byte*)argv));
      }
      break;
    }
  case SCHEDULE_FIRMATA_TASK: 
    {
      if (argc==6) { //one byte taskid, 5 bytes to encode 4 bytes of long
        Encoder7Bit.readBinary(4,argv+1, argv+1); //decode inplace
        schedule(argv[0],*(long*)((byte*)argv+1)); //argv[1] | argv[2]<<8 | argv[3]<<16 | argv[4]<<24
      }
      break;
    }
  case QUERY_ALL_FIRMATA_TASKS: 
    {
      queryAllTasks();
      break;
    }
  case QUERY_FIRMATA_TASK: 
    {
      if (argc==1) {
        queryTask(argv[0]);
      }
      break;
    }
  case RESET_FIRMATA_TASKS:
    {
      reset();
    }
  }
};

void FirmataSchedulerClass::createTask(byte id,int len) {
  firmata_task *existing = findTask(id);
  if (existing) {
    reportTask(id,existing,true);
  } 
  else {
    firmata_task *newTask = (firmata_task*)malloc(sizeof(firmata_task)+len);
    newTask->id=id;
    newTask->time_ms=0;
    newTask->len=len;
    newTask->nextTask=tasks;
    newTask->pos=0;
    tasks = newTask;
  }
};

void FirmataSchedulerClass::deleteTask(byte id) {
  firmata_task *current = tasks;
  firmata_task *previous = NULL;
  while (current) {
    if(current->id == id) {
      if (previous) {
        previous->nextTask=current->nextTask;
      }
      else {
        tasks=current->nextTask;
      }
      free (current);
      return;
    }
    else {
      previous=current;
      current = current->nextTask;
    }
  }
};

void FirmataSchedulerClass::addToTask(byte id, int additionalBytes, byte *message) {
  firmata_task *existing = findTask(id);
  if (existing) { //task exists and has not been fully loaded yet
    if (existing->pos+additionalBytes<=existing->len) {
      for (int i=0;i<additionalBytes;i++) {
        existing->messages[existing->pos++]=message[i];
      }
    }
  }
  else {
    reportTask(id,NULL,true);
  }
};

void FirmataSchedulerClass::schedule(byte id,long delay_ms) {
  firmata_task *existing = findTask(id);
  if (existing) {
    existing->pos = 0;
    existing->time_ms = millis() + delay_ms;
  } 
  else {
    reportTask(id,NULL,true);
  }
};

void FirmataSchedulerClass::delayTask(long delay_ms) {
  if (running) {
    long now = millis();
    running->time_ms += delay_ms;
    if (running->time_ms<now) { //if delay time allready passed by schedule to 'now'.
      running->time_ms = now;
    }
  }
}

void FirmataSchedulerClass::queryAllTasks() {
  Serial.write(START_SYSEX);
  Serial.write(SCHEDULER_DATA);
  Serial.write(QUERY_ALL_TASKS_REPLY);
  firmata_task *task = tasks;
  while(task) {
    Serial.write(task->id);
    task=task->nextTask;
  }
  Serial.write(END_SYSEX);
};

void FirmataSchedulerClass::queryTask(byte id) {
  firmata_task *task = findTask(id);
  reportTask(id,task,false);
}

void FirmataSchedulerClass::reportTask(byte id, firmata_task *task, boolean error) {
  Serial.write(START_SYSEX);
  Serial.write(SCHEDULER_DATA);
  if (error) {
    Serial.write(ERROR_TASK_REPLY);
  } else {
    Serial.write(QUERY_TASK_REPLY);
  }
  Serial.write(id);
  if (task) {
    Encoder7Bit.startBinaryWrite();
    Encoder7Bit.writeBinary(firmata_task_len(task)-3,((byte *)task)+3); //don't write first 3 bytes (firmata_task*, byte); makes use of AVR byteorder (LSB first)
    Encoder7Bit.endBinaryWrite();
  }
  Serial.write(END_SYSEX);
};

void FirmataSchedulerClass::runTasks() {
  if (tasks) {
    long now = millis();
    firmata_task *current = tasks;
    firmata_task *previous = NULL;
    while (current) {
      if (current->time_ms>0 && current->time_ms < now) { // TODO handle overflow
        if(execute(current)) {
          previous=current;
          current = current->nextTask;
        } 
        else {
          if (previous) {
            previous->nextTask=current->nextTask;
            free(current);
            current = previous->nextTask;
          }
          else {
            tasks=current->nextTask;
            free(current);
            current=tasks;
          }
        }
      }
      else {
        current=current->nextTask;
      }
    }
  }
};

void FirmataSchedulerClass::reset() {
  while (tasks) {
    firmata_task *nextTask = tasks->nextTask;
    free(tasks);
    tasks = nextTask;
  }
};

//private
boolean FirmataSchedulerClass::execute(firmata_task *task)
{ 
  long start = task->time_ms;
  int pos = task->pos;
  int len = task->len;
  byte *messages = task->messages;
  running = task;
  while (pos<len) {
    Firmata.parse(messages[pos++]);
    if (start!=task->time_ms) { // return true if task got rescheduled during run.
      task->pos = ( pos == len ? 0 : pos ); // last message executed? -> start over next time
      running = NULL;
      return true;
    }
  };
  running = NULL;
  return false;
}

firmata_task *FirmataSchedulerClass::findTask(byte id)
{
  firmata_task *currentTask = tasks;
  while (currentTask) {
    if (id == currentTask->id) {
      return currentTask;
    } else {
      currentTask = currentTask->nextTask;
    }
  };
  return NULL;
}

FirmataSchedulerClass FirmataScheduler;
