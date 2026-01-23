#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace Grape
{

enum class CompactionState : uint8_t
{
    NONE = 0,
    IN_PROGRESS = 1,
    READY_TO_SWAP = 2
};

class Database
{
private:
    std::fstream file;
    std::unordered_map<std::string, int64_t> index;
    std::string currentPath;
    std::mutex dbMutex;

    int64_t compactionThreshold = 10 * 1024 * 1024;

private:
    void LoadIndex();
    void WriteCompactionMarker(CompactionState state);
    CompactionState ReadCompactionMarker();
    void RecoverFromInterruptedCompaction();

public:
    Database(const std::string &filename);
    Database() = default;
    ~Database();

private:
    void SetImpl(const std::string &key, const std::string &value);
    void CompactImpl();

public:
    std::vector<std::string> ListKeys();
    void Set(const std::string &key, const std::string &value);

    std::string Get(const std::string &key);
    bool Delete(const std::string &key);
    void Open(const std::string &path);

    void SetCompactionThreshold(int64_t threshold);
    void Compact();
};
} // namespace Grape