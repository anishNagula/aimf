#pragma once

#include <vector>
#include <cstdint>

std::vector<char> zstd_compress(const std::vector<uint16_t>& tokens);

std::vector<uint16_t> zstd_decompress(
    const std::vector<char>& data,
    size_t original_size
);
