#include <iostream>
#include <memory>
#include "models/Station.hpp"
#include "models/RouteStop.hpp"
#include "models/Route.hpp"
#include "models/Wagon.hpp"
#include "models/Train.hpp"
#include "models/Schedule.hpp"
#include "models/Passenger.hpp"
#include "models/Reservation.hpp"
#include "db/DBManager.hpp"

int main() {
    DBManager db("db/train.db");
    db.initSchema();
    db.seedData();

    // // Stacje
    // auto krakow   = std::make_shared<Station>("Kraków Główny", "Kraków", 1);
    // auto katowice = std::make_shared<Station>("Katowice", "Katowice", 2);
    // auto wroclaw  = std::make_shared<Station>("Wrocław Główny", "Wrocław", 3);
    // std::cout << "Stacje utworzone" << std::endl;

    // // Trasa Kraków -> Katowice -> Wrocław
    // auto route = std::make_shared<Route>(1, "Kraków - Wrocław", 280);
    // std::cout << "Trasa utworzona" << std::endl;

    // auto stop1 = std::make_unique<RouteStop>(krakow,  "",       "08:00", 1);
    // auto stop2 = std::make_unique<RouteStop>(katowice, "08:45", "08:50", 2);
    // auto stop3 = std::make_unique<RouteStop>(wroclaw,  "10:30", "",      3);
    // route->addStop(std::move(stop1));
    // route->addStop(std::move(stop2));
    // route->addStop(std::move(stop3));
    // std::cout << "Przystanki dodane" << std::endl;

    // // Pociąg z wagonami
    // auto train = std::make_shared<Train>(1, "IC 1234", "IC");
    // auto w1 = std::make_unique<Wagon>(1, 1, 2, 60);
    // auto w2 = std::make_unique<Wagon>(2, 2, 1, 30);
    // train->addWagon(std::move(w1));
    // train->addWagon(std::move(w2));
    // const Wagon* w = train->getWagon(1);
    // if (w) {
    //     std::cout << "Znaleziono wagon nr: " << w->getWagonNumber() << std::endl;
    // } else {
    //     std::cout << "Nie znaleziono wagonu" << std::endl;
    // }
    // std::cout << "Pociąg utworzony" << std::endl;

    // // Pasażer
    // auto passenger = std::make_shared<Passenger>(1, "Jan", "Kowalski", "jan@example.com", "123456789");
    // std::cout << "Pasażer utworzony" << std::endl;

    // // Schedule
    // auto schedule = std::make_shared<Schedule>(1, route, train, "2025-08-01", "08:00");
    // std::cout << "Schedule utworzony" << std::endl;

    // // Rezerwacja
    // auto reservation = std::make_shared<Reservation>(1, 14, 1, "active",
    //                     std::weak_ptr<Schedule>(schedule),
    //                     std::weak_ptr<Passenger>(passenger));
    // std::cout << "Rezerwacja utworzona" << std::endl;

    auto stations = db.getAllStations();
    for (const auto& s : stations) {
        std::cout << s->getName() << " - " << s->getCity() << std::endl;
    }

    auto trains = db.getAllTrains();
    for (const auto& t : trains) {
        std::cout << t->getName() << " - " << t->getType() << std::endl;
    }
    std::cout << "OK." << std::endl;

    return 0;
}