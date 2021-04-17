#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>

#include <cpr/cpr.h>

#include "mqtt/async_client.h"

#include <boost/system/error_code.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace cpr;

std::string sApiKey;

typedef boost::property_tree::ptree ptree ;

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

const int durationInSeconds = 5;

int main(int argc, char *argv[])
{
    chrono::time_point<chrono::system_clock> start, end;

    try
    {
        start = chrono::system_clock::now();

        Response r = Get(Url{"http://api.openweathermap.org/data/2.5/weather"},
        Parameters{
            {"q", "Warsaw"},
            {"units","metric"},
            {"mode","json"},
            {"appid",""}
        });

        if (r.status_code >= 400) {
            cerr << "Error [" << r.status_code << "] making request" << endl;
            return system::errc::operation_not_permitted; // eq. 1 - General Catch
        }

        assert( r.elapsed < durationInSeconds );

        stringstream strstream;
        strstream << r.text ;

        ptree pt ;
        read_json(strstream, pt);

        float temperature = boost::lexical_cast<float> (pt.get("main.temp", "")) ;

        stringstream ssvalue;
        ssvalue << "\"value\": " << temperature;
        stringstream sstimestamp;
        sstimestamp << "\"timestamp\": " << time(0);

        vector< string > payload ;
        payload.push_back("{");
        payload.push_back("\"id\": \"temperature\"");
        payload.push_back(ssvalue.str());
        payload.push_back(sstimestamp.str());
        payload.push_back("}");

        for(auto value: payload)  {
            cout << value.c_str() << endl;
        }

        // https://github.com/eclipse/paho.mqtt.cpp/blob/master/src/samples/topic_publish.cpp

        const string address { "tcp://localhost:1883" };
        const int QOS = 1;

        mqtt::async_client cli(address, "");

        cout << "  ...OK" << endl;

        try {
            cout << "\nConnecting..." << endl;
            cli.connect()->wait();
            cout << "  ...OK" << endl;

            cout << "\nPublishing messages..." << endl;

            mqtt::topic top(cli, "temperature_warsaw", QOS);
            mqtt::token_ptr tok;

            for(auto value: payload) {
                tok = top.publish(value.c_str());
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
            return system::errc::operation_not_permitted; // eq. 1 - General Catch;
        }

        end = chrono::system_clock::now();
        chrono::duration<double> elapsedSeconds = end - start;

        assert( durationInSeconds - elapsedSeconds.count() > 0 );

        this_thread::sleep_for( chrono::seconds(durationInSeconds) - elapsedSeconds );
    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
        return system::errc::operation_not_permitted; // eq. 1 - General Catch
    }
    return system::errc::success; // eq. 0 - ok.
}
