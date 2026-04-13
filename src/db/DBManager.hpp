// DBManager.hpp

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>

#include "../models/Station.hpp"
#include "../models/Train.hpp"
#include "../models/Schedule.hpp"
#include "../models/Reservation.hpp"

class DBManager {
private:
    sqlite3* db_;

public:
    DBManager(const std::string& db_path);
    ~DBManager();

    void initSchema();
    void seedData();

    // getters
    std::vector<std::shared_ptr<Station>> getAllStations();
    std::vector<std::shared_ptr<Train>> getAllTrains();
    std::vector<std::shared_ptr<Schedule>> getAllSchedules();
    std::vector<std::unique_ptr<Wagon>> getWagonsForTrain(int train_id);
    std::vector<std::shared_ptr<Reservation>> getReservationsForPassenger(int passenger_id);

    // reservation operations 
    bool isSeatAvailable(int schedule_id, int wagon_number, int seat_number);
    bool saveReservation(int schedule_id, int passenger_id, int wagon_number, int seat_number);
};