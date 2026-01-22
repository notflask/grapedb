#include <gtest/gtest.h>
#include <sstream>
#include "../grapedb/serializer.h"
#include "../grapedb/record.h"

namespace grape
{
    TEST(SerializerTest, RoundTripTest)
    {
        std::string test_key = "test_key";
        std::string test_value = "Hello, World!";

        std::vector<char> buffer = Serializer::serialize(test_key, test_value);

        std::string data(buffer.begin(), buffer.end());
        std::istringstream iss(data, std::ios::binary);

        Record result = Serializer::deserialize(iss);

        EXPECT_TRUE(result.isValid);
        EXPECT_EQ(result.key, test_key);
        EXPECT_EQ(result.value, test_value);
    }

    TEST(SerializerTest, EmptyStringsTest)
    {
        std::vector<char> buffer = Serializer::serialize("", "");

        std::string data(buffer.begin(), buffer.end());
        std::istringstream iss(data, std::ios::binary);

        Record result = Serializer::deserialize(iss);

        EXPECT_TRUE(result.isValid);
        EXPECT_EQ(result.key, "");
        EXPECT_EQ(result.value, "");
    }

    TEST(SerializerTest, TombstoneStatusTest)
    {
        std::string key = "user";
        std::string value = "alex";

        auto buffer = Serializer::serialize(key, value);
        EXPECT_EQ(buffer[0], 1);

        buffer[0] = 0;

        std::string data(buffer.begin(), buffer.end());
        std::istringstream iss(data, std::ios::binary);

        Record result = Serializer::deserialize(iss);

        EXPECT_FALSE(result.isValid);
    }
};