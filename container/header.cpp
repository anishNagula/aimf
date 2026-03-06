#include <fstream>
#include <cstring>
#include "../include/aimf.hpp"

namespace aimf {
  
void write_header(std::ofstream &out, uint16_t stream_count) {

  AIMFHeader header;

  std::memcpy(header.magic, MAGIC, 4);
  header.version = VERSION;
  header.stream_count = stream_count;
  header.header_size = sizeof(AIMFHeader);

  out.write(reinterpret_cast<char*>(&header), sizeof(header));

}

}
