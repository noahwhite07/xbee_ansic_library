#ifndef __XBEE_PLATFORM_ESP32
#define __XBEE_PLATFORM_ESP32

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define _f_memcpy memcpy
#define _f_memset memset

#define LITTLE_ENDIAN 4321
#define BIG_ENDIAN 1234
#define BYTE_ORDER LITTLE_ENDIAN

#define PACKED_STRUCT     struct __attribute__ ((__packed__))
#define XBEE_PACKED(name, decl) PACKED_STRUCT name decl

typedef uint8_t bool_t;

typedef struct xbee_serial_t {
    uart_port_t     uart_num;
    uint32_t        baudrate;
} xbee_serial_t;

#define XBEE_MS_TIMER_RESOLUTION 1
#define ZCL_TIME_EPOCH_DELTA 0

int xbee_platform_init(void);
#define XBEE_PLATFORM_INIT() xbee_platform_init()

#endif /* __XBEE_PLATFORM_ESP32 */
