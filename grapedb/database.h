#pragma once
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

namespace grape
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
    };
}