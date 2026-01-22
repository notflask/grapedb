#include "serializer.h"

namespace Grape
{
std::vector<char> Serializer::Serialize(const std::string &key, const std::string &value)
{
    std::vector<char> buffer;

    buffer.push_back(1);

    uint32_t key_len = static_cast<uint32_t>(key.size());
    char *k_ptr = reinterpret_cast<char *>(&key_len);
    buffer.insert(buffer.end(), k_ptr, k_ptr + sizeof(uint32_t));
    buffer.insert(buffer.end(), key.begin(), key.end());

    uint32_t val_len = static_cast<uint32_t>(value.size());
    char *v_ptr = reinterpret_cast<char *>(&val_len);
    buffer.insert(buffer.end(), v_ptr, v_ptr + sizeof(uint32_t));
    buffer.insert(buffer.end(), value.begin(), value.end());

    return buffer;
}

Record Serializer::Deserialize(std::istream &is)
{
    Record record;
    record.isValid = false;

    char status;
    if (!is.read(&status, 1))
        return record;

    bool isActive = (status == 1);

    uint32_t key_len;
    is.read(reinterpret_cast<char *>(&key_len), sizeof(uint32_t));
    std::string key(key_len, ' ');
    is.read(&key[0], key_len);

    uint32_t val_len;
    is.read(reinterpret_cast<char *>(&val_len), sizeof(uint32_t));
    std::string value(val_len, ' ');
    is.read(&value[0], val_len);

    if (isActive)
    {
        record.key = key;
        record.value = value;
        record.isValid = true;
    }

    return record;
}
}; // namespace Grape