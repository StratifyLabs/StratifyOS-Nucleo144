
#ifndef CONFIG_H_
#define CONFIG_H_

#include <sos/arch.h>

#include <device/appfs.h>
#include <device/ffifo.h>
#include <sos/dev/appfs.h>
#include <sos/dev/mem.h>
#include <sos/dev/pio.h>
#include <sos/fs/appfs.h>

#define CONFIG_DEBUG_FLAGS                                                  \
  (SOS_DEBUG_INFO | SOS_DEBUG_SYS | SOS_DEBUG_SCHEDULER | SOS_DEBUG_USER0 |    \
   SOS_DEBUG_APPFS | SOS_DEBUG_DEVICE | SOS_DEBUG_SOCKET | SOS_DEBUG_USER2)

#define CONFIG_USB_RX_BUFFER_SIZE 512
#define CONFIG_STDIO_BUFFER_SIZE 512
#define CONFIG_BOARD_FLAGS 0

//Total number of tasks (threads) for the entire system
#define CONFIG_TASK_TOTAL 10
#define CONFIG_EVENT_HANDLER board_event_handler
#define CONFIG_TRACE_EVENT board_trace_event

#define CONFIG_USB_DP_PIN mcu_pin(0, 11)
#define CONFIG_USB_DM_PIN mcu_pin(0, 12)

#if _IS_BOOT
#define VECTOR_TABLE_ADDRESS __BOOT_START_ADDRESS
#else
#define VECTOR_TABLE_ADDRESS __KERNEL_START_ADDRESS
#endif

//--------------------------------------------Symbols-------------------------------------------------

/* By defining Ignore switches, functions can be omitted from the kernel
 * This means any applications that use these functions will fail
 * to install on the BSP.
 * 
 * If you are building a custom board, ignoring symbols is a good
 * way to squeeze down the kernel to only what is necessary. However,
 * if you plan on allowing your users to install applications, they
 * might find it challenging when some functions are missing (the
 * applications will compile but fail to install).
 * 
 * See [sos/symbols/defines.h](https://github.com/StratifyLabs/StratifyOS/blob/master/include/sos/symbols/defines.h) 
 * for all available switches.
 * 
 */ 

#define SYMBOLS_IGNORE_DCOMPLEX 1
#define SYMBOLS_IGNORE_POSIX_TRACE 1 //Always ignore -- deprecated
#define SYMBOLS_IGNORE_SG 1 //Stratify Graphics -- ignore if board will not support displays

#if !defined INCLUDE_ETHERNET
#define SYMBOLS_IGNORE_SOCKET 1
#endif

//other ignore switches
#if 0
#define SYMBOLS_IGNORE_MATH_F 1
#define SYMBOLS_IGNORE_DOUBLE 1
#define SYMBOLS_IGNORE_STDIO_FILE 1
#define SYMBOLS_IGNORE_SIGNAL 1
#define SYMBOLS_IGNORE_PTHREAD_MUTEX 1
#define SYMBOLS_IGNORE_PTHREAD_COND 1
#define SYMBOLS_IGNORE_AIO 1
#define SYMBOLS_IGNORE_WCTYPE 1
#define SYMBOLS_IGNORE_STR 1
#define SYMBOLS_IGNORE_SEM 1
#define SYMBOLS_IGNORE_MQ 1
#endif

/* Uncomment to add ARM CMSIS DSP libraries to the BSP
 * 
 * See [ARM CMSIS Declaration](https://github.com/StratifyLabs/StratifyOS-CMSIS/blob/master/arm_dsp_api_declaration.h)
 * for more detailed link configuration switches.
 * 
 * 
 */
#define SOS_BOARD_ARM_DSP_API_Q7 0
#define SOS_BOARD_ARM_DSP_API_Q15 0
#define SOS_BOARD_ARM_DSP_API_Q31 0
#define SOS_BOARD_ARM_DSP_API_F32 0
#define SOS_BOARD_ARM_DSP_CONVERSION_API 0

//--------------------------------------------Hardware Pins-------------------------------------------------

#define SOS_BOARD_USART2_TX_PORT 3 //PD5
#define SOS_BOARD_USART2_TX_PIN 5
#define SOS_BOARD_USART2_RX_PORT 3 //PD6
#define SOS_BOARD_USART2_RX_PIN 6

#define SOS_BOARD_USART3_TX_PORT 3 //PD8
#define SOS_BOARD_USART3_TX_PIN 8
#define SOS_BOARD_USART3_RX_PORT 3 //PD9
#define SOS_BOARD_USART3_RX_PIN 9

#define SOS_BOARD_USART6_TX_PORT 6 //PG5
#define SOS_BOARD_USART6_TX_PIN 9
#define SOS_BOARD_USART6_RX_PORT 6 //PG14
#define SOS_BOARD_USART6_RX_PIN 14

#define SOS_BOARD_I2C1_SDA_PORT 1 //PB9
#define SOS_BOARD_I2C1_SDA_PIN 9
#define SOS_BOARD_I2C1_SCL_PORT 1 //PB8
#define SOS_BOARD_I2C1_SCL_PIN 8

#define SOS_BOARD_I2C2_SDA_PORT 5 //PF0
#define SOS_BOARD_I2C2_SDA_PIN 0
#define SOS_BOARD_I2C2_SCL_PORT 5 //PF1
#define SOS_BOARD_I2C2_SCL_PIN 1

#define SOS_BOARD_SPI1_MISO_PORT 0 //PA6
#define SOS_BOARD_SPI1_MISO_PIN 6
#define SOS_BOARD_SPI1_MOSI_PORT 0 //PA7
#define SOS_BOARD_SPI1_MOSI_PIN 7
#define SOS_BOARD_SPI1_SCK_PORT 0 //PA5
#define SOS_BOARD_SPI1_SCK_PIN 5
#define SOS_BOARD_SPI1_CS_PORT 0xff //Not used
#define SOS_BOARD_SPI1_CS_PIN 0xff

#define SOS_BOARD_SPI3_MISO_PORT 1 //PB4
#define SOS_BOARD_SPI3_MISO_PIN 4
#define SOS_BOARD_SPI3_MOSI_PORT 1 //PB5
#define SOS_BOARD_SPI3_MOSI_PIN 5
#define SOS_BOARD_SPI3_SCK_PORT 1  //PB3
#define SOS_BOARD_SPI3_SCK_PIN 3
#define SOS_BOARD_SPI3_CS_PORT 0xff //Not used
#define SOS_BOARD_SPI3_CS_PIN 0xff

#define SOS_BOARD_USART2_RX_DMA STM32_DMA1
#define SOS_BOARD_USART2_RX_DMA_STREAM 5
#define SOS_BOARD_USART2_RX_DMA_CHANNEL 4
#define SOS_BOARD_USART2_TX_DMA STM32_DMA1
#define SOS_BOARD_USART2_TX_DMA_STREAM 6
#define SOS_BOARD_USART2_TX_DMA_CHANNEL 4

#define SOS_BOARD_SPI1_RX_DMA STM32_DMA2
#define SOS_BOARD_SPI1_RX_DMA_STREAM 0
#define SOS_BOARD_SPI1_RX_DMA_CHANNEL 3
#define SOS_BOARD_SPI1_TX_DMA STM32_DMA2
#define SOS_BOARD_SPI1_TX_DMA_STREAM 3
#define SOS_BOARD_SPI1_TX_DMA_CHANNEL 3

#define SOS_BOARD_SPI3_RX_DMA STM32_DMA1
#define SOS_BOARD_SPI3_RX_DMA_STREAM 0
#define SOS_BOARD_SPI3_RX_DMA_CHANNEL 0
#define SOS_BOARD_SPI3_TX_DMA STM32_DMA1
#define SOS_BOARD_SPI3_TX_DMA_STREAM 5
#define SOS_BOARD_SPI3_TX_DMA_CHANNEL 0

#define SOS_BOARD_SD_DATA0_PORT 2 //PC8
#define SOS_BOARD_SD_DATA0_PIN 8
#define SOS_BOARD_SD_DATA1_PORT 2 //PC9
#define SOS_BOARD_SD_DATA1_PIN 9
#define SOS_BOARD_SD_DATA2_PORT 2 //PC10
#define SOS_BOARD_SD_DATA2_PIN 10
#define SOS_BOARD_SD_DATA3_PORT 2 //PC11
#define SOS_BOARD_SD_DATA3_PIN 11
#define SOS_BOARD_SD_CLK_PORT 2 //PC12
#define SOS_BOARD_SD_CLK_PIN 12
#define SOS_BOARD_SD_CMD_PORT 3 //PD2
#define SOS_BOARD_SD_CMD_PIN 2

void board_trace_event(void *event);

extern const ffifo_config_t board_trace_config;
extern ffifo_state_t board_trace_state;

void os_event_handler(int event, void *args); // optional event handler

// clock
void clock_initialize(
    int (*handle_match_channel0)(void *context, const mcu_event_t *data),
    int (*handle_match_channel1)(void *context, const mcu_event_t *data),
    int (*handle_overflow)(void *context, const mcu_event_t *data));
void clock_enable();
u32 clock_disable();
void clock_set_channel(const mcu_channel_t *channel);
void clock_get_channel(mcu_channel_t *channel);
u32 clock_microseconds();
u32 clock_nanoseconds();

// sleep
void sleep_idle();
void sleep_hibernate(int seconds);
void sleep_powerdown();

// fs
extern const devfs_device_t devfs_list[];
extern const sysfs_t sysfs_list[];

// debug
void debug_initialize();
void debug_write(const void *buf, int nbyte);
void debug_enable_led();
void debug_disable_led();
void debug_trace_event(void *event);

// sys
void sys_initialize();
int sys_kernel_request(int req, void *arg);
const void *sys_kernel_request_api(u32 request);
void sys_get_serial_number(mcu_sn_t *serial_number);
void sys_pio_set_attributes(int port, const pio_attr_t *attr);
void sys_pio_write(int port, u32 mask, int value);
u32 sys_pio_read(int port, u32 mask);

// mcu
void mcu_reset_watchdog_timer();
void mcu_set_interrupt_priority(int number, int priority);
int mcu_set_pin_function(const mcu_pin_t *pin, int function, int periph_port);

// cache
void cache_enable();
void cache_disable();
void cache_invalidate_instruction();
void cache_invalidate_data();
void cache_invalidate_data_block(void *address, size_t size);
void cache_clean_data();
void cache_clean_data_block(void *address, size_t size);

// usb
int usb_set_attributes(const devfs_handle_t *handle, void *ctl);
int usb_set_action(const devfs_handle_t *handle, mcu_action_t *action);
void usb_write_endpoint(const devfs_handle_t *handle, u32 endpoint_num,
                        const void *src, u32 size);
int usb_read_endpoint(const devfs_handle_t *handle, u32 endpoint_num,
                      void *dest);

int os_kernel_request(int req, void *arg);
const void *os_kernel_request_api(u32 request);

void os_event_handler(int event, void *args);

void boot_event_handler(int event, void *args);
int boot_is_bootloader_requested();
int boot_flash_erase_page(const devfs_handle_t *handle, void *ctl);
int boot_flash_write_page(const devfs_handle_t *handle, void *ctl);
const void *boot_kernel_request_api(u32 request);

extern const appfs_mem_config_t appfs_mem_config;

#endif /* CONFIG_H_ */
