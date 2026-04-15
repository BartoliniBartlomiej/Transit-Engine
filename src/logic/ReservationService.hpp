// ReservationService.hpp

#pragma once

#include "../db/DBManager.hpp"

enum class TrainPriceLevel {
    TLK = 70, 
    IC = 100,
    EIP = 150
};

class ReservationService {
private:
    DBManager& db_;
public:
    ReservationService(DBManager& db) : db_(db) {}
    // ReservationService();

    bool bookSeat(int schedule_id, int passenger_id, int wagon_number, int seat_number);
    
    double calculatePrice(const Schedule& schedule, int wagon_class);
};