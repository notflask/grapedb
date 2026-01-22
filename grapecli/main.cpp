#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <vector>

#include "../grapedb/database.h"
#include <cxxopts.hpp>

enum class Command
{
    GET,
    SET,
    DELETE,
    COMPACT,
    EXIT,
    THRESHOLD,
    LIST,
    UNKNOWN
};

Command ParseCommand(const std::string &str)
{
    if (str == "SET")
        return Command::SET;
    if (str == "GET")
        return Command::GET;
    if (str == "DELETE")
        return Command::DELETE;
    if (str == "COMPACT")
        return Command::COMPACT;
    if (str == "EXIT")
        return Command::EXIT;
    if (str == "THRESHOLD")
        return Command::THRESHOLD;
    if (str == "LIST")
        return Command::LIST;
    return Command::UNKNOWN;
}

int main(int argc, char **argv)
{
    cxxopts::Options options("grapecli", "Interactive CLI for GrapeDB");

    // clang-format off
    options.add_options()
        ("p,path", "Path to database file", cxxopts::value<std::string>()->default_value("default.grape"))
        ("t,threshold", "Compaction threshold in bytes", cxxopts::value<int64_t>()->default_value("10485760"))
        ("h,help", "Print usage");
    // clang-format on

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::println("{}", options.help());
        return 0;
    }

    std::string dbPath = result["path"].as<std::string>();
    int64_t threshold = result["threshold"].as<int64_t>();

    Grape::Database db;
    db.Open(dbPath);
    db.SetCompactionThreshold(threshold);

    std::println("opened database '{}' with threshold {}", dbPath, threshold);

    while (true)
    {
        std::string line;
        std::cout << "grape> ";
        if (!std::getline(std::cin, line))
            break;

        std::stringstream ss(line);
        std::string cmdStr;
        ss >> cmdStr;

        std::transform(cmdStr.begin(), cmdStr.end(), cmdStr.begin(), [](unsigned char c) { return std::toupper(c); });
        Command cmd = ParseCommand(cmdStr);

        switch (cmd)
        {
        case Command::GET:
        {
            std::string key;
            ss >> key;

            auto value = db.Get(key);
            if (value.empty())
                value = "(nil)";

            std::println("'{}' = '{}'", key, value);

            break;
        }
        case Command::SET:
        {
            std::string key, value;

            ss >> key;
            std::getline(ss >> std::ws, value);

            db.Set(key, value);
            std::println("set key '{}' to value '{}'", key, value);

            break;
        }
        case Command::DELETE:
        {
            std::string key;
            ss >> key;

            if (db.Delete(key))
                std::println("deleted value with key '{}'", key);
            else
                std::println("(error) key '{}' not found", key);

            break;
        }
        case Command::COMPACT:
        {
            db.Compact();
            std::println("compaction done");
            break;
        }
        case Command::THRESHOLD:
        {
            std::string newThreshold;
            ss >> newThreshold;

            try
            {
                db.SetCompactionThreshold(std::stoll(newThreshold));
                std::println("compaction threshold updated to '{}'", newThreshold);
            }
            catch (...)
            {
                std::println("(error) invalid number '{}'", newThreshold);
            }

            break;
        }
        case Command::LIST:
        {
            auto keys = db.ListKeys();

            if (keys.empty())
            {
                std::println("(empty database)");
            }
            else
            {
                std::println("found {} key(s): ", keys.size());

                for (const auto &key : keys)
                {
                    std::println("- {}", key);
                }
            }

            break;
        }
        case Command::EXIT:
            return 0;
        case Command::UNKNOWN:
            if (!cmdStr.empty())
                std::println("(error) unknown command '{}'", cmdStr);
            break;
        }
    }

    return 0;
}