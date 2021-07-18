
#include <sos/dev/netif.h>
#include <sos/fs/sysfs.h>

#include <lwip_api.h>

#include "lwip_config.h"


#define LWIP_MAX_PACKET_SIZE 1520
#define NETWORK_INTERFACE_COUNT 1

extern const sysfs_t sysfs_list[];

struct netif lwip_netif_list[NETWORK_INTERFACE_COUNT];
sysfs_shared_state_t lwip_wifi_device_state[NETWORK_INTERFACE_COUNT];
u8 lwip_packet_buffer[LWIP_MAX_PACKET_SIZE];

void lwip_netif_status_callback(struct netif *netif);


const lwip_api_netif_config_t lwip_wifi_netif[NETWORK_INTERFACE_COUNT] = {
    {.device_config = {.devfs = &sysfs_list[1],
                       .name = "eth0",
                       .state = lwip_wifi_device_state},
     .lwip_netif = lwip_netif_list,
     .packet_buffer = lwip_packet_buffer,
     .packet_buffer_size = LWIP_MAX_PACKET_SIZE,
     .host_name = "Nucleo-F746ZG",
     .netif_status_callback = lwip_netif_status_callback}};

const lwip_api_config_t lwip_config = {.netif_config = lwip_wifi_netif,
                                       .netif_config_count =
                                           NETWORK_INTERFACE_COUNT};

LWIP_DECLARE_SOCKET_API(lwip, &lwip_config, NULL);

void lwip_netif_status_callback(struct netif *netif){
  const u32 addr = netif->ip_addr.u_addr.ip4.addr;
  sos_debug_log_info(SOS_DEBUG_SOCKET, "Netif update %d.%d.%d.%d\n",
                     addr & 0xff,
                     (addr >> 8) & 0xff,
                     (addr >> 16) & 0xff,
                     (addr >> 24) & 0xff
                     );
}
