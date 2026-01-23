#include "../grapedb/record.h"
#include "../grapedb/serializer.h"
#include <gtest/gtest.h>
#include <sstream>

namespace Grape
{
TEST(SerializerTest, RoundTripTest)
{
    std::string test_key = "test_key";
    std::string test_value = "Hello, World!";

    std::vector<char> buffer = Serializer::Serialize(test_key, test_value);

    std::string data(buffer.begin(), buffer.end());
    std::istringstream iss(data, std::ios::binary);

    Record result = Serializer::Deserialize(iss);

    EXPECT_TRUE(result.isValid);
    EXPECT_EQ(result.key, test_key);
    EXPECT_EQ(result.value, test_value);
}

TEST(SerializerTest, EmptyStringsTest)
{
    std::vector<char> buffer = Serializer::Serialize("", "");

    std::string data(buffer.begin(), buffer.end());
    std::istringstream iss(data, std::ios::binary);

    Record result = Serializer::Deserialize(iss);

    EXPECT_TRUE(result.isValid);
    EXPECT_EQ(result.key, "");
    EXPECT_EQ(result.value, "");
}

TEST(SerializerTest, TombstoneStatusTest)
{
    std::string key = "user";
    std::string value = "alex";

    auto buffer = Serializer::Serialize(key, value);
    EXPECT_EQ(buffer[0], 1);

    buffer[0] = 0;

    std::string data(buffer.begin(), buffer.end());
    std::istringstream iss(data, std::ios::binary);

    Record result = Serializer::Deserialize(iss);

    EXPECT_FALSE(result.isValid);
}

TEST(SerializerTest, CorruptedDataTest)
{
    std::string corrupted_data = "\x01\x00\x00\x00";
    std::istringstream iss(corrupted_data, std::ios::binary);

    Record result = Serializer::Deserialize(iss);

    EXPECT_FALSE(result.isValid);
}

TEST(SerializerTest, EmptyStreamTest)
{
    std::istringstream iss("", std::ios::binary);

    Record result = Serializer::Deserialize(iss);

    EXPECT_FALSE(result.isValid);
}
}; // namespace Grape