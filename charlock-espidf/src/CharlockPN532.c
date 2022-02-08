#include "CharlockPN532.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "CharlockI2C.h"

#define PACKET_BUFFER_SIZE 64
#define PN532_DELAY(ms) vTaskDelay(ms / portTICK_RATE_MS)

/* FreeRTOS task tag for logging */
static const char *s_tag = "CL_NFC";

static const uint8_t __attribute__((unused)) ack_frame[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
static const uint8_t __attribute__((unused)) nack_frame[] = {0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00};

static uint8_t pn532response_firmwarevers[] = {
    0x00, 0xFF, 0x06, 0xFA, 0xD5, 0x03
};
static uint8_t s_packet_buffer[PACKET_BUFFER_SIZE];

static pn532_t nfc = {
    ._clk  = PN532_SCK,
    ._miso = PN532_MISO,
    ._mosi = PN532_MOSI,
    ._ss   = PN532_SS
};

static void pn532_begin(cl_pn532_t *obj) {
    // TODO: Send dummy command and ignore response
}

bool sendCommandCheckAck(uint8_t *data, size_t size, uint8_t timeout) {

    // CREATE I2C COMMAND QUEUE //
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    /* I2C transmission start */
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PN532_I2C_ADDRESS << 1) | CL_WRITE_BIT, false);
    
    /* Send data frame */
    i2c_master_write_byte(cmd, PN532_PREAMBLE, false);    // PREAMBLE
    i2c_master_write_byte(cmd, PN532_STARTCODE_1, false); // STARTCODE1
    i2c_master_write_byte(cmd, PN532_STARTCODE_2, false); // STARTCODE2
    uint8_t len = ((uint8_t)size)+1;
    i2c_master_write_byte(cmd, len, false);               // LEN
    uint8_t lcs = ~len + 1;
    i2c_master_write_byte(cmd, lcs, false);               // LCS
    uint8_t tfi = PN532_HOSTTOPN532;
    i2c_master_write_byte(cmd, tfi, false);               // TFI
    uint8_t dcs = tfi;
    for (size_t i = 0; i < size; i++) {
        i2c_master_write_byte(cmd, data[i], false);
        dcs += data[i];
    }
    dcs = ~dcs + 1;
    i2c_master_write_byte(cmd, dcs, false);               // DCS
    i2c_master_write_byte(cmd, PN532_POSTAMBLE, false);   // POSTAMBLE

    /* I2C transmission stop */
    i2c_master_stop(cmd);

    // SEND I2C QUEUED COMMAND //
    esp_err_t ret = i2c_master_cmd_begin(CL_I2C_MASTER_NUM, cmd, timeout / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    printf("%d\r\n", ret);
    return true;
}

uint32_t pn532_getFirmwareVersion(void) {
    uint32_t response = 1;

    s_packet_buffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;
    if (!sendCommandCheckAck(s_packet_buffer, 1, 100))
        return 0;

    // s_packet_buffer[0] = PN532_HOSTTOPN532;
    // s_packet_buffer[1] = PN532_COMMAND_GETFIRMWAREVERSION;
    // bool ret = cl_i2c_sendCmdCheckAck(PN532_I2C_ADDRESS, s_packet_buffer, 2, 1000);
    // if (!ret)
        // return 0;
    // cl_i2c_readdata(s_packet_buffer, 12);

    return response;
}

static void pn523_SAMConfig(cl_pn532_t *obj) {
    return 0;
}

void cl_nfc_task(void *pvParameters) {
    // pn532_begin(&nfc);

    uint32_t versiondata = pn532_getFirmwareVersion();
    if (!versiondata) {
        ESP_LOGI(s_tag, "Didn't find PN53x board!");
        while (1) {
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }
    ESP_LOGI(s_tag, "Found chip PN5 %x", (versiondata >> 24) & 0xFF);
    ESP_LOGI(s_tag, "Firmware ver. %d%d", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);
    
    // pn532_SAMConfig(&nfc);

    ESP_LOGI(s_tag, "Waiting for an ISO14443A card ...");

    while (1)
    {
        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

        // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
        // 'uid' will be populated with the UID, and uidLength will indicate
        // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
        success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 0);

        if (success)
        {
            // Display some basic information about the card
            ESP_LOGI(s_tag, "Found an ISO14443A card");
            ESP_LOGI(s_tag, "UID Length: %d bytes", uidLength);
            ESP_LOGI(s_tag, "UID Value:");
            esp_log_buffer_hexdump_internal(s_tag, uid, uidLength, ESP_LOG_INFO);   
            vTaskDelay(1000 / portTICK_RATE_MS);         
        }
        else
        {
            // PN532 probably timed out waiting for a card
            ESP_LOGI(s_tag, "Timed out waiting for a card");
        }
    }
}
