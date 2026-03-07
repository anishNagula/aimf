#pragma once
#include <vector>
#include <string>
#include <cstdint>

std::vector<uint8_t> encode_metadata(
  const std::string& title,
  uint64_t duration
);
