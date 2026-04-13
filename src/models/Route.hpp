// Route.hpp

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "RouteStop.hpp"

class Route {
private:
    int id;
    std::string name;
    int distance_km;
    std::vector<std::unique_ptr<RouteStop>> stations;

public:    
    Route(int id, const std::string& name, int distance_km)
        : id(id), name(name), distance_km(distance_km) {}



    // getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    int getDistanceKm() const { return distance_km; }
    const std::vector<std::unique_ptr<RouteStop>>& getStations() const { return stations; }
    
};