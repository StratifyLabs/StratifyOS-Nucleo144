
#include <mcu/types.h>
#include <mcu/core.h>
#include <mcu/bootloader.h>
#include <mcu/arch.h>

#include "boot_link_config.h"

const struct __sFILE_fake __sf_fake_stdin;
const struct __sFILE_fake __sf_fake_stdout;
const struct __sFILE_fake __sf_fake_stderr;

#include "../src/config.h"
//Bootloader configuration (don't need Stratify OS configuration for just the bootloader)
STM32_NUCLEO144_DECLARE_BOOT_BOARD_CONFIG(&link_transport);

extern void SystemClock_Config();
extern void boot_main();

//Execute the Stratify OS default bootloader
void _main(){ boot_main(); }

void board_event_handler(int event, void *args) {
#if 0
    switch(event){
    case SOS_EVENT_ROOT_STARTUP:
        SystemClock_Config();
        break;
    }
#endif
}
