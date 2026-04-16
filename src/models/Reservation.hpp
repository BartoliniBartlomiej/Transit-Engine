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
    double price;
public:
    // Reservation(int id, int seat_number, int wagon_number, const std::string& status,
    //             std::weak_ptr<Schedule> schedule, std::weak_ptr<Passenger> passenger)
    //     : id(id), seat_number(seat_number), wagon_number(wagon_number), status(status),
    //       schedule(schedule), passenger(passenger) {}

    Reservation(int id, int seat_number, int wagon_number, const std::string& status,
            std::weak_ptr<Schedule> schedule, std::weak_ptr<Passenger> passenger, double price)
    : id(id), seat_number(seat_number), wagon_number(wagon_number), status(status),
      schedule(schedule), passenger(passenger), price(price) {}
      
    // getters
    int getId() const { return id; }
    int getWagonNumber() const { return wagon_number; }
    int getSeatNumber() const { return seat_number; } 
    std::string getStatus() const { return status; }
    double getPrice() const { return price; }
    // std::shared_ptr<Schedule> getSchedule() const { return schedule.lock(); }
    // std::shared_ptr<Passenger> getPassenger() const { return passenger.lock(); } //
};