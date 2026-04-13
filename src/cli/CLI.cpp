// CLI.cpp

// CLI.cpp
#include "CLI.hpp"
#include <iostream>
#include <limits>

void CLI::run() {
    int choice = 0;
    while (true) {
        std::cout << "\n=== Railway Reservation System ===" << std::endl;
        std::cout << "1. List schedules" << std::endl;
        std::cout << "2. Make a reservation" << std::endl;
        std::cout << "3. My reservations" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: listSchedules(); break;
            case 2: makeReservation(); break;
            case 3: listReservations(); break;
            case 0: return;
            default: std::cout << "Invalid choice." << std::endl;
        }
    }
}

void CLI::listSchedules() {
    auto schedules = db_.getAllSchedules();
    std::cout << "\n--- Available Schedules ---" << std::endl;
    for (const auto& s : schedules) {
        std::cout << "[" << s->getId() << "] "
                  << s->getDepartureDate() << " " << s->getDepartureTime()
                  << " | " << s->getRoute()->getName()
                  << " | " << s->getTrain()->getName()
                  << std::endl;
    }
}

void CLI::makeReservation() {
    listSchedules();

    int schedule_id, passenger_id, wagon_number, seat_number;

    std::cout << "\nSchedule ID: ";
    std::cin >> schedule_id;
    std::cout << "Passenger ID: ";
    std::cin >> passenger_id;
    std::cout << "Wagon number: ";
    std::cin >> wagon_number;
    std::cout << "Seat number: ";
    std::cin >> seat_number;

    bool ok = service_.bookSeat(schedule_id, passenger_id, wagon_number, seat_number);
    std::cout << (ok ? "Reservation successful!" : "Reservation failed.") << std::endl;
}

void CLI::listReservations() {
    int passenger_id;
    std::cout << "Passenger ID: ";
    std::cin >> passenger_id;

    auto reservations = db_.getReservationsForPassenger(passenger_id);
    if (reservations.empty()) {
        std::cout << "No reservations found." << std::endl;
        return;
    }
    std::cout << "\n--- Your Reservations ---" << std::endl;
    for (const auto& r : reservations) {
        std::cout << "[" << r->getId() << "] "
                  << "Wagon: " << r->getWagonNumber()
                  << " Seat: " << r->getSeatNumber()
                  << " Status: " << r->getStatus()
                  << std::endl;
    }
}