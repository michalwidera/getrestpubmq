#include "mqtt/async_client.h"
#include "mqttclient.hpp"

#include <stdexcept>

// MQTT ship data - source of this process could be found here:
// https://github.com/eclipse/paho.mqtt.cpp/blob/master/src/samples/topic_publish.cpp

void shipPayloadToMqttBus(Payload_t payload)
{
    const std::string address{"tcp://localhost:1883"};
    const int QOS = 1;

    mqtt::async_client cli(address, "");

    try
    {
        cli.connect()->wait();

        mqtt::topic top(cli, "temperature_warsaw", QOS);
        mqtt::token_ptr tok;

        for (auto value : payload)
        {
            tok = top.publish(value.c_str());
        }
        tok->wait(); // Just wait for the last one to complete.

        cli.disconnect()->wait();
    }
    catch (const mqtt::exception &exc)
    {
        auto errorMessage = std::string("MQTT Error: ") + exc.get_error_str();
        throw std::runtime_error(errorMessage);
    }
}