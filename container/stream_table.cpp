#include <fstream>
#include "../include/aimf.hpp"

namespace aimf {

void write_stream_table(std::ofstream &out, StreamDesc* streams, uint16_t count) {
  for (int i = 0; i < count; i++) {
    out.write(reinterpret_cast<char*>(&streams[i]), sizeof(StreamDesc));
  }
}

}
