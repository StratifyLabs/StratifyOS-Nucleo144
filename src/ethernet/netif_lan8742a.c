
#include <mcu/debug.h>

#include "netif_lan8742a.h"

#define STATUS_REGISTER 1
#define STATUS_IS_LINKED(x) ((x & 0x0004U) == 0x0004U)


#define STATE_UNINITIALIZED 0
#define STATE_CHIP_UP 1
#define STATE_LINK_UP 2

static int initialize_ethernet(const devfs_handle_t * handle){
	netif_lan8742a_state_t * state = handle->state;

	if( state->is_initialized == STATE_LINK_UP ){ return 0; }

	const eth_config_t * eth_config = handle->config;

	if( eth_config == NULL ){
		mcu_debug_log_fatal(MCU_DEBUG_DEVICE, "ethernet not configured");
		return SYSFS_SET_RETURN(ENOTSUP);
	}

	int result;

	if( state->is_initialized == STATE_UNINITIALIZED ){
		eth_attr_t attr;
		memcpy(&attr, &eth_config->attr, sizeof(eth_attr_t));

		//first try without auto negociation
		attr.o_flags &= ~ETH_FLAG_IS_AUTONEGOTIATION_ENABLED;
		result = mcu_eth_setattr(handle, &attr);
		if( result < 0 ){
			return result;
		}
		mcu_debug_printf("chip is up\n");
		state->is_initialized = STATE_CHIP_UP;
	}

	//check it link is up
	mcu_channel_t eth_register;
	eth_register.loc = STATUS_REGISTER; //status regsiter
	if( mcu_eth_getregister(handle, &eth_register) < 0 ){
		return SYSFS_SET_RETURN(EIO);
	}

	if( STATUS_IS_LINKED(eth_register.loc) == 0 ){
		//can't connect now
		return 0;
	}

	result = mcu_eth_setattr(handle, NULL);
	if( result < 0 ){
		return 0;
	}

	state->is_initialized = STATE_LINK_UP;
	return 0;

}

int netif_lan8742a_open(const devfs_handle_t * handle){
	return mcu_eth_open(handle);
}

int netif_lan8742a_ioctl(const devfs_handle_t * handle, int request, void * ctl){
	netif_attr_t * netif_attr = ctl;
	netif_info_t * netif_info = ctl;
	netif_lan8742a_state_t * state = handle->state;
	mcu_channel_t eth_register;
	eth_attr_t attr;
	u32 o_flags;
	switch(request){
		case I_NETIF_GETVERSION: return NETIF_VERSION;
		case I_NETIF_SETATTR:
			o_flags = netif_attr->o_flags;
			if(o_flags & NETIF_FLAG_INIT ){
				//initialize the interface
				return initialize_ethernet(handle);
			}

			//restart after having been stopped
			if( o_flags & NETIF_FLAG_SET_LINK_UP ){

				if( state->is_initialized == 0 ){
					//set attributes using system provided configuration
					initialize_ethernet(handle);
				}

				eth_register.loc = 0; //status regsiter
				if( mcu_eth_getregister(handle, &eth_register) < 0 ){ return SYSFS_SET_RETURN(EIO); }
				eth_register.value &= ~1<<11; //Set power down mode
				if( mcu_eth_setregister(handle, &eth_register) < 0 ){ return SYSFS_SET_RETURN(EIO); }

				attr.o_flags = ETH_FLAG_STOP;
				if( mcu_eth_setattr(handle, &attr) < 0 ){ return SYSFS_SET_RETURN(EIO); }
			}

			if( o_flags & NETIF_FLAG_IS_LINK_UP ){

				if( state->is_initialized == 0 ){
					//set attributes using system provided configuration
					initialize_ethernet(handle);
				}

				if( state->is_initialized == 0 ){
					mcu_debug_log_info(MCU_DEBUG_USER2, "Not initialized");
					return 0;
				}

				eth_register.loc = 1; //status regsiter
				if( mcu_eth_getregister(handle, &eth_register) < 0 ){
					state->is_initialized = 0;
					return SYSFS_SET_RETURN(EIO);
				}

				//mcu_debug_log_info(MCU_DEBUG_USER2, "Status register 0x%lX", eth_register.value);
				return (eth_register.value & (1<<2)) != 0;
			}

			if( o_flags & NETIF_FLAG_SET_LINK_DOWN ){
				mcu_channel_t eth_register;
				eth_register.loc = 0; //status regsiter
				if( mcu_eth_getregister(handle, &eth_register) < 0 ){
					state->is_initialized = 0;
					return SYSFS_SET_RETURN(EIO);
				}
				eth_register.value |= ~1<<11; //Set power down mode
				if( mcu_eth_setregister(handle, &eth_register) < 0 ){
					state->is_initialized = 0;
					return SYSFS_SET_RETURN(EIO);
				}

				attr.o_flags = ETH_FLAG_STOP;
				if( mcu_eth_setattr(handle, &attr) < 0 ){
					state->is_initialized = 0;
					return SYSFS_SET_RETURN(EIO);
				}
			}


			break;


		case I_NETIF_GETINFO:

			netif_info->o_flags = NETIF_FLAG_INIT |
					NETIF_FLAG_DEINIT |
					NETIF_FLAG_IS_LINK_UP |
					NETIF_FLAG_SET_LINK_DOWN |
					NETIF_FLAG_SET_LINK_UP;

			netif_info->o_events = MCU_EVENT_FLAG_DATA_READY | MCU_EVENT_FLAG_WRITE_COMPLETE;

			break;
	}



	return mcu_eth_ioctl(handle, request, ctl);

}

int netif_lan8742a_read(const devfs_handle_t * handle, devfs_async_t * async){
	return mcu_eth_read(handle, async);
}

int netif_lan8742a_write(const devfs_handle_t * handle, devfs_async_t * async){
	return mcu_eth_write(handle, async);
}

int netif_lan8742a_close(const devfs_handle_t * handle){
	return mcu_eth_close(handle);
}
