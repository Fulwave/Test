#include "NUC122.h"
#include "DrvGPIO.h"
#include "DrvSYS.h"
#include "DrvFMC.h"
#include "DataType.h"
#include "function.h"
#include "Io_define.h"

uint32_t	mem_mode;
uint8_t	write_flash_delay_timer;

volatile	uint8_t		b_oper_rep_timer;
static	uint8_t		oper_chk_id;
static	uint8_t		key_code;
static	uint8_t		b_old_key_code;
static	Type_Flags	flags;

const struct flash_dat_tab_t flash_dat_tab[]={
		{&mem_mode,2,0},//{&mem_mode,6,0},//{&mem_mode,5,0},
		{0,205,0},				// end
};
//-----------------------------------------------------------
static uint32_t mem_val_max(uint32_t* p_var)
{
	uint8_t i;
	uint32_t var_max;

	var_max = 0;
	i = 0;
	do{
		if (flash_dat_tab[i].addr==p_var)
			{
				var_max = flash_dat_tab[i].max;
				break;
			}
	}while(flash_dat_tab[++i].addr!=0);
	
	return var_max;
}

static uint8_t smart_var_modi_r_rou(uint32_t *var,int8_t offset)
{
	(*var) += offset;
	if ((*var) > mem_val_max(var))
		{
			if ((*var) > 0x0f0)
				{
					(*var) = mem_val_max(var);
				}
			else
				{
					(*var) = 0;
				}
			return 0;
		}
	else
		{
			return 1;
		}
}

static uint8_t smart_var_modi_rou(uint32_t *var,int8_t offset)
{
	(*var) += offset;
	if ((*var) > mem_val_max(var))
		{
			if ((*var) > 0xf0)
				{
					(*var) = 0;
				}
			else
				{
					(*var) = mem_val_max(var);
				}
			return 0;
		}
	else
		{
			return 1;
		}
}
//-----------------------------------------------------------
//---------------------二分查找法----------------------
static uint32_t located_address;
static uint8_t located_address_success_f;
T_ERROR_OP static DataFlashAddressLocated(void)
{
	uint32_t temp_data,temp_addr,temp_addre,temp_data_length,temp_loacted_addr;
	int32_t error_id;

	temp_data_length=DATA_QUANTITY_MAX;
	temp_addr=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET;
	temp_addre=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET+(temp_data_length-1)*DATA_SIZE*DATA_FLASH_ADDR_OFFSET;
	temp_loacted_addr=temp_addr;

	do
		{
			error_id=DrvFMC_Read((temp_addr+((uint32_t)(temp_data_length/2))*DATA_SIZE*DATA_FLASH_ADDR_OFFSET),&temp_data);
			if(error_id==E_DRVFMC_ERR_ISP_FAIL)
				return TERROR;
			if(temp_data==DATA_SIZE)
				{
					temp_loacted_addr=temp_addr+((uint32_t)(temp_data_length/2))*DATA_SIZE*DATA_FLASH_ADDR_OFFSET;
					temp_addr=temp_addr+((uint32_t)(temp_data_length/2))*DATA_SIZE*DATA_FLASH_ADDR_OFFSET+DATA_SIZE*DATA_FLASH_ADDR_OFFSET;
				}
			else
				{
					temp_addre=temp_addr+((uint32_t)(temp_data_length/2))*DATA_SIZE*DATA_FLASH_ADDR_OFFSET-DATA_SIZE*DATA_FLASH_ADDR_OFFSET;
				}
			if(temp_addr==temp_addre)
				break;
			if(temp_addr>temp_addre)
				return TERROR;
			temp_data_length=(uint32_t)(temp_data_length/2);
		}
	while(1);

	error_id=DrvFMC_Read(temp_addr,&temp_data);
			if(error_id==E_DRVFMC_ERR_ISP_FAIL)
				return TERROR;
	if(temp_data==DATA_SIZE)
		temp_loacted_addr=temp_addr;

	located_address=temp_loacted_addr;
	return OK;
}

T_ERROR_OP static ReadDateLocated(void)
{
	uint32_t i;
	uint32_t temp_data,temp_addr,temp_addrn;
	int32_t error_id;

	temp_addr=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET;
	temp_addrn=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET;

	error_id=DrvFMC_Read(DATA_FLASH_ADDR_START+DATA_FLASH_ADDR_OFFSET,&temp_data);
	if(error_id==E_DRVFMC_ERR_ISP_FAIL)
		return TERROR;

	if((temp_data!=DATA_FLASH_ADDR_USED) && (temp_data==0xffffffff))
		{
			flags.system_flag.data_flash_used=0;
			i=0;
			do
				{
					*flash_dat_tab[i].addr=flash_dat_tab[i].recommand;
				}
			while(flash_dat_tab[++i].addr);

			return OK;
		}
	else
		{
			flags.system_flag.data_flash_used=1;
			error_id=DataFlashAddressLocated();
			if(error_id!=OK)	//顺序查找法
				{
					do
						{
							error_id=DrvFMC_Read(temp_addrn,&temp_data);
							if(error_id==E_DRVFMC_ERR_ISP_FAIL)
								return TERROR;
							if(temp_data==DATA_SIZE)
								{
									temp_addr=temp_addrn;
									temp_addrn=temp_addrn+DATA_SIZE*DATA_FLASH_ADDR_OFFSET;
								}
							else
								{
									break;
								}
						}
					while(temp_addrn<=(DATA_FLASH_ADDR_END-(DATA_SIZE*DATA_FLASH_ADDR_OFFSET)));

					i=0;
					do
						{
							error_id=DrvFMC_Read(temp_addr+(i+1)*DATA_FLASH_ADDR_OFFSET,flash_dat_tab[i].addr);
							if(error_id==E_DRVFMC_ERR_ISP_FAIL)
								return TERROR;
							if(*(flash_dat_tab[i].addr)>flash_dat_tab[i].max)
								*(flash_dat_tab[i].addr)=flash_dat_tab[i].recommand;
						}
					while(flash_dat_tab[++i].addr);
				}
			else				//二分查找法
				{
					located_address_success_f=1;
					i=0;
					do
						{
							error_id=DrvFMC_Read(located_address+(i+1)*DATA_FLASH_ADDR_OFFSET,flash_dat_tab[i].addr);
							if(error_id==E_DRVFMC_ERR_ISP_FAIL)
								return TERROR;
							if(*(flash_dat_tab[i].addr)>flash_dat_tab[i].max)
								*(flash_dat_tab[i].addr)=flash_dat_tab[i].recommand;
						}
					while(flash_dat_tab[++i].addr);
				}

			return OK;
		}
}

T_ERROR_OP static WriteDateLocated(void)
{
	uint32_t i,temp_data;
	static uint32_t temp_addr=0;
	int32_t error_id;
	uint8_t flash_full_f=1;

	if(!(flags.system_flag.data_flash_used))
		{
			error_id=DrvFMC_Write(DATA_FLASH_ADDR_START+DATA_FLASH_ADDR_OFFSET,DATA_FLASH_ADDR_USED);
			if(error_id==E_DRVFMC_ERR_ISP_FAIL)
				return TERROR;
			flags.system_flag.data_flash_used=1;
		}

	if(located_address_success_f)	//二分查找法
		{
			if(located_address==DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET+(DATA_QUANTITY_MAX-1)*DATA_SIZE*DATA_FLASH_ADDR_OFFSET)
				{
					for(i=0;i<=7;i++)
						{
							error_id=DrvFMC_Erase(DATA_FLASH_ADDR_START+(i*DATA_FLASH_PAGE_SIZE));
							if(error_id==E_DRVFMC_ERR_ISP_FAIL)
								return TERROR;
						}

					error_id=DrvFMC_Write(DATA_FLASH_ADDR_START+DATA_FLASH_ADDR_OFFSET,DATA_FLASH_ADDR_USED);
					if(error_id==E_DRVFMC_ERR_ISP_FAIL)
						return TERROR;

					located_address=DATA_FLASH_ADDR_START;
				}
			error_id=DrvFMC_Write(located_address+DATA_SIZE*DATA_FLASH_ADDR_OFFSET,DATA_SIZE);
			if(error_id==E_DRVFMC_ERR_ISP_FAIL)
				return TERROR;
			i=0;
			do
				{
					error_id=DrvFMC_Write(located_address+DATA_SIZE*DATA_FLASH_ADDR_OFFSET+(i+1)*DATA_FLASH_ADDR_OFFSET,*(flash_dat_tab[i].addr));
					if(error_id==E_DRVFMC_ERR_ISP_FAIL)
						return TERROR;
				}
			while(flash_dat_tab[++i].addr);
			located_address=located_address+DATA_SIZE*DATA_FLASH_ADDR_OFFSET;
		}
	else								//顺序查找法
		{
			if(temp_addr==0)
				temp_addr=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET;

			do
				{
					error_id=DrvFMC_Read(temp_addr,&temp_data);
					if(error_id==E_DRVFMC_ERR_ISP_FAIL)
						return TERROR;

					if((temp_data!=DATA_SIZE) && (temp_data==0xffffffff))
						{
							flash_full_f=0;
							break;
						}
					else
						{
							temp_addr=temp_addr+DATA_SIZE*DATA_FLASH_ADDR_OFFSET;
						}
				}
			while(temp_addr<=(DATA_FLASH_ADDR_END-(DATA_SIZE*DATA_FLASH_ADDR_OFFSET)));

			if(flash_full_f)
				{
					for(i=0;i<=7;i++)
						{
							DrvFMC_Erase(DATA_FLASH_ADDR_START+(i*DATA_FLASH_PAGE_SIZE));
						}

					error_id=DrvFMC_Write(DATA_FLASH_ADDR_START+DATA_FLASH_ADDR_OFFSET,DATA_FLASH_ADDR_USED);
					if(error_id==E_DRVFMC_ERR_ISP_FAIL)
						return TERROR;

					temp_addr=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET;
				}

			error_id=DrvFMC_Write(temp_addr,DATA_SIZE);
			if(error_id==E_DRVFMC_ERR_ISP_FAIL)
				return TERROR;
			i=0;
			do
				{
					error_id=DrvFMC_Write(temp_addr+(i+1)*DATA_FLASH_ADDR_OFFSET,*(flash_dat_tab[i].addr));
					if(error_id==E_DRVFMC_ERR_ISP_FAIL)
						return TERROR;
				}
			while(flash_dat_tab[++i].addr);
		}
	return OK;
}
//---------------------二分查找法----------------------
//-----------------------------------------------------------
T_ERROR_OP static ReadDate(void)
{
	uint32_t i;
	uint32_t temp_data,temp_addr,temp_addrn;
	int32_t error_id;

	temp_addr=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET;
	temp_addrn=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET;

	error_id=DrvFMC_Read(DATA_FLASH_ADDR_START+DATA_FLASH_ADDR_OFFSET,&temp_data);
	if(error_id==E_DRVFMC_ERR_ISP_FAIL)
		return TERROR;

	if((temp_data!=DATA_FLASH_ADDR_USED) && (temp_data==0xffffffff))
		{
			flags.system_flag.data_flash_used=0;
			i=0;
			do
				{
					*flash_dat_tab[i].addr=flash_dat_tab[i].recommand;
				}
			while(flash_dat_tab[++i].addr);

			return OK;
		}
	else
		{
			flags.system_flag.data_flash_used=1;

			do
				{
					error_id=DrvFMC_Read(temp_addrn,&temp_data);
					if(error_id==E_DRVFMC_ERR_ISP_FAIL)
						return TERROR;
					if(temp_data==DATA_SIZE)
						{
							temp_addr=temp_addrn;
							temp_addrn=temp_addrn+DATA_SIZE*DATA_FLASH_ADDR_OFFSET;
						}
					else
						{
							break;
						}
				}
			while(temp_addrn<=(DATA_FLASH_ADDR_END-(DATA_SIZE*DATA_FLASH_ADDR_OFFSET)));

			i=0;
			do
				{
					error_id=DrvFMC_Read(temp_addr+(i+1)*DATA_FLASH_ADDR_OFFSET,flash_dat_tab[i].addr);
					if(error_id==E_DRVFMC_ERR_ISP_FAIL)
						return TERROR;
					if(*(flash_dat_tab[i].addr)>flash_dat_tab[i].max)
						*(flash_dat_tab[i].addr)=flash_dat_tab[i].recommand;
				}
			while(flash_dat_tab[++i].addr);

			return OK;
		}
}

T_ERROR_OP static WriteDate(void)
{
	uint32_t i,temp_data;
	static uint32_t temp_addr=0;
	int32_t error_id;
	uint8_t flash_full_f=1;

	if(!(flags.system_flag.data_flash_used))
		{
			error_id=DrvFMC_Write(DATA_FLASH_ADDR_START+DATA_FLASH_ADDR_OFFSET,DATA_FLASH_ADDR_USED);
			if(error_id==E_DRVFMC_ERR_ISP_FAIL)
				return TERROR;
			flags.system_flag.data_flash_used=1;
		}

	if(temp_addr==0)
		temp_addr=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET;

	do
		{
			error_id=DrvFMC_Read(temp_addr,&temp_data);
			if(error_id==E_DRVFMC_ERR_ISP_FAIL)
				return TERROR;

			if((temp_data!=DATA_SIZE) && (temp_data==0xffffffff))
				{
					flash_full_f=0;
					break;
				}
			else
				{
					temp_addr=temp_addr+DATA_SIZE*DATA_FLASH_ADDR_OFFSET;
				}
		}
	while(temp_addr<=(DATA_FLASH_ADDR_END-(DATA_SIZE*DATA_FLASH_ADDR_OFFSET)));

	if(flash_full_f)
		{
			for(i=0;i<=7;i++)
				{
					DrvFMC_Erase(DATA_FLASH_ADDR_START+(i*DATA_FLASH_PAGE_SIZE));
				}

			error_id=DrvFMC_Write(DATA_FLASH_ADDR_START+DATA_FLASH_ADDR_OFFSET,DATA_FLASH_ADDR_USED);
			if(error_id==E_DRVFMC_ERR_ISP_FAIL)
				return TERROR;

			temp_addr=DATA_FLASH_ADDR_START+2*DATA_FLASH_ADDR_OFFSET;
		}

	error_id=DrvFMC_Write(temp_addr,DATA_SIZE);
	if(error_id==E_DRVFMC_ERR_ISP_FAIL)
		return TERROR;
	i=0;
	do
		{
			error_id=DrvFMC_Write(temp_addr+(i+1)*DATA_FLASH_ADDR_OFFSET,*(flash_dat_tab[i].addr));
			if(error_id==E_DRVFMC_ERR_ISP_FAIL)
				return TERROR;
		}
	while(flash_dat_tab[++i].addr);
	return OK;
}
//-----------------------------------------------------------
//QHTF
#if 1
static void Led_Operation(void)
{
	flags.led_flag.led_flag_u8=0xff;
	switch(mem_mode)
		{
			case MODE_A:
				//flags.led_flag.a=0;
				break;
			case MODE_B:
				flags.led_flag.b=0;
				break;
			case MODE_C:
				flags.led_flag.c=0;
				break;
			default:
				break;
		}

	PO_LEDA(flags.led_flag.a);
	PO_LEDB(flags.led_flag.b);
	PO_LEDC(flags.led_flag.c);
}
#endif
#if 0	//normal
static void Led_Operation(void)
{
	flags.led_flag.led_flag_u8=0;
	switch(mem_mode)
		{
			case MODE_A:
				flags.led_flag.a=1;
				break;
			case MODE_B:
				flags.led_flag.b=1;
				break;
			case MODE_C:
				flags.led_flag.c=1;
				break;
			case MODE_D:
				flags.led_flag.d=1;
				break;
			case MODE_E:
				flags.led_flag.e=1;
				break;
			case MODE_F:
				flags.led_flag.f=1;
				break;
			default:
				break;
		}
	
	PO_LEDA(flags.led_flag.a);
	PO_LEDB(flags.led_flag.b);
	PO_LEDC(flags.led_flag.c);
	PO_LEDD(flags.led_flag.d);
	PO_LEDE(flags.led_flag.e);
	PO_LEDF(flags.led_flag.f);
}
#endif
//-----------------------------------------------------------
static void Mode_Operation(int8_t para)
{
	uint8_t temp_f=0;

	if(para!=mem_mode)
	    temp_f=1;

	if(para==MODE_INITIAL)
		{
			;
		}
	else if(para==MODE_LOOP)								//全循环
		{
		    	smart_var_modi_r_rou(&mem_mode,1);
		}
	else
		{
			mem_mode=para;
		}

	if(temp_f)
		{			
			flags.function_flag.led=1;
			write_flash_delay_timer=4;
			OperationADAU1761();
		}
}
//-----------------------------------------------------------
void Initial_Status(void)
{
	//ReadDate();
	//DataFlashAddressLocated();
	ReadDateLocated();
	delays(1000);
	Mode_Operation(MODE_INITIAL);
}
//-----------------------------------------------------------
const struct oper_mess_type  oper_tab[]={
//		remote  key
	{	1,0xff,	KEY_PANAL_MODE,		MODE_LOOP,		Mode_Operation	},
	{	0,0xff,	0x0ff,				0,				Mode_Operation	}  // end
};
//-----------------------------------------------------------
static void Operation_Exe(void)
{
	uint8_t para;

	oper_chk_id &= 0x7f;
	if ( oper_chk_id < 0x070 )
		{
			para = oper_tab[oper_chk_id].para;
			b_oper_rep_timer = 0;    
			oper_tab[oper_chk_id].proc_entry(para);
		}
}

static void Check_Operation(void)
{
	static uint8_t f_oper_dly;
	uint8_t f_oper_found = 0;
	uint8_t i = 0;

	oper_chk_id = 0x0ff;

	if (flags.function_flag.key)
		{
			do{
				if (key_code == oper_tab[i].key)
					{
						f_oper_found = 1;
						break;
					}
			}while( (oper_tab[++i].type ) );
		
			flags.function_flag.key= 0;								//reset

			if (f_oper_found)
				{
					switch ( oper_tab[i].type)
						{
							case	1:						//one times oper
							case	4:						//power key
								if ( flags.function_flag.key_rep == 0 )
									{
										oper_chk_id = i;
									}
								break;
							case	2:						//rep times oper  mode 1
								if (flags.function_flag.key_rep == 0 )
									{
										f_oper_dly = 0;
										oper_chk_id = i;
									}
								else
									{
										if ( f_oper_dly && (b_oper_rep_timer >= 10) )
											{
												oper_chk_id = i;
											}
										else
											{
												if ( (f_oper_dly == 0) &&  (b_oper_rep_timer >= 50) )
													{
														f_oper_dly = 1;
													}
											}
									}
								break;
							case	3:						//rep times oper  mode 2
								if ( flags.function_flag.key_rep == 0 )
									{
										f_oper_dly = 0;
										oper_chk_id = i;
									}
								else
									{
										if ( f_oper_dly && (b_oper_rep_timer >= 10) )
											{
												oper_chk_id = i;
											}
										else
											{
												if ( (f_oper_dly == 0) &&  (b_oper_rep_timer >= 50) )
													{
														f_oper_dly = 1;
													}
											}
									}
								break;
						}
				}
		}
}
//-----------------------------------------------------------
static void Key_Read_Operation(void)
{
	static uint16_t	key_cunt;

	if(!PI_KEY_MODE())
		{
			if(key_cunt<10000)
				key_cunt++;
			if(key_cunt>=10000)
				flags.key_flag.mode=1;
		}
	else
		{
			key_cunt=0;
			flags.key_flag.key_flag_u8=0x00;
		}
}

static uint8_t Key_Convert_Operation(void)
{
	Key_Read_Operation();
	if(flags.key_flag.mode)
		{
			return KEY_PANAL_MODE;
		}
	else
		{
			return KEY_PANAL_NULL;
		}
}

static void Key_Operation(void)
{
	uint8_t key_tmp_panal;


	if (!flags.function_flag.key)
		{
			key_tmp_panal = Key_Convert_Operation();
			
        		if (key_tmp_panal)
				{
					if(b_old_key_code==key_tmp_panal)
						{
							;
						}
					else
						{
							flags.function_flag.key_rep= 0;
							flags.function_flag.key= 1;
							key_code=key_tmp_panal;
						}
					b_old_key_code = key_tmp_panal;
				}
			else
				b_old_key_code = 0xff;
    		}
}
//-----------------------------------------------------------
static void Status_Operation(void)
{
	if(flags.function_flag.led)
		{
			flags.function_flag.led=0;
			Led_Operation();
		}

	if(write_flash_delay_timer==1)
		{
			write_flash_delay_timer=0;
			//WriteDate();
			WriteDateLocated();
			//DataFlashAddressLocated();
		}
}
//-----------------------------------------------------------
void OperationCycle(void)
{
	Key_Operation();
	Check_Operation();
	Operation_Exe();
	Status_Operation();
}
//-----------------------------------------------------------
void delays(uint32_t n)
{
    while(n>0)n--;
}

void delay_1us(void)
{
	uint8_t i;
	i=5;
	while(i--);
}