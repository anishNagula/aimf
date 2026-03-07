#include <vector>
#include <string>
#include <cbor.h>

std::vector<uint8_t> encode_metadata(
  const std::string& title,
  uint64_t duration
  )
{
  cbor_item_t* root = cbor_new_definite_map(2);

  cbor_map_add(
      root,
      (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("title")),
        .value = cbor_move(cbor_build_string(title.c_str()))
      });

  cbor_map_add(
      root,
      (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("duration")),
        .value = cbor_move(cbor_build_uint64(duration))
      });

  unsigned char* buffer;
  size_t size;
  
  cbor_serialize_alloc(root, &buffer, &size);

  std::vector<uint8_t> data(buffer, buffer + size);

  free(buffer);
  cbor_decref(&root);

  return data;
}
