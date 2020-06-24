
#include <sapi/sys/requests.h>
#include <sos/api/crypt_api.h>
#include <mbedtls_api.h>

#include "config.h"

#include <arm_dsp_api_declaration.h>


int kernel_request(int req, void * arg){
	return -1;
}

const void * kernel_request_api(u32 request){
	switch(request){
		case CRYPT_SHA256_API_REQUEST: return &mbedtls_crypt_sha256_api;
	}
	return 0;
}
