flash_page_count = 16;

MEMORY {
	 flash : ORIGIN = 0x08000000, LENGTH = 1536K
	 ram : ORIGIN = 0x20000000, LENGTH = 256K
	 sysmem : ORIGIN = 0x20040000, LENGTH = 64K
	 backup : ORIGIN = 0x40002850, LENGTH = 4K
}

INCLUDE stm32f4xx.ld
