#pragma once
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <istream>
#include "record.h"

namespace grape
{
    class Serializer
    {
    public:
        static std::vector<char> serialize(const std::string &key, const std::string &value);
        static Record deserialize(std::istream &is);
    };
}