
#include <sys/lock.h>
#include <fcntl.h>
#include <errno.h>
#include <mcu/mcu.h>
#include <mcu/debug.h>
#include <mcu/periph.h>
#include <device/sys.h>
#include <device/uartfifo.h>
#include <device/usbfifo.h>
#include <device/fifo.h>
#include <device/cfifo.h>
#include <device/sys.h>
#include <sos/link.h>
#include <sos/fs/sysfs.h>
#include <sos/fs/appfs.h>
#include <sos/fs/devfs.h>
#include <sos/fs/sffs.h>
#include <sos/sos.h>

#include "config.h"
#include "sl_config.h"
#include "link_config.h"

#if _ETHERNET
#include "ethernet/netif_lan8742a.h"
#include "ethernet/lwip_config.h"
#endif

//--------------------------------------------Device Filesystem-------------------------------------------------


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
//USART2
#if 0
UARTFIFO_DECLARE_CONFIG_STATE(uart1_fifo, 1024, 32,
										UART_FLAG_SET_LINE_CODING_DEFAULT, 8, 115200,
										SOS_BOARD_USART2_RX_PORT, SOS_BOARD_USART2_RX_PIN, //RX
										SOS_BOARD_USART2_TX_PORT, SOS_BOARD_USART2_TX_PIN, //TX
										0xff, 0xff,
										0xff, 0xff);
#endif
#if !defined __debug
//USART3
UARTFIFO_DECLARE_CONFIG_STATE(uart2_fifo, 1024, 32,
										UART_FLAG_SET_LINE_CODING_DEFAULT, 8, 115200,
										SOS_BOARD_USART3_RX_PORT, SOS_BOARD_USART3_RX_PIN, //RX
										SOS_BOARD_USART3_TX_PORT, SOS_BOARD_USART3_TX_PIN, //TX
										0xff, 0xff,
										0xff, 0xff);
#endif

char uart1_fifo_buffer[64];
fifo_config_t uart1_fifo_config = {
	.size = 64,
	.buffer = uart1_fifo_buffer
};

uart_config_t uart1_config = {
	.attr = {
		.o_flags = UART_FLAG_SET_LINE_CODING_DEFAULT,
		.freq = 115200,
		.width = 8,
		.pin_assignment = {
			.tx = { SOS_BOARD_USART2_TX_PORT, SOS_BOARD_USART2_TX_PIN },
			.rx = { SOS_BOARD_USART2_RX_PORT, SOS_BOARD_USART2_RX_PIN },
			.rts = { 0xff, 0xff },
			.cts = { 0xff, 0xff }
		}
	},
	.fifo_config = &uart1_fifo_config
};


#define UART_DMA_FLAGS STM32_DMA_FLAG_IS_MEMORY_SINGLE | \
	STM32_DMA_FLAG_IS_PERIPH_SINGLE | \
	STM32_DMA_FLAG_IS_PERIPH_BYTE | \
	STM32_DMA_FLAG_IS_MEMORY_BYTE

stm32_uart_dma_config_t uart1_dma_config = {
	.uart_config = {
		.attr = {
			.o_flags = UART_FLAG_SET_LINE_CODING_DEFAULT,
			.freq = 115200,
			.width = 8,
			.pin_assignment = {
				.tx = { SOS_BOARD_USART2_TX_PORT, SOS_BOARD_USART2_TX_PIN },
				.rx = { SOS_BOARD_USART2_RX_PORT, SOS_BOARD_USART2_RX_PIN },
				.rts = { 0xff, 0xff },
				.cts = { 0xff, 0xff }
			}
		},
		.fifo_config = &uart1_fifo_config
	},
	.dma_config = {
		.rx = {
			.dma_number = SOS_BOARD_USART2_RX_DMA,
			.stream_number = SOS_BOARD_USART2_RX_DMA_STREAM,
			.channel_number = SOS_BOARD_USART2_RX_DMA_CHANNEL,
			.priority = STM32_DMA_PRIORITY_LOW,
			.o_flags = STM32_DMA_FLAG_IS_PERIPH_TO_MEMORY | UART_DMA_FLAGS | STM32_DMA_FLAG_IS_CIRCULAR
		},
		.tx = {
			.dma_number = SOS_BOARD_USART2_TX_DMA,
			.stream_number = SOS_BOARD_USART2_TX_DMA_STREAM,
			.channel_number = SOS_BOARD_USART2_TX_DMA_CHANNEL,
			.priority = STM32_DMA_PRIORITY_LOW,
			.o_flags = STM32_DMA_FLAG_IS_MEMORY_TO_PERIPH | UART_DMA_FLAGS | STM32_DMA_FLAG_IS_NORMAL
		}
	}
};


//USART6
UARTFIFO_DECLARE_CONFIG_STATE(uart5_fifo, 1024, 32,
										UART_FLAG_SET_LINE_CODING_DEFAULT, 8, 115200,
										SOS_BOARD_USART6_RX_PORT, SOS_BOARD_USART6_RX_PIN, //RX
										SOS_BOARD_USART6_TX_PORT, SOS_BOARD_USART6_TX_PIN, //TX
										0xff, 0xff,
										0xff, 0xff);

//I2C1
I2C_DECLARE_CONFIG_MASTER(i2c0,
								  I2C_FLAG_SET_MASTER,
								  100000,
								  SOS_BOARD_I2C1_SDA_PORT, SOS_BOARD_I2C1_SDA_PIN, //SDA
								  SOS_BOARD_I2C1_SCL_PORT, SOS_BOARD_I2C1_SCL_PIN); //SCL

//I2C2
I2C_DECLARE_CONFIG_MASTER(i2c1,
								  I2C_FLAG_SET_MASTER,
								  100000,
								  SOS_BOARD_I2C2_SDA_PORT, SOS_BOARD_I2C2_SDA_PIN, //SDA
								  SOS_BOARD_I2C2_SCL_PORT, SOS_BOARD_I2C2_SCL_PIN); //SCL


#define SPI_DMA_FLAGS STM32_DMA_FLAG_IS_NORMAL | \
	STM32_DMA_FLAG_IS_MEMORY_SINGLE | \
	STM32_DMA_FLAG_IS_PERIPH_SINGLE | \
	STM32_DMA_FLAG_IS_PERIPH_BYTE | \
	STM32_DMA_FLAG_IS_MEMORY_BYTE

const stm32_spi_dma_config_t spi0_dma_config = {
	.spi_config = {
		.attr = {
			.o_flags = SPI_FLAG_SET_MASTER | SPI_FLAG_IS_FORMAT_SPI | SPI_FLAG_IS_MODE0 | SPI_FLAG_IS_FULL_DUPLEX,
			.width = 8,
			.freq = 1000000UL,
			.pin_assignment = {
				.miso = {SOS_BOARD_SPI1_MISO_PORT, SOS_BOARD_SPI1_MISO_PIN},
				.mosi = {SOS_BOARD_SPI1_MOSI_PORT, SOS_BOARD_SPI1_MOSI_PIN},
				.sck = {SOS_BOARD_SPI1_SCK_PORT, SOS_BOARD_SPI1_SCK_PIN},
				.cs = {SOS_BOARD_SPI1_CS_PORT, SOS_BOARD_SPI1_CS_PIN}
			}
		}
	},
	.dma_config = {
		.rx = {
			.dma_number = SOS_BOARD_SPI1_RX_DMA,
			.stream_number = SOS_BOARD_SPI1_RX_DMA_STREAM,
			.channel_number = SOS_BOARD_SPI1_RX_DMA_CHANNEL,
			.priority = STM32_DMA_PRIORITY_LOW,
			.o_flags = STM32_DMA_FLAG_IS_PERIPH_TO_MEMORY |
			SPI_DMA_FLAGS,
		},
		.tx = {
			.dma_number = SOS_BOARD_SPI1_TX_DMA,
			.stream_number = SOS_BOARD_SPI1_TX_DMA_STREAM,
			.channel_number = SOS_BOARD_SPI1_TX_DMA_CHANNEL,
			.priority = STM32_DMA_PRIORITY_LOW,
			.o_flags = STM32_DMA_FLAG_IS_MEMORY_TO_PERIPH |
			SPI_DMA_FLAGS
		}
	}
};

const stm32_spi_dma_config_t spi2_dma_config = {
	.spi_config = {
		.attr = {
			.o_flags = SPI_FLAG_SET_MASTER | SPI_FLAG_IS_FORMAT_SPI | SPI_FLAG_IS_MODE0 | SPI_FLAG_IS_FULL_DUPLEX,
			.width = 8,
			.freq = 1000000UL,
			.pin_assignment = {
				.miso = {SOS_BOARD_SPI3_MISO_PORT, SOS_BOARD_SPI3_MISO_PIN},
				.mosi = {SOS_BOARD_SPI3_MOSI_PORT, SOS_BOARD_SPI3_MOSI_PIN},
				.sck = {SOS_BOARD_SPI3_SCK_PORT, SOS_BOARD_SPI3_SCK_PIN},
				.cs = {SOS_BOARD_SPI3_CS_PORT, SOS_BOARD_SPI3_CS_PIN}
			}
		}
	},
	.dma_config = {
		.rx = {
			.dma_number = SOS_BOARD_SPI3_RX_DMA,
			.stream_number = SOS_BOARD_SPI3_RX_DMA_STREAM,
			.channel_number = SOS_BOARD_SPI3_RX_DMA_CHANNEL,
			.priority = STM32_DMA_PRIORITY_LOW,
			.o_flags = STM32_DMA_FLAG_IS_PERIPH_TO_MEMORY |
			SPI_DMA_FLAGS,
		},
		.tx = {
			.dma_number = SOS_BOARD_SPI3_TX_DMA,
			.stream_number = SOS_BOARD_SPI3_TX_DMA_STREAM,
			.channel_number = SOS_BOARD_SPI3_TX_DMA_CHANNEL,
			.priority = STM32_DMA_PRIORITY_LOW,
			.o_flags = STM32_DMA_FLAG_IS_MEMORY_TO_PERIPH |
			SPI_DMA_FLAGS
		}
	}
};

#if _ETHERNET

u8 eth_tx_buffer[STM32_ETH_DMA_BUFFER_SIZE]; //these use DMA and can't be tightly coupled
u8 eth_rx_buffer[STM32_ETH_DMA_BUFFER_SIZE];

const stm32_eth_dma_config_t eth0_config = {
	.eth_config = {
		.attr = {
			.o_flags = ETH_FLAG_SET_INTERFACE |
			ETH_FLAG_START |
			ETH_FLAG_IS_RMII |
			ETH_FLAG_IS_AUTONEGOTIATION_ENABLED,
			.pin_assignment = {
				.rmii = {
					.clk = {0, 1}, //PA1
					.txd0 = {6, 13}, //PG13
					.txd1 = {1, 13}, //PB13
					.tx_en = {6, 11}, //PG11
					.rxd0 = {2, 4}, //PC4
					.rxd1 = {2, 5}, //PC5
					.crs_dv = {0, 7}, //PA7
					.rx_er = {0xff, 0xff}, //??
					.unused[0] = {0xff, 0xff},
					.unused[1] = {0xff, 0xff},
					.unused[2] = {0xff, 0xff},
					.unused[3] = {0xff, 0xff},
					.unused[4] = {0xff, 0xff},
					.unused[5] = {0xff, 0xff},
					.unused[6] = {0xff, 0xff},
					.unused[7] = {0xff, 0xff}
				},
				.mdio = {0, 2}, //PA2
				.mdc = {2, 1} //PC1
			},
			.mac_address[0] = 0x00,
			.mac_address[1] = 0x80,
			.mac_address[2] = 0xe1,
			.mac_address[3] = 0x00,
			.mac_address[4] = 0x00,
			.mac_address[5] = 0x00,
			.mac_address[6] = 0x00, //unused
			.mac_address[7] = 0x00, //unused
			.phy_address = 0 //address of PHY CHIP
		}
	},
	.tx_buffer = eth_tx_buffer,
	.rx_buffer = eth_rx_buffer
};
#endif

//Coming Soon
//SD Card as DMA
//I2S2
//I2S3
//SAI1A/B


FIFO_DECLARE_CONFIG_STATE(stdio_in, SOS_BOARD_STDIO_BUFFER_SIZE);
FIFO_DECLARE_CONFIG_STATE(stdio_out, SOS_BOARD_STDIO_BUFFER_SIZE);
CFIFO_DECLARE_CONFIG_STATE_4(board_fifo, 256);

#if !defined SOS_BOARD_USB_PORT
#define SOS_BOARD_USB_PORT 0
#endif

/* This is the list of devices that will show up in the /dev folder.
 */
const devfs_device_t devfs_list[] = {
	//System devices
	DEVFS_DEVICE("trace", ffifo, 0, &board_trace_config, &board_trace_state, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("fifo", cfifo, 0, &board_fifo_config, &board_fifo_state, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("stdio-out", fifo, 0, &stdio_out_config, &stdio_out_state, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("stdio-in", fifo, 0, &stdio_in_config, &stdio_in_state, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("link-phy-usb", usbfifo, SOS_BOARD_USB_PORT, &sos_link_transport_usb_fifo_cfg, &sos_link_transport_usb_fifo_state, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("sys", sys, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	//DEVFS_DEVICE("rtc", mcu_rtc, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),

	//MCU peripherals
	DEVFS_DEVICE("core", mcu_core, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("core0", mcu_core, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),


	#if _ETHERNET
	DEVFS_DEVICE("eth0", netif_lan8742a, 0, &eth0_config, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif


	DEVFS_DEVICE("i2c0", mcu_i2c, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("i2c1", mcu_i2c, 1, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("i2c2", mcu_i2c, 2, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("i2c3", mcu_i2c, 3, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),

	DEVFS_DEVICE("pio0", mcu_pio, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR), //GPIOA
	DEVFS_DEVICE("pio1", mcu_pio, 1, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR), //GPIOB
	DEVFS_DEVICE("pio2", mcu_pio, 2, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR), //GPIOC
	DEVFS_DEVICE("pio3", mcu_pio, 3, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR), //GPIOD
	DEVFS_DEVICE("pio4", mcu_pio, 4, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR), //GPIOE
	DEVFS_DEVICE("pio5", mcu_pio, 5, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR), //GPIOF
	DEVFS_DEVICE("pio6", mcu_pio, 6, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR), //GPIOG
	DEVFS_DEVICE("pio7", mcu_pio, 7, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR), //GPIOH

	DEVFS_DEVICE("spi0", mcu_spi, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("spi1", mcu_spi, 1, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("spi2", mcu_spi, 2, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("spi3", mcu_spi, 3, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),

	DEVFS_DEVICE("tmr0", mcu_tmr, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("tmr1", mcu_tmr, 1, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("tmr2", mcu_tmr, 2, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("tmr3", mcu_tmr, 3, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("tmr4", mcu_tmr, 4, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("tmr5", mcu_tmr, 5, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_DEVICE("tmr6", mcu_tmr, 6, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#if MCU_TMR_PORTS >= 7
	DEVFS_DEVICE("tmr7", mcu_tmr, 7, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	#if MCU_TMR_PORTS >= 8
	DEVFS_DEVICE("tmr8", mcu_tmr, 8, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	#if MCU_TMR_PORTS >= 9
	DEVFS_DEVICE("tmr9", mcu_tmr, 9, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	#if MCU_TMR_PORTS >= 10
	DEVFS_DEVICE("tmr10", mcu_tmr, 10, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	#if MCU_TMR_PORTS >= 11
	DEVFS_DEVICE("tmr11", mcu_tmr, 11, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	#if MCU_TMR_PORTS >= 12
	DEVFS_DEVICE("tmr12", mcu_tmr, 12, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	#if MCU_TMR_PORTS >= 13
	DEVFS_DEVICE("tmr13", mcu_tmr, 13, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	#if MCU_TMR_PORTS >= 14
	DEVFS_DEVICE("tmr14", mcu_tmr, 14, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	#if MCU_TMR_PORTS >= 15
	DEVFS_DEVICE("tmr10", mcu_tmr, 15, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	#if MCU_TMR_PORTS >= 16
	DEVFS_DEVICE("tmr10", mcu_tmr, 16, 0, 0, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif

	//
	DEVFS_DEVICE("uart1", mcu_uart_dma, 1, &uart1_dma_config, 0, 0666, SOS_USER_ROOT, S_IFCHR),

	#if !defined __debug
	//DEVFS_DEVICE("uart2", uartfifo, 2, &uart2_fifo_config, &uart2_fifo_state, 0666, SOS_USER_ROOT, S_IFCHR),
	#endif
	DEVFS_DEVICE("uart5", uartfifo, 5, &uart5_fifo_config, &uart5_fifo_state, 0666, SOS_USER_ROOT, S_IFCHR),
	DEVFS_TERMINATOR
};


