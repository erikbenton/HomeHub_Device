#include "i2c_lm75a.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

static i2c_master_dev_handle_t i2c_dev_handle; /* I2C device handle */
static float factor = 0.125; // for temperature calculations

// needed for reading sensor data
static uint8_t write_buffer_to_read[] = {0x00};
static uint8_t read_buffer[2];

void init_i2c_lm75a(void)
{
    // Configure I2C temp sensor
    i2c_master_bus_handle_t bus_handle;

    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_TEMPERATURE_NUM,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LM75A_ADDRESS,
        .scl_speed_hz = I2C_TEMPERATURE_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &i2c_dev_handle));
}

esp_err_t get_temperature_f(float *temperature)
{
    // clear the read buffer
    memset(read_buffer, 0, sizeof(read_buffer));
    esp_err_t i2c_res = ESP_ERROR_CHECK_WITHOUT_ABORT(
        i2c_master_transmit_receive(
            i2c_dev_handle,
            write_buffer_to_read,
            sizeof(write_buffer_to_read),
            read_buffer,
            1,
            -1));

    if (i2c_res == ESP_OK)
    {
        if (read_buffer[0] & IS_NEG_TEMP)
        {
            read_buffer[0] = read_buffer[0] & ~IS_NEG_TEMP;
            factor = -factor;
        }
        int16_t data = read_buffer[0] << 8 | read_buffer[1];
        float temp_celsius = (data >> 5U) * factor;
        *temperature = temp_celsius * 1.8 + 32; // update the temperature
    }

    return i2c_res;
}