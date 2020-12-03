
#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include <device/ffifo.h>
#include <mcu/arch/stm32/stm32_dev_local.h>
#include <sos/config.h>
#include <sos/debug.h>

void board_trace_event(void * event);

extern const ffifo_config_t board_trace_config;
extern ffifo_state_t board_trace_state;

void board_event_handler(int event, void * args); //optional event handler
void board_initialize();

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

// cache
void cache_enable();
void cache_disable();
void cache_invalidate_instruction();
void cache_invalidate_instruction_block(u32 address, u32 size);
void cache_clean_instruction();
void cache_clean_instruction_block(u32 address, u32 size);
void cache_invalidate_data();
void cache_invalidate_data_block(u32 address, u32 size);
void cache_clean_data();
void cache_clean_data_block(u32 address, u32 size);

// usb
int usb_set_attributes(const devfs_handle_t *handle, void *ctl);
int usb_set_action(const devfs_handle_t *handle, mcu_action_t *action);
void usb_write_endpoint(const devfs_handle_t *handle, u32 endpoint_num,
                        const void *src, u32 size);
int usb_read_endpoint(const devfs_handle_t *handle, u32 endpoint_num,
                      void *dest);

int board_kernel_request(int req, void *arg);
const void *board_kernel_request_api(u32 request);

#endif /* BOARD_CONFIG_H_ */
