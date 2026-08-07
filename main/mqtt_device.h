#ifndef __MQTT_DEVICE_H__
#define __MQTT_DEVICE_H__

void init_mqtt(void);
int mqtt_send(const char *topic, const char *payload, bool retain);

#endif