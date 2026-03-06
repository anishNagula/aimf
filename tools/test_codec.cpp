#include <iostream>
#include <vector>
#include "../include/codec.hpp"

// include the dummy codec implementation
#include "../codecs/dummy_codec.cpp"

int main() {

    DummyCodec codec;

    std::string input = "hello";

    auto tokens = codec.encode(input);

    std::cout << "Tokens:\n";

    for (auto t : tokens)
        std::cout << t << " ";

    std::cout << "\n";

    codec.decode(tokens, "output.txt");

    std::cout << "Decoded output written to output.txt\n";
}
