// Train.hpp
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Wagon.hpp"

class Train {
private:
    int id;
    std::string name;
    std::string type;
    std::vector<std::unique_ptr<Wagon>> wagons;
public:
    Train(int id, const std::string& name, const std::string& type)
        : id(id), name(name), type(type) {}

    void addWagon(std::unique_ptr<Wagon> wagon) {
        wagons.push_back(std::move(wagon));
    }

    // getters 
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getType() const { return type; }
    const Wagon* getWagon(int wagon_number) const {
        for (const auto& wagon : wagons) {
            if (wagon->getWagonNumber() == wagon_number) {
                return wagon.get();
            }
        }
        return nullptr;
    }
};