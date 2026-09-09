#include "esp_log.h"
#include "driver/i2c_master.h"
#include "../interfaces/temperature_sensor_interface.h"
#include "../drivers/lm75a_temperature_sensor_driver.h"
#include "../contexts/lm75a_context.h"

static float factor = 0.125; // for temperature calculations
static const char *TAG = "LM75A";

static esp_err_t init_i2c_lm75a(temperature_sensor_t *self)
{
    lm75a_ctx_t *ctx = (lm75a_ctx_t *)self->ctx;

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
        .device_address = ctx->i2c_address,
        .scl_speed_hz = I2C_TEMPERATURE_FREQ_HZ,
    };

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        i2c_master_bus_add_device(bus_handle, &dev_config, &(ctx->i2c_dev_handle)));

    if (ctx->i2c_dev_handle == NULL)
    {
        ESP_LOGE(TAG, "i2c dev handle is null");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t read_fahrenheit(temperature_sensor_t *self, float *temperature)
{
    lm75a_ctx_t *ctx = (lm75a_ctx_t *)self->ctx;

    // clear the read buffer
    memset(ctx->read_buffer, 0, sizeof(ctx->read_buffer));

    //
    esp_err_t i2c_res = ESP_ERROR_CHECK_WITHOUT_ABORT(
        i2c_master_transmit_receive(
            ctx->i2c_dev_handle,
            ctx->write_buffer_to_read,
            sizeof(ctx->write_buffer_to_read),
            ctx->read_buffer,
            1,
            -1));

    if (i2c_res == ESP_OK)
    {
        if (ctx->read_buffer[0] & IS_NEG_TEMP)
        {
            ctx->read_buffer[0] = ctx->read_buffer[0] & ~IS_NEG_TEMP;
            factor = -factor;
        }
        int16_t data = ctx->read_buffer[0] << 8 | ctx->read_buffer[1];
        float temp_celsius = (data >> 5U) * factor;
        *temperature = temp_celsius * 1.8 + 32; // update the temperature
    }

    return i2c_res;
}

void lm75a_create(temperature_sensor_t *sensor, lm75a_ctx_t *ctx)
{
    ctx->write_buffer_to_read[0] = 0x00;
    sensor->ctx = ctx;
    sensor->init = init_i2c_lm75a;
    sensor->read_fahrenheit = read_fahrenheit;
}