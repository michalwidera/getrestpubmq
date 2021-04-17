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

typedef boost::property_tree::ptree ptree ;

const int durationInSeconds = 60;

int main(int argc, char *argv[])
{
    chrono::time_point<chrono::system_clock> start, end;

    try
    {
        while(true) {
            start = chrono::system_clock::now();

            // https://whoshuu.github.io/cpr/introduction.html

            cpr::Response r = cpr::Get(cpr::Url{"http://api.openweathermap.org/data/2.5/weather"},
            cpr::Parameters{
                {"q", "Warsaw"},
                {"units","metric"},
                {"mode","json"},
                {"appid",""}
            });

            if (r.status_code != 200) {
                cerr << "Error [" << r.status_code << "] making REST request" << endl;
                return system::errc::operation_not_permitted; // eq. 1 - General Catch
            }

            // If time of fetching data from openwathermap takes more than assumed duration
            // Entire process need to get another state - timeout.
            // Until there is no such requirement is task - assert should cover this issue.

            assert( r.elapsed < durationInSeconds );

            // forming requested structure for shipping data via mqtt

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

            // Source of this process could be found here:
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
                return system::errc::operation_not_permitted; // eq. 1 - General Catch;
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
