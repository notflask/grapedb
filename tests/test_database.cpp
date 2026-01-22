#include "../grapedb/database.h"
#include <filesystem>
#include <gtest/gtest.h>

namespace Grape
{
class DatabaseTest : public ::testing::Test
{
private:
    void remove_existing_database()
    {
        if (std::filesystem::exists(test_db_name))
            std::filesystem::remove(test_db_name);
    }

protected:
    const std::string test_db_name = "test_db.grapedb";

    void SetUp() override
    {
        remove_existing_database();
    }

    void TearDown() override
    {
        remove_existing_database();
    }
};

TEST_F(DatabaseTest, PutAndGetTest)
{
    Database db(test_db_name);

    std::string key = "name";
    std::string value = "Dmigor";

    db.Set(key, value);

    std::string result = db.Get(key);

    EXPECT_EQ(result, value);
}

TEST_F(DatabaseTest, NonExistentKeyTest)
{
    Database db;
    db.Open(test_db_name);
    EXPECT_EQ(db.Get("unknown_key"), "");
}

TEST_F(DatabaseTest, FullDeleteCycleTest)
{
    std::string key = "session_id";
    std::string value = "abc_123";

    Database db;
    db.Open(test_db_name);

    db.Set(key, value);
    EXPECT_EQ(db.Get(key), value);

    bool deleted = db.Delete(key);
    EXPECT_TRUE(deleted);

    db.Open(test_db_name);
    EXPECT_EQ(db.Get(key), "");
}

TEST_F(DatabaseTest, CompactionTest)
{
    Database db;
    db.Open(test_db_name);

    for (int i = 0; i < 1000; ++i)
    {
        db.Set("key", "value_" + std::to_string(i));
    }

    uintmax_t size_before = std::filesystem::file_size(test_db_name);
    std::cout << "Size before compaction: " << size_before << " bytes" << std::endl;

    db.Compact();

    uintmax_t size_after = std::filesystem::file_size(test_db_name);
    std::cout << "Size after compaction: " << size_after << " bytes" << std::endl;

    EXPECT_EQ(db.Get("key"), "value_999");

    EXPECT_LT(size_after, size_before);
}

TEST_F(DatabaseTest, AutoCompactionTriggeredTest)
{
    Database db;
    db.Open(test_db_name);

    int64_t low_threshold = 10 * 1024;
    db.SetCompactionThreshold(low_threshold);

    std::string key = "spam_key";
    std::string value(1024, 'x');

    for (int i = 0; i < 15; ++i)
    {
        db.Set(key, value);
    }

    auto file_size = std::filesystem::file_size(test_db_name);

    std::cout << "Final file size: " << file_size << " bytes" << std::endl;

    EXPECT_LT(file_size, low_threshold);

    EXPECT_EQ(db.Get(key), value);
}
}; // namespace Grape