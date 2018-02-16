#define KERNAL_C
#include "toolkit.h"

void _interrupt KERNAL_Isr(void)
{                              
 static cur_task = 0;
 
 if (KERNAL_NumTasks>0)
  {
   while (KERNAL_Tasks[cur_task].state!=KERNAL_TASK_ACTIVE)
     {
      if (++cur_task>=MAX_TASKS)
        cur_task = 0;
     }
   KERNAL_Tasks[cur_task].task();
 
   if (++cur_task>=MAX_TASKS)
     cur_task = 0;
  }

 KERNAL_Clicks += KERNAL_Counter;
 if (KERNAL_Clicks >= 0x10000)
  {
   KERNAL_Clicks = 0;
   OldTimerIsr();
  }
 else
  outp(0x20, 0x20);
} 

void KERNAL_Isr_End(void)
{} //Dummy to define end of region for DPMI server to lock!!

void KERNAL_Init(void)
{
 int index;
 
 for (index=0;index<MAX_TASKS;index++)
   {
    KERNAL_Tasks[index].id = index;
    KERNAL_Tasks[index].state = KERNAL_TASK_INACTIVE;
    KERNAL_Tasks[index].task = NULL;
   }
}

void KERNAL_Start(void)
{
 DPMI_LockRegion((void near *)KERNAL_Isr, (char *)KERNAL_Isr_End - (char near *) KERNAL_Isr);
 OldTimerIsr = _dos_getvect(TIME_KEEPER_INT);
 _dos_setvect(TIME_KEEPER_INT, KERNAL_Isr);
}

void KERNAL_Stop(void)
{
 Change_Timer(TIMER_18HZ);
 _dos_setvect(TIME_KEEPER_INT, OldTimerIsr);
}

void KERNAL_SetSpeed(unsigned int TimesPerSec, unsigned int t)
{
  KERNAL_Counter = 1193180 / TimesPerSec;
  KERNAL_Clicks = 0;
  KERNAL_TotalClicks = t;

  outp(0x43, 0x3C);
  outp(0x40, KERNAL_Counter & 0x00FF);
  outp(0x40, (KERNAL_Counter>>8) & 0x00FF);
}

int KERNAL_AddTask(void (near *function)())
{
 int i;

 for (i=0;i<MAX_TASKS;i++)
   {
    if (KERNAL_Tasks[i].state == KERNAL_TASK_INACTIVE)
      {
       KERNAL_Tasks[i].state = KERNAL_TASK_ACTIVE;
       KERNAL_Tasks[i].id = i;
       KERNAL_Tasks[i].task = function;
       KERNAL_NumTasks++;
       return(KERNAL_Tasks[i].id);
      }                           
   }
 return(-1);
}   

void KERNAL_DeleteTask(int id)
{
 if (KERNAL_Tasks[id].state == KERNAL_TASK_ACTIVE)
  {
   KERNAL_Tasks[id].task = NULL;
   KERNAL_Tasks[id].state = KERNAL_TASK_INACTIVE;
   
   KERNAL_NumTasks--;
  }
}

void Change_Timer(unsigned int new_count)
{

// send the control word, mode 2, binary, least/most load sequence

outp(CONTROL_8253, CONTROL_WORD);

// now write the least significant byte to the counter register

outp(COUNTER_0,LOW_BYTE(new_count));

// and now the the most significant byte

outp(COUNTER_0,HI_BYTE(new_count));

} // end Change_Timer

