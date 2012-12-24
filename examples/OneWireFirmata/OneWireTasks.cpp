#include <OneWireTasks.h>
#include <new.h>

OneWireCommand::OneWireCommand(byte command,int len, byte *data)
{
  this.command = command;
  this.len = len;
  this.data = data;
  nextCommand = NULL;
}

OneWireScheduler::OneWireScheduler(void(*resultFn)(byte,ow_task *,byte)) {
  this.resultFn = resultFn;
}

void OneWireScheduler::schedule(byte *addr, byte *commands, long time_ms, boolean runOnce);
{
  ow_task *newTask = new ow_task;
  if (addr) {
    for (byte i=0;i<8;i++) {
      newTask->addr[i]=addr[i];
    }
  }
  newTask->currentcommand = commands;
  if (!runOnce) {
    newTask->commands = commands;
  }
  newTask->time_ms = time_ms;

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
}

boolean *OneWireScheduler::execute(ow_task *task)
{
  ow_config *config = task->config;
  OneWire *ow = config->ow;
  while (TRUE) {
    switch(task->currentcommand) {
    case ONEWIRE_SEARCH: 
      {
        ow->reset_search();
        if (ow->search(task->addr)) {
          scheduler->resultFn(ONEWIRE_SEARCH,task,0);
        };
        scheduler->resultFn(ONEWIRE_DONE,task,0);
        task->currentcommand+=sizeof(ow_command);
        break;
      }
    case ONEWIRE_SKIP: 
      {
        ow->skip();
        task->currentcommand+=sizeof(ow_command);
        break;
      }
    case ONEWIRE_SELECT:
      {
        ow->select(task->addr);
        task->currentcommand+=sizeof(ow_command);
        break;
      }
    case ONEWIRE_WRITE: 
      {
        ow_writecommand *writecommand = (ow_writecommand *)(task->currentcommand);
        for (int i=0;i<writecommand->numbytes;i++) {
          ow->write(writecommand->data[i],config->power);
        }
        task->currentcommand+=(sizeof(ow_writecommand)+writecommand->numbytes);
        break;
      }
    case ONEWIRE_READ: 
      {
        ow_readcommand *readcommand = (ow_readcommand *)(task->currentcommand);
        for (int i=0;i<readcommand->numbytes;i++) {
          scheduler->resultFn(ONEWIRE_READ,task,ow->read());
        }
        scheduler->resultFn(ONEWIRE_DONE,task,NULL);
        task->currentcommand+=sizeof(ow_readcommand);
        break;
      }
    case ONEWIRE_RESET:
      {
        ow->reset();
        task->currentcommand+=sizeof(ow_command);
        break;
      }
    case ONEWIRE_DELAY: 
      {
        ow_delaycommand *delaycommand = (ow_delaycommand *)(task->currentcommand);
        task->time_ms+=delaycommand->time_ms;
        task->currentcommand+=sizeof(ow_delaycommand);
        return TRUE;
      }
    case ONEWIRE_RESCHEDULE:
      {
        ow_delaycommand *delaycommand = (ow_delaycommand *)(task->currentcommand);
        task->time_ms+=delaycommand->time_ms;
        task->currentcommand=task->commands;
        return TRUE;
      }
    case ONEWIRE_DONE:
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
        } 
        else {
          if (previous) {
            previous->nextTask=current->nextTask;
          }
        }
      }
      current = current->nextTask; 
    }
  }
}







