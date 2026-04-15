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
    double price; // TODO: calculate price based on schedule and wagon class
public:
    Reservation(int id, int seat_number, int wagon_number, const std::string& status,
                std::weak_ptr<Schedule> schedule, std::weak_ptr<Passenger> passenger)
        : id(id), seat_number(seat_number), wagon_number(wagon_number), status(status),
          schedule(schedule), passenger(passenger) {}

    // getters
    int getId() const { return id; }
    int getWagonNumber() const { return wagon_number; }
    int getSeatNumber() const { return seat_number; } 
    std::string getStatus() const { return status; }
    // std::shared_ptr<Schedule> getSchedule() const { return schedule.lock(); }
    // std::shared_ptr<Passenger> getPassenger() const { return passenger.lock(); } //
};