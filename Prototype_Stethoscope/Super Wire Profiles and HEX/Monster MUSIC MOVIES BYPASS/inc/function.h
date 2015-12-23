#ifndef __function_H__
#define __function_H__

#define	DATA_FLASH_SIZE			(4096)													// 4K DATA FLASH
#define	DATA_FLASH_PAGE_SIZE		(512)
#define	DATA_FLASH_ADDR_START	(DrvFMC_ReadDataFlashBaseAddr())
#define	DATA_FLASH_ADDR_END		(DrvFMC_ReadDataFlashBaseAddr()+DATA_FLASH_SIZE-1)
#define	DATA_FLASH_ADDR_OFFSET	(sizeof(uint32_t))
#define	DATA_FLASH_ADDR_USED	(0x40465A44)												// This is just a joke!^_^
#define	DATA_SIZE					((sizeof(flash_dat_tab))/(sizeof(struct flash_dat_tab_t)))
#define	DATA_QUANTITY_MAX		((uint32_t)((DATA_FLASH_SIZE-(2*DATA_FLASH_ADDR_OFFSET))/(DATA_SIZE*DATA_FLASH_ADDR_OFFSET)))


#define	KEY_PANAL_NULL	(0)
#define	KEY_PANAL_MODE	(1)
	#define		MODE_INITIAL	(0x20)
	#define		MODE_LOOP		(0x10)
	#define		MODE_A			(0)
	#define		MODE_B			(1)
	#define		MODE_C			(2)
	#define		MODE_D			(3)
	#define		MODE_E			(4)
	#define		MODE_F			(5)
	#define		MODE_G			(6)

extern	uint32_t	mem_mode;
extern	uint8_t	write_flash_delay_timer;

void OperationCycle(void);
void Initial_Status(void);

void delays(uint32_t n);
void delay_1us(void);

T_ERROR_OP OperationADAU1761(void);
#endif

