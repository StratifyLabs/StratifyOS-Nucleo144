#ifndef LWIP_CONFIG_H
#define LWIP_CONFIG_H

#if INCLUDE_ETHERNET

#include <lwip/lwip_api.h>
#include <sos/sos.h>

const sos_socket_api_t lwip_api;
#endif

#endif // LWIP_CONFIG_H
