#include <gtest/gtest.h>
#include "utils/PriceFormatter.hpp"
#include "db/DBManager.hpp"
#include "logic/ReservationService.hpp"

// PRICE FORMATTER TESTS ==========================================================================================
// Standard formating test
TEST(PriceFormatterTest, ShouldFormatRegularPrice) {
    std::string result = formatPrice(25.5);
    
    EXPECT_EQ(result, "25.50 PLN");
}

// Rounding test
TEST(PriceFormatterTest, ShouldRoundExtraDecimals) {
    std::string result = formatPrice(10.1279);
    EXPECT_EQ(result, "10.13 PLN");
}

// Zero price test
TEST(PriceFormatterTest, ShouldFormatZeroPrice) {
    EXPECT_EQ(formatPrice(0.0), "0.00 PLN");
}

// Negative price test
TEST(PriceFormatterTest, ShouldFormatNegativePrice) {
    EXPECT_EQ(formatPrice(-15.5), "-15.50 PLN");
}

// DBManager integration tests =====================================================================================

// Retrieving stations from in-memory database
TEST(DatabaseIntegrationTest, ShouldRetrieveStationsFromInMemoryDB) {
    DBManager db(":memory:");

    auto stations = db.getAllStations();

    ASSERT_FALSE(stations.empty()) << "Database in RAM should be injected with starting data.";
    
    EXPECT_EQ(stations[0]->getName(), "Kraków Główny");
}

// Retrieving schedules with route and train details
TEST(DatabaseIntegrationTest, ShouldRetrieveSchedulesFromInMemoryDB) {
    DBManager db(":memory:");

    auto schedules = db.getAllSchedules();

    ASSERT_FALSE(schedules.empty()) << "Database in RAM should be injected with starting data.";
    
    EXPECT_EQ(schedules[0]->getRoute()->getName(), "Kraków - Wrocław");
}

// Retrieving wagons for a train
TEST(DatabaseIntegrationTest, ShouldRetrieveWagonsForTrain) {
    DBManager db(":memory:");

    auto wagons = db.getWagonsForTrain(1);

    ASSERT_FALSE(wagons.empty()) << "Train with ID 1 should have wagons in the database.";
    
    EXPECT_EQ(wagons[0]->getWagonNumber(), 1);
}

// Retrieving reservations for a passenger
TEST(DatabaseIntegrationTest, ShouldReturnEmptyReservationsForUnknownPassenger) {
    DBManager db(":memory:");

    auto reservations = db.getReservationsForPassenger(999);

    EXPECT_TRUE(reservations.empty()) << "Unknown passenger should have no reservations.";
}

// Booking a seat should mark it as unavailable
TEST(DatabaseIntegrationTest, ShouldMarkSeatAsUnavailableAfterBooking) {
    DBManager db(":memory:");
    ReservationService service(db);

    // Najpierw sprawdzamy, że miejsce jest dostępne
    EXPECT_TRUE(db.isSeatAvailable(1, 1, 1)) << "Seat should be available before booking.";

    // Rezerwujemy miejsce
    bool bookingResult = service.bookSeat(1, 1, 1, 1);
    EXPECT_TRUE(bookingResult) << "Booking should succeed for an available seat.";

    // Teraz miejsce powinno być niedostępne
    EXPECT_FALSE(db.isSeatAvailable(1, 1, 1)) << "Seat should be unavailable after booking.";
}


// RESERVATION SERVICE TESTS ==========================================================================================

TEST(ReservationServiceTest, ShouldNotBookSeatIfAlreadyTaken) {
    DBManager db(":memory:");
    ReservationService service(db);

    bool firstBooking = service.bookSeat(1, 1, 1, 1);
    bool secondBooking = service.bookSeat(1, 1, 1, 1);

    EXPECT_TRUE(firstBooking) << "First booking should succeed for an available seat";
    EXPECT_FALSE(secondBooking) << "Second booking for the same seat should fail";
}   

TEST(ReservationServiceTest, ShouldNotBookSeatIfNotExist) {
    DBManager db(":memory:");
    ReservationService service(db);

    EXPECT_FALSE(service.bookSeat(1, 1, 999, 1)) << "Booking should fail for non-existent wagon"; 
    EXPECT_FALSE(service.bookSeat(1, 1, 1, 999)) << "Booking should fail for non-existent seat"; 
}

// TLK, 2-class: 200km * 0.70 * 1.0 * 0.3125 = 43.75 PLN
TEST(ReservationServiceTest, ShouldCalculatePriceForTLK) {
    DBManager db(":memory:");
    ReservationService service(db);

    EXPECT_EQ(service.calculatePrice(Schedule(1, std::make_shared<Route>(1, "Test Route", 200), std::make_shared<Train>(1, "Test Train", "TLK"), "2024-01-01", "12:00"), 2), 43.75);
}

// IC, 2-class: 200km * 1.00 * 1.0 * 0.3125 = 62.50 PLN
TEST(ReservationServiceTest, ShouldCalculatePriceForIC) {
    DBManager db(":memory:");
    ReservationService service(db);

    EXPECT_EQ(service.calculatePrice(Schedule(1, std::make_shared<Route>(1, "Test Route", 200), std::make_shared<Train>(1, "Test Train", "IC"), "2024-01-01", "12:00"), 2), 62.50);
}


// EIP, 1 class: 200km * 1.50 * 2.0 * 0.3125 = 187.50 PLN
TEST(ReservationServiceTest, ShouldCalculatePriceForEIPFirstClass) {
    DBManager db(":memory:");
    ReservationService service(db);

    EXPECT_EQ(service.calculatePrice(Schedule(1, std::make_shared<Route>(1, "Test Route", 200), std::make_shared<Train>(1, "Test Train", "EIP"), "2024-01-01", "12:00"), 1), 187.50);
}