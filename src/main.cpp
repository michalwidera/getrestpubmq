#include <boost/system/error_code.hpp>

#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <assert.h>

#include "restclient.hpp"
#include "mqttclient.hpp"
#include "mgrapikey.hpp"
#include "mgrpayload.hpp"

int main(int argc, char* argv[])
{
    std::chrono::time_point<std::chrono::system_clock> start, end;

    try
    {
        std::string apikey = loadApikey();

        while (true)
        {
            start = std::chrono::system_clock::now();
            shipPayloadToMqttBus(createPayload(getRestResponse(apikey)));
            // To get more precise 60 duration we need to correct sleep time
            // with time of sending of mqtt and fetching rest response.
            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedSeconds = end - start;
            // If entire process of fetching data from openweathermap takes more than
            // assumed period - we need to cover this issue as out of scenario.
            // Special case 'timeout' should be covered here.
            const int durationInSeconds = 60;
            assert(durationInSeconds - elapsedSeconds.count() > 0);
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
