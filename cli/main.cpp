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

  // -------- HEADER --------
  AIMFHeader header;
  std::memcpy(header.magic, MAGIC, 4);
  header.version = VERSION;
  header.stream_count = 1;
  header.header_size = sizeof(AIMFHeader);
  header.index_offset = 0;   // placeholder

  out.write(reinterpret_cast<char*>(&header), sizeof(header));

  // -------- STREAM TABLE --------
  StreamDesc stream;

  stream.stream_id = 0;
  stream.stream_type = VIDEO_TOKENS;
  stream.codec_id = 1;
  stream.token_rate = 30;
  stream.compression = 0;
  stream.data_offset = 0;

  write_stream_table(out, &stream, 1);

  // -------- CHUNKS --------
  std::vector<ChunkIndexEntry> index;

  for (int t = 0; t < 5; t++)
  {
    uint64_t chunk_offset = out.tellp();

    std::vector<uint16_t> tokens = {
      (uint16_t)(100 + t),
      (uint16_t)(200 + t),
      (uint16_t)(300 + t),
      (uint16_t)(400 + t),
      (uint16_t)(500 + t)
    };

    write_chunk(out, 0, t * 1000000, tokens);

    ChunkIndexEntry entry;
    entry.stream_id = 0;
    entry.timestamp_us = t * 1000000;
    entry.file_offset = chunk_offset;

    index.push_back(entry);
  }

  // -------- WRITE INDEX --------
  uint64_t index_offset = out.tellp();

  write_chunk_index(out, index);

  // -------- UPDATE HEADER --------
  header.index_offset = index_offset;

  out.seekp(0);
  out.write(reinterpret_cast<char*>(&header), sizeof(header));

  std::cout << "AIMF file created\n";
}
