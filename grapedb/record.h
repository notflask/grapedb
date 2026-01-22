#pragma once
#include <string>

namespace Grape
{
struct Record
{
    std::string key;
    std::string value;
    bool isValid;
};
} // namespace Grape