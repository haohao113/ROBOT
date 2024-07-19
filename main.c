#include "main.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.c"
#include "Control3Omni.h"
#include "LCD_I2C.h"
#include "myiic.h"

//#include "gamepad.h"
//#include "PS3_wireless.h"
//#include "gamepadBluetooth.h"
//======================================================
#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//======================================================	
	
s16 data_gyro,temp, pwm;
u32 receiver_ok=0;
	
unsigned char GP_BTN [6];	
uint16_t tempData = 0;

char buffer[10];
int bufferIndex = 0;

int index = 0;
int value = 0;

int tempa;	
int i;	
int a = 0;
float angle;
char dataUsart3;
char rxUsart3;

char msg[20];
char encoder[20];
	
vs16 IMU;
u8 _index_gamepad = 0;
	
extern u8 dataBuff[6];
unsigned int value_game, value_game_joyL, value_game_joyR; //gamepad
unsigned int k;
unsigned int thoi_gian, giay, time;	//gamepad
volatile bool newDataAvailable = false;
GPIO_InitTypeDef  GPIO_InitStructure;

//================ DELAY MICROSECOND ===================
void delay_us(uint32_t delay_us)
{
	TIM_SetCounter(TIM6, 0);
	while(TIM_GetCounter(TIM6) < delay_us){
	}
}
void delay_ms(uint32_t delay_ms)
{
	while (delay_ms) {
		/*
		TIM6->CNT = 0;
		while (TIM6->CNT < 1000) {
		}
		*/
		TIM_SetCounter(TIM6, 0);
		while (TIM_GetCounter(TIM6) < 1000) {
		}
		--delay_ms;
	}
}

//======================================================
void TIM7_IRQHandler(void)
{
	TIM7->SR = (uint16_t)~TIM_IT_Update; // xoa co ngat timer 7
	robotAnalytics();	
//	putchar4(16);
}
//======================================================
//void gp_update_btn (void)
//{
//	putchar4(16);
//}
void TestBLT(char* data) {
		for (int i = 0; i < strlen(data); i++) {
    switch(data[i]) {
        case 'A':
            robotRun(1800, 100);
//						robotRunAngle(1800,40, 900, 0.6);
            break;
        case 'B':
						robotRun(0, 100);
//						robotRunAngle(0,40, -900, 0.6);
            break;
        case 'C':
						robotRun(-900, 100);
            break;
        case 'D':
            robotRun(900, 100);
            break;
        case 'S':
            robotStop(10);
						delay_ms(300);
            break;
        case 'E':
//					  robotRotate(1800, 0.7, 0);
						robotStop(10);
						delay_ms(200);
				
						robotRunAngle(1800,30, 1800, 1.2);
						delay_ms(6000);
				
            robotStop(10);
						delay_ms(200);
				
						robotRunAngle(0,30, 0, 1.2);
						delay_ms(6000);
				
						robotStop(10);
						delay_ms(200);
																		
						robotRun(-450,20);
						delay_ms(2000);
						
						robotStop(10);
						delay_ms(200);
						
						robotRun(1350,20);
						delay_ms(4000);
						
						robotStop(10);
						delay_ms(200);
						
						robotRun(-450,20);
						delay_ms(2000);
						
						robotStop(10);
						delay_ms(200);

						robotRun(450,20);
						delay_ms(2000);
						
						robotStop(10);
						delay_ms(200);
						
						robotRun(-1350,20);
						delay_ms(4000);
						
						robotStop(10);
						delay_ms(200);
						
						robotRun(450,20);
						delay_ms(2000);
						
						robotStop(10);
						delay_ms(200);
						
						
            break;
        case 'F':
						robotRotate(-1800, -0.9, 0);
						delay_ms(6000);
            break;
				
				case 'G':
					
						robotStop(10);
						delay_ms(200);
				
						robotRunAngle(1800,40, 0, 0.9);
						delay_ms(3000);
				
            robotStop(10);
						delay_ms(200);
				
						robotRunAngle(0,40, 1800, -0.9);
						delay_ms(3000);
				
						robotStop(10);
						delay_ms(200);
						
						
				
//						robotRun(1800, 40);
//						delay_ms(2000);
//				
//						robotStop(10);
//						delay_ms(200);
        default:
            break;
				}
		}
}

void UART4_IRQHandler(void)	// Ngat USART4 Game Ps2
{ 
//    // Read the received data
//    char received = USART_ReceiveData(UART4);

//    // Store the received data in the buffer
//    buffer[bufferIndex] = received;
//    bufferIndex++;

//    // If we have received a newline character, process the data
//    if (received == '\n') {
//        TestBLT(buffer);
//        bufferIndex = 0;
	
	    char received = USART_ReceiveData(UART4);
    buffer[bufferIndex] = received;
    bufferIndex++;

    if (received == '\n') {
        newDataAvailable = true;
        bufferIndex = 0;
    }
}

//======================================================
//======================================================
//================== CHUONG TRINH CHINH ================

int main(void)
{ 
	if (SysTick_Config(SystemCoreClock / 1000)) while (1);  // 1ms truyen du lieu usart den cac slever
	configInOutPort();						// khai bao ngo vào - ra cua mot chan bat ky	
	config_TIM8_PWM();						// cai dat timer 8 o che do dieu xung
	
//	Config_encoder_timer4(); 		// khai bao encorder timer 4
//	reset_en_T4();					  	// reset encoder timer 4
	config_uart2_dma(115200);			// usart2 giao tiep voi laban
	config_uart4(115200);						// usart4 giao tiep voi GamePad
	Config_time6();			  	      // cau hinh timer delay
	config_TIM7();				  			// timer 1ms chay chuong trinh control omni
	run_read_gyro_uart2();				// reset lai laban 
	while(1) 
		{	
			if (newDataAvailable) 
			{
					TestBLT(buffer);
					newDataAvailable = false;
			}
			delay_ms(10);
//				robotStop(10);
//				delay_ms(2000);
//			
//				robotRun(1800, 50);
//				delay_ms(5000);
//			
//			  robotStop(10);
//				delay_ms(2000);
//			
//				robotRun(0,50);
//				delay_ms(5000);
				
		}
}
//========================== END ====================================

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  //USART_SendData(EVAL_COM1, (uint8_t) ch);
	//lcd_Data_Write((uint8_t)ch);
  /* Loop until the end of transmission */
//  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
 // {}
  return ch;
}

//*****END OF FILE****/
