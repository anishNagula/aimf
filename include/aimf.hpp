#pragma once
#include <cstdint>
#include <vector>

namespace aimf {

const char MAGIC[4] = {'A', 'I', 'M','F'};
const uint16_t VERSION = 1;

enum StreamType : uint8_t {
  AUDIO_TOKENS = 0,
  VIDEO_TOKENS = 1,
  METADATA = 2
};

enum CodecID : uint16_t {
    CODEC_ENCODEC = 1,
    CODEC_VQGAN = 2,
    CODEC_DUMMY = 999
};

struct AIMFHeader {
  char magic[4];
  uint16_t version;
  uint16_t stream_count;
  uint16_t header_size;
  uint64_t index_offset;
};

struct StreamDesc {
  uint8_t stream_id;
  uint8_t stream_type;
  uint16_t codec_id;
  uint32_t token_rate;
  uint8_t compression;
  uint64_t data_offset;
};

struct ChunkHeader {
  uint8_t stream_id;
  uint64_t timestamp_us;
  uint32_t token_count;
};

struct ChunkIndexEntry {
  uint8_t stream_id;
  uint64_t timestamp_us;
  uint64_t file_offset;
};

void write_stream_table(std::ofstream &out, StreamDesc* streams, uint16_t count);

void write_chunk(
    std::ofstream &out,
    uint8_t stream_id,
    uint64_t timestamp_us,
    std::vector<uint16_t> &tokens);

void write_chunk_index(
    std::ofstream &out,
    std::vector<ChunkIndexEntry> &index);

}
