#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "../include/aimf.hpp"

using namespace aimf;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: aimf_inspect <file>\n";
        return 1;
    }

    std::ifstream in(argv[1], std::ios::binary);

    if (!in)
    {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    // -------- HEADER --------
    AIMFHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(header));

    std::cout << "File: " << argv[1] << "\n";
    std::cout << "Version: " << header.version << "\n";
    std::cout << "Streams: " << header.stream_count << "\n\n";

    // -------- STREAM TABLE --------
    for (int i = 0; i < header.stream_count; i++)
    {
        StreamDesc stream;
        in.read(reinterpret_cast<char*>(&stream), sizeof(stream));

        std::cout << "Stream " << (int)stream.stream_id << "\n";
        std::cout << "Type: " << (int)stream.stream_type << "\n";
        std::cout << "Codec: " << stream.codec_id << "\n";
        std::cout << "Token Rate: " << stream.token_rate << "\n\n";
    }

    // -------- READ INDEX --------

in.seekg(0, std::ios::end);
std::streamoff file_size = in.tellg();

std::streamoff entry_size = sizeof(ChunkIndexEntry);

// read last entry first
in.seekg(file_size - entry_size);

ChunkIndexEntry last;
in.read(reinterpret_cast<char*>(&last), sizeof(last));

uint64_t last_ts = last.timestamp_us;

// now scan backwards until timestamps stop decreasing
std::vector<ChunkIndexEntry> index;
index.push_back(last);

while (true)
{
    std::streamoff pos =
        file_size - (index.size() + 1) * entry_size;

    if (pos < 0)
        break;

    in.seekg(pos);

    ChunkIndexEntry e;
    in.read(reinterpret_cast<char*>(&e), sizeof(e));

    if (!in)
        break;

    if (e.timestamp_us > last_ts)
        break;

    last_ts = e.timestamp_us;
    index.push_back(e);
}

std::reverse(index.begin(), index.end());

uint32_t index_count = index.size();

std::cout << "Chunks: " << index_count << "\n";

if (index_count > 0)
{
    uint64_t duration_us = index.back().timestamp_us;
    double duration_s = duration_us / 1000000.0;

    std::cout << "Duration: " << duration_s << " seconds\n";
}

    std::cout << "\nInspection complete\n";
}
