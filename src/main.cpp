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
#include "logic/ReservationService.hpp"
#include "cli/CLI.hpp"

int main() {
    DBManager db("db/train.db");
    db.initSchema();
    db.seedData();

    ReservationService service(db);

    // CLI cli(db, service);
    // cli.run();

    db.getAllSchedulesFromStationToStation("Kraków Główny", "Wrocław Główny");
    for (const auto& s : db.getAllSchedulesFromStationToStation("Kraków Główny", "Wrocław Główny")) {
        std::cout << s->getDepartureDate() << " " 
                  << s->getDepartureTime() << " | "
                  << s->getRoute()->getName() << " | "
                  << s->getTrain()->getName() << std::endl;
    }
    // auto stations = db.getAllStations();
    // for (const auto& s : stations) {
    //     std::cout << s->getName() << " - " << s->getCity() << std::endl;
    // }

    // auto trains = db.getAllTrains();
    // for (const auto& t : trains) {
    //     std::cout << t->getName() << " - " << t->getType() << std::endl;
    // }
    
    // auto schedules = db.getAllSchedules();
    // for (const auto& s : schedules) {
    //     std::cout << s->getDepartureDate() << " " 
    //             << s->getDepartureTime() << " | "
    //             << s->getRoute()->getName() << " | "
    //             << s->getTrain()->getName() << std::endl;
    // }

    // // auto trains = db.getAllTrains();
    // for (auto& train : trains) {
    //     auto wagons = db.getWagonsForTrain(train->getId());
    //     for (auto& wagon : wagons) {
    //         train->addWagon(std::move(wagon));
    //     }
    //     std::cout << train->getName() << " — wagons: " << train->getWagonCount() << std::endl;
    // }


    // ReservationService service(db);

    // // first reservation should succeed
    // bool r1 = service.bookSeat(1, 1, 1, 14);
    // std::cout << "Reservation 1: " << (r1 ? "OK" : "FAILED") << std::endl;

    // // second reservation for the same seat should fail
    // bool r2 = service.bookSeat(1, 1, 1, 14);
    // std::cout << "Reservation 2: " << (r2 ? "OK" : "FAILED") << std::endl;



    std::cout << "OK." << std::endl;
    
    return 0;
}