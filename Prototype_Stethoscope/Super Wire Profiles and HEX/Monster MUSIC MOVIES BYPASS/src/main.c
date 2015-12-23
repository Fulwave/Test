/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "NUC122.h"
#include "DrvGPIO.h"
#include "DrvTIMER.h"
#include "DrvSYS.h"
#include "DrvFMC.h"
#include "io_define.h"
#include "DataType.h"
#include "function.h"



/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/    
static void GPIO_Initial(void)
{
	DrvGPIO_Open(E_GPA, 10, E_IO_OPENDRAIN);	//i2c sda
	DrvGPIO_Open(E_GPA, 11, E_IO_OPENDRAIN);	//i2c scl

	DrvGPIO_Open(E_GPC, 10, E_IO_OUTPUT);	//led
	DrvGPIO_Open(E_GPC, 11, E_IO_OUTPUT);	//led
	DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT);	//led
	DrvGPIO_Open(E_GPC, 13, E_IO_OUTPUT);	//led
	DrvGPIO_Open(E_GPC, 0, E_IO_OUTPUT);	//led
	DrvGPIO_Open(E_GPC, 1, E_IO_OUTPUT);	//led

	DrvGPIO_Open(E_GPC, 2, E_IO_INPUT);		//key mode

	PO_LEDA(0);
	PO_LEDB(0);
	PO_LEDC(0);
	PO_LEDD(0);
	PO_LEDE(0);
	PO_LEDF(0);

}

static void TMR0_Initial(void)
{
	DrvTIMER_Init();
	DrvSYS_SelectIPClockSource(E_SYS_TMR0_CLKSRC, 0);
	DrvTIMER_Open(E_TMR0, 1000, E_PERIODIC_MODE);//中断基础值定为1ms
	DrvTIMER_EnableInt(E_TMR0);
	DrvTIMER_Start(E_TMR0);
	
}

static int32_t DrvSYS_Open_12d288m_48m(uint32_t u32Hclk)
{
    //uint32_t u32PllCr;  
    E_SYS_PLL_CLKSRC ePllSrc = E_SYS_EXTERNAL_12M;

    //if ( (u32Hclk > FREQ_60MHZ) || (u32Hclk < FREQ_25MHZ) )
    //{
       // return E_DRVSYS_ERR_OUT_OF_RANGE;
    //}
        
    /* Check the status of Register Write-Protection Function */
    if (DrvSYS_IsProtectedRegLocked() == 0)
    {
        return E_DRVSYS_ERR_REG_PROTECTED;
    }
    
    if (SYSCLK->PLLCON.PLL_SRC == 0)
    {
        SYSCLK->PWRCON.XTL12M_EN = 1;
        
        /* Wait for external crystal clock stable */
        DrvSYS_Delay(1200);

        if (SYSCLK->CLKSTATUS.XTL12M_STB)
        {
            ePllSrc = E_SYS_EXTERNAL_12M;
        }
        else
        {
            /* Swtich PLL source to RC 22M */
            SYSCLK->PLLCON.PLL_SRC = 1;
            ePllSrc = E_SYS_INTERNAL_22M;
        }
    }
    else
    {
        SYSCLK->PWRCON.OSC22M_EN = 1;
        
        /* Wait for 22M stable */
        while (SYSCLK->CLKSTATUS.OSC22M_STB == 0);

        ePllSrc = E_SYS_INTERNAL_22M;
    }

    //u32PllCr = DrvSYS_GetPLLContent(ePllSrc, u32Hclk);

    //DrvSYS_SetPLLContent(u32PllCr);
    DrvSYS_SetPLLContent(0x5c7b);//12.288M to 40M
    SYSCLK->PLLCON.OE = 0;
    SYSCLK->PLLCON.PD = 0;
    
    /* Wait for PLL stable */
    while (SYSCLK->CLKSTATUS.PLL_STB == 0);

    /* Change HCLK clock source to be PLL. */
    DrvSYS_SelectHCLKSource(2);
    
    return E_SUCCESS;
}


int main (void)
{
	UNLOCKREG();

	SYSCLK->PWRCON.OSC10K_EN = 1;

	/* Enable External 12M Xtal */
	DrvSYS_SetOscCtrl(E_SYS_XTL12M, ENABLE);

	/* Waiting for 12M Xtal stable */
	while (DrvSYS_GetChipClockSourceStatus(E_SYS_XTL12M) != 1);

	/* Configure PLL clock */
	DrvSYS_SelectPLLSource(E_SYS_EXTERNAL_12M);
	DrvSYS_Open_12d288m_48m(48000000);

	/* Get PLL clock frequency */
	//printf("PLL clock %d Hz\n", DrvSYS_GetPLLClockFreq());
	GPIO_Initial();
	TMR0_Initial();

	DrvFMC_EnableISP();

	delays(500000);
	Initial_Status();
	while(1)
	{
		OperationCycle();
	}
	//DrvGPIO_Close(E_GPA, 10);
	//DrvGPIO_Close(E_GPB, 9);
}




