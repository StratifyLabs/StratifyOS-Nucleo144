
#include <mcu/core.h>
#include <mcu/flash.h>
#include <sdk/types.h>
#include <sos/arch.h>
#include <sos/boot/bootloader.h>
#include <sos/config.h>
#include <sos/events.h>
#include <stm32/stm32_config.h>

#include "boot_link_config.h"

const struct __sFILE_fake __sf_fake_stdin;
const struct __sFILE_fake __sf_fake_stdout;
const struct __sFILE_fake __sf_fake_stderr;

#include "../src/config.h"

void boot_event_handler(int event, void *args) {}

int boot_is_bootloader_requested() {
  // check HW pin for bootloader request
  stm32_initialize(); // start GPIO clocks
  pio_attr_t attr;
  attr.o_pinmask = (1 << 13);
  attr.o_flags = PIO_FLAG_IS_PULLDOWN;
  sos_config.sys.pio_set_attributes(2, &attr);
  return sos_config.sys.pio_read(2, 1 << 13);
}

int boot_flash_erase_page(const devfs_handle_t *handle, void *ctl) {
  return mcu_flash_erasepage(handle, ctl);
}

int boot_flash_write_page(const devfs_handle_t *handle, void *ctl) {
  return mcu_flash_writepage(handle, ctl);
}
