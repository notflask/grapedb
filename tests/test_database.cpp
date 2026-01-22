#include <gtest/gtest.h>
#include <filesystem>
#include "../grapedb/database.h"

namespace grape
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
        Database db(test_db_name);
        EXPECT_EQ(db.Get("unknown_key"), "");
    }
};