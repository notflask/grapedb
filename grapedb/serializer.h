#pragma once
#include "record.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <istream>
#include <optional>
#include <string>
#include <vector>


namespace Grape
{
class Serializer
{
private:
    static void WriteUInt32LE(std::vector<char> &buffer, uint32_t value);
    static std::optional<uint32_t> ReadUInt32LE(std::istream &is);

public:
    static std::vector<char> Serialize(const std::string &key, const std::string &value);
    static Record Deserialize(std::istream &is);
};
} // namespace Grape