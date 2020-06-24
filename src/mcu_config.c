
#include <mcu/arch.h>
#include <mcu/mcu.h>

#include "config.h"

//--------------------------------------------MCU Configuration-------------------------------------------------

static char stm32_usb_rx_buffer[SOS_BOARD_USB_RX_BUFFER_SIZE] MCU_SYS_MEM;

const stm32_config_t stm32_config = {
	.flash_program_millivolts = 3300,
	.usb_rx_buffer = stm32_usb_rx_buffer,
	.usb_rx_buffer_size = SOS_BOARD_USB_RX_BUFFER_SIZE
};

STM32_NUCLEO144_DECLARE_MCU_BOARD_CONFIG(SOS_BOARD_SYSTEM_CLOCK, SOS_BOARD_EVENT_HANDLER, &stm32_config, SOS_BOARD_DEBUG_FLAGS);

