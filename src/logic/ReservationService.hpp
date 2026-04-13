// ReservationService.hpp

#pragma once

#include "../db/DBManager.hpp"

enum PriceLevel {
    TLK = 70, 
    IC = 100,
    EIP = 150
};

class ReservationService {
private:
    DBManager& db_;
public:
    ReservationService(DBManager& db) : db_(db) {}

    // double calculatePrice();
    bool bookSeat(int schedule_id, int passenger_id, int wagon_number, int seat_number);
};