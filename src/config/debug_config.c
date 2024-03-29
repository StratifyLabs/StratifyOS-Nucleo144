
#include <cortexm/task.h>
#include <sos/debug.h>
#include <sos/sos.h>
#include <stm32_config.h>
#include <stm32_types.h>

#include "debug_config.h"

#if ___debug
static UART_HandleTypeDef m_huart3;
#endif

#define DEBUG_LED_PORT GPIOB
#define DEBUG_LED_PINMASK (1 << 7)

void debug_initialize() {

  // set LED 1,7 as output
  GPIO_InitTypeDef gpio_init;
  gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init.Pin = DEBUG_LED_PINMASK;
  gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DEBUG_LED_PORT, &gpio_init);
  debug_disable_led();

#if ___debug
  __HAL_RCC_USART3_CLK_ENABLE();

  /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
  gpio_init.Pin = (1 << 8) | (1 << 9);
  gpio_init.Mode = GPIO_MODE_AF_PP;
  gpio_init.Pull = GPIO_PULLUP;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOD, &gpio_init);

  m_huart3.Instance = USART3;
  m_huart3.Init.BaudRate = 400000;
  m_huart3.Init.WordLength = UART_WORDLENGTH_8B;
  m_huart3.Init.StopBits = UART_STOPBITS_1;
  m_huart3.Init.Parity = UART_PARITY_NONE;
  m_huart3.Init.Mode = UART_MODE_TX_RX;
  m_huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  m_huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&m_huart3);
#endif
  //debug_write("hey1\n", 5);
  //sos_debug_printf("Hello %p\n", &sos_config);
}

void debug_write(const void *buf, int nbyte) {
#if ___debug
  const char *cbuf = buf;
  for (int i = 0; i < nbyte; i++) {
    u8 c = cbuf[i];
    HAL_UART_Transmit(&m_huart3, &c, 1, HAL_MAX_DELAY);
  }
#endif
}

void debug_enable_led() {
  HAL_GPIO_WritePin(DEBUG_LED_PORT, DEBUG_LED_PINMASK, GPIO_PIN_SET);
}

void debug_disable_led() {
  HAL_GPIO_WritePin(DEBUG_LED_PORT, DEBUG_LED_PINMASK, GPIO_PIN_RESET);
}

#define TRACE_COUNT 16
#define TRACE_FRAME_SIZE sizeof(link_trace_event_t)
#define TRACE_BUFFER_SIZE (sizeof(link_trace_event_t) * TRACE_COUNT)
static char trace_buffer[TRACE_FRAME_SIZE * TRACE_COUNT];
const ffifo_config_t debug_trace_config = {.frame_count = TRACE_COUNT,
                                           .frame_size =
                                               sizeof(link_trace_event_t),
                                           .buffer = trace_buffer};
ffifo_state_t debug_trace_state;

void debug_trace_event(void *event) {
  link_trace_event_header_t *header = event;
  devfs_async_t async;
  const devfs_device_t *trace_dev = &(sos_config.fs.devfs_list[0]);

  // write the event to the fifo
  memset(&async, 0, sizeof(devfs_async_t));
  async.tid = task_get_current();
  async.buf = event;
  async.nbyte = header->size;
  async.flags = O_RDWR;
  trace_dev->driver.write(&(trace_dev->handle), &async);
}
