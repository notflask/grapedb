#pragma once
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <istream>

#include "record.h"

namespace grape
{
    class Deserializer
    {
    public:
        static Record deserialize(std::istream &is)
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
}