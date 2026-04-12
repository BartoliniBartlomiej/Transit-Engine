// RouteStop.hpp

#pragma once

#include <string>
#include "Station.hpp"
class RouteStop {
private:
    std::shared_ptr<Station> station;
    std::string arrival_time;
    std::string departure_time;
    int stop_number;
public:
    RouteStop(const std::shared_ptr<Station>& station, const std::string& arrival_time, const std::string& departure_time, int stop_number)
        : station(station), arrival_time(arrival_time), departure_time(departure_time), stop_number(stop_number) {}

    // getters 
    std::string getStationName() const { return station->getName(); }
    std::string getArrivalTime() const { return arrival_time; }
    std::string getDepartureTime() const { return departure_time; }
    int getStopNumber() const { return stop_number; }
};