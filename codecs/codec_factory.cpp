#include "../include/codec_factory.hpp"
#include "../include/aimf.hpp"
#include "dummy_codec.cpp"

Codec* create_codec(uint16_t codec_id)
{
    if (codec_id == aimf::CODEC_DUMMY)
        return new DummyCodec();

    return nullptr;
}
