// ReservationService.cpp

#include "ReservationService.hpp"
#include "../utils/PriceFormatter.hpp"

bool ReservationService::bookSeat(int schedule_id, int passenger_id, int wagon_number, int seat_number) {
    if (!db_.isWagonAndSeatValid(schedule_id, wagon_number, seat_number)) {
        std::cerr << "Wagon " << wagon_number << " or seat " << seat_number 
                  << " does not exist for schedule " << schedule_id << "." << std::endl;
        return false;
    }
    if (!db_.isSeatAvailable(schedule_id, wagon_number, seat_number)) {
        std::cerr << "Seat " << seat_number << " in wagon " << wagon_number 
                  << " is not available for schedule " << schedule_id << "." << std::endl;
        return false;
    }
    return db_.saveReservation(schedule_id, passenger_id, wagon_number, seat_number);
}

double ReservationService::calculatePrice(const Schedule& schedule, int wagon_class) {
    TrainPriceLevel level = TrainPriceLevel::TLK;

    if(schedule.getTrain()->getType() == "TLK") {
        level = TrainPriceLevel::TLK;
    } else if(schedule.getTrain()->getType() == "IC") {
        level = TrainPriceLevel::IC;
    } else if(schedule.getTrain()->getType() == "EIP") {
        level = TrainPriceLevel::EIP;
    }
    
    int type_multiplier = static_cast<int>(level); // 70, 100 lub 150 %
    int distance = schedule.getRoute()->getDistanceKm(); // km
    double base = 0.3125; // PLN/km
    
    double price = distance * (type_multiplier / 100.0) * (wagon_class == 1 ? 2.0 : 1.0) * base; // PLN

    return price;
}

