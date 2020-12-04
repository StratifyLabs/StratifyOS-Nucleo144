
#include <device/auth.h>
#include <device/cfifo.h>
#include <device/device_fifo.h>
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
#include <sys/lock.h>

#include <mcu/arch/stm32/stm32_dev_local.h>

#include "config.h"
#include "link_config.h"
#include "sl_config.h"

#if INCLUDE_ETHERNET
#include "ethernet/lwip_config.h"
#include "ethernet/netif_lan8742a.h"
#endif

//--------------------------------------------Device
// Filesystem-------------------------------------------------

/*
 * Defaults configurations
 *
 * This provides the default pin assignments and settings for peripherals. If
 * the defaults are not provided, the application must specify them.
 *
 * Defaults should be added for peripherals that are dedicated for use on the
 * board. For example, if a UART has an external connection and label on the
 * board, the BSP should provide the default configuration.
 *
 *
 *
 */

char uart1_fifo_buffer[64];
fifo_config_t uart1_fifo_config = {.size = 64, .buffer = uart1_fifo_buffer};

uart_config_t uart1_config = {
    .attr = {.o_flags = UART_FLAG_SET_LINE_CODING_DEFAULT,
             .freq = 115200,
             .width = 8,
             .pin_assignment = {.tx = {SOS_BOARD_USART2_TX_PORT,
                                       SOS_BOARD_USART2_TX_PIN},
                                .rx = {SOS_BOARD_USART2_RX_PORT,
                                       SOS_BOARD_USART2_RX_PIN},
                                .rts = {0xff, 0xff},
                                .cts = {0xff, 0xff}}},
    .fifo_config = &uart1_fifo_config};

#define UART_DMA_FLAGS                                                         \
  STM32_DMA_FLAG_IS_MEMORY_SINGLE | STM32_DMA_FLAG_IS_PERIPH_SINGLE |          \
      STM32_DMA_FLAG_IS_PERIPH_BYTE | STM32_DMA_FLAG_IS_MEMORY_BYTE

stm32_uart_dma_config_t uart1_dma_config = {
    .uart_config = {.attr = {.o_flags = UART_FLAG_SET_LINE_CODING_DEFAULT,
                             .freq = 115200,
                             .width = 8,
                             .pin_assignment = {.tx = {SOS_BOARD_USART2_TX_PORT,
                                                       SOS_BOARD_USART2_TX_PIN},
                                                .rx = {SOS_BOARD_USART2_RX_PORT,
                                                       SOS_BOARD_USART2_RX_PIN},
                                                .rts = {0xff, 0xff},
                                                .cts = {0xff, 0xff}}},
                    .fifo_config = &uart1_fifo_config},
    .dma_config = {
        .rx = {.dma_number = SOS_BOARD_USART2_RX_DMA,
               .stream_number = SOS_BOARD_USART2_RX_DMA_STREAM,
               .channel_number = SOS_BOARD_USART2_RX_DMA_CHANNEL,
               .priority = STM32_DMA_PRIORITY_LOW,
               .o_flags = STM32_DMA_FLAG_IS_PERIPH_TO_MEMORY | UART_DMA_FLAGS |
                          STM32_DMA_FLAG_IS_CIRCULAR},
        .tx = {.dma_number = SOS_BOARD_USART2_TX_DMA,
               .stream_number = SOS_BOARD_USART2_TX_DMA_STREAM,
               .channel_number = SOS_BOARD_USART2_TX_DMA_CHANNEL,
               .priority = STM32_DMA_PRIORITY_LOW,
               .o_flags = STM32_DMA_FLAG_IS_MEMORY_TO_PERIPH | UART_DMA_FLAGS |
                          STM32_DMA_FLAG_IS_NORMAL}}};

// USART6
UARTFIFO_DECLARE_CONFIG_STATE(uart5_fifo, 1024, 32,
                              UART_FLAG_SET_LINE_CODING_DEFAULT, 8, 115200,
                              SOS_BOARD_USART6_RX_PORT,
                              SOS_BOARD_USART6_RX_PIN, // RX
                              SOS_BOARD_USART6_TX_PORT,
                              SOS_BOARD_USART6_TX_PIN, // TX
                              0xff, 0xff, 0xff, 0xff);

// I2C1
I2C_DECLARE_CONFIG_MASTER(i2c0, I2C_FLAG_SET_MASTER, 100000,
                          SOS_BOARD_I2C1_SDA_PORT,
                          SOS_BOARD_I2C1_SDA_PIN, // SDA
                          SOS_BOARD_I2C1_SCL_PORT,
                          SOS_BOARD_I2C1_SCL_PIN); // SCL

// I2C2
I2C_DECLARE_CONFIG_MASTER(i2c1, I2C_FLAG_SET_MASTER, 100000,
                          SOS_BOARD_I2C2_SDA_PORT,
                          SOS_BOARD_I2C2_SDA_PIN, // SDA
                          SOS_BOARD_I2C2_SCL_PORT,
                          SOS_BOARD_I2C2_SCL_PIN); // SCL

#define SPI_DMA_FLAGS                                                          \
  STM32_DMA_FLAG_IS_NORMAL | STM32_DMA_FLAG_IS_MEMORY_SINGLE |                 \
      STM32_DMA_FLAG_IS_PERIPH_SINGLE | STM32_DMA_FLAG_IS_PERIPH_BYTE |        \
      STM32_DMA_FLAG_IS_MEMORY_BYTE

const stm32_spi_dma_config_t spi0_dma_config =
    {.spi_config =
         {.attr = {.o_flags = SPI_FLAG_SET_MASTER | SPI_FLAG_IS_FORMAT_SPI |
                              SPI_FLAG_IS_MODE0 | SPI_FLAG_IS_FULL_DUPLEX,
                   .width = 8,
                   .freq = 1000000UL,
                   .pin_assignment =
                       {.miso = {SOS_BOARD_SPI1_MISO_PORT,
                                 SOS_BOARD_SPI1_MISO_PIN},
                        .mosi = {SOS_BOARD_SPI1_MOSI_PORT,
                                 SOS_BOARD_SPI1_MOSI_PIN},
                        .sck = {SOS_BOARD_SPI1_SCK_PORT,
                                SOS_BOARD_SPI1_SCK_PIN},
                        .cs = {SOS_BOARD_SPI1_CS_PORT, SOS_BOARD_SPI1_CS_PIN}}}},
     .dma_config = {.rx =
                        {
                            .dma_number = SOS_BOARD_SPI1_RX_DMA,
                            .stream_number = SOS_BOARD_SPI1_RX_DMA_STREAM,
                            .channel_number = SOS_BOARD_SPI1_RX_DMA_CHANNEL,
                            .priority = STM32_DMA_PRIORITY_LOW,
                            .o_flags = STM32_DMA_FLAG_IS_PERIPH_TO_MEMORY |
                                       SPI_DMA_FLAGS,
                        },
                    .tx = {.dma_number = SOS_BOARD_SPI1_TX_DMA,
                           .stream_number = SOS_BOARD_SPI1_TX_DMA_STREAM,
                           .channel_number = SOS_BOARD_SPI1_TX_DMA_CHANNEL,
                           .priority = STM32_DMA_PRIORITY_LOW,
                           .o_flags = STM32_DMA_FLAG_IS_MEMORY_TO_PERIPH |
                                      SPI_DMA_FLAGS}}};

const stm32_spi_dma_config_t spi2_dma_config =
    {.spi_config =
         {.attr = {.o_flags = SPI_FLAG_SET_MASTER | SPI_FLAG_IS_FORMAT_SPI |
                              SPI_FLAG_IS_MODE0 | SPI_FLAG_IS_FULL_DUPLEX,
                   .width = 8,
                   .freq = 1000000UL,
                   .pin_assignment =
                       {.miso = {SOS_BOARD_SPI3_MISO_PORT,
                                 SOS_BOARD_SPI3_MISO_PIN},
                        .mosi = {SOS_BOARD_SPI3_MOSI_PORT,
                                 SOS_BOARD_SPI3_MOSI_PIN},
                        .sck = {SOS_BOARD_SPI3_SCK_PORT,
                                SOS_BOARD_SPI3_SCK_PIN},
                        .cs = {SOS_BOARD_SPI3_CS_PORT, SOS_BOARD_SPI3_CS_PIN}}}},
     .dma_config = {.rx =
                        {
                            .dma_number = SOS_BOARD_SPI3_RX_DMA,
                            .stream_number = SOS_BOARD_SPI3_RX_DMA_STREAM,
                            .channel_number = SOS_BOARD_SPI3_RX_DMA_CHANNEL,
                            .priority = STM32_DMA_PRIORITY_LOW,
                            .o_flags = STM32_DMA_FLAG_IS_PERIPH_TO_MEMORY |
                                       SPI_DMA_FLAGS,
                        },
                    .tx = {.dma_number = SOS_BOARD_SPI3_TX_DMA,
                           .stream_number = SOS_BOARD_SPI3_TX_DMA_STREAM,
                           .channel_number = SOS_BOARD_SPI3_TX_DMA_CHANNEL,
                           .priority = STM32_DMA_PRIORITY_LOW,
                           .o_flags = STM32_DMA_FLAG_IS_MEMORY_TO_PERIPH |
                                      SPI_DMA_FLAGS}}};

#if INCLUDE_ETHERNET

u8 eth_tx_buffer[STM32_ETH_DMA_BUFFER_SIZE]; // these use DMA and can't be
                                             // tightly coupled
u8 eth_rx_buffer[STM32_ETH_DMA_BUFFER_SIZE];

const stm32_eth_dma_config_t
    eth0_config =
        {
            .eth_config = {.attr =
                               {
                                   .o_flags =
                                       ETH_FLAG_SET_INTERFACE | ETH_FLAG_START |
                                       ETH_FLAG_IS_RMII |
                                       ETH_FLAG_IS_AUTONEGOTIATION_ENABLED,
                                   .pin_assignment =
                                       {
                                           .rmii = {.clk = {0, 1},    // PA1
                                                    .txd0 = {6, 13},  // PG13
                                                    .txd1 = {1, 13},  // PB13
                                                    .tx_en = {6, 11}, // PG11
                                                    .rxd0 = {2, 4},   // PC4
                                                    .rxd1 = {2, 5},   // PC5
                                                    .crs_dv = {0, 7}, // PA7
                                                    .rx_er = {0xff, 0xff}, //??
                                                    .unused[0] = {0xff, 0xff},
                                                    .unused[1] = {0xff, 0xff},
                                                    .unused[2] = {0xff, 0xff},
                                                    .unused[3] = {0xff, 0xff},
                                                    .unused[4] = {0xff, 0xff},
                                                    .unused[5] = {0xff, 0xff},
                                                    .unused[6] = {0xff, 0xff},
                                                    .unused[7] = {0xff, 0xff}},
                                           .mdio = {0, 2}, // PA2
                                           .mdc = {2, 1}   // PC1
                                       },
                                   .mac_address[0] = 0x00,
                                   .mac_address[1] = 0x80,
                                   .mac_address[2] = 0xe1,
                                   .mac_address[3] = 0x00,
                                   .mac_address[4] = 0x00,
                                   .mac_address[5] = 0x00,
                                   .mac_address[6] = 0x00, // unused
                                   .mac_address[7] = 0x00, // unused
                                   .mac_address[8] = 0x00, // unused
                                   .mac_address[9] = 0x00, // unused
                                   .phy_address = 0 // address of PHY CHIP
                               }},
            .tx_buffer = eth_tx_buffer,
            .rx_buffer = eth_rx_buffer};

netif_lan8742a_state_t netif_lan8742a_state MCU_SYS_MEM;
#endif

#define USB_DEVICE_FIFO_BUFFER_SIZE 64
u8 usb_device_fifo_buffer[USB_DEVICE_FIFO_BUFFER_SIZE] MCU_SYS_MEM;
u8 usb_device_fifo_read_buffer[USB_DEVICE_FIFO_BUFFER_SIZE] MCU_SYS_MEM;
device_fifo_state_t usb_device_fifo_state MCU_SYS_MEM;
const device_fifo_config_t usb_device_fifo_config = {
    .device =
        DEVFS_DEVICE("usb", mcu_usb, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
    .location = SOS_LINK_TRANSPORT_USB_BULK_ENDPOINT,
    .fifo =
        {
            .buffer = usb_device_fifo_buffer,
            .size = USB_DEVICE_FIFO_BUFFER_SIZE,
        },
    .read_buffer = usb_device_fifo_read_buffer,
    .read_buffer_size = USB_DEVICE_FIFO_BUFFER_SIZE,
};

// Coming Soon
// SD Card as DMA
// I2S2
// I2S3
// SAI1A/B

FIFO_DECLARE_CONFIG_STATE(stdio_in, SOS_BOARD_STDIO_BUFFER_SIZE);
FIFO_DECLARE_CONFIG_STATE(stdio_out, SOS_BOARD_STDIO_BUFFER_SIZE);

#if !defined SOS_BOARD_USB_PORT
#define SOS_BOARD_USB_PORT 0
#endif

/* This is the list of devices that will show up in the /dev folder.
 */
const devfs_device_t devfs_list[] = {
    // System devices
    DEVFS_DEVICE("trace", ffifo, 0, &board_trace_config, &board_trace_state,
                 0666, SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("stdio-out", fifo, 0, &stdio_out_config, &stdio_out_state,
                 0666, SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("stdio-in", fifo, 0, &stdio_in_config, &stdio_in_state, 0666,
                 SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("link-phy-usb", usbfifo, SOS_BOARD_USB_PORT,
                 &sos_link_transport_usb_fifo_cfg,
                 &sos_link_transport_usb_fifo_state, 0666, SOS_USER_ROOT,
                 S_IFCHR),
    DEVFS_DEVICE("link-phy-usb2", device_fifo, SOS_BOARD_USB_PORT,
                 &usb_device_fifo_config, &usb_device_fifo_state, 0666,
                 SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("sys", sys, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("auth", auth, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
    // DEVFS_DEVICE("rtc", mcu_rtc, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),

    // MCU peripherals
    DEVFS_DEVICE("core", mcu_core, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),

#if INCLUDE_ETHERNET
    DEVFS_DEVICE("eth0", netif_lan8742a, 0, &eth0_config, &netif_lan8742a_state,
                 0666, SOS_USER_ROOT, S_IFCHR),
#endif

    DEVFS_DEVICE("i2c0", mcu_i2c, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("i2c1", mcu_i2c, 1, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),

    DEVFS_DEVICE("pio0", mcu_pio, 0, 0, 0, 0666, SOS_USER_ROOT,
                 S_IFCHR), // GPIOA
    DEVFS_DEVICE("pio1", mcu_pio, 1, 0, 0, 0666, SOS_USER_ROOT,
                 S_IFCHR), // GPIOB
    DEVFS_DEVICE("pio2", mcu_pio, 2, 0, 0, 0666, SOS_USER_ROOT,
                 S_IFCHR), // GPIOC
    DEVFS_DEVICE("pio3", mcu_pio, 3, 0, 0, 0666, SOS_USER_ROOT,
                 S_IFCHR), // GPIOD
    DEVFS_DEVICE("pio4", mcu_pio, 4, 0, 0, 0666, SOS_USER_ROOT,
                 S_IFCHR), // GPIOE
    DEVFS_DEVICE("pio5", mcu_pio, 5, 0, 0, 0666, SOS_USER_ROOT,
                 S_IFCHR), // GPIOF
    DEVFS_DEVICE("pio6", mcu_pio, 6, 0, 0, 0666, SOS_USER_ROOT,
                 S_IFCHR), // GPIOG
    DEVFS_DEVICE("pio7", mcu_pio, 7, 0, 0, 0666, SOS_USER_ROOT,
                 S_IFCHR), // GPIOH

    DEVFS_DEVICE("spi0", mcu_spi, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("spi1", mcu_spi, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),

    DEVFS_DEVICE("tmr0", mcu_tmr, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("tmr2", mcu_tmr, 2, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("tmr3", mcu_tmr, 3, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
    DEVFS_DEVICE("tmr4", mcu_tmr, 4, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),

    DEVFS_TERMINATOR};
