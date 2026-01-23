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

void Database::WriteCompactionMarker(CompactionState state)
{
    std::string markerPath = currentPath + ".compaction";
    std::ofstream marker(markerPath, std::ios::binary);

    if (!marker.is_open())
    {
        throw std::runtime_error("cannot write compaction marker");
    }

    char stateChar = static_cast<char>(state);

    marker.write(&stateChar, 1);
    marker.flush();
    marker.close();
}

CompactionState Database::ReadCompactionMarker()
{
    std::string markerPath = currentPath + ".compaction";

    if (!std::filesystem::exists(markerPath))
        return CompactionState::NONE;

    std::ifstream marker(markerPath, std::ios::binary);
    char stateChar;

    if (!marker.read(&stateChar, 1))
        return CompactionState::NONE;

    return static_cast<CompactionState>(stateChar);
}

void Database::RecoverFromInterruptedCompaction()
{
    std::string tempPath = currentPath + ".tmp";
    CompactionState state = ReadCompactionMarker();

    switch (state)
    {
    case CompactionState::READY_TO_SWAP:
    {
        std::cout << "recovering: completing interrupted compaction swap" << std::endl;

        if (std::filesystem::exists(tempPath))
            std::filesystem::rename(tempPath, currentPath);

        break;
    }
    case CompactionState::IN_PROGRESS:
    {
        std::cout << "recovering: discarding incomplete compaction" << std::endl;

        if (std::filesystem::exists(tempPath))
            std::filesystem::remove(tempPath);

        break;
    }
    case CompactionState::NONE:
        break;
    }

    std::filesystem::remove(currentPath + ".compaction");
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

    if (std::filesystem::exists(currentPath + ".compaction"))
        RecoverFromInterruptedCompaction();

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

    WriteCompactionMarker(CompactionState::IN_PROGRESS);

    std::fstream tempFile;
    tempFile.open(tempPath, std::ios::out | std::ios::binary);

    if (!tempFile.is_open())
    {
        WriteCompactionMarker(CompactionState::NONE);
        throw std::runtime_error("could not create temp file for compaction");
    }

    for (auto &pair : index)
    {
        std::string key = pair.first;
        int64_t old_offset = pair.second;

        file.seekg(old_offset, std::ios::beg);
        Record record = Serializer::Deserialize(file);

        if (!record.isValid)
        {
            tempFile.close();
            std::filesystem::remove(tempPath);
            WriteCompactionMarker(CompactionState::NONE);
            throw std::runtime_error("corrupted record during compaction");
        }

        int64_t new_offset = tempFile.tellp();

        std::vector<char> data = Serializer::Serialize(key, record.value);
        tempFile.write(data.data(), data.size());

        index[key] = new_offset;
    }

    tempFile.flush();
    tempFile.close();

    WriteCompactionMarker(CompactionState::READY_TO_SWAP);

    file.close();

    std::filesystem::rename(tempPath, currentPath);

    file.open(currentPath, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);

    std::filesystem::remove(currentPath + ".compaction");
}

void Database::Compact()
{
    std::lock_guard<std::mutex> lock(dbMutex);
    CompactImpl();
}
} // namespace Grape