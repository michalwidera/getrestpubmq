#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

#include <sstream>
#include <assert.h>

#include "mgrpayload.hpp"

#include <iostream>

typedef boost::property_tree::ptree ptree;

Payload_t createPayload(std::string jsonInText)
{
    std::stringstream strstream;
    strstream << jsonInText;

    ptree pt;
    read_json(strstream, pt);

    std::string sTemperature(pt.get("main.temp", ""));

    // We do not trust open service - it's better to fail task than ship incorrect value.
    // if openweather map will change format of delivered data - this should fail.

    // This will throw exception if NAN

    std::stod(sTemperature.c_str());

    // There is another check here.
    // if delivered data will not be in float number form this conversion will claim another error.

    float temperature = boost::lexical_cast<float>(sTemperature);

    std::stringstream ssvalue;
    ssvalue << "\"value\": " << temperature << ",";
    std::stringstream sstimestamp;
    sstimestamp << "\"timestamp\": " << time(0);

    Payload_t payload;
    payload.push_back("{");
    payload.push_back("\"id\": \"temperature\",");
    payload.push_back(ssvalue.str());
    payload.push_back(sstimestamp.str());
    payload.push_back("}");

    int i = 0;
    for (auto v : payload)
    {
        std::cout << ++i << " " <<  v << std::endl;
    }

    return payload;
}