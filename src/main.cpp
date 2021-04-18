#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <fstream>

#include <cpr/cpr.h>

#include "mqtt/async_client.h"

#include <boost/system/error_code.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

typedef boost::property_tree::ptree ptree ;

int main(int argc, char *argv[])
{
    chrono::time_point<chrono::system_clock> start, end;

    // Notice: api.key should contain api key from openweathermap.org
    // There's shoudn't be enter at the end of file.
    // only hexadecimal 32 digits from delivered api key from web service

    ifstream ifs("api.key");
    string apikey((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    if (apikey.empty())
    {
        cerr << "Please create api.key file (with apikey from openweathermap.org)" << endl;
        return system::errc::invalid_argument;
    }

    const int sizeOfApiKey = 32;

    if (apikey.length() != sizeOfApiKey)
    {
        cerr << "Malformed api.key file" << endl ;
        return system::errc::invalid_argument;
    }

    try
    {
        while(true) {
            start = chrono::system_clock::now();

            // exmaple from:
            // https://whoshuu.github.io/cpr/introduction.html

            cpr::Response r = cpr::Get(cpr::Url{"http://api.openweathermap.org/data/2.5/weather"},
            cpr::Parameters{
                {"q", "Warsaw"},
                {"units","metric"},
                {"mode","json"},
                {"appid",apikey.c_str()}
            });

            // Due to fact that there is no defined action if case of web failure
            // we are exiting from procedure with io_error message.
            // There should considered another state in this process ie. connection_lost

            const int httpResponseOk = 200;

            if (r.status_code != httpResponseOk)
            {
                cerr << "Error [" << r.status_code << "] making REST request" << endl;
                return system::errc::io_error; // eq. 5
            }

            // If time of fetching data from openwathermap takes more than assumed duration
            // Entire process need to get another state - timeout.
            // Until there is no such requirement is task - assert should cover this issue.

            const int durationInSeconds = 60;

            assert( r.elapsed < durationInSeconds );

            // Checking if openwathermap delivery is non empty and contains at least something
            // that looks like json

            assert(!r.text.empty());
            assert(r.text[0] == '{');

            // forming requested structure for shipping data via mqtt

            stringstream strstream;
            strstream << r.text ;

            ptree pt;
            read_json(strstream, pt);

            string sTemperature(pt.get("main.temp", ""));

            // We do not trust open service - it's better to fail task than ship incorrect value.
            // if openweather map will change format of delivered data - this assert should fail.

            assert(!sTemperature.empty());

            // There is another check here.
            // if delivered data will not be in float number form this conversion will claim another error.

            float temperature = boost::lexical_cast<float>(sTemperature);

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

            // MQTT ship data - source of this process could be found here:
            // https://github.com/eclipse/paho.mqtt.cpp/blob/master/src/samples/topic_publish.cpp

            const string address { "tcp://localhost:1883" };
            const int QOS = 1;

            mqtt::async_client cli(address, "");

            try {
                cli.connect()->wait();

                mqtt::topic top(cli, "temperature_warsaw", QOS);
                mqtt::token_ptr tok;

                for(auto value: payload) {
                    tok = top.publish(value.c_str());
                }
                tok->wait();	// Just wait for the last one to complete.

                cli.disconnect()->wait();
            }
            catch (const mqtt::exception& exc) {
                cerr << "Error [" << exc << "] making MQTT request" << endl;
                return system::errc::io_error; // eq. 5
            }

            // To get more precise 60 duration we need to correct sleep time
            // with time of sending of mqtt and fetching rest response.

            end = chrono::system_clock::now();
            chrono::duration<double> elapsedSeconds = end - start;

            // If entire process of fetching data from openweathermap take more than
            // assumed period - we need to cover this issue as out of scenario.
            // Special case 'timeout' should be covered here.

            assert( durationInSeconds - elapsedSeconds.count() > 0 );

            this_thread::sleep_for( chrono::seconds(durationInSeconds) - elapsedSeconds );
        }
    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
        return system::errc::operation_not_permitted; // eq. 1 - General Catch
    }
    return system::errc::success; // eq. 0 - ok.
}
