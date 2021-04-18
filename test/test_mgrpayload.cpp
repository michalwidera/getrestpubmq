#define BOOST_TEST_MODULE GetRestPubMq
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/execution_monitor.hpp> // for execution_exception

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <string>
#include <vector>

#include "mgrpayload.hpp"

using namespace std;

string example_json_response = "{\"coord\":\
    {\"lon\":21.0118,\"lat\":52.2298},\
    \"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01n\"}],\
    \"base\":\"stations\",\
    \"main\":{\"temp\":12.48,\"feels_like\":11.4,\"temp_min\":11.67,\"temp_max\":13,\"pressure\":1018,\"humidity\":62},\
    \"visibility\":10000,\
    \"wind\":{\"speed\":2.06,\"deg\":360},\
    \"clouds\":{\"all\":0},\"dt\":1618768143,\
    \"sys\":{\"type\":1,\"id\":1713,\"country\":\"PL\",\"sunrise\":1618716715,\"sunset\":1618767493},\
    \"timezone\":7200,\"id\":756135,\
    \"name\":\"Warsaw\",\"cod\":200}";

BOOST_AUTO_TEST_CASE(check_createPayload_success_1)
{
    std::vector<std::string> payload = createPayload(example_json_response);
    BOOST_CHECK(payload.size() == 5);
    BOOST_CHECK(payload[0] == "{");
    BOOST_CHECK(payload[1] == "\"id\": \"temperature\",");
    BOOST_CHECK(payload[2] == "\"value\": 12.48,");
    // Timestamp may vary - we do not check here.
    BOOST_CHECK(payload[4] == "}");
}

BOOST_AUTO_TEST_CASE(check_createPayload_success_2)
{
    std::vector<std::string> payload = createPayload("{\"main\":{\"temp\":12.48}}");
    BOOST_CHECK(payload[2] == "\"value\": 12.48,");
    BOOST_CHECK(payload.size() != 0);
}

BOOST_AUTO_TEST_CASE(check_createPayload_failure_scenario_1)
{
    BOOST_CHECK_THROW(createPayload("garbage here"), boost::wrapexcept<boost::property_tree::json_parser::json_parser_error>);
}

BOOST_AUTO_TEST_CASE(check_createPayload_fail_scenario_2)
{
    BOOST_CHECK_THROW(createPayload("{\"main\":{\"temp\":SOME_CRAP_HERE}}"), std::exception);
}
