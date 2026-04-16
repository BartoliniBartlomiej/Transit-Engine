// Passenger.hpp

#pragma once

#include <string>
#include <memory>

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

    // getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getSurname() const { return surname; }
    std::string getEmail() const { return email; }
    std::string getPhoneNumber() const { return phone_number; }
    
};