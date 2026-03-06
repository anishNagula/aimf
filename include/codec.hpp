#pragma once

#include <vector>
#include <cstdint>
#include <string>

class Codec {
public:
    virtual ~Codec() {}

    virtual std::vector<uint16_t> encode(const std::string& input) = 0;
    virtual void decode(const std::vector<uint16_t>& tokens,
                        const std::string& output) = 0;
};
