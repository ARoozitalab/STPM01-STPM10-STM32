

/*********************************************************************************************************
*
* File                : STPMxx.h
* Version             : V1.0
* By                  : alireza roozitalab
* email               : alireza.roozitalab@gmail.com
* git user            : ARoozitalab
*                                                
*
*********************************************************************************************************/








#ifndef __STPMxx_H
#define __STPMxx_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"



//Config pins

#define SCL_L    GPIO_ResetBits(GPIOE,GPIO_Pin_4);
#define SCL_H    GPIO_SetBits(GPIOE,GPIO_Pin_4);


#define SYN_L   GPIO_ResetBits(GPIOE,GPIO_Pin_3);
#define SYN_H   GPIO_SetBits(GPIOE,GPIO_Pin_3);


#define CLK_L   GPIO_ResetBits(GPIOE,GPIO_Pin_2);
#define CLK_H   GPIO_SetBits(GPIOE,GPIO_Pin_2);

#define DAT_L  GPIO_ResetBits(GPIOE,GPIO_Pin_5); 
#define DAT_H  GPIO_SetBits(GPIOE,GPIO_Pin_5);


#define  DATA_PIN   GPIO_Pin_5
#define  DATA_PORT  GPIOE
#define	 DATA_CLK		RCC_AHB1Periph_GPIOE




#define  CLK_PIN   GPIO_Pin_2
#define  CLK_PORT  GPIOE
#define	 CLK_CLK		RCC_AHB1Periph_GPIOE


#define  SYN_PIN   GPIO_Pin_3
#define  SYN_PORT  GPIOE
#define	 SYN_CLK		RCC_AHB1Periph_GPIOE

#define  SCL_PIN   GPIO_Pin_4
#define  SCL_PORT  GPIOE
#define	 SCL_CLK 		RCC_AHB1Periph_GPIOE



#define  Error  0
#define  Ok			1

#define Ku 0.875 //Voltage Calibrator Ideal Value
#define Ki 0.875 //Current Calibrator Ideal Value
#define Au 4 //Voltage Channel Gain
#define len_i 65532 //RMS Current Register Length
#define len_u 2048 //RMS Voltage Register Length
#define len_i_mom 32768 //Momentary Current Register Length
#define len_u_mom 1024 //Momentary Voltage Register Length
#define len_f 16384 //Frequency Register Length
#define Kint_comp 1.004 //Gain of decimation filter
#define pi 3.14159
#define Dint 65535
#define Dint_p 32768
#define Fm 8388608
//2^23 if oscillator frequency is 4.194 or 8.388 MHz
//8 * 10^6 if oscillator frequency is 4.000 or 8.000 MHz
#define Kut 2
//2 for CT/Shunt
//1 for Rogowski coil
#define Vref 1.23 //Internal voltage reference

#define Kdif 0.6135
//0,6135 Gain of differentiator @ line frequency = 50 Hz
//0.7359 Gain of differentiator @ line frequency = 60 Hz
#define Kint 0.815
//0,815 Gain of integrator @ line frequency = 50 Hz
//0.679 Gain of integrator @ line frequency = 60 Hz

#define Ai  1//Current Channel Gain
#define Ks  1//Current Sensor Sensitivity Value [V/A]

#define R1  783000
#define R2  475

typedef struct
{
  uint32_t DAP;         		  /*type0 active energy||Status*/

  uint32_t DRP;                /*reactive energy||upper f(u) */

  uint32_t DSP;           		 /*apparent energy||lower f(u)*/

  uint32_t DFP;                /*type 1 energy||mode signals */

  uint32_t DEV;               				 /*uRMS||iRMS*/

  uint32_t DMV;                        /*uMOM||iMOM*/
 
  uint32_t CFL;   										/*lower part of configurators */ 

  uint32_t CFH;           					 /*upper part of configurators */

}STPM_DATA;


typedef struct
{
  uint32_t active_energy;         		  /*type0 active energy*/

  uint32_t reactive_energy;                /*reactive energy */

  uint32_t apparent_energy;           		 /*apparent energy*/

  uint32_t type1_energy;                /*type 1 energy */

	
	uint8_t modsignal;								/*modsignal */ 
	
	uint8_t status; 									/*status */ 

		
	double freq;  										/*freq */
	
  double uRMS;               				 /*uRMS*/

  double uMOM;                        /*uMOM*/
 
  double iRMS;   										/*iRMS */ 

  double iMOM;           					 /*iMOM */
	
	 uint8_t flag;  
	
	

}POWER_DATA;
void delay(uint8_t d);
void stpm_read_config(void);
void stpm_write_config(void);
POWER_DATA read_stpm (void);
void reset_chip  (void);
void stpm_spi_Init(void);
void write_chip(uint8_t value,uint8_t addr);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F4xx_H */
