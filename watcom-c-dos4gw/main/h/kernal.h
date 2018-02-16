#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

#define TASK (void near *)

#define KERNAL_TASK_ACTIVE 1
#define KERNAL_TASK_INACTIVE 0
#define TIME_KEEPER_INT 0x1C
#define MAX_TASKS 16
#define CONTROL_8253  0x43  // the 8253's control register
#define CONTROL_WORD  0x3C  // the control word to set mode 2,binary least/most
#define COUNTER_0     0x40  // counter 0
#define COUNTER_1     0x41  // counter 1
#define COUNTER_2     0x42  // counter 2

#define TIMER_96HZ    12428
#define TIMER_60HZ    0x4DAE // 60 hz
#define TIMER_50HZ    0x5D37 // 50 hz
#define TIMER_40HZ    0x7486 // 40 hz
#define TIMER_30HZ    0x965C // 30 hz
#define TIMER_20HZ    0xE90B // 20 hz
#define TIMER_18HZ    0xFFFF // 18.2 hz (the standard count and the slowest possible)
#define LOW_BYTE(n) (n & 0x00ff)       // extracts the low-byte of a word
#define HI_BYTE(n)  ((n>>8) & 0x00ff)  // extracts the hi-byte of a word

typedef struct task_typ
        {
         int id;
         int state;
         void (near *task)();
    } KERNAL_Task, *KERNAL_TaskPtr;
    

void KERNAL_Init(void);
void KERNAL_Start(void);
void KERNAL_Stop(void);
void KERNAL_SetSpeed(unsigned int TimesPerSec, unsigned int t);
int  KERNAL_AddTask(void near *function());
void KERNAL_DeleteTask(int id);

#ifndef KERNAL_C
 extern void (_interrupt far *OldTimerIsr)();

 extern KERNAL_Task KERNAL_Tasks[MAX_TASKS];
 extern int KERNAL_NumTasks;
 extern unsigned int KERNAL_Counter, KERNAL_Clicks, KERNAL_TotalClicks;
#else
 void (_interrupt far *OldTimerIsr)();

 KERNAL_Task KERNAL_Tasks[MAX_TASKS];
 int KERNAL_NumTasks;
 unsigned int KERNAL_Counter, KERNAL_Clicks, KERNAL_TotalClicks;
#endif

void Change_Timer(unsigned int new_count);
