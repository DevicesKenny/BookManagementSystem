#include "stm32f10x.h"
#include "delay.h"
#include "gpio.h"

void GPIO_Configuration(void)//GPIO初始化
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	/*初始化结构体*/   
	GPIO_StructInit(&GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);//使能PC端口时钟 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//普通推挽输出 
	GPIO_Init(GPIOC, &GPIO_InitStructure);      //初始化PC.15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//普通推挽输出 
	GPIO_Init(GPIOC, &GPIO_InitStructure);      //初始化PC.6

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//普通推挽输出 
	GPIO_Init(GPIOC, &GPIO_InitStructure);      //初始化PC.13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//普通推挽输出 
	GPIO_Init(GPIOC, &GPIO_InitStructure);      //初始化PC.13
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//普通推挽输出 
	GPIO_Init(GPIOB, &GPIO_InitStructure);      //初始化PB.9
	
	GPIO_SetBits ( GPIOB, GPIO_Pin_9 );

    // RST
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);

			// CS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化

    // SCK
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // MISO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}
