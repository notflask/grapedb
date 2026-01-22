#pragma once
#include "record.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <istream>
#include <string>
#include <vector>

namespace Grape
{
class Serializer
{
public:
    static std::vector<char> Serialize(const std::string &key, const std::string &value);
    static Record Deserialize(std::istream &is);
};
} // namespace Grape