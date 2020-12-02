
#include "board_config.h"

// Root Filesystem---------------------------------------------------

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

const sysfs_t sysfs_list[] = {
    // managing applications
    // APPFS_MOUNT("/app", &mem0, 0777, SYSFS_ROOT),
    // the list of devices
    DEVFS_MOUNT("/dev", devfs_list, 0777, SYSFS_ROOT),
    // root mount
    SYSFS_MOUNT("/", sysfs_list, 0777, SYSFS_ROOT), SYSFS_TERMINATOR};
