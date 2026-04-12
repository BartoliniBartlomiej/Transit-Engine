// Wagon.hpp
#pragma once

#include <string>

class Wagon {
private:
    int id;
    int wagon_number;
    int class_;
    int seat_count;
public:
    Wagon(int id, int wagon_number, int class_, int seat_count)
        : id(id), wagon_number(wagon_number), class_(class_), seat_count(seat_count) {}

    // getters
    int getId() const { return id; }
    int getWagonNumber() const { return wagon_number; }
    int getClass() const { return class_; }
    int getSeatCount() const { return seat_count; }

};