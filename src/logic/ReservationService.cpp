// ReservationService.cpp

#include "ReservationService.hpp"
#include "../utils/PriceFormatter.hpp"

bool ReservationService::bookSeat(int schedule_id, int passenger_id, int wagon_number, int seat_number) {
    if (!db_.isWagonAndSeatValid(schedule_id, wagon_number, seat_number)) {
        std::cerr << "Wagon or seat does not exist." << std::endl;
        return false;
    }
    if (!db_.isSeatAvailable(schedule_id, wagon_number, seat_number)) {
        std::cerr << "Seat already taken." << std::endl;
        return false;
    }

    auto schedules = db_.getAllSchedules();
    auto it = std::find_if(schedules.begin(), schedules.end(),
        [schedule_id](const auto& s) { return s->getId() == schedule_id; });

    if (it == schedules.end()) {
        std::cerr << "Schedule not found." << std::endl;
        return false;
    }

    auto wagons = db_.getWagonsForTrain((*it)->getTrain()->getId());
    auto wit = std::find_if(wagons.begin(), wagons.end(),
        [wagon_number](const auto& w) { return w->getWagonNumber() == wagon_number; });

    int wagon_class = (wit != wagons.end()) ? (*wit)->getClass() : 2;
    double price = calculatePrice(**it, wagon_class);

    return db_.saveReservation(schedule_id, passenger_id, wagon_number, seat_number, price);
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

