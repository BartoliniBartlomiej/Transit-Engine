// DBManager.cpp

#include "DBManager.hpp"


DBManager::DBManager(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "Database not opened: " << sqlite3_errmsg(db_) << std::endl;
        db_ = nullptr;
    } else {
        std::cout << "Connected to database: " << db_path << std::endl;
    }
}

DBManager::~DBManager() {
    if (db_) {
        sqlite3_close(db_);
        std::cout << "Connection to database closed." << std::endl;
    }
}

void DBManager::initSchema() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS stations (
            id      INTEGER PRIMARY KEY,
            name    TEXT NOT NULL,
            city    TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS routes (
            id           INTEGER PRIMARY KEY,
            name         TEXT NOT NULL,
            distance_km  INTEGER
        );
        CREATE TABLE IF NOT EXISTS route_stops (
            id               INTEGER PRIMARY KEY,
            route_id         INTEGER REFERENCES routes(id),
            station_id       INTEGER REFERENCES stations(id),
            stop_number      INTEGER NOT NULL,
            arrival_time     TEXT,
            departure_time   TEXT
        );
        CREATE TABLE IF NOT EXISTS trains (
            id    INTEGER PRIMARY KEY,
            name  TEXT NOT NULL,
            type  TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS wagons (
            id           INTEGER PRIMARY KEY,
            train_id     INTEGER REFERENCES trains(id),
            wagon_number INTEGER NOT NULL,
            class        INTEGER NOT NULL,
            seat_count   INTEGER NOT NULL
        );
        CREATE TABLE IF NOT EXISTS passengers (
            id           INTEGER PRIMARY KEY,
            name         TEXT NOT NULL,
            surname      TEXT NOT NULL,
            email        TEXT UNIQUE NOT NULL,
            phone_number TEXT
        );
        CREATE TABLE IF NOT EXISTS schedules (
            id               INTEGER PRIMARY KEY,
            route_id         INTEGER REFERENCES routes(id),
            train_id         INTEGER REFERENCES trains(id),
            departure_date   TEXT NOT NULL,
            departure_time   TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS reservations (
            id           INTEGER PRIMARY KEY,
            schedule_id  INTEGER REFERENCES schedules(id),
            passenger_id INTEGER REFERENCES passengers(id),
            wagon_number INTEGER NOT NULL,
            seat_number  INTEGER NOT NULL,
            status       TEXT NOT NULL
        );
    )";

    char* err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "Error creating schema: " << err << std::endl;
        sqlite3_free(err);
    } else {
        std::cout << "Schema created." << std::endl;
    }
}

void DBManager::seedData() {
    std::string _sql = loadSQL("src/db/seed.sql");
    const char* sql = _sql.c_str();

    char* err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "Error seeding data: " << err << std::endl;
        sqlite3_free(err);
    } else {
        std::cout << "Seed data inserted." << std::endl;
    }
}

std::vector<std::shared_ptr<Station>> DBManager::getAllStations() {
    std::vector<std::shared_ptr<Station>> result;

    const char* sql = "SELECT id, name, city FROM stations;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Eroor: " << sqlite3_errmsg(db_) << std::endl;
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id          = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string city = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        result.push_back(std::make_shared<Station>(name, city, id));
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::shared_ptr<Train>> DBManager::getAllTrains() {
    std::vector<std::shared_ptr<Train>> result;

    const char* sql = "SELECT id, name, type FROM trains;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error:" << sqlite3_errmsg(db_) << std::endl;
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id          = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        result.push_back(std::make_shared<Train>(id, name, type));
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::shared_ptr<Schedule>> DBManager::getAllSchedules() {
    std::vector<std::shared_ptr<Schedule>> result;

    const char* sql = R"(SELECT s.id, s.departure_date, s.departure_time, 
        r.id, r.name, r.distance_km,
        t.id, t.name, t.type
        FROM schedules s
        JOIN routes r ON s.route_id = r.id
        JOIN trains t ON s.train_id = t.id;
    )";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error:" << sqlite3_errmsg(db_) << std::endl;
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id          = sqlite3_column_int(stmt, 0);
        // std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string departure_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string departure_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int route_id = sqlite3_column_int(stmt, 3);
        std::string route_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        int route_distance = sqlite3_column_int(stmt, 5);
        int train_id = sqlite3_column_int(stmt, 6);
        std::string train_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        std::string train_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));

        auto route = std::make_shared<Route>(route_id, route_name, route_distance);
        auto train = std::make_shared<Train>(train_id, train_name, train_type);
        result.push_back(std::make_shared<Schedule>(id, route, train, departure_date, departure_time));
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::unique_ptr<Wagon>> DBManager::getWagonsForTrain(int train_id) {
    std::vector<std::unique_ptr<Wagon>> result;

    const char* sql = "SELECT id, wagon_number, class, seat_count FROM wagons WHERE train_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error:" << sqlite3_errmsg(db_) << std::endl;
        return result;
    }

    sqlite3_bind_int(stmt, 1, train_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id          = sqlite3_column_int(stmt, 0);
        int wagon_number = sqlite3_column_int(stmt, 1);
        int class_       = sqlite3_column_int(stmt, 2);
        int seat_count   = sqlite3_column_int(stmt, 3);

        result.push_back(std::make_unique<Wagon>(id, wagon_number, class_, seat_count));
    }

    sqlite3_finalize(stmt);
    return result;
}

// DBManager.cpp
std::vector<std::shared_ptr<Reservation>> DBManager::getReservationsForPassenger(int passenger_id) {
    std::vector<std::shared_ptr<Reservation>> result;

    const char* sql = R"(
        SELECT id, schedule_id, wagon_number, seat_number, status
        FROM reservations
        WHERE passenger_id = ?;
    )";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error: " << sqlite3_errmsg(db_) << std::endl;
        return result;
    }

    sqlite3_bind_int(stmt, 1, passenger_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id           = sqlite3_column_int(stmt, 0);
        int schedule_id  = sqlite3_column_int(stmt, 1);
        int wagon_number = sqlite3_column_int(stmt, 2);
        int seat_number  = sqlite3_column_int(stmt, 3);
        std::string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        // Reservation needs weak_ptr<Schedule> and weak_ptr<Passenger>
        // at this layer we don't have them, so we store raw data only
        result.push_back(std::make_shared<Reservation>(id, seat_number, wagon_number, status,
                         std::weak_ptr<Schedule>(), std::weak_ptr<Passenger>()));
    }

    sqlite3_finalize(stmt);
    return result;
}

// Reservation operations
bool DBManager::isSeatAvailable(int schedule_id, int wagon_number, int seat_number) {
    const char* sql = R"(
        SELECT COUNT(*) FROM reservations 
        WHERE schedule_id = ? 
        AND wagon_number = ? 
        AND seat_number = ?
        AND status = 'active';
    )";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error:" << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, schedule_id);
    sqlite3_bind_int(stmt, 2, wagon_number);
    sqlite3_bind_int(stmt, 3, seat_number);

    bool available = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        available = (count == 0);
    }

    sqlite3_finalize(stmt);
    return available;
}

bool DBManager::saveReservation(int schedule_id, int passenger_id, int wagon_number, int seat_number) {
    const char* sql = R"(
        INSERT INTO reservations (schedule_id, passenger_id, wagon_number, seat_number, status)
        VALUES (?, ?, ?, ?, 'active');
    )";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error:" << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, schedule_id);
    sqlite3_bind_int(stmt, 2, passenger_id);
    sqlite3_bind_int(stmt, 3, wagon_number);
    sqlite3_bind_int(stmt, 4, seat_number);

    bool success = false;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        success = true;
    } else {
        std::cerr << "Error saving reservation: " << sqlite3_errmsg(db_) << std::endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

std::vector<std::shared_ptr<Schedule>> DBManager::getAllSchedulesFromStationToStation(std::string from, std::string to) {
    std::vector<std::shared_ptr<Schedule>> result;

    const char* sql = R"(
        SELECT s.id, s.departure_date, s.departure_time, 
               r.id, r.name, r.distance_km,
               t.id, t.name, t.type
        FROM schedules s
        JOIN routes r ON s.route_id = r.id
        JOIN trains t ON s.train_id = t.id
        JOIN route_stops rs_from ON r.id = rs_from.route_id
        JOIN stations st_from ON rs_from.station_id = st_from.id
        JOIN route_stops rs_to ON r.id = rs_to.route_id
        JOIN stations st_to ON rs_to.station_id = st_to.id
        WHERE st_from.name = ? AND st_to.name = ?
          AND rs_from.stop_number < rs_to.stop_number;
    )";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error:" << sqlite3_errmsg(db_) << std::endl;
        return result;
    }

    sqlite3_bind_text(stmt, 1, from.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, to.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id          = sqlite3_column_int(stmt, 0);
        std::string departure_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string departure_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int route_id = sqlite3_column_int(stmt, 3);
        std::string route_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        int route_distance = sqlite3_column_int(stmt, 5);
        int train_id = sqlite3_column_int(stmt, 6);
        std::string train_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        std::string train_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));

        auto route = std::make_shared<Route>(route_id, route_name, route_distance);
        auto train = std::make_shared<Train>(train_id, train_name, train_type);
        result.push_back(std::make_shared<Schedule>(id, route, train, departure_date, departure_time));
    } 
    sqlite3_finalize(stmt);
    return result;
}

std::string DBManager::getTrainType(int train_id) {
    const char* sql = "SELECT type FROM trains WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error:" << sqlite3_errmsg(db_) << std::endl;
        return "";
    }

    sqlite3_bind_int(stmt, 1, train_id);

    std::string type = "";
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return type;
}
