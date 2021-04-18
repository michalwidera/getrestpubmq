#define BOOST_TEST_MODULE GetRestPubMq
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>

#include <string>
#include <vector>

#include "mgrapikey.hpp"

BOOST_AUTO_TEST_CASE(check_loadApikey)
{
    std::string apikey = loadApikey();

    BOOST_CHECK(!apikey.empty());
    BOOST_CHECK(apikey == "112233445566778899AABBCCDDEEFF00");
}