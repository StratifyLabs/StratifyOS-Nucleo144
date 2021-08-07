
#include <sdk/api.h>
#include <stm32_config.h>

#include <device/auth.h>

#include <micro_ecc_api.h>
#include <tinycrypt_api.h>

#include "config.h"

// use `sl keys.ping:id=162ZEPiD33bF1T8diV0t,code` to get the C style format
const appfs_public_key_t public_keys[] = {
    {.index = 0,
     .id = "162ZEPiD33bF1T8diV0t",
     .value = {0x3b, 0x79, 0x1c, 0x4e, 0x97, 0x9e, 0x19, 0xbf, 0x34, 0x65, 0xba,
               0xd7, 0x39, 0x08, 0xa4, 0x7b, 0x3c, 0x24, 0x3e, 0xde, 0x3c, 0x34,
               0x89, 0x24, 0x54, 0x22, 0x0b, 0xcb, 0xb8, 0xc4, 0xb9, 0x5c, 0x23,
               0xd5, 0x71, 0x58, 0xf2, 0x28, 0xfe, 0x1a, 0xb5, 0xa8, 0x35, 0xfe,
               0xa5, 0x5a, 0xb9, 0x0d, 0x02, 0xc1, 0x32, 0x48, 0x3d, 0xea, 0x71,
               0x74, 0x14, 0x67, 0x50, 0xdd, 0xaa, 0x41, 0x2d, 0x2a},
     //allow all APPFS flags when installing: e.g. APPFS_FLAG_IS_AUTHENTICATED
     .o_flags = 0xffffffff}};

extern void SystemClock_Config();

void sys_initialize() {
  stm32_initialize();
  SystemClock_Config();
  stm32_initialize_systick();
}

void sys_get_serial_number(mcu_sn_t *serial_number) {
  stm32_get_serial_number(serial_number);
}

int sys_kernel_request(int req, void *arg) {
  MCU_UNUSED_ARGUMENT(req);
  MCU_UNUSED_ARGUMENT(arg);
  return -1;
}

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
#if _IS_BOOT == 0
  case CRYPT_ECC_API_REQUEST:
    return &micro_ecc_api;
  case CRYPT_SHA256_API_REQUEST:
    return &tinycrypt_sha256_api;
#endif

  case CRYPT_RANDOM_ROOT_API_REQUEST:
    return &random_root_api;
  }
  return 0;
}

void sys_pio_set_attributes(int port, const pio_attr_t *attr) {
  stm32_pio_set_attributes(port, attr);
}

void sys_pio_write(int port, u32 mask, int value) {
  stm32_pio_write(port, mask, value);
}

u32 sys_pio_read(int port, u32 mask) { return stm32_pio_read(port, mask); }

int sys_get_public_key(u32 index, appfs_public_key_t *dest) {
  if (index < sizeof(public_keys) / sizeof(appfs_public_key_t)) {
    *dest = public_keys[index];
    return 0;
  }

  *dest = (appfs_public_key_t){};
  return -1;
}
