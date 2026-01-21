#pragma once
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>

namespace grape
{
    class Serializer
    {
    public:
        static std::vector<char> serialize(const std::string &key, const std::string &value)
        {
            uint32_t key_length = static_cast<uint32_t>(key.size());
            uint32_t value_length = static_cast<uint32_t>(value.size());

            size_t total_size = sizeof(uint32_t) * 2 + key_length + value_length;

            std::vector<char> buffer(total_size);
            char *ptr = buffer.data();

            std::memcpy(ptr, &key_length, sizeof(uint32_t));
            ptr += sizeof(uint32_t);

            std::memcpy(ptr, &value_length, sizeof(uint32_t));
            ptr += sizeof(uint32_t);

            std::memcpy(ptr, key.data(), key_length);
            ptr += key_length;

            std::memcpy(ptr, value.data(), value_length);

            return buffer;
        }
    };
}