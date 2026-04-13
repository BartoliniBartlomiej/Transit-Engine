// CLI.hpp

#pragma once

#include "../logic/ReservationService.hpp"
#include "../db/DBManager.hpp"

class CLI {
private:
    DBManager& db_;
    ReservationService& service_;

    void listSchedules();
    void makeReservation();
    void listReservations();

public:
    CLI(DBManager& db, ReservationService& service)
        : db_(db), service_(service) {}

    void run();
};