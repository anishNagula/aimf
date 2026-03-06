#include <vector>
#include <zstd.h>

std::vector<char> zstd_compress(const std::vector<uint16_t>& tokens)
{
    size_t input_size = tokens.size() * sizeof(uint16_t);

    const char* input = reinterpret_cast<const char*>(tokens.data());

    size_t max_size = ZSTD_compressBound(input_size);

    std::vector<char> compressed(max_size);

    size_t compressed_size = ZSTD_compress(
        compressed.data(),
        max_size,
        input,
        input_size,
        1
    );

    compressed.resize(compressed_size);

    return compressed;
}

std::vector<uint16_t> zstd_decompress(
    const std::vector<char>& data,
    size_t original_size_bytes)
{
    std::vector<uint16_t> tokens(original_size_bytes / sizeof(uint16_t));

    ZSTD_decompress(
        tokens.data(),
        original_size_bytes,
        data.data(),
        data.size()
    );

    return tokens;
}
