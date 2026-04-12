// Reservation.hpp

#pragma once

#include <string>
#include <memory>

class Schedule;
class Passenger;

class Reservation {
private:
    int id;
    int seat_number;
    int wagon_number;
    std::string status;
    std::weak_ptr<Schedule> schedule;
    std::weak_ptr<Passenger> passenger;
public:
    Reservation(int id, int seat_number, int wagon_number, const std::string& status,
                std::weak_ptr<Schedule> schedule, std::weak_ptr<Passenger> passenger)
        : id(id), seat_number(seat_number), wagon_number(wagon_number), status(status),
          schedule(schedule), passenger(passenger) {}
};