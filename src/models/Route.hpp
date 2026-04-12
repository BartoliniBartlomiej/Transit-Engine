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

    void addStop(std::unique_ptr<RouteStop> stop) {
        stations.push_back(std::move(stop));
        std::cout << "Added stop: " << stations.back()->getStationName() << std::endl;
    }
};