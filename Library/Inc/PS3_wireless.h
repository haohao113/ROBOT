
#ifndef __GAMEPAD_H
#define __GAMEPAD_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"

#define L1  	64511
#define L2  	63487
#define L3  	61439
#define L4 		57343

#define R1  	65527
#define R2  	65534
#define R3  	65533
#define R4  	65531

#define LL1 	65023
#define LL2 	65279
#define RR2 	65503

#define SELECT 49151
#define START  32767

#define SWL 	65407
#define SWR 	65471

#define LAL 	255
#define LAR 	 0

#define LAU 	255
#define LAD 	0

#define RAL 	255
#define RAR  	0

#define RAU  	255
#define RAD  	0

#define RR1_L2  	63471
#define RR1_L4  	57327

////---------------------------------------
//#define L1_L2  
//#define L1_L3		

//#define L1_R1 	
//#define L1_R2 
//#define L1_R3 
//#define L1_R4 

//#define L1_SWL 
//#define L1_SWR

//#define L1_LL1 
//#define L1_LL2 

//#define L1_RR2

//#define L1_SELECT
//#define L1_START

////--------------------------------------- 
//#define L2_L4		

//#define L2_R1 	
//#define L2_R2 
//#define L2_R3 
//#define L2_R4 

//#define L2_SWL 
//#define L2_SWR

//#define L2_LL1 
//#define L2_LL2 

//#define L2_RR2

//#define L2_SELECT
//#define L2_START

////---------------------------------------
//#define L3_L4		

//#define L3_R1 	
//#define L3_R2 
//#define L3_R3 
//#define L3_R4 

//#define L3_SWL 
//#define L3_SWR

//#define L3_LL1 
//#define L3_LL2 

//#define L3_RR2

//#define L3_SELECT
//#define L3_START

////---------------------------------------
//#define L4_R1 	
//#define L4_R2 
//#define L4_R3 
//#define L4_R4 

//#define L4_SWL 
//#define L4_SWR

//#define L4_LL1 
//#define L4_LL2 

//#define L4_RR2

//#define L4_SELECT
//#define L4_START


////---------------------------------------
//#define R1_SWL 
//#define R1_SWR

//#define R1_LL1 
//#define R1_LL2 

//#define R1_RR2

//#define R1_SELECT
//#define R1_START

////---------------------------------------
//#define R2_SWL 
//#define R2_SWR

//#define R2_LL1 
//#define R2_LL2 

//#define R2_RR2

//#define R2_SELECT
//#define R2_START

////---------------------------------------
//#define R3_SWL 
//#define R3_SWR

//#define R3_LL1 
//#define R3_LL2 

//#define R3_RR2

//#define R3_SELECT
//#define R3_START

////---------------------------------------
//#define R4_SWL 
//#define R4_SWR

//#define R4_LL1 
//#define R4_LL2 

//#define R4_RR2

//#define R4_SELECT
//#define R4_START

////---------------------------------------

#define SELECT_START		16383  // Trang thai giu 3 giay sau khi nha nut - calibration_IMU

////---------------------------------------
//#define LL1_LL2 
//#define LL1_RR2 

//#define LL2_RR2

////---------------------------------------
//#define LL1_SELECT
//#define LL1_START

//#define LL1_SWL 
//#define LL1_SWR

////---------------------------------------
//#define LL2_SELECT
//#define LL2_START

//#define LL2_SWL 
//#define LL2_SWR

////---------------------------------------
//#define RR2_SELECT
//#define RR2_START

//#define RR2_SWL 
//#define RR2_SWR

////---------------------------------------

#endif
/****************END OF FILE***************/


