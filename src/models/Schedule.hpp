// Schedule.hpp
#pragma once
#include <string>
#include <memory>
#include "Route.hpp"
#include "Train.hpp"

class Schedule {
private:
    int id;
    std::shared_ptr<Route> route;
    std::shared_ptr<Train> train;
    std::string departure_date;
    std::string departure_time;
public:
    Schedule(int id, const std::shared_ptr<Route>& route, const std::shared_ptr<Train>& train,
    const std::string& departure_date, const std::string& departure_time)
    : id(id), route(route), train(train), departure_date(departure_date), departure_time(departure_time) {}

    //getters
    int getId() const { return id; }
    std::shared_ptr<Route> getRoute() const { return route; }
    std::shared_ptr<Train> getTrain() const { return train; }
    std::string getDepartureDate() const { return departure_date; }
    std::string getDepartureTime() const { return departure_time; }

};