// DBManager.hpp

#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>

#include "../models/Station.hpp"
#include "../models/Train.hpp"

class DBManager {
private:
    sqlite3* db_;

public:
    DBManager(const std::string& db_path);
    ~DBManager();

    void initSchema();
    void seedData();

    std::vector<std::shared_ptr<Station>> getAllStations();
    std::vector<std::shared_ptr<Train>> getAllTrains();
};