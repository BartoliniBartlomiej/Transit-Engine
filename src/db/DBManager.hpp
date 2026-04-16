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
#include "loadSQL.hpp"


class DBManager {
private:
    sqlite3* db_;

public:
    explicit DBManager(const std::string& dbPath = "db/train.db");
    ~DBManager();

    void initSchema();
    void seedData();

    // getters
    std::vector<std::shared_ptr<Station>> getAllStations();
    std::vector<std::shared_ptr<Train>> getAllTrains();
    std::vector<std::shared_ptr<Schedule>> getAllSchedules();
    std::vector<std::unique_ptr<Wagon>> getWagonsForTrain(int train_id);
    std::vector<std::shared_ptr<Reservation>> getReservationsForPassenger(int passenger_id);

    std::vector<std::shared_ptr<Schedule>> getAllSchedulesFromStationToStation(std::string from, std::string to);

    std::string getTrainType(int train_id);
    // reservation operations 
    bool isSeatAvailable(int schedule_id, int wagon_number, int seat_number);
    bool isWagonAndSeatValid(int schedule_id, int wagon_number, int seat_number);

    bool saveReservation(int schedule_id, int passenger_id, int wagon_number, int seat_number);
    bool saveReservation(int schedule_id, int passenger_id, int wagon_number, int seat_number, double price);
};