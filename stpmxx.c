


/*********************************************************************************************************
*
* File                : STPMxx.c
* Version             : V1.0
* By                  : alireza roozitalab
* email               : alireza.roozitalab@gmail.com
* git user            : ARoozitalab
*                                                
*
*********************************************************************************************************/






#include "stm32f4xx.h"
#include "stpmxx.h"


void delay(uint8_t d)
{
uint32_t j,i; 
	for(i=0;i<d;i++)
for(j=0;j<6000;j++);

	
}

/*******************************************************************************
* Function Name  : BadParity
* Description    : Check parity of 8 bytes
* Input          : bp(data addres)
* Output         : None
* Return         : None
*******************************************************************************/	
int BadParity (uint8_t *bp)
{
register unsigned char prty; /* temporary register */
prty = *bp, /* take the 1st byte of data */
prty ^= *(bp+1), /* XOR it with the 2nd byte */
prty ^= *(bp+2), /* and with the 3rd byte */
prty ^= *(bp+3), /* and with the 4th byte */
prty ^= prty<<4, prty &= 0xF0;/* combine and remove the lower nibble */
return (prty != 0xF0); /* returns 1, if bad parity */
}



/*******************************************************************************
* Function Name  : read_stpm
* Description    : read all data
* Input          : None
* Output         : POWER_DATA stract
* Return         : data of voltage , current , ...
*******************************************************************************/	

POWER_DATA read_stpm(void)
{
		uint8_t i=0,j=0;
		uint8_t num=0;
		STPM_DATA stpm_value;
	  POWER_DATA get_value;
		uint8_t datab;
		uint8_t *p;
	  int uRMS=0;            
    int uMOM=0;                    
    int iRMS=0;   									
    int iMOM=0; 
	
	
		p=(uint8_t *)&stpm_value;
	
	
		CLK_H
		SCL_H
		SYN_L
		delay(4);
		SCL_L
		delay(2);
		SYN_H
		delay(1);
		num=4;
		for(j=1;j<33;j++)
		{
			datab=0;
				for(i=0;i<8;i++)
				{
						CLK_L
						delay(1);	
						datab=datab | GPIO_ReadInputDataBit(DATA_PORT,DATA_PIN);
				   	if(i<7)
						datab=datab<<1;
						CLK_H					
						delay(1);
				}
						delay(1);

				
				p[j-1]=datab;
			  if((((j)%4)==0)&&(j>0))
				num=num+8;

		}

		SCL_H;
		SYN_L;
		CLK_H;
		delay(200);	

		// cheack parity bits
   if(BadParity((uint8_t *)&stpm_value.CFH) || BadParity((uint8_t *)&stpm_value.CFL) ||
		BadParity((uint8_t *)&stpm_value.DAP) || BadParity((uint8_t *)&stpm_value.DEV)    || 
		BadParity((uint8_t *)&stpm_value.DFP) || BadParity((uint8_t *)&stpm_value.DMV)    || 
		BadParity((uint8_t *)&stpm_value.DRP) || BadParity((uint8_t *)&stpm_value.DSP))
		{

		get_value.flag=0;	
			
		return get_value;
		}
 // if parity ok calculate and store data
			delay(40);
		get_value.flag=1;
		
		get_value.active_energy=((stpm_value.DAP>>8)& 0x7FFFF);
		get_value.reactive_energy=((stpm_value.DRP>>8)& 0x7FFFF);
		get_value.apparent_energy=((stpm_value.DSP>>8)& 0x7FFFF);
		
		uRMS=((stpm_value.DEV>>16)& 0x7FF);
	  uMOM=((stpm_value.DMV>>16)& 0x7FF);
		
		
		iRMS=((stpm_value.DEV)& 0xFFFF);
	 iMOM=((stpm_value.DMV)& 0xFFFF);
		
		
if (iMOM & 0x08000) // positive current
get_value.iMOM = iMOM & 0x07FFF;
else // negative current
{
get_value.iMOM = 0x08000 - iMOM;
get_value.iMOM = iMOM * (-1);
}
if ( uMOM & 0x0400) // positive voltage
 get_value.uMOM = (uMOM) & 0x3FF;
else // negative voltage
{
 get_value.uMOM = 0x0400 - (uMOM);
 get_value.uMOM =  uMOM * (-1);
}		
		



get_value.uRMS = (1+R1/R2) * uRMS *Vref /(Au * Ku * Kint_comp * Kint * Kdif * len_u * Kut);
get_value.iRMS = iRMS * Vref/(Ks * Ai * Ki * Kint * Kint_comp * Kdif * len_i);
get_value.uMOM = (1+R1/R2) * uMOM * Vref /(Au * Ku * Kint_comp * Kint * Kdif * len_u_mom *Kut);
get_value.iMOM = iMOM * Vref /(Ks * Ai * Ki * Kint * Kint_comp * Kdif * len_i_mom);
		
		
		
		
		
		return get_value;

		
		
		
		}
		

	

/*******************************************************************************
* Function Name  : reset_chip
* Description    : reset chip
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/	

void reset_chip (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
// set sda pin as output
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	 GPIO_InitStructure.GPIO_Pin = DATA_PIN;
  GPIO_Init(DATA_PORT, &GPIO_InitStructure);
		CLK_H
		SCL_H
		SYN_H
		DAT_H
		delay(10);
		SCL_L
		delay(1);
		SYN_L
		delay(1);
		CLK_L
		delay(1);
		DAT_L
		delay(1);
		DAT_H
		delay(1);
		DAT_L
		delay(1);
		DAT_H
		delay(3);
		CLK_H
		delay(1);
		SYN_H
		delay(1);
			SCL_H
		
		delay(100);	
	// set sda pin as input
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	GPIO_InitStructure.GPIO_Pin = DATA_PIN;
  GPIO_Init(DATA_PORT, &GPIO_InitStructure);
		SCL_H;
		SYN_L;
		CLK_H;
		delay(100);	
		
		
		}
		


/*******************************************************************************
* Function Name  : write_chip
* Description    : reset chip
* Input          : value( value of bit 0 or 1 ),addr(address of bit ex: 47 part of the secondary current channel calibrat )
* Output         : None 
* Return         : None
*******************************************************************************/	

void write_chip(uint8_t value,uint8_t addr)
{
	int i=0;
	uint8_t data=0;
	
	  GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	GPIO_InitStructure.GPIO_Pin = DATA_PIN;
  GPIO_Init(DATA_PORT, &GPIO_InitStructure);
// merge addres and EXE value (defaut
	data=((addr<<1)|0);
	SYN_H
	CLK_H
	delay(20);
	
	SCL_L
	delay(2);
	SYN_L
	delay(2);	
		
		CLK_L
		if(value)
		DAT_H			
		else
		DAT_L	
		delay(1);	
		CLK_H
		delay(1);	
for(i=6;i>=0;i--)
	{
		CLK_L
			//send 6 bits of address
		if((data>>i)&0x1)
		DAT_H			
		else
		DAT_L	
		delay(1);	
		CLK_H
		delay(1);
		
	}
	CLK_L	
	delay(1);
	CLK_H
	delay(2);
	SYN_H
	delay(2);	
	SCL_H
	delay(2);

	
		delay(100);	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	GPIO_InitStructure.GPIO_Pin = DATA_PIN;
  GPIO_Init(DATA_PORT, &GPIO_InitStructure);
		SCL_H;
		SYN_L;
		CLK_H;
		delay(100);	
		
		
		
		
		
		}
		




/*******************************************************************************
* Function Name  : stpm_spi_Init
* Description    : init ports
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/	


void stpm_spi_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;


  RCC_AHB1PeriphClockCmd(DATA_CLK|CLK_CLK|SCL_CLK|SYN_CLK , ENABLE);


  GPIO_InitStructure.GPIO_Pin = CLK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CLK_PORT, &GPIO_InitStructure);
	
	  GPIO_InitStructure.GPIO_Pin =  SCL_PIN;
  GPIO_Init(SCL_PORT, &GPIO_InitStructure);
	
		  GPIO_InitStructure.GPIO_Pin =  SYN_PIN;
  GPIO_Init(SYN_PORT, &GPIO_InitStructure);
	

  
stpm_read_config();
}  


/*******************************************************************************
* Function Name  : stpm_read_config
* Description    : config pins for read value
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/	

void stpm_read_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	GPIO_InitStructure.GPIO_Pin = DATA_PIN;
  GPIO_Init(DATA_PORT, &GPIO_InitStructure);

	
	
		SCL_H;
		SYN_L;
		CLK_H;
		delay(100);	
	
}  

