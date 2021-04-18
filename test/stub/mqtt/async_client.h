#ifndef GRPM_PAHO_STUB_HPP
#define GRPM_PAHO_STUB_HPP

#include <string>

namespace mqtt
{

    struct topic;
    struct token;

    typedef token* token_ptr;

    struct token
    {
        void wait() {};
    } token_stub;

    struct async_client
    {
        async_client(std::string, std::string) {};
        token_ptr connect() { return &token_stub; };
        token_ptr disconnect() { return &token_stub; };
    };

    struct topic
    {
        topic() {};
        topic(async_client v1) {};
        topic(async_client v1, std::string v2, int v3) {};
        token_ptr publish(std::string) { return &token_stub; };
    };

    struct exception
    {
        std::string get_error_str() const { return ""; }
    };
}

#endif
