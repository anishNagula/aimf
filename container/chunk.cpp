#include <fstream>
#include <vector>
#include "../include/aimf.hpp"
#include "../include/compression.hpp"

namespace aimf {

void write_chunk(
    std::ofstream &out,
    uint8_t stream_id,
    uint64_t timestamp_us,
    std::vector<uint16_t> &tokens,
    bool compress)
{
    ChunkHeader header;

    header.stream_id = stream_id;
    header.timestamp_us = timestamp_us;
    header.token_count = tokens.size();

    if (compress) {

        auto compressed = zstd_compress(tokens);

        header.compressed_size = compressed.size();

        out.write(reinterpret_cast<char*>(&header), sizeof(header));

        out.write(compressed.data(), compressed.size());
    }
    else {

        header.compressed_size = 0;

        out.write(reinterpret_cast<char*>(&header), sizeof(header));

        out.write(
            reinterpret_cast<char*>(tokens.data()),
            tokens.size() * sizeof(uint16_t));
    }
}

void write_chunk_index(
    std::ofstream &out,
    std::vector<ChunkIndexEntry> &index)
{
    uint32_t count = index.size();

    out.write(
        reinterpret_cast<char*>(&count),
        sizeof(count));

    for (auto &entry : index)
    {
        out.write(
            reinterpret_cast<char*>(&entry),
            sizeof(ChunkIndexEntry));
    }
}

}
