// ReservationService.hpp

#pragma once

#include "../db/DBManager.hpp"

class ReservationService {
private:
    DBManager& db_;
public:
    ReservationService(DBManager& db) : db_(db) {}

    bool bookSeat(int schedule_id, int passenger_id, int wagon_number, int seat_number);
};