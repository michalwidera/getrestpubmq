#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>

#include <cpr/cpr.h>

#include "mqtt/async_client.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace boost;
using namespace cpr;

std::string sApiKey;
std::string sConfigFile;

/*
Write an application in C++, which makes REST calls to OpenWeatherMap online service
in order to read current weather conditions in Warsaw.
The application should do it periodically (every 60 seconds) and every time it
gets the fresh data, it should parse received JSON message and publish the
current temperature in degrees of Celsius to local MQTT bus.
Address of the service (API details for reference):
https://openweathermap.org/api/one-call-api
The output MQTT message should be published to the following MQTT topic:
temperature_warsaw
and should contain the following payload:
{
"id": "temperature",
"value": <value of the temperature (FLOAT)>,
"timestamp": <current unix timestamp (INTEGER)>
}
Note, that in order to use the API of the service one needs to create
an account in the service and get API key. Creation of the account is free of charge.
*/

const char* PAYLOADS[] = {
    "Hello World!",
    "Hi there!",
    "Is anyone listening?",
    "Someone is always listening.",
    nullptr
};

int main(int argc, char *argv[])
{
    try
    {
        namespace po = boost::program_options;
        po::options_description desc("Allowed options");
        desc.add_options()
        ("apikey,a", po::value<string>(&sApiKey), "show this stream")
        ("help,h", "show options")
        ;
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

        po::notify(vm);

        if (vm.count("apikey"))
        {
            cout << "apikey:" << sApiKey << endl;
        }
        if (vm.count("help"))
        {
            cerr << argv[0] << " - xtoy tool." << endl;
            cerr << desc << endl;
            return system::errc::success;
        }

        Response r = Get(Url{"http://www.httpbin.org/get"},
        Parameters{{"hello", "world"}});
        cout << r.url << endl; // http://www.httpbin.org/get?hello=world
        cout << r.text << endl;


        // https://github.com/eclipse/paho.mqtt.cpp/blob/master/src/samples/topic_publish.cpp

        const std::string address { "tcp://localhost:1883" };
        const int QOS = 1;

        mqtt::async_client cli(address, "");

        cout << "  ...OK" << endl;

        try {
            cout << "\nConnecting..." << endl;
            cli.connect()->wait();
            cout << "  ...OK" << endl;

            cout << "\nPublishing messages..." << endl;

            mqtt::topic top(cli, "test", QOS);
            mqtt::token_ptr tok;

            size_t i = 0;
            while (PAYLOADS[i]) {
                tok = top.publish(PAYLOADS[i++]);
            }
            tok->wait();	// Just wait for the last one to complete.
            cout << "OK" << endl;

            // Disconnect
            cout << "\nDisconnecting..." << endl;
            cli.disconnect()->wait();
            cout << "  ...OK" << endl;
        }
        catch (const mqtt::exception& exc) {
            cerr << exc << endl;
            return 1;
        }

    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
        return system::errc::operation_not_permitted; // eq. 1 - General Catch
    }
    return system::errc::success; // eq. 0 - ok.
}
