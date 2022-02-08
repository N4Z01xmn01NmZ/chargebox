#ifndef CL_I2C_H
#define CL_I2C_H

#pragma once

#include <esp_err.h>
#include <driver/gpio.h>
#include <driver/i2c.h>

#define CL_SCL_IO         GPIO_NUM_21
#define CL_SDA_IO         GPIO_NUM_22

// #define CL_HSPI_CLK_IO    GPIO_NUM_14
// #define CL_HSPI_MISO_IO   GPIO_NUM_12
// #define CL_HSPI_MOSI_IO   GPIO_NUM_13
// #define CL_HSPI_CSO_IO    GPIO_NUM_15

// #define CL_VSPI_CLK_IO    GPIO_NUM_18
// #define CL_VSPI_MISO_IO   GPIO_NUM_19
// #define CL_VSPI_MOSI_IO   GPIO_NUM_23
// #define CL_VSPI_CSO_IO    GPIO_NUM_5

#define CL_I2C_MASTER_NUM            I2C_NUM_0
#define CL_I2C_MASTER_FREQ_HZ        200000
#define CL_I2C_MASTER_TX_BUF_DISABLE 0
#define CL_I2C_MASTER_RX_BUF_DISABLE 0 
#define CL_I2C_MASTER_TIMEOUT_MS     1000

#define CL_WRITE_BIT                 I2C_MASTER_WRITE
#define CL_READ_BIT                  I2C_MASTER_READ
#define CL_NACK                  0x0 
#define CL_ACK                   0x1

#ifdef __cplusplus
extern "C" {
#endif

bool cl_i2c_sendCmdCheckAck(uint8_t address, uint8_t *data, size_t size, uint16_t timeout);
esp_err_t cl_i2c_master_driver_init(void);
void cl_i2c_detect(void);

#ifdef __cplusplus
}
#endif

#endif // CL_I2C_H