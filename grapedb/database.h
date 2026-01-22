#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

namespace Grape
{
class Database
{
private:
    std::fstream file;
    std::unordered_map<std::string, int64_t> index;
    std::string currentPath;
    int64_t compactionThreshold = 10 * 1024 * 1024;

private:
    void LoadIndex();

public:
    Database(const std::string &filename);
    Database() = default;
    ~Database();

public:
    void Set(const std::string &key, const std::string &value);
    std::string Get(const std::string &key);
    bool Delete(const std::string &key);
    void Open(const std::string &path);

    void SetCompactionThreshold(int64_t threshold);
    void Compact();
};
} // namespace Grape