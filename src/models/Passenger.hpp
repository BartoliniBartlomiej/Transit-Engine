// Passenger.hpp

#pragma once

#include <string>

class Passenger {
private:
    int id;
    std::string name;
    std::string surname;
    std::string email;
    std::string phone_number;
public:
    Passenger(int id, const std::string& name, const std::string& surname, const std::string& email, const std::string& phone_number)
        : id(id), name(name), surname(surname), email(email), phone_number(phone_number) {}
};