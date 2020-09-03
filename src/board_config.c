
#include "board_config.h"
#include <cortexm/task.h>
#include <fcntl.h>
#include <mcu/debug.h>
#include <sos/link/types.h>
#include <sos/sos.h>
#include <string.h>

#include "ethernet/lwip_config.h"

#define TRACE_COUNT 16
#define TRACE_FRAME_SIZE sizeof(link_trace_event_t)
#define TRACE_BUFFER_SIZE (sizeof(link_trace_event_t) * TRACE_COUNT)
static char trace_buffer[TRACE_FRAME_SIZE * TRACE_COUNT];
const ffifo_config_t board_trace_config = {.frame_count = TRACE_COUNT,
                                           .frame_size =
                                               sizeof(link_trace_event_t),
                                           .buffer = trace_buffer};
ffifo_state_t board_trace_state;

extern void SystemClock_Config();

void board_trace_event(void *event) {
  link_trace_event_header_t *header = event;
  devfs_async_t async;
  const devfs_device_t *trace_dev = &(devfs_list[0]);

  // write the event to the fifo
  memset(&async, 0, sizeof(devfs_async_t));
  async.tid = task_get_current();
  async.buf = event;
  async.nbyte = header->size;
  async.flags = O_RDWR;
  trace_dev->driver.write(&(trace_dev->handle), &async);
}

void board_event_handler(int event, void *args) {
  switch (event) {
  case MCU_BOARD_CONFIG_EVENT_ROOT_TASK_INIT:
    break;

  case MCU_BOARD_CONFIG_EVENT_ROOT_FATAL:
    // start the bootloader on a fatal event
    // mcu_core_invokebootloader(0, 0);
    if (args != 0) {
      mcu_debug_log_error(MCU_DEBUG_SYS, "Fatal Error %s", (const char *)args);
    } else {
      mcu_debug_log_error(MCU_DEBUG_SYS, "Fatal Error unknown");
    }
    while (1) {
      ;
    }
    break;

  case MCU_BOARD_CONFIG_EVENT_ROOT_INITIALIZE_CLOCK:
    SystemClock_Config();
    break;

  case MCU_BOARD_CONFIG_EVENT_START_INIT:
    break;

  case MCU_BOARD_CONFIG_EVENT_START_LINK:

#if INCLUDE_ETHERNET
    // start LWIP
    mcu_debug_log_info(MCU_DEBUG_USER0, "Start LWIP");

    usleep(500 * 1000);
    lwip_api.startup(&lwip_api);
#endif

    mcu_debug_log_info(MCU_DEBUG_USER1, "Start LED %d");
    sos_led_startup();
    break;

  case MCU_BOARD_CONFIG_EVENT_START_FILESYSTEM:
    break;
  }
}
