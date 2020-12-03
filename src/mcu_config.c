
#include <mcu/arch.h>
#include <mcu/mcu.h>

#include <mcu/arch/stm32/stm32_config.h>

#include "config.h"

//--------------------------------------------MCU
// Configuration-------------------------------------------------

static char stm32_usb_rx_buffer[SOS_BOARD_USB_RX_BUFFER_SIZE] MCU_SYS_MEM;

const stm32_config_t stm32_config = {
    .flash_program_millivolts = 3300,
    .usb_max_packet_zero = 64,
    .usb_rx_buffer = stm32_usb_rx_buffer,
    .usb_rx_buffer_size = SOS_BOARD_USB_RX_BUFFER_SIZE,
    .debug_uart_config = {.port = 2,
                          .attr = {.pin_assignment = {.rx = {3, 9},
                                                      .tx = {3, 8},
                                                      .cts = {0xff, 0xff},
                                                      .rts = {0xff, 0xff}},
                                   .freq = 115200,
                                   .o_flags = UART_FLAG_SET_LINE_CODING_DEFAULT,
                                   .width = 8}},
};
#if 0
MCU_DECLARE_SECRET_KEY_32(secret_key)

const mcu_board_config_t mcu_board_config = {
    .core_cpu_freq = SOS_BOARD_SYSTEM_CLOCK,
    .usb_max_packet_zero = 64,
    .debug_uart_port = 2,
    .debug_uart_attr = {.pin_assignment = {.rx = {3, 9},
                                           .tx = {3, 8},
                                           .cts = {0xff, 0xff},
                                           .rts = {0xff, 0xff}},
                        .freq = 115200,
                        .o_flags = UART_FLAG_SET_LINE_CODING_DEFAULT,
                        .width = 8},
    .secret_key_address = secret_key,
    .secret_key_size = 32,
    .o_flags = MCU_BOARD_CONFIG_FLAG_LED_ACTIVE_HIGH,
    .event_handler = SOS_BOARD_EVENT_HANDLER,
    .led = {1, 7},
    .arch_config = &stm32_config,
    .o_mcu_debug = SOS_BOARD_DEBUG_FLAGS};
#endif
