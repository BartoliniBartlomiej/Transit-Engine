// Station.hpp

#pragma once
#include <string>

class Station {
private:
    std::string name;
    std::string city;
    int id;

public:
    Station(const std::string& name, const std::string& city, int id)
        : name(name), city(city), id(id) {}

    // getters
    std::string getName() const { return name; }
    std::string getCity() const { return city; }
    int getId() const { return id; }
};