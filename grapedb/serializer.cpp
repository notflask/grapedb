#include "serializer.h"

namespace Grape
{
void Serializer::WriteUInt32LE(std::vector<char> &buffer, uint32_t value)
{
    buffer.push_back(static_cast<char>(value & 0xFF));
    buffer.push_back(static_cast<char>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<char>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<char>((value >> 24) & 0xFF));
}

std::optional<uint32_t> Serializer::ReadUInt32LE(std::istream &is)
{
    unsigned char bytes[4];

    if (!is.read(reinterpret_cast<char *>(bytes), 4))
    {
        return std::nullopt;
    }

    // Збираємо число з байтів
    uint32_t value = static_cast<uint32_t>(bytes[0]) | (static_cast<uint32_t>(bytes[1]) << 8) |
                     (static_cast<uint32_t>(bytes[2]) << 16) | (static_cast<uint32_t>(bytes[3]) << 24);

    return value;
}

std::vector<char> Serializer::Serialize(const std::string &key, const std::string &value)
{
    std::vector<char> buffer;

    buffer.push_back(1);

    WriteUInt32LE(buffer, static_cast<uint32_t>(key.size()));

    buffer.insert(buffer.end(), key.begin(), key.end());

    WriteUInt32LE(buffer, static_cast<uint32_t>(value.size()));

    buffer.insert(buffer.end(), value.begin(), value.end());

    return buffer;
}

Record Serializer::Deserialize(std::istream &is)
{
    Record record;
    record.isValid = false;

    char status;
    if (!is.read(&status, 1))
    {
        return record;
    }

    bool isActive = (status == 1);

    auto key_len_opt = ReadUInt32LE(is);
    if (!key_len_opt.has_value())
    {
        return record;
    }
    uint32_t key_len = key_len_opt.value();

    std::string key(key_len, '\0');
    if (!is.read(key.data(), key_len))
    {
        return record;
    }

    auto val_len_opt = ReadUInt32LE(is);
    if (!val_len_opt.has_value())
    {
        return record;
    }
    uint32_t val_len = val_len_opt.value();

    std::string value(val_len, '\0');
    if (!is.read(value.data(), val_len))
    {
        return record;
    }

    if (isActive)
    {
        record.key = std::move(key);
        record.value = std::move(value);
        record.isValid = true;
    }

    return record;
}
}; // namespace Grape