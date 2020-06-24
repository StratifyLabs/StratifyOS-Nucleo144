
#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include <device/ffifo.h>
#include <mcu/debug.h>

void board_trace_event(void * event);

extern const ffifo_config_t board_trace_config;
extern ffifo_state_t board_trace_state;

void board_event_handler(int event, void * args); //optional event handler


#endif /* BOARD_CONFIG_H_ */
