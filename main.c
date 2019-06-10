

/*********************************************************************************************************
*
* File                : main.c
* Version             : V1.0
* By                  : alireza roozitalab
* email               : alireza.roozitalab@gmail.com
* git user            : ARoozitalab
*                                                
*
*********************************************************************************************************/

///MOSI-SDA-PE5 -2
//SCLK-SCL-PE2 -1
//SS-SCS-PE4 -3
//SYN-PE3 -0






/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stpmxx.h"
/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup MEMS_Example
  * @{
  */ 
void SPI_LowLevel_Init(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t Buffer[32];
uint8_t parity[8];

uint8_t testdata[4]={0x6f,0xff,0xfe,0x07};
uint8_t data;
uint8_t datab;
float X_BiasError, Y_BiasError, Z_BiasError = 0.0;
uint8_t Xval, Yval = 0x00;
static __IO uint32_t TimingDelay;
/* Private function prototypes -----------------------------------------------*/

POWER_DATA test;
RCC_ClocksTypeDef RCC_Clocks;
/**
  * @brief   Main program
  * @param  None
  * @retval None
  */




int main(void)
{
	uint32_t i=0,j=0;
	uint8_t num=0;
  uint8_t buf[10];
	uint8_t *p;



	
  /* Initialize the LCD */
  LCD_Init();
  /* Initialize the LCD Layers*/
  LCD_LayerInit();
  
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
  
  /* Set LCD Background Layer  */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
  
  /* Clear the Background Layer */ 
  LCD_Clear(LCD_COLOR_BLUE);

  stpm_spi_Init();


  delay(100);
  reset_chip  ();
  delay(100);
  write_chip(1,24);
  delay(100);
  while (1)
  {
		
    test=read_stpm();
		sprintf(buf,"vrms=%1.2f       ",test.uRMS);
		LCD_DisplayStringLine(1, buf);
		sprintf(buf,"vmom=%1.2f       ",test.uMOM);
		LCD_DisplayStringLine(20, buf);
		
		sprintf(buf,"irms=%1.2f        ",test.iRMS);
		LCD_DisplayStringLine(40, buf);
		sprintf(buf,"imom=%1.2f       ",test.iMOM);
		LCD_DisplayStringLine(60, buf);
		
		
			sprintf(buf,"active=%u      ",test.active_energy);
		LCD_DisplayStringLine(80, buf);
		sprintf(buf,"reactive=%u        ",test.reactive_energy);
		LCD_DisplayStringLine(100, buf);
				sprintf(buf,"apparent=%u      ",test.apparent_energy);
		LCD_DisplayStringLine(120, buf);
		
		delay(1000);
	}
	
	
	
}



void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
