
flash_page_count = 8;

MEMORY {
	flash : ORIGIN = 0x08000000, LENGTH = 512K
	ram : ORIGIN = 0x20000000, LENGTH = 112K
	sysmem : ORIGIN = 0x2001C000, LENGTH = 16K
	backup : ORIGIN = 0x40024000, LENGTH = 80

}

INCLUDE stm32f4xx.ld

