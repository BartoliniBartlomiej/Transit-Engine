#include <gtest/gtest.h>
#include <sqlite3.h>
#include <memory>
#include <string>
#include <cstdio> 

#include "../src/db/DBManager.hpp"
#include "../src/models/Station.hpp"

class DBManagerTest : public ::testing::Test {
protected:
    const std::string testDbPath = "test_train.db";
    std::unique_ptr<DBManager> dbManager_;

    void SetUp() override {
        std::remove(testDbPath.c_str());

        sqlite3* tempDb;
        ASSERT_EQ(sqlite3_open(testDbPath.c_str(), &tempDb), SQLITE_OK);

        const char* setupSql = 
            "CREATE TABLE stations (id INTEGER PRIMARY KEY, name TEXT, city TEXT);"
            "INSERT INTO stations (id, name, city) VALUES (1, 'Warszawa Centralna', 'Warszawa');"
            "INSERT INTO stations (id, name, city) VALUES (2, 'Kraków Główny', 'Kraków');";
        
        ASSERT_EQ(sqlite3_exec(tempDb, setupSql, nullptr, nullptr, nullptr), SQLITE_OK);
        sqlite3_close(tempDb);

        dbManager_ = std::make_unique<DBManager>(testDbPath);
    }

    void TearDown() override {
        dbManager_.reset();

        std::remove(testDbPath.c_str());
    }
};


TEST_F(DBManagerTest, RetrievesStationsCorrectly) {
    auto stations = dbManager_->getAllStations();

    ASSERT_EQ(stations.size(), 21);

    EXPECT_EQ(stations[0]->getId(), 1);
    EXPECT_EQ(stations[0]->getName(), "Warszawa Centralna");
    EXPECT_EQ(stations[0]->getCity(), "Warszawa");

    EXPECT_EQ(stations[1]->getId(), 2);
    EXPECT_EQ(stations[1]->getName(), "Kraków Główny");
    EXPECT_EQ(stations[1]->getCity(), "Kraków");
}


TEST_F(DBManagerTest, ReturnsEmptyWhenNoStations) {
    sqlite3* tempDb;
    sqlite3_open(testDbPath.c_str(), &tempDb);
    sqlite3_exec(tempDb, "DELETE FROM stations;", nullptr, nullptr, nullptr);
    sqlite3_close(tempDb);

    auto stations = dbManager_->getAllStations();
    
    EXPECT_TRUE(stations.empty());
}