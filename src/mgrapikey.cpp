#include "mgrapikey.hpp"

#include <fstream>

std::string loadApikey()
{
    // Notice: api.key should contain api key from openweathermap.org
    // There's shoudn't be enter at the end of file.
    // only hexadecimal 32 digits from delivered api key from web service
    std::ifstream ifs("api.key");
    std::string apikey((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    if (apikey.empty())
    {
        throw std::runtime_error("Please create api.key file (with apikey from openweathermap.org)");
    }

    const int sizeOfApiKey = 32;

    if (apikey.length() != sizeOfApiKey)
    {
        throw std::runtime_error("Malformed api.key file");
    }

    return apikey;
}
