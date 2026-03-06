#include "../include/codec.hpp"

class DummyCodec : public Codec {

public:

    std::vector<uint16_t> encode(const std::string& input) override {

        std::vector<uint16_t> tokens;

        for (char c : input)
            tokens.push_back((uint16_t)c);

        return tokens;
    }

    void decode(const std::vector<uint16_t>& tokens,
                const std::string& output) override {

        FILE* f = fopen(output.c_str(), "wb");

        for (auto t : tokens)
            fputc((char)t, f);

        fclose(f);
    }
};
