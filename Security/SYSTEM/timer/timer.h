#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h" 

extern unsigned char Count_timer;
extern unsigned char Flag_timer_2S;
extern unsigned char Count_Timer3_value_USART3_receive_timeout2;
extern unsigned char Count_timer_8S;

void TIM3_Int_Init(u16 arr,u16 psc);
#endif
