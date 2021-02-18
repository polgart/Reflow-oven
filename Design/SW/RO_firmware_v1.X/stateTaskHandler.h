/* 
 * File:   stateTaskHandler.h
 * Author: EDU_OBOS_5057
 *
 * Created on February 16, 2021, 3:03 PM
 */

#ifndef STATETASKHANDLER_H
#define	STATETASKHANDLER_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct stateTaskList_s stateTaskList;
void stateTaskHandler(stateTaskList* task);
stateTaskList* createTask(void(*function)(void));
void initilaizeTaskHandler(stateTaskList* idleTask);
void addTask(stateTaskList* idleTask,stateTaskList* newTask);


struct stateTaskList_s {
    void (*fun_ptr)(void);
    struct stateTaskList_s* next;
    struct stateTaskList_s* prev;
    int simaint;
};


#ifdef	__cplusplus
}
#endif

#endif	/* STATETASKHANDLER_H */

