
#ifndef __I2C_LM75A_H__
#define __I2C_LM75A_H__

#include "esp_err.h"
#include "driver/i2c_master.h"

// I2C Temp sensor pins/config
#define I2C_MASTER_SCL_IO 4            /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 5            /*!< GPIO number used for I2C master data  */
#define I2C_TEMPERATURE_NUM I2C_NUM_0  /*!< I2C port number for master dev */
#define I2C_TEMPERATURE_FREQ_HZ 400000 /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0    /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0    /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000
#define LM75A_ADDRESS 0x48
#define IS_NEG_TEMP (1 << 8U)

void init_i2c_lm75a(void);
esp_err_t get_temperature_f(float *temperature);

#endif