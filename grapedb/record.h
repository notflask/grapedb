#pragma once
#include <string>

namespace grape
{
    struct Record
    {
        std::string key;
        std::string value;
        bool isValid;
    };
}