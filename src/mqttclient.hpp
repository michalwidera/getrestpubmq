#ifndef GRPM_MQTT_CLIENT_HPP
#define GRPM_MQTT_CLIENT_HPP

#include <string>
#include <vector>

typedef std::vector<std::string> Payload_t;

void shipPayloadToMqttBus(Payload_t);

#endif //GRPM_MQTT_CLIENT_HPP
