#include "serializer.h"

namespace grape
{
    std::vector<char> Serializer::serialize(const std::string &key, const std::string &value)
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

    Record Serializer::deserialize(std::istream &is)
    {
        Record record;
        record.isValid = false;

        uint32_t key_length = 0;
        uint32_t value_length = 0;

        if (!is.read(reinterpret_cast<char *>(&key_length), sizeof(uint32_t)))
            return record;

        if (!is.read(reinterpret_cast<char *>(&value_length), sizeof(uint32_t)))
            return record;

        record.key.resize(key_length);
        record.value.resize(value_length);

        if (key_length > 0)
            if (!is.read(&record.key[0], key_length))
                return record;

        if (value_length > 0)
            if (!is.read(&record.value[0], value_length))
                return record;

        record.isValid = true;
        return record;
    }
};