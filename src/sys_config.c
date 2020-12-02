
#include <mcu/arch/stm32/stm32_config.h>

#include "board_config.h"

extern void SystemClock_Config();

void sys_initialize() {
  stm32_initialize();
  SystemClock_Config();
  stm32_initialize_systick();
}

int sys_kernel_request(int req, void *arg) { return -1; }

const void *sys_kernel_request_api(u32 request) {
  switch (request) {
#if INCLUDE_AUTH
  case CRYPT_SHA256_API_REQUEST:
    return &mbedtls_crypt_sha256_api;
#endif
#if INCLUDE_ETHERNET && INCLUDE_TLS
  case MBEDTLS_API_REQUEST:
    return &mbedtls_api; // about 200KB
#endif
#if INCLUDE_JANSSON_API
  case JANSSON_API_REQUEST:
    return &jansson_api; // about 20KB
#endif
  }
  return 0;
}
