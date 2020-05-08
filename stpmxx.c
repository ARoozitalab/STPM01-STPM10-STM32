


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

long long int active_pow=0;
long long int active_q=0;
long long int powl_active=0;
long long int powl_q=0;



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
* Function Name  : update_pow
* Description    : update power and store to long variable
* Input          : POWER_DATA stract
* Output         : none
* Return         : data of voltage , current , ...
*******************************************************************************/	

void update_pow(POWER_DATA  pow)
{

	uint32_t temp=0;
	if (pow.active_energy >= powl_active){
	if ((pow.active_energy - powl_active) >= 0x80000){ //roll over, negative energy
	temp = 0x100000 + powl_active -pow.active_energy;
	active_pow -= temp;//total ernergy minus the negative energy
	}
	else{
	temp = pow.active_energy - powl_active; //positive energy
	active_pow += temp; //add the positive energy to the total energy
	}
	}
	else{
	if ((powl_active - pow.active_energy) >= 0x80000) { //roll over, positive energy
	temp = 0x100000 + pow.active_energy - powl_active;
	active_pow+= temp; //total ernergy plus the positive energy
	}
	else{
	temp= powl_active - pow.active_energy; //negative energy
	active_pow -= temp; //total energy minus the negative energy
	}
	}

powl_active=pow.active_energy;
	////////
	temp=0;
	if (pow.apparent_energy >= powl_q){
	if ((pow.apparent_energy - powl_q) >= 0x80000){ //roll over, negative energy
	temp = 0x100000 + powl_q -pow.apparent_energy;
	active_q -= temp;//total ernergy minus the negative energy
	}
	else{
	temp = pow.apparent_energy - powl_q; //positive energy
	active_q += temp; //add the positive energy to the total energy
	}
	}
	else{
	if ((powl_q - pow.apparent_energy) >= 0x80000) { //roll over, positive energy
	temp = 0x100000 + pow.apparent_energy - powl_q;
	active_q+= temp; //total ernergy plus the positive energy
	}
	else{
	temp= powl_q - pow.apparent_energy; //negative energy
	active_q -= temp; //total energy minus the negative energy
	}
	}

powl_q=pow.apparent_energy;

}
/*******************************************************************************
* Function Name  : read_stpm
* Description    : read all data
* Input          : None
* Output         : POWER_DATA stract
* Return         : data of voltage , current , ...
*******************************************************************************/	
#define avr  7
POWER_DATA read_stpm(void)
{
		uint8_t i=0,j=0,k=0;
		uint8_t num=0;

		STPM_DATA stpm_value[avr];
	  POWER_DATA get_value;
		uint8_t datab;
		uint8_t *p;
	  int uRMS[avr];            
    int uMOM[avr];                    
    int iRMS[avr];   									
    int iMOM[avr]; 
    uint64_t	frequency;
	
	

for(k=0;k<avr;k++)
	{	
			p=(uint8_t *)&stpm_value[k];

			for(j=1;j<33;j++)
		  {
				p[j-1]=0;

			}
		}


	
for(k=0;k<avr;k++)
{	
	p=(uint8_t *)&stpm_value[k];
	
		CLK_H
		SCL_H
		SYN_L
		delay(4);
		SCL_L
		delay(2);
		SYN_H
		delay(1);
		num=4;
//			while( !GPIO_ReadInputDataBit(ZC_PORT,ZC_PIN));
//			 while( GPIO_ReadInputDataBit(ZC_PORT,ZC_PIN));
	
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
		SYN_H;
		CLK_H;
//		delay1(50);	

		// cheack parity bits
   if(BadParity((uint8_t *)&stpm_value[k].CFH) || BadParity((uint8_t *)&stpm_value[k].CFL) ||
		BadParity((uint8_t *)&stpm_value[k].DAP) || BadParity((uint8_t *)&stpm_value[k].DEV)    || 
		BadParity((uint8_t *)&stpm_value[k].DFP) || BadParity((uint8_t *)&stpm_value[k].DMV)    || 
		BadParity((uint8_t *)&stpm_value[k].DRP) || BadParity((uint8_t *)&stpm_value[k].DSP))
		{

		get_value.flag=0;	
			
		return get_value;
		}
 // if parity ok calculate and store data
			delay(40);
		get_value.flag=1;
		
		get_value.active_energy=((stpm_value[k].DAP>>8)& 0xfFFFF);
		get_value.reactive_energy=((stpm_value[k].DRP>>8)& 0xfFFFF);
		get_value.apparent_energy=((stpm_value[k].DSP>>8)& 0xfFFFF);
		get_value.status=stpm_value[k].DAP&0xff;
		get_value.modsignal=stpm_value[k].DFP&0xff;

		frequency=((stpm_value[k].DRP&0x3f)<<8);		
		frequency=	(frequency|(stpm_value[k].DSP&0xff));
		uRMS[k]=((stpm_value[k].DEV>>16)& 0x7FF);
	  uMOM[k]=((stpm_value[k].DMV>>16)& 0x7FF);
		
		
		iRMS[k]=((stpm_value[k].DEV)& 0xFFFF);
	  iMOM[k]=((stpm_value[k].DMV)& 0xFFFF);
		
		
if (iMOM[k] & 0x08000) // positive current
iMOM[k] = iMOM[k] & 0x07FFF;
else // negative current
{
iMOM[k] = 0x08000 - iMOM[k];
iMOM[k] = iMOM[k] * (-1);
}
if ( uMOM[k]& 0x0400) // positive voltage
uMOM[k] = (uMOM[k]) & 0x3FF;
else // negative voltage
{
uMOM[k] = 0x0400 - (uMOM[k]);
uMOM[k] =  uMOM[k] * (-1);
}		
		
}		
get_value.uRMS=0;

for(k=0;k<avr;k++)
get_value.uRMS=uRMS[k]+get_value.uRMS;
get_value.uRMS=get_value.uRMS/avr;

get_value.iRMS=0;
for(k=0;k<avr;k++)
get_value.iRMS=iRMS[k]+get_value.iRMS;
get_value.iRMS=get_value.iRMS/avr;

get_value.uMOM=0;
for(k=0;k<avr;k++)
get_value.uMOM=uMOM[k]+get_value.uMOM;
get_value.uMOM=get_value.uMOM/avr;

get_value.iMOM=0;
for(k=0;k<avr;k++)
get_value.iMOM=iMOM[k]+get_value.iMOM;
get_value.iMOM=get_value.iMOM/avr;



get_value.uRMS = (1+R1/R2) * get_value.uRMS *Vref /(Au * Ku * Kint_comp * Kint * Kdif * len_u * Kut);
get_value.iRMS = get_value.iRMS * Vref/(Ks * Ai * Ki * Kint * Kint_comp * Kdif * len_i);
get_value.uMOM = (1+R1/R2) * get_value.uMOM * Vref /(Au * Ku * Kint_comp * Kint * Kdif * len_u_mom *Kut);
get_value.iMOM = get_value.iMOM * Vref /(Ks * Ai * Ki * Kint * Kint_comp * Kdif * len_i_mom);


frequency=frequency*2 * Fm ;
 get_value.freq=frequency /(3.14159 * len_f * Dint);

		

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

