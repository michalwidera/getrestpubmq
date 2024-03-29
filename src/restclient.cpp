#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/buffers_iterator.hpp>

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "restclient.hpp"

// Based on:
// https://www.boost.org/doc/libs/1_74_0/libs/beast/example/http/client/sync/http_client_sync.cpp

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

std::string getRestResponse(std::string apikey)
{
    std::string retValue { "" };

    try
    {
        auto const host = "api.openweathermap.org";
        auto const port = "80";
        int version = 11;
        std::vector<std::string> uriParametrs { "q=Warsaw", "units=metric", "mode=json"};
        std::string target = "/data/2.5/weather?";

        for (auto value : uriParametrs)
        {
            target += value + "&";
        }

        target += "appid=" + apikey;
        // The io_context is required for all I/O
        net::io_context ioc;
        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);
        // Look up the domain name
        auto const results = resolver.resolve(host, port);
        // Make the connection on the IP address we get from a lookup
        stream.connect(results);
        // Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::get, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::pragma, "no-cache");
        req.set(http::field::cache_control, "no-cache, no-store, must-revalidate");
        req.set(http::field::expires, "0");
        // Send the HTTP request to the remote host
        http::write(stream, req);
        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;
        // Declare a container to hold the response
        http::response<http::dynamic_body> res;
        // Receive the HTTP response
        http::read(stream, buffer, res);
        std::string body{boost::asio::buffers_begin(res.body().data()),
            boost::asio::buffers_end(res.body().data())};
        retValue = body;
        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if (ec && ec != beast::errc::not_connected)
            throw beast::system_error{ec};

        // If we get here then the connection is closed gracefully
    }
    catch (std::exception const &e)
    {
        auto errorMessage = std::string("REST Error: ") + e.what();
        throw std::runtime_error(errorMessage);
    }

    return retValue;
}
