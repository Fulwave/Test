#include "NUC122.h"
#include "DrvGPIO.h"
#include "DrvSYS.h"
#include "DataType.h"
#include "function.h"
#include "Io_define.h"
#include "1761_IC_1.h"
#include "1761_IC_1_REG.h"

#define ADAU1761SDA_DIR_OUT  	DrvGPIO_Open(E_GPA, 10, E_IO_OPENDRAIN)
#define ADAU1761SDA_DIR_IN   	DrvGPIO_Open(E_GPA, 10, E_IO_INPUT)


static uint8_t OperationWriteADI1761(uint8_t devAddress, uint16_t address, uint16_t numBytes, const uint8_t *data)
{
	int8_t i;
	uint8_t error=0,Hiaddres,Loaddres;
	uint16_t j;
	
	if(numBytes == 0)
		return 0;

/***************************************************

START: make sure here SDIO_DIR =OUT, SCLK = 1,	SDIO = 1

****************************************************/

	ADAU1761SDA_DIR_OUT;
	PO_1761_SCL(1);
	PO_1761_SDA(1);
	delay_1us();
	PO_1761_SDA(0);
	delay_1us();
	PO_1761_SCL(0);
	delay_1us();


/***************************************************

WRITE ADDRESS AND DATA: make sure here: SLCK = 0; SDIO = 0

****************************************************/


//address

	devAddress = devAddress|0;

	for(i = 7; i>=0; i--)
		{
			if((devAddress >> i) & 0x01)
				PO_1761_SDA(1);
			else
				PO_1761_SDA(0);
			delay_1us();
			PO_1761_SCL(1);
			delay_1us();
			PO_1761_SCL(0);
			delay_1us();
		}
//ack of address
	ADAU1761SDA_DIR_IN;

	delay_1us();
	PO_1761_SCL(1);
	delay_1us();	
	if(PI_1761_SDA() != 0)
		{
			error = 1;
			goto STOP;
		}
	PO_1761_SCL(0);
	delay_1us();

//address2

	Hiaddres=(uint8_t)(address>>8);
	Loaddres=(uint8_t)(0x00ff & address);
//high byte	
	ADAU1761SDA_DIR_OUT;
	for(i = 7; i>=0; i--)
		{
			if((Hiaddres >> i) & 0x01)
				PO_1761_SDA(1);
			else
				PO_1761_SDA(0);
			delay_1us();
			PO_1761_SCL(1);
			delay_1us();
			PO_1761_SCL(0);
			delay_1us();
		}
//ack of high byte	
	ADAU1761SDA_DIR_IN;

	delay_1us();
	PO_1761_SCL(1);
	delay_1us();	
	if(PI_1761_SDA() != 0)
		{
			error = 1;
			goto STOP;
		}
	PO_1761_SCL(0);
	delay_1us();
//low byte	
	ADAU1761SDA_DIR_OUT;
	for(i = 7; i>=0; i--)
		{
			if((Loaddres >> i) & 0x01)
				PO_1761_SDA(1);
			else
				PO_1761_SDA(0);
			delay_1us();
			PO_1761_SCL(1);
			delay_1us();
			PO_1761_SCL(0);
			delay_1us();
		}
//ack of low byte	
	ADAU1761SDA_DIR_IN;

	delay_1us();
	PO_1761_SCL(1);
	delay_1us();	
	if(PI_1761_SDA() != 0)
		{
			error = 1;
			goto STOP;
		}
	PO_1761_SCL(0);
	delay_1us();
			
//data

	for(j = 0; j < numBytes; j++, data++)
		{

			ADAU1761SDA_DIR_OUT; 

			
			for(i = 7; i>=0; i--)
			{
					if((*data >> i) & 0x01)
						PO_1761_SDA(1);
					else
						PO_1761_SDA(0);
				delay_1us();
				PO_1761_SCL(1);
				delay_1us();

				PO_1761_SCL(0);
				delay_1us();
			}
//ack of data

			ADAU1761SDA_DIR_IN;
			delay_1us();
			PO_1761_SCL(1);
			delay_1us();
			if(PI_1761_SDA() != 0)
				{
					error = 1;
					goto STOP;
				}
			PO_1761_SCL(0);
			delay_1us();
		}

	
	STOP:

		ADAU1761SDA_DIR_OUT;
		PO_1761_SDA(0);
		delay_1us();
		PO_1761_SCL(1);
		delay_1us();
		PO_1761_SDA(1);
		delay_1us();

		return(error);

}

static uint8_t OperationReadADI1761( uint8_t devAddress, uint16_t address, uint16_t numBytes,  uint8_t *data)
{
	int8_t i;
	uint8_t error=0,Hiaddres,Loaddres;
	uint16_t j;
	
	if(numBytes == 0)
		return 0;

/***************************************************

START: make sure here SDIO_DIR =OUT, SCLK = 1,	SDIO = 1

****************************************************/

	ADAU1761SDA_DIR_OUT;
	PO_1761_SCL(1);
	PO_1761_SDA(1);
	delay_1us();
	PO_1761_SDA(0);
	delay_1us();
	PO_1761_SCL(0);
	delay_1us();


/***************************************************

WRITE ADDRESS AND DATA: make sure here: SLCK = 0; SDIO = 0

****************************************************/


//address
	
	devAddress = devAddress|1;


	for(i = 7; i>=0; i--)
		{
			if((devAddress >> i) & 0x01)
				PO_1761_SDA(1);
			else
				PO_1761_SDA(0);
			delay_1us();
			PO_1761_SCL(1);
			delay_1us();
			PO_1761_SCL(0);
			delay_1us();
		}
//ack of address
	ADAU1761SDA_DIR_IN;

	delay_1us();
	PO_1761_SCL(1);
	delay_1us();	
	if(PI_1761_SDA() != 0)
		{
			error = 1;
			goto STOP;
		}
	PO_1761_SCL(0);
	delay_1us();

//address2

	Hiaddres=(uint8_t)(address>>8);
	Loaddres=(uint8_t)(0x00ff & address);
//high byte	
	ADAU1761SDA_DIR_OUT;
	for(i = 7; i>=0; i--)
		{
			if((Hiaddres >> i) & 0x01)
				PO_1761_SDA(1);
			else
				PO_1761_SDA(0);
			delay_1us();
			PO_1761_SCL(1);
			delay_1us();
			PO_1761_SCL(0);
			delay_1us();
		}
//ack of high byte	
	ADAU1761SDA_DIR_IN;

	delay_1us();
	PO_1761_SCL(1);
	delay_1us();	
	if(PI_1761_SDA() != 0)
		{
			error = 1;
			goto STOP;
		}
	PO_1761_SCL(0);
	delay_1us();
//low byte	
	ADAU1761SDA_DIR_OUT;
	for(i = 7; i>=0; i--)
		{
			if((Loaddres >> i) & 0x01)
				PO_1761_SDA(1);
			else
				PO_1761_SDA(0);
			delay_1us();
			PO_1761_SCL(1);
			delay_1us();
			PO_1761_SCL(0);
			delay_1us();
		}
//ack of low byte	
	ADAU1761SDA_DIR_IN;

	delay_1us();
	PO_1761_SCL(1);
	delay_1us();	
	if(PI_1761_SDA() != 0)
		{
			error = 1;
			goto STOP;
		}
	PO_1761_SCL(0);
	delay_1us();
			
//data

	for(j = 0; j < numBytes; j++, data++)
		{
			
			ADAU1761SDA_DIR_IN;
			
			for(i = 7; i>=0; i--)
			{
				delay_1us();
				PO_1761_SCL(1);
				delay_1us();
				if(PI_1761_SDA())
					*data = (*data << 1) | 1;
				else
					*data = (*data << 1) ;
				PO_1761_SCL(0);
				delay_1us();
			}
//ack of data
			ADAU1761SDA_DIR_OUT;
			PO_1761_SDA(0);
				
			delay_1us();
			PO_1761_SCL(1);
			delay_1us();

			PO_1761_SCL(0);
			delay_1us();
		}

	
	STOP:

		ADAU1761SDA_DIR_OUT;
		PO_1761_SDA(0);
		delay_1us();
		PO_1761_SCL(1);
		delay_1us();
		PO_1761_SDA(1);
		delay_1us();

		return(error);

}
/************************************************************/
#if 1
//QHTF
T_ERROR_OP OperationADAU1761(void)
{
	uint8_t error_id=0;


	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, R0_SAMPLE_RATE_SETTING_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DSP_RUN_REGISTER_IC_1_ADDR, REG_DSP_RUN_REGISTER_IC_1_BYTE, R1_DSP_RUN_REGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_CLKCTRLREGISTER_IC_1_ADDR, REG_CLKCTRLREGISTER_IC_1_BYTE, R2_CLKCTRLREGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_PLLCRLREGISTER_IC_1_ADDR, REG_PLLCRLREGISTER_IC_1_BYTE, R3_PLLCRLREGISTER_IC_1_Default );
	//SIGMA_WRITE_DELAY( DEVICE_ADDR_IC_1, R4_DELAY_IC_1_SIZE, R4_DELAY_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SERIAL_PORT_CONTROL_0_IC_1_ADDR , R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_SIZE, R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_ALC_CONTROL_0_IC_1_ADDR , R6_ALC_CONTROL_REGISTERS_IC_1_SIZE, R6_ALC_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_MICCTRLREGISTER_IC_1_ADDR, REG_MICCTRLREGISTER_IC_1_BYTE, R7_MICCTRLREGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_RECORD_PWR_MANAGEMENT_IC_1_ADDR , R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE, R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_ADC_CONTROL_0_IC_1_ADDR , R9_ADC_CONTROL_REGISTERS_IC_1_SIZE, R9_ADC_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_PLAYBACK_MIXER_LEFT_CONTROL_0_IC_1_ADDR , R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE, R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_CONVERTER_CTRL_0_IC_1_ADDR , R11_CONVERTER_CONTROL_REGISTERS_IC_1_SIZE, R11_CONVERTER_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DAC_CONTROL_0_IC_1_ADDR , R12_DAC_CONTROL_REGISTERS_IC_1_SIZE, R12_DAC_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SERIAL_PORT_PAD_CONTROL_0_IC_1_ADDR , R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE, R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_COMM_PORT_PAD_CTRL_0_IC_1_ADDR , R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE, R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_JACKREGISTER_IC_1_ADDR, REG_JACKREGISTER_IC_1_BYTE, R15_JACKREGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DSP_ENABLE_REGISTER_IC_1_ADDR, REG_DSP_ENABLE_REGISTER_IC_1_BYTE, R16_DSP_ENABLE_REGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_CRC_IDEAL_1_IC_1_ADDR , R17_CRC_REGISTERS_IC_1_SIZE, R17_CRC_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_GPIO_0_CONTROL_IC_1_ADDR , R18_GPIO_REGISTERS_IC_1_SIZE, R18_GPIO_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_NON_MODULO_RAM_1_IC_1_ADDR , R19_NON_MODULO_REGISTERS_IC_1_SIZE, R19_NON_MODULO_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_WATCHDOG_ENABLE_IC_1_ADDR , R20_WATCHDOG_REGISTERS_IC_1_SIZE, R20_WATCHDOG_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, R21_SAMPLE_RATE_SETTING_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_ROUTING_MATRIX_INPUTS_IC_1_ADDR, REG_ROUTING_MATRIX_INPUTS_IC_1_BYTE, R22_ROUTING_MATRIX_INPUTS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_ROUTING_MATRIX_OUTPUTS_IC_1_ADDR, REG_ROUTING_MATRIX_OUTPUTS_IC_1_BYTE, R23_ROUTING_MATRIX_OUTPUTS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_ADDR, REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_BYTE, R24_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DSP_SLEW_MODES_IC_1_ADDR, REG_DSP_SLEW_MODES_IC_1_BYTE, R25_DSP_SLEW_MODES_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_BYTE, R26_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_CLOCK_ENABLE_REG_0_IC_1_ADDR , R27_CLOCK_ENABLE_REGISTERS_IC_1_SIZE, R27_CLOCK_ENABLE_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PROGRAM_ADDR_IC_1, PROGRAM_SIZE_IC_1, Program_Data_IC_1 );
	
	//error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_1, PARAM_SIZE_IC_1, Param_Data_IC_1 );
	switch(mem_mode)
		{
			case MODE_A:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_BP_IC_1, PARAM_SIZE_BP_IC_1, Param_Data_BP_IC_1);
				break;
			case MODE_B:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_1_music, PARAM_SIZE_IC_1_music, Param_Data_IC_1_music);
				break;
			case MODE_C:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_1_movie, PARAM_SIZE_IC_1_movie, Param_Data_IC_1_movie);
				break;
			default:
				break;
		}
	
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, R30_SAMPLE_RATE_SETTING_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DSP_RUN_REGISTER_IC_1_ADDR, REG_DSP_RUN_REGISTER_IC_1_BYTE, R31_DSP_RUN_REGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DEJITTER_REGISTER_CONTROL_IC_1_ADDR, REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE, R32_DEJITTER_REGISTER_CONTROL_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DEJITTER_REGISTER_CONTROL_IC_1_ADDR, REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE, R33_DEJITTER_REGISTER_CONTROL_IC_1_Default );

	return OK;
}
#endif
#if 0
T_ERROR_OP OperationADAU1761(void)
{
	uint8_t error_id=0;

	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, R0_SAMPLE_RATE_SETTING_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DSP_RUN_REGISTER_IC_1_ADDR, REG_DSP_RUN_REGISTER_IC_1_BYTE, R1_DSP_RUN_REGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_CLKCTRLREGISTER_IC_1_ADDR, REG_CLKCTRLREGISTER_IC_1_BYTE, R2_CLKCTRLREGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_PLLCRLREGISTER_IC_1_ADDR, REG_PLLCRLREGISTER_IC_1_BYTE, R3_PLLCRLREGISTER_IC_1_Default );
	//SIGMA_WRITE_DELAY( DEVICE_ADDR_IC_1, R4_DELAY_IC_1_SIZE, R4_DELAY_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SERIAL_PORT_CONTROL_0_IC_1_ADDR , R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_SIZE, R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_ALC_CONTROL_0_IC_1_ADDR , R6_ALC_CONTROL_REGISTERS_IC_1_SIZE, R6_ALC_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_MICCTRLREGISTER_IC_1_ADDR, REG_MICCTRLREGISTER_IC_1_BYTE, R7_MICCTRLREGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_RECORD_PWR_MANAGEMENT_IC_1_ADDR , R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE, R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_ADC_CONTROL_0_IC_1_ADDR , R9_ADC_CONTROL_REGISTERS_IC_1_SIZE, R9_ADC_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_PLAYBACK_MIXER_LEFT_CONTROL_0_IC_1_ADDR , R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE, R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_CONVERTER_CTRL_0_IC_1_ADDR , R11_CONVERTER_CONTROL_REGISTERS_IC_1_SIZE, R11_CONVERTER_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DAC_CONTROL_0_IC_1_ADDR , R12_DAC_CONTROL_REGISTERS_IC_1_SIZE, R12_DAC_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SERIAL_PORT_PAD_CONTROL_0_IC_1_ADDR , R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE, R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_COMM_PORT_PAD_CTRL_0_IC_1_ADDR , R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE, R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_JACKREGISTER_IC_1_ADDR, REG_JACKREGISTER_IC_1_BYTE, R15_JACKREGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DSP_ENABLE_REGISTER_IC_1_ADDR, REG_DSP_ENABLE_REGISTER_IC_1_BYTE, R16_DSP_ENABLE_REGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_CRC_IDEAL_1_IC_1_ADDR , R17_CRC_REGISTERS_IC_1_SIZE, R17_CRC_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_GPIO_0_CONTROL_IC_1_ADDR , R18_GPIO_REGISTERS_IC_1_SIZE, R18_GPIO_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_NON_MODULO_RAM_1_IC_1_ADDR , R19_NON_MODULO_REGISTERS_IC_1_SIZE, R19_NON_MODULO_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_WATCHDOG_ENABLE_IC_1_ADDR , R20_WATCHDOG_REGISTERS_IC_1_SIZE, R20_WATCHDOG_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, R21_SAMPLE_RATE_SETTING_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_ROUTING_MATRIX_INPUTS_IC_1_ADDR, REG_ROUTING_MATRIX_INPUTS_IC_1_BYTE, R22_ROUTING_MATRIX_INPUTS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_ROUTING_MATRIX_OUTPUTS_IC_1_ADDR, REG_ROUTING_MATRIX_OUTPUTS_IC_1_BYTE, R23_ROUTING_MATRIX_OUTPUTS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_ADDR, REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_BYTE, R24_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DSP_SLEW_MODES_IC_1_ADDR, REG_DSP_SLEW_MODES_IC_1_BYTE, R25_DSP_SLEW_MODES_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_BYTE, R26_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_CLOCK_ENABLE_REG_0_IC_1_ADDR , R27_CLOCK_ENABLE_REGISTERS_IC_1_SIZE, R27_CLOCK_ENABLE_REGISTERS_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PROGRAM_ADDR_IC_1, PROGRAM_SIZE_IC_1, Program_Data_IC_1 );
	//error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_1, PARAM_SIZE_IC_1, Param_Data_IC_1 );
	switch(mem_mode)
		{
			case MODE_A:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_TonyBM_1, PARAM_SIZE_IC_TonyBM_1, Param_Data_IC_TonyBM_1);
				break;
			case MODE_B:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_Thickness_1, PARAM_SIZE_IC_Thickness_1, Param_Data_IC_Thickness_1);
				break;
			case MODE_C:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_BigMovies_soundbar_1, PARAM_SIZE_IC_BigMovies_soundbar_1, Param_Data_IC_BigMovies_soundbar_1);
				break;
			case MODE_D:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_Magnetic_1, PARAM_SIZE_IC_Magnetic_1, Param_Data_IC_Magnetic_1);
				break;
			case MODE_E:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_Warm_1, PARAM_SIZE_IC_Warm_1, Param_Data_IC_Warm_1);
				break;
			case MODE_F:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_Joey_Music_1, PARAM_SIZE_IC_Joey_Music_1, Param_Data_IC_Joey_Music_1);
				break;
			case MODE_G:
				error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_Stereo_1, PARAM_SIZE_IC_Stereo_1, Param_Data_IC_Stereo_1);
				break;
			default:
				break;
		}
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, R30_SAMPLE_RATE_SETTING_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DSP_RUN_REGISTER_IC_1_ADDR, REG_DSP_RUN_REGISTER_IC_1_BYTE, R31_DSP_RUN_REGISTER_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DEJITTER_REGISTER_CONTROL_IC_1_ADDR, REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE, R32_DEJITTER_REGISTER_CONTROL_IC_1_Default );
	error_id=OperationWriteADI1761( DEVICE_ADDR_IC_1, REG_DEJITTER_REGISTER_CONTROL_IC_1_ADDR, REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE, R33_DEJITTER_REGISTER_CONTROL_IC_1_Default );

	return OK;
}
#endif

