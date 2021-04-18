#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

#include <sstream>

#include "mgrpayload.hpp"

typedef boost::property_tree::ptree ptree;

std::vector<std::string> createPayload( std::string jsonInText )
{

    std::stringstream strstream;
    strstream << jsonInText;

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

    std::vector<std::string> payload;
    payload.push_back("{");
    payload.push_back("\"id\": \"temperature\"");
    payload.push_back(ssvalue.str());
    payload.push_back(sstimestamp.str());
    payload.push_back("}");

    return payload;
}