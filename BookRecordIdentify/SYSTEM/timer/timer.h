#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h" 

extern unsigned char Count_timer;
extern unsigned char Flag_timer_2S;
extern unsigned char count_time_motor;
extern unsigned char Flag_timer_15S;
extern unsigned char Count_Timer3_value_USART3_receive_timeout2 ;
extern unsigned char Count_time_10S;
extern unsigned char Flag_timer_5S;
extern unsigned char Flag_timer_2S_read;
extern unsigned char Flag_timer_read_10S;
extern unsigned char Flag_timer_20S;
extern unsigned char Count_timer_20S;
extern unsigned char Count_timer_read_10S;
extern unsigned char Count_timer_read_20S;
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_PWM_Init(u16 arr,u16 psc);
#endif
