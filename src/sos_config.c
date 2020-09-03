
#include <device/cfifo.h>
#include <device/fifo.h>
#include <device/sys.h>
#include <device/uartfifo.h>
#include <device/usbfifo.h>
#include <errno.h>
#include <fcntl.h>
#include <mcu/debug.h>
#include <mcu/mcu.h>
#include <mcu/periph.h>
#include <sos/fs/appfs.h>
#include <sos/fs/devfs.h>
#include <sos/fs/sffs.h>
#include <sos/fs/sysfs.h>
#include <sos/link.h>
#include <sos/sos.h>
#include <sys/lock.h>

#include "config.h"
#include "link_config.h"
#include "sl_config.h"

#if INCLUDE_ETHERNET
#include "ethernet/lwip_config.h"
#define SOCKET_API &lwip_api
#else
#define SOCKET_API NULL
#endif

#if !defined SOS_BOARD_FLAGS
#define SOS_BOARD_FLAGS 0
#endif

//--------------------------------------------Stratify OS
// Configuration-------------------------------------------------
const sos_board_config_t sos_board_config = {
    .clk_usecond_tmr = SOS_BOARD_TMR, // TIM2 -- 32 bit timer
    .task_total = SOS_BOARD_TASK_TOTAL,
    .stdin_dev = "/dev/stdio-in",
    .stdout_dev = "/dev/stdio-out",
    .stderr_dev = "/dev/stdio-out",
    .o_sys_flags = SYS_FLAG_IS_STDIO_FIFO | SYS_FLAG_IS_TRACE | SOS_BOARD_FLAGS,
    .sys_name = SL_CONFIG_NAME,
    .sys_version = SL_CONFIG_VERSION_STRING,
    .sys_id = SL_CONFIG_DOCUMENT_ID,
    .team_id = SL_CONFIG_TEAM_ID,
    .sys_memory_size = SOS_BOARD_SYSTEM_MEMORY_SIZE,
    .start = sos_default_thread,
    .start_args = &link_transport,
    .start_stack_size = SOS_DEFAULT_START_STACK_SIZE,
    .socket_api = SOCKET_API,
    .request = 0,
    .trace_dev = "/dev/trace",
    .trace_event = SOS_BOARD_TRACE_EVENT,
    .git_hash = SOS_GIT_HASH};

// This declares the task tables required by Stratify OS for applications and
// threads
SOS_DECLARE_TASK_TABLE(SOS_BOARD_TASK_TOTAL);

//--------------------------------------------Root
// Filesystem---------------------------------------------------

/*
 * This is the root filesystem that determines what is mounted at /.
 *
 * The default is /app (for installing and running applciations in RAM and
 * flash) and /dev which provides the device tree defined above.
 *
 * Additional filesystems (such as FatFs) can be added if the hardware and
 * drivers are provided by the board.
 *
 */

const devfs_device_t mem0 =
    DEVFS_DEVICE("mem0", mcu_mem, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFBLK);
const sysfs_t sysfs_list[] = {
    APPFS_MOUNT("/app", &mem0, 0777,
                SYSFS_ROOT), // the folder for ram/flash applications
    DEVFS_MOUNT("/dev", devfs_list, 0777, SYSFS_ROOT), // the list of devices
    SYSFS_MOUNT("/", sysfs_list, 0777,
                SYSFS_ROOT), // the root filesystem (must be last)
    SYSFS_TERMINATOR};
