#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "../include/aimf.hpp"

using namespace aimf;

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cout << "Usage: aimf <output_file>\n";
    return 1;
  }

  std::ofstream out(argv[1], std::ios::binary);

  if (!out) {
    std::cerr << "Failed to open file\n";
    return 1;
  }

  AIMFHeader header;
  std::memcpy(header.magic, MAGIC, 4);
  header.version = VERSION;
  header.stream_count = 1;
  header.header_size = sizeof(AIMFHeader);

  out.write(reinterpret_cast<char*>(&header), sizeof(header));

  StreamDesc stream;

  stream.stream_id = 0;
  stream.stream_type = VIDEO_TOKENS;
  stream.codec_id = 1;
  stream.token_rate = 30;
  stream.compression = 0;
  stream.data_offset = 0;

  write_stream_table(out, &stream, 1);

  std::vector<ChunkIndexEntry> index;

  uint64_t chunk_offset = out.tellp();

  std::vector<uint16_t> tokens = {
    882, 12, 991, 443, 33
  };

  write_chunk(out, 0, 0, tokens);

  ChunkIndexEntry entry;
  entry.stream_id = 0;
  entry.timestamp_us = 0;
  entry.file_offset = chunk_offset;

  index.push_back(entry);

  write_chunk_index(out, index);

  std::cout << "AIMF file created\n";

}
