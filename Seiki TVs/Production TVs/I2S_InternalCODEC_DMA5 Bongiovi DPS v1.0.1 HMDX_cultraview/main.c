/**************************************************************************//**
 * @file     main.c
 * @version  V1.0.0 Including indices.c for HMDX P-420 demo
 * $Revision: 2 $
 * $Date: 14/09/19 9:30a $
 * @brief    NUC505 Series I2S Driver Sample Code
 *           This is a I2S demo with DMA function connected with Internal CODEC.
 *
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NUC505Series.h"
#include "gpio.h"
#include "config.h"
#include "DPSLib.h"

#include <math.h>
#include <arm_math.h>



uint32_t BUFF_LEN = 512;

#ifdef BITS_16
int16_t PcmRxBuff[2][MAX_BUFF_LEN*2] = {0};
int16_t PcmTxBuff[2][MAX_BUFF_LEN*2] = {0};
#else
int32_t PcmRxBuff[2][MAX_BUFF_LEN*2] = {0};
int32_t PcmTxBuff[2][MAX_BUFF_LEN*2] = {0};
#endif


//I2C----------------------------
uint32_t slave_buff_addr;
uint8_t g_u8SlvData[256];
uint8_t g_au8RxData[3];
uint8_t g_u8DeviceAddr;
uint8_t g_au8TxData[3];
uint8_t g_u8RxData;
uint8_t g_u8DataLen;
typedef void (*I2C_FUNC)(uint32_t u32Status);
static I2C_FUNC s_I2C1HandlerFn = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;

    u32Status = I2C_GET_STATUS(I2C1);

    if (I2C_GET_TIMEOUT_FLAG(I2C1)) {
        /* Clear I2C1 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    } else {
        if (s_I2C1HandlerFn != NULL)
            s_I2C1HandlerFn(u32Status);
    }

    /* To avoid the synchronization issue between system and APB clock domain */
    u32Status = I2C_GET_STATUS(I2C1);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C TRx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_SlaveTRx(uint32_t u32Status)
{

	 //printf("\n [ Status = 0x%x ]\n", u32Status);

    if (u32Status == 0x60) {                    /* Own SLA+W has been receive; ACK has been return */
        g_u8DataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    } else if (u32Status == 0x80)                 /* Previously address with own SLA address
                                                   Data has been received; ACK has been returned*/
    {
        g_au8RxData[g_u8DataLen] = I2C_GET_DATA(I2C1);;
        g_u8DataLen++;

        if (g_u8DataLen == 2) {
            slave_buff_addr = (g_au8RxData[0] << 8) + g_au8RxData[1];
        }
        if (g_u8DataLen == 3) {
            g_u8SlvData[slave_buff_addr] = g_au8RxData[2];
            g_u8DataLen = 0;
			 //printf("\n g_u8SlvData[%d] = 0x%x\n",slave_buff_addr ,g_u8SlvData[slave_buff_addr]);
        }
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    } else if(u32Status == 0xA8) {              /* Own SLA+R has been receive; ACK has been return */

        I2C_SET_DATA(I2C1, g_u8SlvData[slave_buff_addr]);
        slave_buff_addr++;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    } else if (u32Status == 0xC0)                 /* Data byte or last data in I2CDAT has been transmitted
                                                   Not ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    } else if (u32Status == 0x88)                 /* Previously addressed with own SLA address; NOT ACK has
                                                   been returned */
    {
        g_u8DataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    } else if (u32Status == 0xA0)                 /* A STOP or repeated START has been received while still
                                                   addressed as Slave/Receiver*/
    {
        g_u8DataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    } else {
        /* TO DO */
        //printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

void I2C1_Init(void)
{
    CLK_EnableModuleClock(I2C1_MODULE);
    /* Open I2C1 and set clock to 100k */
    I2C_Open(I2C1, 100000);
    /* Get I2C0 Bus Clock */
    //printf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C0));
    /* Get I2C0 Bus Clock */
   // printf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C1));
	/* Set GPA10,GPA11 multi-function pins for I2C1 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA10MFP_Msk) ) | SYS_GPA_MFPH_PA10MFP_I2C1_SCL;
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA11MFP_Msk) ) | SYS_GPA_MFPH_PA11MFP_I2C1_SDA;


    I2C_SetSlaveAddr(I2C1, 0, 0x15, I2C_GCMODE_DISABLE);   /* Slave Address : 0x15 */
    //I2C_SetSlaveAddr(I2C1, 1, 0x35, I2C_GCMODE_DISABLE);   /* Slave Address : 0x35 */
    //I2C_SetSlaveAddr(I2C1, 2, 0x55, I2C_GCMODE_DISABLE);   /* Slave Address : 0x55 */
    //I2C_SetSlaveAddr(I2C1, 3, 0x75, I2C_GCMODE_DISABLE);   /* Slave Address : 0x75 */


    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);

}

void Delay(int count)
{
	volatile uint32_t i;
	for (i = 0; i < count ; i++);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Internal CODEC Settings with Virtual I2C interface                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void InternalCODEC_Setup()
{
	printf("\nConfigure Internal CODEC ...");

	Delay(0x200);

	I2S_SET_INTERNAL_CODEC(I2S, 0x02, 0xC0);			// CODEC Slave Mode
	I2S_SET_INTERNAL_CODEC(I2S, 0x0B, 0xF0);			// DAC Power On
	I2S_SET_INTERNAL_CODEC(I2S, 0x0F, 0xE0);			// ADC Power On
	I2S_SET_INTERNAL_CODEC(I2S, 0x0D, 0x32);			// PLL Power Off
	I2S_SET_INTERNAL_CODEC(I2S, 0x01, 0x80);			// DAC Open
	I2S_SET_INTERNAL_CODEC(I2S, 0x00, 0xD0);			// ADC Open
	I2S_SET_INTERNAL_CODEC(I2S, 0x08, 0x01);			// DAC Analog Left Volume - DAC sounds best at this volume level
	I2S_SET_INTERNAL_CODEC(I2S, 0x09, 0x01);			// DAC Analog Right Volume
	I2S_SET_INTERNAL_CODEC(I2S, 0x10, 0x00);			// ADC Left Channel Volume
	I2S_SET_INTERNAL_CODEC(I2S, 0x11, 0x00);			// ADC Right Channel Volume - ADC sounds best at this volume level
	SYS->GPD_MFPL  = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD4MFP_Msk) ) | SYS_GPD_MFPL_PD4MFP_RIGHT_LINE_IN;
	SYS_SetSharedPinType(SYS_PORT_D, 4, 0, 0);
}

void SYS_Init(void)
{
	CLK_SetCoreClock(100000000);
	CLK_SetModuleClock(PCLK_MODULE, NULL, 1);
}

void DMA_Init(void)
{
#ifdef BITS_16
	I2S_SET_TXDMA_STADDR(I2S, (uint32_t) &PcmTxBuff[0]);							// Tx Start Address
	I2S_SET_TXDMA_THADDR(I2S, (uint32_t) &PcmTxBuff[0][BUFF_LEN*2-2]);	// Tx Threshold Address
	I2S_SET_TXDMA_EADDR( I2S, (uint32_t) &PcmTxBuff[1][BUFF_LEN*2-2]);			// Tx End Address

	I2S_SET_RXDMA_STADDR(I2S, (uint32_t) &PcmRxBuff[0]);							// Rx Start Address
	I2S_SET_RXDMA_THADDR(I2S, (uint32_t) &PcmRxBuff[0][BUFF_LEN*2-2]);	// Rx Threshold Address
	I2S_SET_RXDMA_EADDR( I2S, (uint32_t) &PcmRxBuff[1][BUFF_LEN*2-2]);			// Rx End Address
#else
	I2S_SET_TXDMA_STADDR(I2S, (uint32_t) &PcmTxBuff[0]);							// Tx Start Address
	I2S_SET_TXDMA_THADDR(I2S, (uint32_t) &PcmTxBuff[0][BUFF_LEN*2-1]);	// Tx Threshold Address
	I2S_SET_TXDMA_EADDR( I2S, (uint32_t) &PcmTxBuff[1][BUFF_LEN*2-1]);			// Tx End Address

	I2S_SET_RXDMA_STADDR(I2S, (uint32_t) &PcmRxBuff[0]);							// Rx Start Address
	I2S_SET_RXDMA_THADDR(I2S, (uint32_t) &PcmRxBuff[0][BUFF_LEN*2-1]);	// Rx Threshold Address
	I2S_SET_RXDMA_EADDR( I2S, (uint32_t) &PcmRxBuff[1][BUFF_LEN*2-1]);			// Rx End Address
#endif
}

void UART0_Init(void)
{
	CLK_EnableModuleClock(UART0_MODULE);
	CLK_SetModuleClock(UART0_MODULE, CLK_UART0_SRC_EXT, 0);
	SYS_ResetModule(UART0_RST);
	UART_Open(UART0, 115200);
	SYS->GPB_MFPL  = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB0MFP_Msk) ) | SYS_GPB_MFPL_PB0MFP_UART0_TXD;
	SYS->GPB_MFPL  = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk) ) | SYS_GPB_MFPL_PB1MFP_UART0_RXD;
}

void I2S_Init(void)
{
#ifdef USE_INTERNAL_CODEC	
	CLK_EnableModuleClock(I2S_MODULE);
	CLK_SET_APLL(0xD382);	// APLL is 45.142MHz
	CLK_SetModuleClock(I2S_MODULE, CLK_I2S_SRC_APLL, 1);	// 1 means (1+1=2, 45.142/2)
	SYS_ResetModule(I2S_RST);
	SYS->GPA_MFPL  = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA2MFP_Msk) ) | SYS_GPA_MFPL_PA2MFP_I2S_MCLK;	
	SYS->GPA_MFPL  = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA3MFP_Msk) ) | SYS_GPA_MFPL_PA3MFP_I2S_DIN;	
	SYS->GPA_MFPL  = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA4MFP_Msk) ) | SYS_GPA_MFPL_PA4MFP_I2S_DOUT;	
	SYS->GPA_MFPH  = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk) ) | SYS_GPA_MFPH_PA8MFP_I2S_LRCLK;
	SYS->GPA_MFPH  = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk) ) | SYS_GPA_MFPH_PA9MFP_I2S_BCLK;
#else
	CLK_EnableModuleClock(I2S_MODULE);
	CLK_SET_APLL(CLK_APLL_49152031);
	CLK_SetModuleClock(I2S_MODULE, CLK_I2S_SRC_APLL, 0);
	SYS_ResetModule(I2S_RST);
	SYS->GPA_MFPL  = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA2MFP_Msk) ) | SYS_GPA_MFPL_PA2MFP_I2S_MCLK;	
	SYS->GPA_MFPL  = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA3MFP_Msk) ) | SYS_GPA_MFPL_PA3MFP_I2S_DIN;	
	SYS->GPA_MFPL  = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA4MFP_Msk) ) | SYS_GPA_MFPL_PA4MFP_I2S_DOUT;	
	SYS->GPA_MFPH  = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk) ) | SYS_GPA_MFPH_PA8MFP_I2S_LRCLK;
	SYS->GPA_MFPH  = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk) ) | SYS_GPA_MFPH_PA9MFP_I2S_BCLK;
#endif	
}

void SysTick_Handler(void)
{
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main (void)
{
	static uint8_t bPreIO7Status=0;
	static uint8_t bPreAudioMode=3;
	static uint8_t bSurroundMode=0;
	uint32_t i,err;
	SYS_Init();
	UART0_Init();
	/* Init I2C1 */
    I2C1_Init();

    /* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    for (i = 0; i < 0x100; i++) {
        g_u8SlvData[i] = 0;
    }

    /* I2C function to Slave receive/transmit data */
	s_I2C1HandlerFn=I2C1_SlaveTRx;
	
    {
			extern uint32_t __Vectors[];
			extern uint32_t __Vectors_Size[];
			extern uint32_t Image$$ER_VECTOR2$$ZI$$Base[];
			memcpy((void *) Image$$ER_VECTOR2$$ZI$$Base, (void *) __Vectors, (unsigned int) __Vectors_Size);
			SCB->VTOR = (uint32_t) Image$$ER_VECTOR2$$ZI$$Base;
    }

	CLK_DisableModuleClock(SPIM_MODULE);
	CLK_DisableModuleClock(ROM_MODULE);
	I2S_Init();
	err = DPS_GlobalInitialize();	
	
	if (err == 1)
		printf("not a NUC505");
	
	DPS_EnableDPS();	
	DPS_EnableStereoWidening();	
	DPS_LoadPreset(1);	// Load Chinese Music Demo Profile


#ifdef RATE_48K	
	#ifdef USE_INTERNAL_CODEC	
		#ifdef BITS_16	
			I2S_Open(I2S, I2S_MODE_MASTER, 48000, I2S_DATABIT_16, I2S_STEREO, I2S_FORMAT_I2S, I2S_ENABLE_INTERNAL_CODEC);
			I2S_EnableMCLK(I2S, 256*48000);
		#else
			I2S_Open(I2S, I2S_MODE_MASTER, 48000, I2S_DATABIT_32, I2S_STEREO, I2S_FORMAT_I2S, I2S_ENABLE_INTERNAL_CODEC);
			I2S_EnableMCLK(I2S, 256*48000);
		#endif
		InternalCODEC_Setup();
	#else	
		I2S_Open(I2S, I2S_MODE_SLAVE, 48000, I2S_DATABIT_32, I2S_STEREO, I2S_FORMAT_I2S, I2S_DISABLE_INTERNAL_CODEC);
	#endif

#else	

	#ifdef USE_INTERNAL_CODEC	
		#ifdef BITS_16	
			I2S_Open(I2S, I2S_MODE_MASTER, 44100, I2S_DATABIT_16, I2S_STEREO, I2S_FORMAT_I2S, I2S_ENABLE_INTERNAL_CODEC);
			I2S_EnableMCLK(I2S, 256*44100);
		#else
			I2S_Open(I2S, I2S_MODE_MASTER, 44100, I2S_DATABIT_32, I2S_STEREO, I2S_FORMAT_I2S, I2S_ENABLE_INTERNAL_CODEC);
			I2S_EnableMCLK(I2S, 256*44100);
		#endif
		InternalCODEC_Setup();
	#else	
		I2S_Open(I2S, I2S_MODE_SLAVE, 44100, I2S_DATABIT_32, I2S_STEREO, I2S_FORMAT_I2S, I2S_DISABLE_INTERNAL_CODEC);
	#endif
#endif

	DMA_Init();
	I2S_ENABLE_RXDMA(I2S);
	I2S_ENABLE_RX(I2S);
	NVIC_EnableIRQ(I2S_IRQn);
	I2S_EnableInt(I2S, (I2S_IEN_RDMATIEN_Msk|I2S_IEN_RDMAEIEN_Msk));
	UART_EnableInt(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk));

   //GPIO----------------------------------------------------------------------																		  
    	GPIO_SetMode(PB, BIT7, GPIO_MODE_INPUT);
    	GPIO_SetPullMode(PB, BIT7, GPIO_PULL_UP_EN);

		while(1)
		{
			if(PB7_PIN!=bPreIO7Status)
			{
				//printf("\n PB7_PIN  %d -> %d \n",bPreIO7Status,PB7_PIN);
				bPreIO7Status=PB7_PIN;	
				if(PB7_PIN==0)
				{
						//DPS_EnableDPS();
					  //I2S_EnableInt(I2S, (I2S_IEN_RDMATIEN_Msk|I2S_IEN_RDMAEIEN_Msk));
						NVIC_EnableIRQ(I2S_IRQn);
					  //UART_EnableInt(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk));
						//printf("\n LOW \n");
				}
				else if(PB7_PIN==1)
				{
						//DPS_BypassDPS();
					  //NVIC_DisableIRQ(UART0_IRQn);	
						NVIC_DisableIRQ(I2S_IRQn);
						
						//printf("\n HIGH \n");
				}
			}
			if(bPreAudioMode != g_u8SlvData[1])
			{
		    //printf("\n Audio Mode Change from %d to %d \n",bPreAudioMode,g_u8SlvData[1]);
				bPreAudioMode=g_u8SlvData[1];
				switch(bPreAudioMode)
				{
					case 0:
						//not use
						//printf("\n Audio Mode by pass \n");
						break;
					case 1:
					  DPS_LoadPreset(0);
					  //printf("\n Audio Mode 0 \n");
						break;
					case 2:
					  DPS_LoadPreset(1);
					  //printf("\n Audio Mode 1 \n");
						break;
					case 3:
					  DPS_LoadPreset(2);
					  //printf("\n Audio Mode 2 \n");
						break;
					case 4:
					  DPS_LoadPreset(3);
					  //printf("\n Audio Mode 3 \n");
						break;
					case 5:
					  DPS_LoadPreset(4);
					  //printf("\n Audio Mode 4 \n");
						break;
					default:
						break;
				}
			}
			if(bSurroundMode!=g_u8SlvData[2])
			{
				bSurroundMode=g_u8SlvData[2];
				if(bSurroundMode==1)
				{
					DPS_EnableStereoWidening();
					//printf("\n DPS_EnableStereoWidening \n");
				}
				else if(bSurroundMode==0)
				{
					DPS_BypassStereoWidening();
					//printf("\n DPS_BypassStereoWidening \n");
				}
			}
		}
}
/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
