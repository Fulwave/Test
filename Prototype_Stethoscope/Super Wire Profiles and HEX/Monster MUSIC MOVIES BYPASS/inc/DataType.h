#ifndef	__DATATYPE_H
#define	__DATATYPE_H


//---------------------------------------------------------


//---------------------------------------------------------


//---------------------------------------------------------
struct flash_dat_tab_t{
	uint32_t*	addr;
	uint32_t	max;
	uint32_t	recommand;
};

struct	oper_mess_type{
	uint8_t	type;
	uint8_t	remote;
	uint8_t	key;
	int8_t	para;
	void (*proc_entry)(int8_t para);
};
//---------------------------------------
//110p
typedef enum OPERA_MODE {
    READ = 1,
    WRITE = 2
} T_OPERA_MODE;

typedef enum ERROR_OP {
    OK = 1,
    I2C_ERROR = 2,
    LOOP_EXP_ERROR = 3,
    TERROR = 4
} T_ERROR_OP;
//---------------------------------------

typedef union
{
	struct
		{
			union
				{
				struct
					{
						uint8_t	data_flash_used:1,
								bit1:1,
								bit2:1,
								bit3:1,
								bit4:1,
								bit5:1,
								bit6:1,
								bit7:1;
					};
				uint8_t system_flag_u8;
				}system_flag;
			union
				{
				struct
					{
						uint8_t	mode:1,
								bit1:1,
								bit2:1,
								bit3:1,
								bit4:1,
								bit5:1,
								bit6:1,
								bit7:1;
					};
				uint8_t key_flag_u8;
				}key_flag;
			union
				{
				struct
					{
						uint8_t	a:1,
								b:1,
								c:1,
								d:1,
								e:1,
								f:1,
								bit6:1,
								bit7:1;
					};
				uint8_t led_flag_u8;
				}led_flag;
			union
				{
				struct
					{
						uint8_t	key:1,
								key_rep:1,
								led:1,
								bit3:1,
								bit4:1,
								bit5:1,
								bit6:1,
								bit7:1;
					};
				uint8_t function_flag_u8;
				}function_flag;
		};
	uint32_t flag_u32;
}Type_Flags;

#endif

