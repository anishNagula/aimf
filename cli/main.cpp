#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>

#include "../include/aimf.hpp"
#include "../include/codec.hpp"
#include "../codecs/dummy_codec.cpp"

using namespace aimf;

int main(int argc, char** argv) {

  if (argc < 4) {
    std::cout << "Usage:\n";
    std::cout << "  aimf encode <input> <output>\n";
    std::cout << "  aimf decode <input> <output>\n";
    return 1;
  }

  std::string command = argv[1];

  // =========================
  // ENCODE
  // =========================
  if (command == "encode") {

    std::string input_file = argv[2];
    std::string output_file = argv[3];

    std::ifstream input(input_file, std::ios::binary);

    if (!input) {
      std::cerr << "Failed to open input file\n";
      return 1;
    }

    std::string data(
        (std::istreambuf_iterator<char>(input)),
        std::istreambuf_iterator<char>());

    DummyCodec codec;
    std::vector<uint16_t> tokens = codec.encode(data);

    const size_t CHUNK_SIZE = 5;
    std::vector<std::vector<uint16_t>> chunks;

    for (size_t i = 0; i < tokens.size(); i += CHUNK_SIZE) {

      size_t end = std::min(i + CHUNK_SIZE, tokens.size());

      chunks.emplace_back(
          tokens.begin() + i,
          tokens.begin() + end);
    }

    std::ofstream out(output_file, std::ios::binary);

    if (!out) {
      std::cerr << "Failed to open output file\n";
      return 1;
    }

    AIMFHeader header;
    std::memcpy(header.magic, MAGIC, 4);
    header.version = VERSION;
    header.stream_count = 1;
    header.header_size = sizeof(AIMFHeader);
    header.index_offset = 0;

    out.write(reinterpret_cast<char*>(&header), sizeof(header));

    StreamDesc stream;

    stream.stream_id = 0;
    stream.stream_type = VIDEO_TOKENS;
    stream.codec_id = CODEC_DUMMY;
    stream.token_rate = 30;
    stream.compression = 0;
    stream.data_offset = 0;

    write_stream_table(out, &stream, 1);

    std::vector<ChunkIndexEntry> index;

    for (size_t i = 0; i < chunks.size(); i++)
    {
      uint64_t chunk_offset = out.tellp();

      write_chunk(out, 0, i * 1000000, chunks[i]);

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

    std::cout << "AIMF file created\n";
  }

  // =========================
  // DECODE
  // =========================
  else if (command == "decode") {

    std::string input_file = argv[2];
    std::string output_file = argv[3];

    std::ifstream in(input_file, std::ios::binary);

    if (!in) {
      std::cerr << "Failed to open AIMF file\n";
      return 1;
    }

    AIMFHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(header));

    StreamDesc stream;
    in.read(reinterpret_cast<char*>(&stream), sizeof(stream));

    in.seekg(header.index_offset);

    uint32_t index_count;
    in.read(reinterpret_cast<char*>(&index_count), sizeof(index_count));

    std::vector<ChunkIndexEntry> index(index_count);

    for (uint32_t i = 0; i < index_count; i++) {
      in.read(reinterpret_cast<char*>(&index[i]), sizeof(ChunkIndexEntry));
    }

    std::vector<uint16_t> all_tokens;

    for (auto& entry : index) {

      in.seekg(entry.file_offset);

      ChunkHeader ch;
      in.read(reinterpret_cast<char*>(&ch), sizeof(ch));

      std::vector<uint16_t> tokens(ch.token_count);

      in.read(reinterpret_cast<char*>(tokens.data()),
              ch.token_count * sizeof(uint16_t));

      all_tokens.insert(
          all_tokens.end(),
          tokens.begin(),
          tokens.end());
    }

    DummyCodec codec;

    codec.decode(all_tokens, output_file);

    std::cout << "Decoded to " << output_file << "\n";
  }

  else {
    std::cout << "Unknown command\n";
    return 1;
  }

  return 0;
}
