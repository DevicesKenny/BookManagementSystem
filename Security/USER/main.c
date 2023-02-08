#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "spi.h"
#include "usart.h"
#include "timer.h"
#include "gpio.h"
#include "wifi.h"

#define 	Human_body_infrared		1//人体红外

unsigned char Read_Human_body_infrared_CMD[7] = {0xCA,0x20,0xFE,0x60,0x01,0x01,0xAC};//读取人体红外传感器的状态命令
unsigned char Flag_LED3_ONOFF = 0;
char Data_buf[50];
unsigned char Flag_Sonser_Device_onoff = 0;
char AT_send_buf[20];
void copy_str(char* des,char* src,unsigned char len);
unsigned char Get_data_len(char* addr);//获取字符串/数组有效数据长度
void esp8266_init();
void senddata();
void alarm();
int flag_havepeople = 0;
int flag_alarm = 0;

unsigned char x,flag = 1;

int main(void)
{
    GPIO_Configuration();//GPIO初始化
    delay_init();	    	 			//延时函数初始化
    Lcd_Init();					//初始化LCD
    Usart1_Init(115200);			//串口1初始化
    Usart2_Init(115200);			//串口2初始化
    Usart3_Remap_Init(9600);//串口3重映射接口初始化
    NVIC_Configuration(); 		//设置NVIC中断分组2:2位抢占优先级，2位响应优先级 	LED_Init();			     //LED端口初始化
    TIM3_Int_Init(999,7199);	//开启定时器3，计数100ms
    for(x = 0; x < 20; x ++)
        AT_send_buf[x] = '\0';
    BACK_COLOR=BLACK;		//背景为黑色
    POINT_COLOR=YELLOW;	//画笔为黄色
    LCD_Show_Chinese16x16(90,10,"人体红外");
    //LCD_ShowString(140,10,"4.5.2");//显示英文字符串
    POINT_COLOR=GREEN;//绿色
    LCD_DrawRectangle(0, 30, 320, 240);//画矩形框

    LED1_OFF();
    LED2_OFF();
    Buzzer_OFF();

    LCD_Show_Chinese16x16(80,40+18*6,"人体红外传感器");
    POINT_COLOR=GREEN;//绿色
    LCD_Show_Chinese16x16(140,40+20*7,"无人");

    while(1)
    {
        Connect_wifi();
        if(Flag_timer_2S)
        {   //2S时间到
            Flag_timer_2S = 0;
            if(Flag_LED3_ONOFF)
            {
                Flag_LED3_ONOFF = 0;
                LED3_ON();
                LCD_Fill(20,40+20*7,20+16,40+20*7+16,YELLOW);
            }
            else
            {
                Flag_LED3_ONOFF = 1;
                LED3_OFF();
                LCD_Fill(20,40+20*7,20+16,40+20*7+16,BLACK);
            }
            //发送采集传感器数据命令
            USART3TxData_hex(Read_Human_body_infrared_CMD,7);
        }
				//添加闭馆开馆的标志，实现开馆闭馆的切换，默认为开馆
				if(Query(USART1_RX_BUF,"Hwchj0502onT",USART1_REC_LEN))
				{
					//USART2TxStr("jkshgueliehuigbilgydis");
					flag_alarm = 0;
					CLR_Buf1();
				}	
			
				else if(Query(USART1_RX_BUF,"Hwchj0503offT",USART1_REC_LEN))
				{
					//USART2TxStr("jkshgueliehuigbilgydis");
					flag_alarm = 1;
					CLR_Buf1();
				}
        ///////////////////////////////////////////////////////////////////
        if(Flag_Usart3_Receive//如果串口3有数据接收
                &&(!Count_Timer3_value_USART3_receive_timeout))//并且如果接收超时
        {
            Flag_Usart3_Receive = 0;
            //显示传感器数据到LCD上
					
            //CA 20 FE 18 02 01 01 AC //状态发生变化后自动返回的数据（倒数第二个字节,0x00表示没人，0x01表示有人）
            //CA B0 01 01 AC 					//查询状态后响应的数据（倒数第二个字节,0x00表示没人，0x01表示有人）
            if(USART3_RX_BUF[0] == 0xCA)
            {
                if(USART3_RX_BUF[1] == 0x20)
                {
                    if(USART3_RX_BUF[6])
                    {
                        Flag_Sonser_Device_onoff = 1;
                        copy_str(Data_buf,"Hwdhi05011  T",13);
                        POINT_COLOR=YELLOW;	//画笔为黄色
                        LCD_Show_Chinese16x16(140,40+20*7,"有人");
												//USART2TxChar(flag_alarm+'0');	
												flag_havepeople = 1;												
                    }
                    else
                    {
                        Flag_Sonser_Device_onoff = 0;
                        copy_str(Data_buf,"Hwdhi05010  T",13);
                        POINT_COLOR=GREEN;//绿色
                        LCD_Show_Chinese16x16(140,40+20*7,"无人");
												//USART2TxChar(flag_alarm+'0');	
												flag_havepeople = 0;
                    }
                }
                else if(USART3_RX_BUF[1] == 0xB0)
                {
                    if(USART3_RX_BUF[3])
                    {
                        Flag_Sonser_Device_onoff = 1;
                        copy_str(Data_buf,"Hwdhi05011  T",13);
                        POINT_COLOR=YELLOW;	//画笔为黄色
                        LCD_Show_Chinese16x16(140,40+20*7,"有人");
												//USART2TxChar(flag_alarm+'0');			
												flag_havepeople = 1;											
													
                    }
                    else
                    {
                        Flag_Sonser_Device_onoff = 0;
                        copy_str(Data_buf,"Hwdhi05010  T",13);
                        POINT_COLOR=GREEN;//绿色
                        LCD_Show_Chinese16x16(140,40+20*7,"无人");
												//USART2TxChar(flag_alarm+'0');	
												flag_havepeople = 0;
                    }
                }
            }
						if(flag_alarm)
						{
							copy_str(&Data_buf[10],"of",2);
						}
						else
							copy_str(&Data_buf[10],"on",2);						
            CLR_Buf3();		
						
						//if(flag_time_serv == 0)
						//if(!Query(USART1_RX_BUF,"busy",USART1_REC_LEN))
						if(flag_time_serv == 0)
							senddata();
						if(flag_alarm&&flag_havepeople)
						{
							alarm();
						}					
				}
			
    }
}

void copy_str(char* des,char* src,unsigned char len)
{
    unsigned char i;
    for(i = 0; i < len; i ++)
    {
        *(des+i) = *(src+i);
    }
}
unsigned char Get_data_len(char* addr)//获取字符串/数组有效数据长度
{
    unsigned len;
    for(len = 0; *(addr+len) != '\0'; len ++);
    return len;
}

void esp8266_init()
{
    //重启WiFi模块
    USART2TxStr("重启模块...\r\n");
    USART1TxStr("AT+RST\r\n");
    LCD_ShowString(1,40,"Restart wifi mode...");
    delay_ms(1000);//值最大为1864
    delay_ms(1000);//值最大为1864
    delay_ms(1000);//值最大为1864
    delay_ms(1000);//值最大为1864
    delay_ms(1000);//值最大为1864
    delay_ms(1000);//值最大为1864
    CLR_Buf1();     //清除串口1接收缓存
    Flag_usart1_receive_OK = 0;
    //设置工作模式
    USART2TxStr("设置工作模式...\r\n");
    USART1TxStr("AT+CWMODE=3\r\n");
    wait_OK();//等待接收到OK字符串
    LCD_ShowString(1,40+18*1,"AT+CWMODE=3");
    //连接已知WiFi
    USART2TxStr("连接已知WiFi...\r\n");
    USART1TxStr("AT+CWJAP=\"WIFI_100\",\"12345678\"\r\n");
    wait_OK();//等待接收到OK字符串
    LCD_ShowString(1,40+18*2,"AT+CWJAP=\"WIFI_100\",\"12345678\"");
    //启动多模块连接
    USART2TxStr("启动多模块连接...\r\n");
    USART1TxStr("AT+CIPMUX=1\r\n");
    wait_OK();//等待接收到OK字符串
    LCD_ShowString(1,40+18*3,"AT+CIPMUX=1");
    //连接服务器
    USART2TxStr("连接服务器...\r\n");
    USART1TxStr("AT+CIPSTART=4,\"TCP\",\"192.168.1.200\",6000\r\n");
    wait_OK();//等待接收到OK字符串
    LCD_ShowString(1,40+18*4,"AT+CIPSTART=4,\"TCP\",\"192.168.1.200\",");
    LCD_ShowString(1,40+18*5,"6000");
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

void alarm()
{
	Count_timer_8S = 80;
	Buzzer_ON();
	while(Count_timer_8S)
	{
		if(Query(USART1_RX_BUF,"Hwchj0502onT",USART1_REC_LEN))
		{
			flag_alarm = !flag_alarm;
			CLR_Buf1();
			break;
		}	
		LED1_ON();
		delay_ms(50);
		LED1_OFF();
		delay_ms(50);
	}
	Buzzer_OFF();
}

