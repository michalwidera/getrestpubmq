
#define BOOST_TEST_MODULE GetRestPubMq
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>

#include <string>
#include <vector>

#include "mqttclient.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE(check_shipPayloadToMqttBus)
{
    Payload_t payloadData;
    payloadData.push_back("test string");
    shipPayloadToMqttBus(payloadData);
    BOOST_CHECK(true);
}
