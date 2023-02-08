#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "spi.h"
#include "usart.h"
#include "timer.h"
#include "gpio.h"
#include "exti.h"
#include "string.h"
#include "wifi.h"
#include "rc522.h"

#define Gas 1

#define          BEEP_0()            GPIO_ResetBits( GPIOB, GPIO_Pin_9 )
#define          BEEP_1()            GPIO_SetBits ( GPIOB, GPIO_Pin_9 )

void esp8266_init();		//wifi初始化
//void copy_str(char* des,char* src,unsigned char len);
void SuccessfullyId(); 	//身份识别成功
void senddata();
void idread();
void Identification();

unsigned char Read_Gas_CMD[8] = {0x01,0x03,0x00,0x06,0x00,0x01,0x64,0x0B};			//可燃气体传感器命令--模拟烟雾传感
unsigned char Get_data_len(char* addr);
unsigned char x = 0;
char AT_send_buf[20];
char Data_buf[50];
char Gas_value_str[7];
unsigned int Gas_value;
int flag = 0;
unsigned char Flag_LED3_ONOFF = 0;
int flag_identity = 0;			//身份识别标志位
char IDCard[30];
char BorrowId[3];
char ReturnId[3];
int i = 0;
int num = 0;
int flag_idcard = 0; 		//录入身份信息
int flag_bookrecord = 0;//录入图书信息
int flag_idcard_ok = 0;	//判断信息卡是否存在
int flag_bookborrow = 0;
int flag_bookreturn = 0;
int flag_book = 0;

int main() 
{
	CT[0]=0x44;
	CT[1]=0x22;
	
	GPIO_Configuration();//GPIO初始化
	delay_init();	    	 			//延时函数初始化
	Lcd_Init();					//初始化LCD
	EXTIX_Init();					//按键中断事件
	Usart1_Init(115200);			//串口1初始化
	Usart2_Init(115200);			//串口2初始化
	Usart3_Remap_Init(9600);//串口3重映射接口初始化 
	NVIC_Configuration(); 		//设置NVIC中断分组2:2位抢占优先级，2位响应优先级 	LED_Init();			     //LED端口初始化	
	TIM3_Int_Init(999,7199);	//开启定时器3，计数100ms
	TIM2_PWM_Init(2249,80);		//PWM初始化
	RC522_Init();							//射频模块初始化
	BACK_COLOR=BLACK;		//背景为黑色
	POINT_COLOR=YELLOW;	//画笔为黄色
	LCD_Show_Chinese16x16(90,10,"图书录入身份识别");
	//LCD_ShowString(140,10,"4.5.1");//显示英文字符串
	POINT_COLOR=GREEN;//绿色	
	LCD_DrawRectangle(0, 40, 320, 240);//画矩形框	
	
	for(x = 0;x < 20;x ++)
		AT_send_buf[x] = 0;
	for(x = 0;x < 50;x ++)
		Data_buf[x] = 0;
	LED1_OFF();
	LCD_ShowString(16*2,16*11,"Card ID =");
	   
	//********************************************
	copy_str(Data_buf,"Hwdaf0702onT",12);
	LCD_ShowString(16*2+8*10,16*8,"202126");
	while(1)
	{
		Connect_wifi();
		//身份识别成功后进入图书馆，进行图书借阅
		//copy_str(Data_buf,"Hwdaf0702offid100T",18);

		//正常出入图书馆，刷卡
			//两秒上传一次数据
			//idread();
			//LCD_ShowString(16*2,16*6,"                ");
		
			//copy_str(&Data_buf[20],"read",4);
		//idread();
		if(Query(USART1_RX_BUF,"Hwcaf0702offokT",USART1_REC_LEN))
		{
			idread();
		}
			if(Flag_timer_2S&&flag_time_serv == 0)
			{
				Flag_timer_2S = 0;
				senddata();
			}			
			//返回数据错误数据帧
		else if(Query(USART1_RX_BUF,"Hwcar0702lighterrorT",USART1_REC_LEN))
		{
			//USART2TxStr("jhhhhhhhhhhhhhh");
			PCout(6) = 1;
		}
		else if(Query(USART1_RX_BUF,"Hwcar0702dengerrorT",USART1_REC_LEN))
		{
			PCout(6) = 1;
		}
		else if(Query(USART1_RX_BUF,"Hwcar0702motorerrorT",USART1_REC_LEN))
		{
			PCout(6) = 1;
		}
		else if(Query(USART1_RX_BUF,"Hwcar0702lightworkT",USART1_REC_LEN))
		{
			PCout(6) = 0;
		}
		else if(Query(USART1_RX_BUF,"Hwcar0702dengworkT",USART1_REC_LEN))
		{
			PCout(6) = 0;
		}
		else if(Query(USART1_RX_BUF,"Hwcar0702motorworkT",USART1_REC_LEN))
		{
			PCout(6) = 0;
		}
	}
	//
	
	return 0;
}

//void copy_str(char* des,char* src,unsigned char len)
//{
//	unsigned char i;
//	for(i = 0;i < len;i ++)
//	{
//		*(des+i) = *(src+i);
//	}
//}

unsigned char Get_data_len(char* addr)//获取字符串/数组有效数据长度
{
	unsigned len;
	for(len = 0;*(addr+len) != '\0';len ++);
	return len;
}

void senddata()
{
    //通过WIFI发送数据给服务器
    copy_str(AT_send_buf,"AT+CIPSEND=4,0",14);//拷贝字符串
    x = Get_data_len(Data_buf);//获取字符串/数组有效数据长度
    if(x < 10)
    {
        AT_send_buf[13] = x % 10 + '0';
        AT_send_buf[14] = '\r';
        AT_send_buf[15] = '\n';
    }
    else
    {
        AT_send_buf[13] = x % 100 / 10 + '0';
        AT_send_buf[14] = x % 10  + '0';
        AT_send_buf[15] = '\r';
        AT_send_buf[16] = '\n';
    }
    USART2TxStr(AT_send_buf);
    USART1TxStr(AT_send_buf);//发送字符串长度
    Count_Timer3_value_USART3_receive_timeout2 = 30;
    POINT_COLOR=YELLOW;	//画笔为黄色
    LCD_ShowString(120+16*6,40+18*6,"send len... ");
    while(flag)
    {
        for(x = 0; x < USART1_RX_STA; x ++)
        {
            if(USART1_RX_BUF[x] == '>')
            {   //等待收到大于号字符串
                flag = 0;
                break;
            }
        }
        if(Count_Timer3_value_USART3_receive_timeout2 == 0)
        {
            flag = 0;
            flag_time_serv = 1;
            break;
        }
    }
    if(flag_time_serv == 0)
    {
        flag = 1;
        USART2TxStr(Data_buf);
        USART1TxStr(Data_buf);//发送字符串
        LCD_ShowString(120+16*6,40+18*6,"send data...");
        wait_OK();//等待接收到OK字符串
        LCD_ShowString(120+16*6,40+18*6,"Done !      ");
        CLR_Buf1();     //清除串口1接收缓存
        CLR_Buf2();     //清除串口2接收缓存
        for(x = 0; x < 20; x ++)
            AT_send_buf[x] = 0;
			 
        //copy_str(Data_buf,"HwdFF0T",7);
    }
}

void SuccessfullyId()
{
	Count_time_10S = 100;
	while(Count_time_10S)
	{
		if(Query(USART1_RX_BUF,"Hwcei0702noT",USART1_REC_LEN))
		{
			CLR_Buf1();
			PCout(6) = 0;
			break;
		}
		
		LED1_ON();
	}
	LED1_OFF();
}

void idread()
{
	if(Flag_timer_2S_read)
	{//2S时间到
		RC522_Init();
		Flag_timer_2S_read = 0;
		if(Flag_LED3_ONOFF)
		{
			Flag_LED3_ONOFF = 0;
			LED3_ON();
			LCD_Fill(300,40+20*7,300+16,40+20*7+16,YELLOW);
		}
		else
		{
			Flag_LED3_ONOFF = 1;
			LED3_OFF();
			LCD_Fill(300,40+20*7,300+16,40+20*7+16,BLACK);
		}
	}
	if(Flag_find_card)
	{
		delay_ms(1000);
		Flag_find_card = 0;
	}						
	RC522_Read(0x01);
}

void Identification()
{
	
}


