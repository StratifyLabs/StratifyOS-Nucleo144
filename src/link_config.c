

#include <fcntl.h>
#include <sos/debug.h>
#include <sos/dev/pio.h>
#include <sos/sos_config.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "link_config.h"

#if !defined SOS_BOARD_RX_FIFO_WORDS
#define SOS_BOARD_RX_FIFO_WORDS 128
#endif

#if !defined SOS_BOARD_TX0_FIFO_WORDS
#define SOS_BOARD_TX0_FIFO_WORDS 32
#endif

#if !defined SOS_BOARD_TX1_FIFO_WORDS
#define SOS_BOARD_TX1_FIFO_WORDS 32
#endif

#if !defined SOS_BOARD_TX2_FIFO_WORDS
#define SOS_BOARD_TX2_FIFO_WORDS 32
#endif

#if !defined SOS_BOARD_TX3_FIFO_WORDS
#define SOS_BOARD_TX3_FIFO_WORDS 64
#endif

#if !defined SOS_BOARD_TX4_FIFO_WORDS
#define SOS_BOARD_TX4_FIFO_WORDS 0
#endif

#if !defined SOS_BOARD_TX5_FIFO_WORDS
#define SOS_BOARD_TX5_FIFO_WORDS 0
#endif

#if !defined SOS_BOARD_USB_DP_PIN
#define SOS_BOARD_USB_DP_PIN mcu_pin(0, 11)
#endif

#if !defined SOS_BOARD_USB_DM_PIN
#define SOS_BOARD_USB_DM_PIN mcu_pin(0, 12)
#endif

static link_transport_phy_t link_transport_open(const char *name,
                                                const void *options);

link_transport_driver_t link_transport = {
    .handle = -1,
    .open = link_transport_open,
    .read = sos_link_transport_usb_read,
    .write = sos_link_transport_usb_write,
    .close = sos_link_transport_usb_close,
    .wait = sos_link_transport_usb_wait,
    .flush = sos_link_transport_usb_flush,
    .timeout = 500,
    .o_flags = 0, // no checksums -- checksums already happen over USB
    .transport_read = link1_transport_slaveread,
    .transport_write = link1_transport_slavewrite};

static usbd_control_t m_usb_control;

#if defined SOS_BOARD_USB_PORT
const usbd_control_constants_t link_transport_usb_constants = {
    .handle.port = SOS_BOARD_USB_PORT,
    .handle.config = 0,
    .handle.state = 0,
    .device = &sos_link_transport_usb_dev_desc,
    .config = &sos_link_transport_usb_cfg_desc,
    .string = &sos_link_transport_usb_string_desc,
    .class_event_handler = sos_link_usbd_cdc_event_handler};
#else
#define link_transport_usb_constants sos_link_transport_usb_link_constants
#endif

link_transport_phy_t link_transport_open(const char *name,
                                         const void *options) {
  usb_attr_t usb_attr;
  link_transport_phy_t fd;
  MCU_UNUSED_ARGUMENT(options);

  // set up the USB attributes
  memset(&(usb_attr.pin_assignment), 0xff, sizeof(usb_pin_assignment_t));
#if defined SOS_BOARD_USB_ATTR_FLAGS
  usb_attr.o_flags = SOS_BOARD_USB_ATTR_FLAGS;
#else
  usb_attr.o_flags = USB_FLAG_SET_DEVICE;
#endif
  usb_attr.pin_assignment.dp = SOS_BOARD_USB_DP_PIN;
  usb_attr.pin_assignment.dm = SOS_BOARD_USB_DM_PIN;
  usb_attr.freq = sos_config.clock.frequency;
  memset(usb_attr.tx_fifo_word_size, 0, USB_TX_FIFO_WORD_SIZE_COUNT);
  usb_attr.rx_fifo_word_size =
      SOS_BOARD_RX_FIFO_WORDS; // RX fifo for all endpoints
  usb_attr.tx_fifo_word_size[0] = SOS_BOARD_TX0_FIFO_WORDS; // TX endpoint 0
  usb_attr.tx_fifo_word_size[1] = SOS_BOARD_TX1_FIFO_WORDS; // TX endpoint 1
  usb_attr.tx_fifo_word_size[2] = SOS_BOARD_TX2_FIFO_WORDS; // TX endpoint 2
  usb_attr.tx_fifo_word_size[3] = SOS_BOARD_TX3_FIFO_WORDS; // TX endpoint 3
  usb_attr.tx_fifo_word_size[4] = SOS_BOARD_TX4_FIFO_WORDS; // TX endpoint 4
  usb_attr.tx_fifo_word_size[5] = SOS_BOARD_TX5_FIFO_WORDS; // TX endpoint 5

  sos_debug_log_info(SOS_DEBUG_USER1, "Open USB");

  fd = sos_link_transport_usb_open(name, &m_usb_control,
                                   &link_transport_usb_constants, &usb_attr,
                                   mcu_pin(0xff, 0xff),
                                   1); // USB pin is active high

  sos_debug_log_info(SOS_DEBUG_USER1, "Returned %d", fd);

  return fd;
}
