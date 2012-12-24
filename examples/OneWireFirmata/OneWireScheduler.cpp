#include "Arduino.h"
#include "OneWireScheduler.h"
//#include <new.h>

OneWireScheduler::OneWireScheduler(ow_callbackFunction resultFn) {
  this->resultFn = resultFn;
};

void OneWireScheduler::schedule(ow_config* config, byte *addr,int commandlen, byte *commands, long time_ms)
{
  ow_task *newTask = new ow_task;
  for (byte i=0;i<8;i++) {
    newTask->addr[i]= addr ? addr[i] : 0x00;
  }
  newTask->commands = (byte *) malloc(commandlen);
  for (int i=0;i<commandlen;i++) {
    newTask->commands[i]=commands[i];
  }
  newTask->currentcommand = commands;
  newTask->commands = commands;
  newTask->time_ms = time_ms;
  newTask->config = config;
  newTask->nextTask = NULL;

  if (tasks) {
    ow_task *currentTask = tasks;
    while (currentTask->nextTask) {
      currentTask = currentTask->nextTask;
    }
    currentTask->nextTask = newTask;
  } 
  else {
    tasks = newTask;
  }
};

boolean OneWireScheduler::execute(ow_task *task)
{
  ow_config *config = task->config;
  OneWire *ow = config->ow;
  while (TRUE) {
    switch(*task->currentcommand) {
    case ONEWIRE_SEARCH_COMMAND: 
      {
        ow->reset_search();
        while (ow->search(task->addr)) {
          this->resultFn(ONEWIRE_SEARCH_COMMAND,task,0);
        };
        this->resultFn(ONEWIRE_DONE_COMMAND,task,0);
        task->currentcommand+=sizeof(ow_command);
        break;
      }
    case ONEWIRE_SKIP_COMMAND: 
      {
        ow->skip();
        task->currentcommand+=sizeof(ow_command);
        break;
      }
    case ONEWIRE_SELECT_COMMAND:
      {
        if (task->addr) {
          for(byte i=0;i<8;i++) {
          }
        }
        ow->select(task->addr);
        task->currentcommand+=sizeof(ow_command);
        break;
      }
    case ONEWIRE_WRITE_COMMAND: 
      {
        ow_writecommand *writecommand = (ow_writecommand *)(task->currentcommand);
        for (int i=0;i<writecommand->numbytes;i++) {
          ow->write(writecommand->data[i],config->power);
        }
        task->currentcommand+=(sizeof(ow_writecommand)+writecommand->numbytes);
        break;
      }
    case ONEWIRE_READ_COMMAND: 
      {
        ow_readcommand *readcommand = (ow_readcommand *)(task->currentcommand);
        for (int i=0;i<readcommand->numbytes;i++) {
          this->resultFn(ONEWIRE_READ_COMMAND,task,ow->read());
        }
        this->resultFn(ONEWIRE_DONE_COMMAND,task,NULL);
        task->currentcommand+=sizeof(ow_readcommand);
        break;
      }
    case ONEWIRE_RESET_COMMAND:
      {
        ow->reset();
        task->currentcommand+=sizeof(ow_command);
        break;
      }
    case ONEWIRE_DELAY_COMMAND: 
      {
        ow_delaycommand *delaycommand = (ow_delaycommand *)(task->currentcommand);
        task->time_ms+=delaycommand->time_ms;
        task->currentcommand+=sizeof(ow_delaycommand);
        return TRUE;
      }
    case ONEWIRE_RESCHEDULE_COMMAND:
      {
        ow_delaycommand *delaycommand = (ow_delaycommand *)(task->currentcommand);
        task->time_ms+=delaycommand->time_ms;
        task->currentcommand=task->commands;
        return TRUE;
      }
    case ONEWIRE_DONE_COMMAND:
      {
        return FALSE;
      }
    }
  }
}

void OneWireScheduler::runTasks()
{
  if (tasks) {
    long now = millis();
    ow_task *current = tasks;
    ow_task *previous = NULL;
    while (current) {
      if (current->time_ms < now) {
        if(this->execute(current)) {
          previous=current;
          current = current->nextTask;
        } 
        else {
          if (previous) {
            previous->nextTask=current->nextTask;
            delete current->commands;
            delete current;
            current = previous->nextTask;
          }
          else {
            tasks=current->nextTask;
            delete current->commands;
            delete current;
            current=tasks;
          }
        }
      }
      else {
        current=current->nextTask;
      }
    }
  }
}

void OneWireScheduler::reset()
{
  while (tasks) {
    ow_task *next = tasks->nextTask;
    delete tasks->commands;
    delete tasks;
    tasks = next;
  }
}

