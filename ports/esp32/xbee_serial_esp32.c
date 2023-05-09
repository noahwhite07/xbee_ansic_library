
#include "xbee/platform.h"
#include "xbee/serial.h"
#include "xbee/serial.h"
#include "esp_err.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


// Sending and receiving serial data
// =================================================================================== 
int xbee_ser_write( xbee_serial_t *serial, const void FAR *buffer, int length){
    if (serial == NULL || xbee_ser_invalid(serial) || buffer == NULL) {
        return -EINVAL;
    }

    int bytes_written = uart_write_bytes(serial->uart_num, (const char *)buffer, length);
    if (bytes_written < 0) {
        return -EIO;
    }

    return bytes_written;
}

int xbee_ser_read( xbee_serial_t *serial, void FAR *buffer, int bufsize){
    if (serial == NULL || xbee_ser_invalid(serial) || buffer == NULL) {
        return -EINVAL;
    }

    int bytes_read = 0;
    int available = uart_read_bytes(serial->uart_num, (uint8_t *)buffer, bufsize, 10 / portTICK_RATE_MS);
    if (available > 0) {
        bytes_read = available;
    } else if (available < 0) {
        return -EIO;
    }

    return bytes_read;
}

int xbee_ser_putchar( xbee_serial_t *serial, uint8_t ch){
    if (serial == NULL || xbee_ser_invalid(serial)) {
        return -EINVAL;
    }

    // Try to write the character to the UART buffer with a short timeout
    int result = uart_write_bytes(serial->uart_num, (const char *)&ch, 1);

    if (result == 1) {
        return 0; // Successfully sent (queued) character
    } else {
        return -ENOSPC; // The write buffer is full and the character wasn't sent
    }
}

int xbee_ser_getchar( xbee_serial_t *serial){
    if (serial == NULL || xbee_ser_invalid(serial)) {
        return -EINVAL;
    }

    uint8_t ch;
    int result = uart_read_bytes(serial->uart_num, &ch, 1, 10 / portTICK_RATE_MS);

    if (result == 1) {
        return ch; // Return the character read from the XBee serial port
    } else {
        return -ENODATA; // There aren't any characters in the read buffer
    }
}
// ===================================================================================


// Checking the status of transmit and receive buffers
// ===================================================================================
int xbee_ser_tx_free( xbee_serial_t *serial){
    return 100;
}

int xbee_ser_tx_used( xbee_serial_t *serial){
    return 0;
}

int xbee_ser_tx_flush( xbee_serial_t *serial){
    // do nothing
}

int xbee_ser_rx_free( xbee_serial_t *serial){
    return 100;
}

int xbee_ser_rx_used( xbee_serial_t *serial){
    return 0;

}

int xbee_ser_rx_flush( xbee_serial_t *serial){
    // do nothing
}
// ===================================================================================


// Managing the serial port and control lines
// ===================================================================================

int xbee_ser_open(xbee_serial_t *serial, uint32_t baudrate) {
    if (serial == NULL) {
        return -EINVAL;
    }

    // Custom config, change this later
    serial->uart_num = 2;
    const uint8_t TX_PIN = 17;
    const uint8_t RX_PIN = 16; 

    // UART configuration
    uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    // Configure and install the UART driver
    esp_err_t err = uart_param_config(serial->uart_num, &uart_config);
    if (err != ESP_OK) {
        return -EIO;
    }

    // Set up UART pins
    // Replace TX_PIN and RX_PIN with actual GPIO numbers for your hardware setup
    err = uart_set_pin(serial->uart_num, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        return -EIO;
    }

    // Install UART driver
    err = uart_driver_install(serial->uart_num, 256, 256, 0, NULL, 0);
    if (err != ESP_OK) {
        return -EIO;
    }

    // Update the baudrate field in the xbee_serial_t structure
    serial->baudrate = baudrate;

    return 0;
}



int xbee_ser_baudrate(xbee_serial_t *serial, uint32_t baudrate) {
    if (serial == NULL || xbee_ser_invalid(serial)) {
        return -EINVAL;
    }

    esp_err_t err = uart_set_baudrate(serial->uart_num, baudrate);
    if (err == ESP_OK) {
        serial->baudrate = baudrate;
        return 0;
    } else if (err == ESP_ERR_INVALID_ARG) {
        return -EINVAL;
    } else {
        return -EIO;
    }
}


int xbee_ser_close(xbee_serial_t *serial) {
    if (serial == NULL || xbee_ser_invalid(serial)) {
        return -EINVAL;
    }

    esp_err_t err = uart_driver_delete(serial->uart_num);
    if (err == ESP_OK) {
        serial->uart_num = UART_NUM_MAX; // Set to an invalid UART number to mark as closed.
        return 0;
    } else if (err == ESP_ERR_INVALID_ARG) {
        return -EINVAL;
    } else {
        return -EIO;
    }
}

int xbee_ser_break( xbee_serial_t *serial, bool_t enabled){
    // do nothing
}

int xbee_ser_flowcontrol( xbee_serial_t *serial, bool_t enabled){
    // do nothing
}

int xbee_ser_set_rts( xbee_serial_t *serial, bool_t asserted){
    // do nothing
}

int xbee_ser_get_cts( xbee_serial_t *serial){
    return 1;
}

bool_t xbee_ser_invalid( xbee_serial_t *serial){
    return 0;
}
 
const char *xbee_ser_portname( xbee_serial_t *serial){
    static const char *uart_port_names[] = {
        "UART0",
        "UART1",
        "UART2"
    };

    if (serial == NULL || xbee_ser_invalid(serial) || serial->uart_num >= UART_NUM_MAX) {
        return "(invalid)";
    }

    return uart_port_names[serial->uart_num];
}
// ===================================================================================