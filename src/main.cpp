#include <boost/system/error_code.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <fstream>
#include <thread>

#include "restclient.hpp"
#include "mqttclient.hpp"

typedef boost::property_tree::ptree ptree ;

int main(int argc, char *argv[])
{
    std::chrono::time_point<std::chrono::system_clock> start, end;

    // Notice: api.key should contain api key from openweathermap.org
    // There's shoudn't be enter at the end of file.
    // only hexadecimal 32 digits from delivered api key from web service

    std::ifstream ifs("api.key");
    std::string apikey((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    if (apikey.empty())
    {
        std::cerr << "Please create api.key file (with apikey from openweathermap.org)" << std::endl;
        return boost::system::errc::invalid_argument;
    }

    const int sizeOfApiKey = 32;

    if (apikey.length() != sizeOfApiKey)
    {
        std::cerr << "Malformed api.key file" << std::endl ;
        return boost::system::errc::invalid_argument;
    }

    try
    {
        while(true) {
            start = std::chrono::system_clock::now();

            // If time of fetching data from openwathermap takes more than assumed duration
            // Entire process need to get another state - timeout.
            // Until there is no such requirement is task - assert should cover this issue.

            const int durationInSeconds = 60;

            std::stringstream strstream;
            strstream << getRestResponse(apikey);

            ptree pt;
            read_json(strstream, pt);

            std::string sTemperature(pt.get("main.temp", ""));

            // We do not trust open service - it's better to fail task than ship incorrect value.
            // if openweather map will change format of delivered data - this assert should fail.

            assert(!sTemperature.empty());

            // There is another check here.
            // if delivered data will not be in float number form this conversion will claim another error.

            float temperature = boost::lexical_cast<float>(sTemperature);

            std::stringstream ssvalue;
            ssvalue << "\"value\": " << temperature;
            std::stringstream sstimestamp;
            sstimestamp << "\"timestamp\": " << time(0);

            std::vector< std::string > payload ;
            payload.push_back("{");
            payload.push_back("\"id\": \"temperature\"");
            payload.push_back(ssvalue.str());
            payload.push_back(sstimestamp.str());
            payload.push_back("}");

            shipPayloadToMqttBus(payload);

            // To get more precise 60 duration we need to correct sleep time
            // with time of sending of mqtt and fetching rest response.

            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedSeconds = end - start;

            // If entire process of fetching data from openweathermap take more than
            // assumed period - we need to cover this issue as out of scenario.
            // Special case 'timeout' should be covered here.

            assert( durationInSeconds - elapsedSeconds.count() > 0 );

            std::this_thread::sleep_for(std::chrono::seconds(durationInSeconds) - elapsedSeconds);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return boost::system::errc::operation_not_permitted; // eq. 1 - General Catch
    }
    return boost::system::errc::success; // eq. 0 - ok.
}
