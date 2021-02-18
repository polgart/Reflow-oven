#include "stateTaskHandler.h"
#include <stdlib.h>

/**
  stateTaskList 

  @Summary


  @Description

 */


/**
  stateTaskList 

  @Summary


  @Description

 */

void stateTaskHandler(stateTaskList* task) {
    while(1) {
        task->fun_ptr(); // Perform current task
        task->prev->next = task->next; // Remove current task from que
        task->next->prev = task->prev; // Remove current task from que
        task=task->next; // Go to next task
    }
}

/**
  stateTaskList 

  @Summary


  @Description

 */

void initilaizeTaskHandler(stateTaskList* idleTask) {
    idleTask->prev=idleTask;
    idleTask->next=idleTask;
    idleTask->simaint=42;
}

/**
  stateTaskList 

  @Summary


  @Description

 */

stateTaskList* createTask(void(*function)(void)) {
    stateTaskList* newTask = (stateTaskList *)malloc(sizeof(struct stateTaskList_s));
    newTask->next=NULL;
    newTask->prev=NULL;
    newTask->fun_ptr=function;
    return newTask;
}

/**
  stateTaskList 

  @Summary


  @Description

 */

void addTask(stateTaskList* idleTask,stateTaskList* newTask) {
    /* Check the input */
    if (idleTask==newTask)
        return;
    
    
    stateTaskList* lastTask = idleTask;
    /* Find last task */
    char alreadyAddedTask = 0x0;
    while (lastTask->next->prev != lastTask->next) {
        lastTask=lastTask->next;
        if (newTask==lastTask) {
            alreadyAddedTask = 0x1;
            break;
        }
    }
    
    /* Append the new task to the list */
    if (alreadyAddedTask==0x0) {
        lastTask->next = newTask;
        newTask->prev = lastTask;
        newTask->next = idleTask;
    }
}