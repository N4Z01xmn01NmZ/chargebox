#include "CharlockI2C.h"

#include <esp_console.h>

static i2c_port_t i2c_master_port = CL_I2C_MASTER_NUM;

esp_err_t cl_i2c_master_driver_init(void) {
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = CL_SCL_IO,
        .scl_io_num = CL_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = CL_I2C_MASTER_FREQ_HZ
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &config);
    if (err != ESP_OK)
        return err;
    return i2c_driver_install(
        i2c_master_port,
        I2C_MODE_MASTER,
        CL_I2C_MASTER_RX_BUF_DISABLE,
        CL_I2C_MASTER_TX_BUF_DISABLE,
        0
    );
}

bool cl_i2c_sendCmdCheckAck(uint8_t address, uint8_t *data, size_t size, uint16_t timeout) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | CL_WRITE_BIT, CL_ACK);
    
    uint8_t checksum = 0x00 + 0x00 + 0xFF;
    i2c_master_write_byte(cmd, 0x00, CL_NACK);
    i2c_master_write_byte(cmd, 0x00, CL_NACK);
    i2c_master_write_byte(cmd, 0xFF, CL_NACK);

    i2c_master_write_byte(cmd, 0x01, CL_NACK);
    i2c_master_write_byte(cmd, ~(0x01+1), CL_NACK);

    checksum += 0xD4;
    i2c_master_write_byte(cmd, 0xD4, CL_NACK);

    checksum += 0x02;
    i2c_master_write_byte(cmd, 0x02, CL_NACK);

    i2c_master_write_byte(cmd, ~checksum, CL_NACK);
    i2c_master_write_byte(cmd, 0x00, CL_ACK);
    // i2c_master_write(cmd, data, size, CL_ACK);
    i2c_master_stop(cmd);
    // printf("%02x %02x %02x %02x %02x\r\n", ((uint8_t*)cmd)[0], ((uint8_t*)cmd)[1], ((uint8_t*)cmd)[2], ((uint8_t*)cmd)[3], ((uint8_t*)cmd)[4]);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, timeout / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret == ESP_OK){

        return true;
    }
    else if (ret ==  ESP_ERR_TIMEOUT) {
        printf("error I2C timeout\r\n");
        return false;
    }
    else {

        return false;
    } 
}

esp_err_t cl_i2c_master_write_slave(uint8_t addr, uint8_t *data_wr, size_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | CL_WRITE_BIT, CL_ACK);
    i2c_master_write(cmd, data_wr, size, CL_ACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t cl_i2c_master_read(uint8_t addr, uint8_t *data, size_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | CL_WRITE_BIT, CL_NACK);
    i2c_master_write(cmd, data, size, CL_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void cl_i2c_detect(void) {
    uint8_t addr;
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
    for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
            fflush(stdout);
            addr = i + j;
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (addr << 1) | CL_WRITE_BIT, CL_ACK);
            i2c_master_stop(cmd);
            esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 50 / portTICK_RATE_MS);
            i2c_cmd_link_delete(cmd);
            if (ret == ESP_OK)
                printf("%02x ", addr);
            else if (ret ==  ESP_ERR_TIMEOUT)
                printf("UU ");
            else
                printf("-- ");
        }
        printf("\r\n");
    }
}
