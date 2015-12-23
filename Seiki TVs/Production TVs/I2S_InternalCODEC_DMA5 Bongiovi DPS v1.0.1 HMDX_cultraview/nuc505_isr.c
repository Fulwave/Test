/******************************************************************************
 * @file     nuc505_isr.c
 * @version  V0.10
 * $Revision: 2 $
 * $Date: 14/09/19 9:30a $
 * @brief    NUC505 series ISR source file
 *
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NUC505Series.h"

#include "config.h"
#include "DPSLib.h"

#include <math.h>
#include <arm_math.h>



#define INV_MAX_POS 1.0f/32767.0f
#define INV_MAX_NEG 1.0f/32768.0f

#define INV_MAX_POS_32 1.0f/(2147483647.0f)
#define INV_MAX_NEG_32 1.0f/(2147483648.0f)

extern uint32_t BUFF_LEN;

#ifdef BITS_16
extern int16_t PcmRxBuff[2][MAX_BUFF_LEN*2];
extern int16_t PcmTxBuff[2][MAX_BUFF_LEN*2];
#else
extern int32_t PcmRxBuff[2][MAX_BUFF_LEN*2];
extern int32_t PcmTxBuff[2][MAX_BUFF_LEN*2];
#endif

float32_t x_l, x_r;
float32_t xf_L[MAX_BUFF_LEN];
float32_t xf_R[MAX_BUFF_LEN];
int16_t xtemp;
uint8_t i;

void callback(uint8_t *, uint8_t);



uint32_t err;

///////////////////
void callback(uint8_t *data, uint8_t N)
{
	for (i = 0; i <= N; i++)
	{
		UART_WAIT_TX_EMPTY(UART0);
		UART_WRITE(UART0, data[i]);
	}
}
////////////////////

volatile static uint32_t s_flag1;



#ifdef BITS_16
void fixedToFloat(int16_t *input, float32_t *output, uint32_t N)
{
	int i;
	
	for (i = 0; i < N; i++)
	{
		xtemp = input[2*i];
		
		if (xtemp > 0)
			output[i] = (float32_t)xtemp * INV_MAX_POS;
		else
			output[i] = (float32_t)xtemp * INV_MAX_NEG;
	}
}

void floatToFixed(float32_t *input, int16_t *output, uint32_t N)
{
	int i;
	float fxmax = 0.99f;
	float fxmin = -0.99f;
	
	for (i = 0; i < N; i++)
	{
		if (input[i] > 0.0f)
		{	
			if (input[i] > fxmax)
			{
				input[i] = fxmax;
			}
			output[2*i] = (int16_t)(32767 * input[i]);
		}
		else
		{
			if (input[i] < fxmin)
			{
				input[i] = fxmin;
			}
			
			output[2*i] = (int16_t)(32768 * input[i]);
		}
	}
}	
#else
void fixedToFloat(int32_t *input, float32_t *output, uint32_t N)
{
	int32_t i;
	
	for (i = 0; i < N; i++)
	{
		output[i] = (float32_t)input[2*i] * 4.656612875e-10f;
	}
}

void floatToFixed(float32_t *input, int32_t *output, uint32_t N)
{
	int32_t i;
	
	for (i = 0; i < N; i++)
	{
		if (input[i] > 0.99f)
			input[i] = 0.99f;
		if (input[i] < -0.99f)
			input[i] = -0.99f;
		
			output[2*i] = (int32_t)(input[i] * 2147483647.0f);
	}
}

#endif

void StereoExpand(float32_t *leftBuffer, float32_t *rightBuffer)
{
#ifdef RATE_48K
	SimpleStereoWiden(leftBuffer, rightBuffer);
#else
	StereoWiden(leftBuffer, rightBuffer);
#endif
}



void UART0_IRQHandler(void)
{
	uint8_t x;
	uint32_t u32IntSts = UART0->INTSTS;

	if(u32IntSts & UART_INTSTS_RDAINT_Msk) 
	{
		x = UART_READ(UART0);
		//printf("%c",x);
		DPS_parse_byte(x);
	}
}

void I2S_IRQHandler(void)
{
	uint32_t u32I2SIntFlag;

	u32I2SIntFlag = I2S_GET_INT_FLAG(I2S, (I2S_STATUS_RDMATIF_Msk | I2S_STATUS_RDMAEIF_Msk));
	
	/* Copy RX data to TX buffer */
	if (u32I2SIntFlag & I2S_STATUS_RDMATIF_Msk)
	{
		I2S_CLR_INT_FLAG(I2S, I2S_STATUS_RDMATIF_Msk);

		fixedToFloat(&PcmRxBuff[0][0], xf_L, BUFF_LEN);
		fixedToFloat(&PcmRxBuff[0][1], xf_R, BUFF_LEN);

		NoiseGateProcess(xf_L, xf_R);
//		StereoExpand(xf_L, xf_R);
		StereoWiden(xf_L, xf_R);
		DPS_Left(xf_L);
		DPS_Right(xf_R);
				
		floatToFixed(xf_L, &PcmTxBuff[0][0], BUFF_LEN);
		floatToFixed(xf_R, &PcmTxBuff[0][1], BUFF_LEN);
	}
	else if (u32I2SIntFlag & I2S_STATUS_RDMAEIF_Msk)
	{
		I2S_CLR_INT_FLAG(I2S, I2S_STATUS_RDMAEIF_Msk);
		if ( s_flag1 == 0 )
		{
			s_flag1 = 1;
			I2S_ENABLE_TXDMA(I2S);
			I2S_ENABLE_TX(I2S);
		}
				
		fixedToFloat(&PcmRxBuff[1][0], xf_L, BUFF_LEN);
		fixedToFloat(&PcmRxBuff[1][1], xf_R, BUFF_LEN);

		NoiseGateProcess(xf_L, xf_R);
//		StereoExpand(xf_L, xf_R);
		StereoWiden(xf_L, xf_R);
		DPS_Left(xf_L);
		DPS_Right(xf_R);

		floatToFixed(xf_L, &PcmTxBuff[1][0], BUFF_LEN);
		floatToFixed(xf_R, &PcmTxBuff[1][1], BUFF_LEN);
	}
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
