// ReservationService.cpp

#include "ReservationService.hpp"

bool ReservationService::bookSeat(int schedule_id, int passenger_id, int wagon_number, int seat_number) {
    if (!db_.isSeatAvailable(schedule_id, wagon_number, seat_number)) {
        std::cerr << "Seat " << seat_number << " in wagon " << wagon_number 
                  << " is not available for schedule " << schedule_id << "." << std::endl;
        return false;
    }
    return db_.saveReservation(schedule_id, passenger_id, wagon_number, seat_number);
}

