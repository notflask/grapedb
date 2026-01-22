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

void Database::SetImpl(const std::string &key, const std::string &value)
{
    file.clear();
    file.seekp(0, std::ios::end);

    int64_t offset = file.tellp();

    std::vector<char> data = Serializer::Serialize(key, value);
    file.write(data.data(), data.size());

    file.flush();

    index[key] = offset;

    if (file.tellp() > compactionThreshold)
    {
        CompactImpl();
    }
}

std::vector<std::string> Database::ListKeys()
{
    std::lock_guard<std::mutex> lock(dbMutex);

    std::vector<std::string> keys;
    keys.reserve(index.size());

    for (const auto &pair : index)
    {
        keys.push_back(pair.first);
    }

    return keys;
}

void Database::Set(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    SetImpl(key, value);
}

std::string Database::Get(const std::string &key)
{
    std::lock_guard<std::mutex> lock(dbMutex);

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
    std::lock_guard<std::mutex> lock(dbMutex);

    if (index.find(key) == index.end())
    {
        return false;
    }

    SetImpl(key, "");

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

void Database::SetCompactionThreshold(int64_t threshold)
{
    this->compactionThreshold = threshold;
}

void Database::CompactImpl()
{
    std::string tempPath = currentPath + ".tmp";
    std::fstream tempFile;

    tempFile.open(tempPath, std::ios::out | std::ios::binary);

    if (!tempFile.is_open())
        throw std::runtime_error("could not create temp file for compaction");

    for (auto &pair : index)
    {
        std::string key = pair.first;
        int64_t old_offset = pair.second;

        file.seekg(old_offset, std::ios::beg);
        Record record = Serializer::Deserialize(file);

        if (!record.isValid)
            return;

        int64_t new_offset = tempFile.tellp();

        std::vector<char> data = Serializer::Serialize(key, record.value);
        tempFile.write(data.data(), data.size());

        index[key] = new_offset;
    }

    tempFile.close();
    file.close();

    std::filesystem::rename(tempPath, currentPath);

    Open(currentPath);
}

void Database::Compact()
{
    std::lock_guard<std::mutex> lock(dbMutex);
    CompactImpl();
}
} // namespace Grape