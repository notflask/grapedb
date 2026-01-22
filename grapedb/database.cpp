#include "database.h"

#include "serializer.h"

namespace Grape
{
void Database::LoadIndex()
{
    file.seekg(0, std::ios::beg);

    while (file.peek() != EOF)
    {
        long offset = file.tellg();

        Record record = Serializer::Deserialize(file);

        if (record.isValid)
            index[record.key] = offset;
        else
            break;
    }

    file.clear();
}

Database::Database(const std::string &filename)
{
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open())
    {
        file.open(filename, std::ios::out | std::ios::binary);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    LoadIndex();
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

    long offset = file.tellp();

    std::vector<char> data = Serializer::Serialize(key, value);
    file.write(data.data(), data.size());

    file.flush();

    index[key] = offset;
}

std::string Database::Get(const std::string &key)
{
    if (index.find(key) == index.end())
        return "";

    long offset = index[key];

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

    long offset = index[key];

    file.clear();
    file.seekp(offset, std::ios::beg);

    char tombstone = 0;
    file.write(&tombstone, sizeof(tombstone));
    file.flush();

    index.erase(key);

    return true;
}
void Database::Open(const std::string &path)
{
  currentPath = path;
  if (file.is_open()) file.close();

  file.open(path, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);

  LoadIndex();
}
} // namespace Grape