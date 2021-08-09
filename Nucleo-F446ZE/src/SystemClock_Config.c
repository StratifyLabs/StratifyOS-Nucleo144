
#include <mcu/flash.h>
#include <sos/arch.h>
#include <stm32f4xx/stm32f4xx_hal.h>

#include "config.h"

void SystemClock_Config() {

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  /**Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  PeriphClkInitStruct.PeriphClockSelection =
      RCC_PERIPHCLK_I2S_APB1 | RCC_PERIPHCLK_I2S_APB2 | RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
  PeriphClkInitStruct.PLLI2S.PLLI2SP = RCC_PLLI2SP_DIV2;
  PeriphClkInitStruct.PLLI2S.PLLI2SM = 4;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  PeriphClkInitStruct.PLLI2S.PLLI2SQ = 2;
  PeriphClkInitStruct.PLLI2SDivQ = 1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
  PeriphClkInitStruct.I2sApb2ClockSelection = RCC_I2SAPB2CLKSOURCE_PLLI2S;
  PeriphClkInitStruct.I2sApb1ClockSelection = RCC_I2SAPB1CLKSOURCE_PLLI2S;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}

#define RAM_PAGES (112 - SYSTEM_MEMORY_SIZE / 1024UL)
#define FLASH_START (0x08000000 + 2 * 16 * 1024UL)
#define RAM_START (0x20000000 + SYSTEM_MEMORY_SIZE)

// Application Filesystem ------------------------------------------
static u32 ram_usage_table[APPFS_RAM_USAGE_WORDS(RAM_PAGES)] MCU_SYS_MEM;
// flash doesn't need config or state
static const devfs_device_t flash0 =
    DEVFS_DEVICE("flash0", mcu_flash, 0, 0, 0, 0666, SYSFS_ROOT, S_IFBLK);

const appfs_mem_config_t appfs_mem_config = {
    .usage_size = sizeof(ram_usage_table),
    .section_count = 4,
    .usage = ram_usage_table,
    .flash_driver = &flash0,
    .sections = {
        {.o_flags = MEM_FLAG_IS_FLASH,
         .page_count = 2,
         .page_size = 16 * 1024UL,
         // skip the first 2 pages for the bootloader
         .address = FLASH_START},
        {.o_flags = MEM_FLAG_IS_FLASH,
         .page_count = 1,
         .page_size = 64 * 1024UL,
         .address = FLASH_START + 2 * 16 * 1024UL},
        {.o_flags = MEM_FLAG_IS_FLASH,
         .page_count = 1,
         .page_size = 128 * 1024UL,
         .address = FLASH_START + 2 * 16 * 1024UL + 64 * 1024UL},
        // the last 2 128K flash pages are used for the OS
        {.o_flags = MEM_FLAG_IS_RAM,
         .page_count = RAM_PAGES,
         .page_size = MCU_RAM_PAGE_SIZE,
         .address = RAM_START},
    }};
