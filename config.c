
//Tong Van Hao: K72
#include "stm32f4xx.h" 
#include "main.h"

//============= DEFINE 3 MOTOR WHEEL TIM8 ================== 

#define mor_0h	 							TIM8->CCR2							
#define mor_0h_next						GPIOD->BSRRH = 	GPIO_Pin_6
#define mor_0h_back			  		GPIOD->BSRRL = 	GPIO_Pin_6

#define mor_4h	 							TIM8->CCR4							  
#define mor_4h_next						GPIOD->BSRRH = 	GPIO_Pin_1		
#define mor_4h_back						GPIOD->BSRRL = 	GPIO_Pin_1

#define mor_8h	 							TIM8->CCR3									
#define mor_8h_next						GPIOD->BSRRH = 	GPIO_Pin_5		
#define mor_8h_back						GPIOD->BSRRL = 	GPIO_Pin_5

vs32 num_over_t4 = 0;
vu8  data_tx_gyro, en_gyro;
char mang[2];

vu8 RxBuffer_compass[2],RX_USART2[15];

//===============================================================

TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
TIM_OCInitTypeDef  			  TIM_OCInitStructure;
GPIO_InitTypeDef 					GPIO_InitStructure;
DMA_InitTypeDef 					DMA_InitStructure;
NVIC_InitTypeDef 					NVIC_InitStructure;
ADC_InitTypeDef 					ADC_InitStructure;

//===================== CONFIG IN/OUT ===========================
void configInOutPort(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = 0xffff;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_Write(GPIOB, 0xffff);
	GPIO_Write(GPIOC, 0xffff);
	GPIO_Write(GPIOD, 0xffff);
	GPIO_Write(GPIOE, 0xffff);

	// Dua portA len muc 1 tru chan A.13 va A.14, 2 chan mach nap
	GPIO_InitStructure.GPIO_Pin = 0x9FFF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_Write(GPIOA, 0xffff);
	
//	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_5 ;
//	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
//	GPIO_Init(GPIOE, &GPIO_InitStructure);				
}

//======================== PWM TIM8 =============================

void config_TIM8_PWM(void)				// PWM C6 - C7 - C8 - C9
{
	GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  /* GPIOA Configuration:TIM1 Channel1, 2, 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	  /* Connect TIM1 pins to AF2 */  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8); 
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 255;
  TIM_TimeBaseStructure.TIM_Prescaler = 400; //1900;  // 1900 for board driver cu (400 Board thay vinh)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
  TIM_OC1Init(TIM8, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC2Init(TIM8, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC3Init(TIM8, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
  /* PWM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC4Init(TIM8, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
  TIM_ARRPreloadConfig(TIM8, ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM8, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM8, ENABLE);
}

//==================== config encoder timer4 ====================

void Config_encoder_timer4(void)		// PD12 - PD13 
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4); // tim3
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI1, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4,ENABLE);
	TIM_SetCounter(TIM4, 0);
}

//===============================================================
s32 readEncorderT4(void)
{
	return (num_over_t4<<16)|TIM_GetCounter(TIM4);	
}

//---------------------------------------------------------------
void reset_en_T4(void)
{
	TIM4->CNT = 0;
	num_over_t4 = 0;
}

//======================= config uart2 ==========================
void config_uart2(u32 baudrate)		// PA2 Tx - PA3 Rx ( la bàn )
{ 
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //tx
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//rx
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2,&USART_InitStructure);
	 
/* Clear the rx bit in the SR register by writing 0 to it */
  USART_ClearFlag(USART2, USART_FLAG_RXNE);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
 
	USART_Cmd(USART2, ENABLE);
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;// 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // muc do uu tien ngat thu 15
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;// cho phep su dung ngat
  NVIC_Init(&NVIC_InitStructure);  								// cau hinh cho bang vector ngat
}

//===============================================================
void config_uart2_dma(u32 baudrate) // PA2 Tx - PA3 Rx ( la bàn )
{		
	DMA_InitTypeDef 			DMA_InitStructure;
	NVIC_InitTypeDef 			NVIC_InitStructure;
	USART_InitTypeDef 		USART_InitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
   /*-------------------------- GPIO Configuration ----------------------------*/
   GPIO_InitStructure.GPIO_Pin 			= GPIO_Pin_2 | GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode 			= GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType 		= GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd			= GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed 		= GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
	
   /* Connect USART pins to AF */
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

   USART_InitStructure.USART_BaudRate 					= baudrate;
   USART_InitStructure.USART_WordLength 				= USART_WordLength_8b;
   USART_InitStructure.USART_StopBits 					= USART_StopBits_1;
   USART_InitStructure.USART_Parity 					= USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None; 
   USART_InitStructure.USART_Mode 						= USART_Mode_Rx | USART_Mode_Tx; 
   USART_Init(USART2, &USART_InitStructure); 
   USART_Cmd(USART2, ENABLE);

	//===============================================================
   /* Configure the Priority Group to 2 bits */
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
   /* Enable the UART4 RX DMA Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel 							= DMA1_Stream5_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd 						= ENABLE;
   NVIC_Init(&NVIC_InitStructure);
	
	 DMA_DeInit(DMA1_Stream5);
   DMA_InitStructure.DMA_Channel 								= DMA_Channel_4;
   DMA_InitStructure.DMA_DIR 										= DMA_DIR_PeripheralToMemory; // Receive
	
   DMA_InitStructure.DMA_Memory0BaseAddr 						= (uint32_t)RX_USART2;
   DMA_InitStructure.DMA_BufferSize 							= 10;									//(uint16_t)sizeof(Buffer);
   DMA_InitStructure.DMA_PeripheralBaseAddr 					= (uint32_t)&USART2->DR;
	
   DMA_InitStructure.DMA_PeripheralInc 						= DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc 								= DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize 					= DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize 						= DMA_MemoryDataSize_Byte;
	
   DMA_InitStructure.DMA_Mode 									= DMA_Mode_Circular;
   DMA_InitStructure.DMA_Priority 								= DMA_Priority_High;
   DMA_InitStructure.DMA_FIFOMode 								= DMA_FIFOMode_Enable;
	
   DMA_InitStructure.DMA_FIFOThreshold 						= DMA_FIFOThreshold_Full;
   DMA_InitStructure.DMA_MemoryBurst 							= DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst 						= DMA_PeripheralBurst_Single; 
   DMA_Init(DMA1_Stream5, &DMA_InitStructure); 
	
   /* Enable DMA Stream Half Transfer and Transfer Complete interrupt */
   USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); // Enable USART Rx DMA Request
	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);  
   DMA_Cmd(DMA1_Stream5, ENABLE);  					  // Enable the DMA RX Stream 
}
//--------------------------- send uart2 ------------------------
void putchar2(u8 ch)
{
  USART_SendData(USART2, (uint8_t) ch);
 	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}       
}


void config_uart3(u32 baudrate)
{ 
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //tx
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//rx
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	 
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART2);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART3,&USART_InitStructure);
	 
//	/* Clear the rx bit in the SR register by writing 0 to it */
  USART_ClearFlag(USART3, USART_FLAG_RXNE);
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
 
	USART_Cmd(USART3, ENABLE);
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // muc do uu tien ngat thu 15
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // cho phep su dung ngat
  NVIC_Init(&NVIC_InitStructure);  								// cau hinh cho bang vector ngat
	
 }

//--------------------------- send uart3 ------------------------
void putchar3(u8 ch)
{
  USART_SendData(USART3, (uint8_t) ch);
 	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET){}       
}

void config_uart4(u32 baudrate) 		// Tay Game C10 Tx - C11 Rx
{ 
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //tx
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//rx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	 
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	
	USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(UART4,&USART_InitStructure);
	 
//	/* Clear the rx bit in the SR register by writing 0 to it */
  USART_ClearFlag(UART4, USART_FLAG_RXNE);
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);
 
	USART_Cmd(UART4, ENABLE);
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;// 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // muc do uu tien ngat thu 15
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // cho phep su dung ngat
  NVIC_Init(&NVIC_InitStructure);  								// cau hinh cho bang vector ngat
	
 }

//--------------------------- send uart4 ------------------------
void putchar4(u8 ch)
{
  USART_SendData(UART4, (uint8_t) ch);
 	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET){}       
}
void putchar1(u8 ch)
{
  USART_SendData(USART1, (uint8_t) ch);
 	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}       
}
//--------------------------- read uart2 ------------------------
u8 Read_uart2()
{
 	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET){} 
  return USART2->DR;    
}
//===============================================================
void run_read_gyro_uart2(void)
{ 
	u32 i;
  en_gyro = 0;
  for(i=0;i<2;i++)
  {
		putchar2('a');
		delay_ms(150); 
  }
  en_gyro = 1;
  data_tx_gyro = 'y';
}
//---------------------------------------------------------------
void reset_IMU(void)
{
	putchar2('a');
	delay_ms(100);
}

//==================== config timer6 1ms ========================
void Config_time6(void)
{
	TIM_TimeBaseInitTypeDef timInit;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	timInit.TIM_Prescaler = 84 - 1;
	timInit.TIM_Period = 0xFFFF;
	TIM_TimeBaseInit(TIM6, &timInit);
	TIM_Cmd(TIM6, ENABLE);	

}

//===============================================================
void config_TIM7(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  TIM_TimeBaseStructure.TIM_Period = 1;// 1ms
  TIM_TimeBaseStructure.TIM_Prescaler =(42000 - 1);
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM7, ENABLE);  
}
//====================== END OF FILE ============================
