
#include <device/cfifo.h>
#include <device/fifo.h>
#include <device/sys.h>
#include <device/uartfifo.h>
#include <device/usbfifo.h>
#include <errno.h>
#include <fcntl.h>
#include <mcu/mcu.h>
#include <mcu/periph.h>
#include <sos/debug.h>
#include <sos/fs/appfs.h>
#include <sos/fs/devfs.h>
#include <sos/fs/sffs.h>
#include <sos/fs/sysfs.h>
#include <sos/link.h>
#include <sos/sos.h>
#include <sos/symbols.h>
#include <sys/lock.h>

#include <stm32_config.h>

#include <cortexm/mpu.h>

#include "config.h"

#include "cache_config.h"
#include "clock_config.h"
#include "debug_config.h"
#include "fs_config.h"
#include "mcu_config.h"
#include "os_config.h"
#include "sleep_config.h"
#include "sys_config.h"
#include "usb_config.h"

#if _IS_BOOT
#include "../boot/boot_link_config.h"
#else
#include "link_config.h"
#endif
#include "sl_config.h"

#define WRITE_BACK_NO_WRITE_ALLOCATE                                           \
  ((0 << 21) | (0 << 20) | (0 << 19) | (0 << 18) | (1 << 17) | (1 << 16))

#define WRITE_BACK_READ_WRITE_ALLOCATE                                         \
  ((0 << 21) | (0 << 20) | (1 << 19) | (0 << 18) | (1 << 17) | (1 << 16))

#if INCLUDE_ETHERNET && !_IS_BOOT
#include "ethernet/lwip_config.h"
#define SOCKET_API &lwip_api
#else
#define SOCKET_API NULL
#endif

// Stratify OS Configuration-------------------------------------------

#if _IS_BOOT
SOS_DECLARE_PUBLIC_KEY_64(public_key)
#endif

const sos_config_t sos_config = {
#if !_IS_BOOT
    .fs = {.devfs_list = devfs_list,
           .rootfs_list = sysfs_list,
           .stdin_dev = "/dev/stdio-in",
           .stdout_dev = "/dev/stdio-out",
           .stderr_dev = "/dev/stdio-out",
           .trace_dev = "/dev/trace"},

    .clock = {.initialize = clock_initialize,
              .enable = clock_enable,
              .disable = clock_disable,
              .set_channel = clock_set_channel,
              .get_channel = clock_get_channel,
              .microseconds = clock_microseconds,
              .nanoseconds = clock_nanoseconds},

    .task =
        {//.task_total = CONFIG_TASK_TOTAL,
         .start_stack_size = 2048,
         .start = sos_default_thread,
         .start_args = &link_transport},

    .sleep = {.idle = sleep_idle,
              .hibernate = sleep_hibernate,
              .powerdown = sleep_powerdown},

#endif
    .usb = {.control_endpoint_max_size = 64,
            .set_attributes = usb_set_attributes,
            .set_action = usb_set_action,
            .write_endpoint = usb_write_endpoint,
            .read_endpoint = usb_read_endpoint},

    .cache = {.enable = cache_enable,
              .disable = cache_disable,
              .invalidate_instruction = cache_invalidate_instruction,
              .invalidate_data = cache_invalidate_data,
              .invalidate_data_block = cache_invalidate_data_block,
              .clean_data = cache_clean_data,
              .clean_data_block = cache_clean_data_block,
              .external_sram_policy = SOS_CACHE_DEFAULT_POLICY,
              .external_flash_policy = SOS_CACHE_DEFAULT_POLICY,
              .sram_policy = WRITE_BACK_READ_WRITE_ALLOCATE,
              .flash_policy = WRITE_BACK_NO_WRITE_ALLOCATE,
              .peripherals_policy = SOS_CACHE_PERIPHERALS_POLICY,
              .lcd_policy = SOS_CACHE_PERIPHERALS_POLICY,
              .tightly_coupled_data_policy = SOS_CACHE_PERIPHERALS_POLICY,
              .tightly_coupled_instruction_policy = SOS_CACHE_PERIPHERALS_POLICY,
              .os_code_mpu_type = MPU_MEMORY_FLASH,
              .os_data_mpu_type = MPU_MEMORY_SRAM,
              .os_system_data_mpu_type = MPU_MEMORY_TIGHTLY_COUPLED_DATA
    },

    .mcu = {.interrupt_request_total = MCU_LAST_IRQ + 1,
            .interrupt_middle_priority = MCU_MIDDLE_IRQ_PRIORITY,
            .task_mpu_region_offset = 0,
            .set_interrupt_priority = mcu_set_interrupt_priority,
            .reset_watchdog_timer = mcu_reset_watchdog_timer,
            .set_pin_function = mcu_set_pin_function},

    .sys = {.initialize = sys_initialize,
            .bootloader_start_address = __BOOT_START_ADDRESS,
            .memory_size = SYSTEM_MEMORY_SIZE,
            .get_serial_number = sys_get_serial_number,
            .os_mpu_text_mask = 0,
            .flags = SYS_FLAG_IS_STDIO_FIFO | SYS_FLAG_IS_TRACE |
                     SYS_FLAG_IS_FIRST_THREAD_AUTHENTICATED |
                     CONFIG_BOARD_FLAGS,
            .name = SL_CONFIG_NAME,
            .version = SL_CONFIG_VERSION_STRING,
            .git_hash = SOS_GIT_HASH,
            .mcu_git_hash = NULL,
            .id = SL_CONFIG_DOCUMENT_ID,
            .team_id = SL_CONFIG_TEAM_ID,
#if _IS_BOOT
            .secret_key_size = 64,
            .secret_key_address = public_key,
#else
            .secret_key_size = 0,
            .secret_key_address = 0,
#endif
            .vector_table = (void *)(VECTOR_TABLE_ADDRESS),
            .pio_write = sys_pio_write,
            .pio_read = sys_pio_read,
            .pio_set_attributes = sys_pio_set_attributes,
            .core_clock_frequency = SOS_BOARD_SYSTEM_CLOCK,
#if !_IS_BOOT
            .kernel_request = sys_kernel_request,
            .kernel_request_api = sys_kernel_request_api,
            .get_public_key = sys_get_public_key
#else
            .kernel_request_api = boot_kernel_request_api
#endif
    },

    .debug = {.initialize = debug_initialize,
              .write = debug_write,
              .trace_event = debug_trace_event,
              .disable_led = debug_disable_led,
              .enable_led = debug_enable_led,
              .flags = CONFIG_DEBUG_FLAGS},

#if _IS_BOOT
    .boot = {.api = {.code_size = (u32)&_etext,
                     .exec = boot_invoke_bootloader,
                     .event = boot_event_handler,
                     .hardware_id = __HARDWARE_ID},
             .program_start_address = __KERNEL_START_ADDRESS,
             .software_bootloader_request_address = 0x20004000,
             .software_bootloader_request_value = 0x55AA55AA,
             .is_bootloader_requested = boot_is_bootloader_requested,
             .flash_handle = {.port = 0},
             .flash_erase_page = mcu_flash_erasepage,
             .flash_write_page = mcu_flash_writepage,
             .link_transport_driver = &boot_link_usb_transport},
    .event_handler = boot_event_handler,
#else
    .event_handler = os_event_handler,
#endif
    .socket_api = SOCKET_API};
