#include <thread>
#include <chrono>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "aumdb/db.h" // Ensure you provide a proper relative path or setup your include paths correctly.

namespace
{

    using namespace AumDb;

    class DBTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Start the DB
            auto startResult = start();
            EXPECT_EQ(startResult.error_code_, nullptr);
        }

        void TearDown() override
        {
            stop();
        }
    };

    TEST_F(DBTest, CreateTableNoPkey)
    {
        auto result = executeQuery("CREATE TABLE users(id INT);");
        EXPECT_EQ(*result.error_code_, ErrorCode::BadInput);
        EXPECT_EQ(*result.err_msg_, "NoPrimaryKey");
    }

    TEST_F(DBTest, CreateTableMalformed)
    {
        auto result = executeQuery("CREATE TABEL users(id INT);"); // "TABEL" is deliberately misspelled
        EXPECT_EQ(*result.error_code_, ErrorCode::BadInput);
        EXPECT_NE(result.error_code_, nullptr);
    }

    TEST_F(DBTest, EmptyCommand)
    {
        auto result = executeQuery("");
        EXPECT_EQ(*result.error_code_, ErrorCode::BadInput);
        EXPECT_NE(result.err_msg_, nullptr);
    }

    TEST_F(DBTest, NonCreateTableCommands)
    {
        auto result = executeQuery("INSERT INTO users VALUES(1, 'John');");
        EXPECT_EQ(*result.error_code_, ErrorCode::BadInput);
        EXPECT_EQ(*result.err_msg_, "Unexpected query");
    }

    TEST_F(DBTest, CreateTableSuccess)
    {
        auto result = executeQuery("CREATE TABLE users(id INT PRIMARY KEY);");
        EXPECT_EQ(result.error_code_, nullptr);
        EXPECT_EQ(result.err_msg_, nullptr);

        // std::this_thread::sleep_for(std::chrono::minutes(3));
    }

    TEST_F(DBTest, CreateTableAlreadyExists)
    {
        auto result1 = executeQuery("CREATE TABLE users(id INT PRIMARY KEY);");
        EXPECT_EQ(result1.error_code_, nullptr);

        auto result2 = executeQuery("CREATE TABLE users(id INT PRIMARY KEY);");
        EXPECT_EQ(*result2.error_code_, ErrorCode::ResourceAlreadyExists);
        EXPECT_EQ(*result2.err_msg_, "TableAlreadyExists");
    }
} // namespace
