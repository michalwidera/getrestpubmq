#ifndef GRPM_MGR_PAYLOAD_HPP
#define GRPM_MGR_PAYLOAD_HPP

#include <string>
#include <vector>

typedef std::vector<std::string> Payload_t;

Payload_t createPayload(std::string jsonInText);

#endif //GRPM_MGR_PAYLOAD_HPP
