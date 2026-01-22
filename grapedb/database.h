#pragma once
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
    std::unordered_map<std::string, long> index;
    std::string currentPath;
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
    void Open(const std::string& path);
};
} // namespace Grape