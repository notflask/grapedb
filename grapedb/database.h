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
    std::string filename;

    std::unordered_map<std::string, long> index;

    void load_index();

public:
    Database(const std::string &filename);
    ~Database();

public:
    void Set(const std::string &key, const std::string &value);
    std::string Get(const std::string &key);
    bool Delete(const std::string &key);
};
} // namespace Grape