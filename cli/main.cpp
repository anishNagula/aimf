#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>

#include "../include/aimf.hpp"
#include "../include/codec.hpp"
#include "../include/codec_factory.hpp"
#include "../include/compression.hpp"
#include "../include/metadata.hpp"

using namespace aimf;

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        std::cout << "Usage:\n";
        std::cout << "  aimf encode <input> <output>\n";
        std::cout << "  aimf decode <input> <output>\n";
        return 1;
    }

    std::string command = argv[1];

    // ======================================================
    // ENCODE
    // ======================================================
    if (command == "encode")
    {
        std::string input_file = argv[2];
        std::string output_file = argv[3];

        std::ifstream input(input_file, std::ios::binary);

        if (!input)
        {
            std::cerr << "Failed to open input file\n";
            return 1;
        }

        std::string data(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>());

        uint16_t codec_id = CODEC_DUMMY;

        Codec* codec = create_codec(codec_id);

        if (!codec)
        {
            std::cerr << "Codec not found\n";
            return 1;
        }

        std::vector<uint16_t> tokens = codec->encode(data);

        const size_t CHUNK_SIZE = 5;

        std::vector<std::vector<uint16_t>> chunks;

        for (size_t i = 0; i < tokens.size(); i += CHUNK_SIZE)
        {
            size_t end = std::min(i + CHUNK_SIZE, tokens.size());

            chunks.emplace_back(
                tokens.begin() + i,
                tokens.begin() + end);
        }

        std::ofstream out(output_file, std::ios::binary);

        if (!out)
        {
            std::cerr << "Failed to open output file\n";
            return 1;
        }

        AIMFHeader header;

        std::memcpy(header.magic, MAGIC, 4);
        header.version = VERSION;
        header.stream_count = 1;
        header.header_size = sizeof(AIMFHeader);
        header.stream_table_offset = sizeof(AIMFHeader);
        header.index_offset = 0;

        out.write(reinterpret_cast<char*>(&header), sizeof(header));

        StreamDesc stream;

        stream.stream_id = 0;
        stream.stream_type = VIDEO_TOKENS;
        stream.codec_id = codec_id;
        stream.token_rate = 30;
        stream.compression = 1;
        stream.data_offset = 0;

        write_stream_table(out, &stream, 1);

        auto metadata = encode_metadata("example", chunks.size());

        uint32_t meta_size = metadata.size();

        out.write(reinterpret_cast<char*>(&meta_size), sizeof(meta_size));
        out.write(reinterpret_cast<char*>(metadata.data()), meta_size);

        std::vector<ChunkIndexEntry> index;

        for (size_t i = 0; i < chunks.size(); i++)
        {
            uint64_t chunk_offset = out.tellp();

            write_chunk(out, 0, i * 1000000, chunks[i], true);

            ChunkIndexEntry entry;

            entry.stream_id = 0;
            entry.timestamp_us = i * 1000000;
            entry.file_offset = chunk_offset;

            index.push_back(entry);
        }

        uint64_t index_offset = out.tellp();

        write_chunk_index(out, index);

        header.index_offset = index_offset;

        out.seekp(0);
        out.write(reinterpret_cast<char*>(&header), sizeof(header));

        delete codec;

        std::cout << "AIMF file created\n";
    }

    // ======================================================
    // DECODE
    // ======================================================
    else if (command == "decode")
    {
        std::string input_file = argv[2];
        std::string output_file = argv[3];

        std::ifstream in(input_file, std::ios::binary);

        if (!in)
        {
            std::cerr << "Failed to open AIMF file\n";
            return 1;
        }

        AIMFHeader header;

        in.read(reinterpret_cast<char*>(&header), sizeof(header));

        // --------------------------
        // READ STREAM TABLE
        // --------------------------

        in.seekg(header.stream_table_offset);

        std::vector<StreamDesc> streams(header.stream_count);

        for (uint16_t i = 0; i < header.stream_count; i++)
        {
            in.read(reinterpret_cast<char*>(&streams[i]), sizeof(StreamDesc));
        }

        StreamDesc& stream = streams[0];

        // --------------------------
        // READ METADATA
        // --------------------------

        uint32_t meta_size;

        in.read(reinterpret_cast<char*>(&meta_size), sizeof(meta_size));

        std::vector<uint8_t> metadata(meta_size);

        in.read(reinterpret_cast<char*>(metadata.data()), meta_size);

        Codec* codec = create_codec(stream.codec_id);

        if (!codec)
        {
            std::cerr << "Unsupported codec\n";
            return 1;
        }

        // --------------------------
        // READ INDEX
        // --------------------------

        in.seekg(header.index_offset);

        uint32_t index_count;

        in.read(reinterpret_cast<char*>(&index_count), sizeof(index_count));

        std::vector<ChunkIndexEntry> index(index_count);

        for (uint32_t i = 0; i < index_count; i++)
        {
            in.read(reinterpret_cast<char*>(&index[i]), sizeof(ChunkIndexEntry));
        }

        std::vector<uint16_t> all_tokens;

        for (auto& entry : index)
        {
            in.seekg(entry.file_offset);

            ChunkHeader ch;

            in.read(reinterpret_cast<char*>(&ch), sizeof(ch));

            std::vector<uint16_t> tokens;

            if (stream.compression == 1)
            {
                std::vector<char> compressed(ch.compressed_size);

                in.read(compressed.data(), ch.compressed_size);

                tokens = zstd_decompress(
                    compressed,
                    ch.token_count * sizeof(uint16_t));
            }
            else
            {
                tokens.resize(ch.token_count);

                in.read(reinterpret_cast<char*>(tokens.data()),
                        ch.token_count * sizeof(uint16_t));
            }

            all_tokens.insert(
                all_tokens.end(),
                tokens.begin(),
                tokens.end());
        }

        codec->decode(all_tokens, output_file);

        delete codec;

        std::cout << "Decoded to " << output_file << "\n";
    }

    else
    {
        std::cout << "Unknown command\n";
        return 1;
    }

    return 0;
}
