
#include <sos/arch.h>
#include <stm32_config.h>

#include "config.h"

//--------------------------------------------MCU
// Configuration-------------------------------------------------

static char stm32_usb_rx_buffer[CONFIG_USB_RX_BUFFER_SIZE] MCU_SYS_MEM;

#define CONFIG_RX_FIFO_WORDS 128
#define CONFIG_TX0_FIFO_WORDS 32
#define CONFIG_TX1_FIFO_WORDS 32
#define CONFIG_TX2_FIFO_WORDS 32
#define CONFIG_TX3_FIFO_WORDS 64
#define CONFIG_TX4_FIFO_WORDS 0
#define CONFIG_TX5_FIFO_WORDS 0

const stm32_config_t stm32_config = {
    .flash_program_millivolts = 3300,
    .usb =
        {
            .rx_buffer = stm32_usb_rx_buffer,
            .rx_buffer_size = CONFIG_USB_RX_BUFFER_SIZE,
            .rx_fifo_word_size =
                CONFIG_RX_FIFO_WORDS, // RX fifo for all endpoints
            .tx_fifo_word_size[0] = CONFIG_TX0_FIFO_WORDS, // TX endpoint 0
            .tx_fifo_word_size[1] = CONFIG_TX1_FIFO_WORDS, // TX endpoint 1
            .tx_fifo_word_size[2] = CONFIG_TX2_FIFO_WORDS, // TX endpoint 2
            .tx_fifo_word_size[3] = CONFIG_TX3_FIFO_WORDS, // TX endpoint 3
            .tx_fifo_word_size[4] = CONFIG_TX4_FIFO_WORDS, // TX endpoint 4
            .tx_fifo_word_size[5] = CONFIG_TX5_FIFO_WORDS  // TX endpoint 5
        }
};
#if 0
MCU_DECLARE_SECRET_KEY_32(secret_key)

const mcu_board_config_t mcu_board_config = {
    .core_cpu_freq = CONFIG_SYSTEM_CLOCK,
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
    .event_handler = CONFIG_EVENT_HANDLER,
    .led = {1, 7},
    .arch_config = &stm32_config,
    .o_mcu_debug = CONFIG_DEBUG_FLAGS};
#endif
