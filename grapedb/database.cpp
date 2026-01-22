#include "database.h"

#include "serializer.h"

namespace Grape
{
void Database::LoadIndex()
{
    file.seekg(0, std::ios::beg);
    index.clear();

    while (file.peek() != EOF)
    {
        int64_t offset = file.tellg();
        Record record = Serializer::Deserialize(file);

        if (record.isValid)
        {
            if (record.value.empty())
            {
                index.erase(record.key);
            }
            else
            {
                index[record.key] = offset;
            }
        }
        else
        {
            break;
        }
    }

    file.clear();
}

Database::Database(const std::string &filename)
{
    Open(filename);
}

Database::~Database()
{
    if (file.is_open())
        file.close();
}

void Database::Set(const std::string &key, const std::string &value)
{
    file.clear();
    file.seekp(0, std::ios::end);

    int64_t offset = file.tellp();

    std::vector<char> data = Serializer::Serialize(key, value);
    file.write(data.data(), data.size());

    file.flush();

    index[key] = offset;
}

std::string Database::Get(const std::string &key)
{
    if (index.find(key) == index.end())
        return "";

    int64_t offset = index[key];

    file.clear();
    file.seekg(offset, std::ios::beg);

    Record record = Serializer::Deserialize(file);

    if (record.isValid)
        return record.value;

    return "";
}

bool Database::Delete(const std::string &key)
{
    if (index.find(key) == index.end())
    {
        return false;
    }

    Set(key, "");

    index.erase(key);

    return true;
}

void Database::Open(const std::string &path)
{
    if (file.is_open())
        file.close();

    currentPath = path;
    file.open(currentPath, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);

    if (!file.is_open())
    {
        file.open(currentPath, std::ios::out | std::ios::binary);
        file.close();
        file.open(currentPath, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    }

    LoadIndex();
}
} // namespace Grape