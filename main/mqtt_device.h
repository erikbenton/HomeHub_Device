#ifndef __MQTT_DEVICE_H__
#define __MQTT_DEVICE_H__

void init_mqtt(void);
int mqtt_device_send(const char *topic, const char *payload, bool retain);
int mqtt_send_device_temperature_value(const char *payload, bool retain);

#endif