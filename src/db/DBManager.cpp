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
    const char* sql = R"(
        INSERT OR IGNORE INTO stations (id, name, city) VALUES
            (1, 'Kraków Główny', 'Kraków'),
            (2, 'Katowice', 'Katowice'),
            (3, 'Wrocław Główny', 'Wrocław'),
            (4, 'Warszawa Centralna', 'Warszawa'),
            (5, 'Gdańsk Główny', 'Gdańsk'),
            (6, 'Opole Główne', 'Opole'),
            (7, 'Łódź Fabryczna', 'Łódź'),
            (8, 'Poznań Główny', 'Poznań'),
            (9, 'Lublin Główny', 'Lublin'),
            (10, 'Rzeszów Główny', 'Rzeszów'),
            (11, 'Szczecin Główny', 'Szczecin'),
            (12, 'Bydgoszcz Główna', 'Bydgoszcz'),
            (13, 'Częstochowa', 'Częstochowa'),
            (14, 'Radom', 'Radom'),
            (15, 'Gliwice', 'Gliwice'),
            (16, 'Białystok', 'Białystok'),
            (17, 'Gdynia Główna', 'Gdynia'),
            (18, 'Ustka', 'Ustka'),
            (19, 'Zakopane', 'Zakopane'),
            (20, 'Kołobrzeg', 'Kołobrzeg'),
            (21, 'Świnoujście', 'Świnoujście');

        -- Trasy
        INSERT OR IGNORE INTO routes (id, name, distance_km) VALUES
            (1,  'Kraków - Wrocław',           280),
            (2,  'Wrocław - Kraków',           280),
            (3,  'Warszawa - Gdańsk',          340),
            (4,  'Gdańsk - Warszawa',          340),
            (5,  'Warszawa - Kraków',          295),
            (6,  'Kraków - Warszawa',          295),
            (7,  'Warszawa - Poznań',          310),
            (8,  'Poznań - Warszawa',          310),
            (9,  'Kraków - Rzeszów',           170),
            (10, 'Rzeszów - Kraków',           170),
            (11, 'Warszawa - Białystok',       195),
            (12, 'Białystok - Warszawa',       195),
            (13, 'Gdańsk - Szczecin',          330),
            (14, 'Szczecin - Gdańsk',          330),
            (15, 'Kraków - Zakopane',           90),
            (16, 'Zakopane - Kraków',           90),
            (17, 'Warszawa - Lublin',          170),
            (18, 'Lublin - Warszawa',          170);

        -- Przystanki
        INSERT OR IGNORE INTO route_stops (id, route_id, station_id, stop_number, arrival_time, departure_time) VALUES
            -- Kraków - Wrocław (przez Katowice, Opole)
            (1,  1, 1,  1, NULL,    '08:00'),
            (2,  1, 2,  2, '08:45', '08:50'),
            (3,  1, 6,  3, '09:40', '09:45'),
            (4,  1, 3,  4, '10:30', NULL),
            -- Wrocław - Kraków (przez Opole, Katowice)
            (5,  2, 3,  1, NULL,    '11:00'),
            (6,  2, 6,  2, '11:45', '11:50'),
            (7,  2, 2,  3, '12:40', '12:45'),
            (8,  2, 1,  4, '13:30', NULL),
            -- Warszawa - Gdańsk (przez Bydgoszcz, Gdynię)
            (9,  3, 4,  1, NULL,    '07:00'),
            (10, 3, 12, 2, '09:00', '09:05'),
            (11, 3, 17, 3, '09:50', '09:55'),
            (12, 3, 5,  4, '10:10', NULL),
            -- Gdańsk - Warszawa (przez Gdynię, Bydgoszcz)
            (13, 4, 5,  1, NULL,    '12:00'),
            (14, 4, 17, 2, '12:15', '12:20'),
            (15, 4, 12, 3, '13:05', '13:10'),
            (16, 4, 4,  4, '15:10', NULL),
            -- Warszawa - Kraków (przez Radom, Częstochowę)
            (17, 5, 4,  1, NULL,    '06:30'),
            (18, 5, 14, 2, '07:30', '07:35'),
            (19, 5, 13, 3, '08:30', '08:35'),
            (20, 5, 1,  4, '09:30', NULL),
            -- Kraków - Warszawa (przez Częstochowę, Radom)
            (21, 6, 1,  1, NULL,    '10:00'),
            (22, 6, 13, 2, '11:00', '11:05'),
            (23, 6, 14, 3, '12:05', '12:10'),
            (24, 6, 4,  4, '13:10', NULL),
            -- Warszawa - Poznań (przez Łódź)
            (25, 7, 4,  1, NULL,    '08:00'),
            (26, 7, 7,  2, '09:10', '09:15'),
            (27, 7, 8,  3, '10:50', NULL),
            -- Poznań - Warszawa (przez Łódź)
            (28, 8, 8,  1, NULL,    '11:30'),
            (29, 8, 7,  2, '13:05', '13:10'),
            (30, 8, 4,  3, '14:20', NULL),
            -- Kraków - Rzeszów
            (31, 9,  1,  1, NULL,    '07:00'),
            (32, 9,  10, 2, '08:50', NULL),
            -- Rzeszów - Kraków
            (33, 10, 10, 1, NULL,    '09:30'),
            (34, 10, 1,  2, '11:20', NULL),
            -- Warszawa - Białystok
            (35, 11, 4,  1, NULL,    '09:00'),
            (36, 11, 16, 2, '11:10', NULL),
            -- Białystok - Warszawa
            (37, 12, 16, 1, NULL,    '13:00'),
            (38, 12, 4,  2, '15:10', NULL),
            -- Gdańsk - Szczecin (przez Gdynię, Kołobrzeg)
            (39, 13, 5,  1, NULL,    '08:00'),
            (40, 13, 17, 2, '08:15', '08:20'),
            (41, 13, 20, 3, '10:00', '10:05'),
            (42, 13, 11, 4, '11:30', NULL),
            -- Szczecin - Gdańsk (przez Kołobrzeg, Gdynię)
            (43, 14, 11, 1, NULL,    '12:00'),
            (44, 14, 20, 2, '13:30', '13:35'),
            (45, 14, 17, 3, '15:15', '15:20'),
            (46, 14, 5,  4, '15:35', NULL),
            -- Kraków - Zakopane
            (47, 15, 1,  1, NULL,    '07:30'),
            (48, 15, 19, 2, '09:00', NULL),
            -- Zakopane - Kraków
            (49, 16, 19, 1, NULL,    '10:00'),
            (50, 16, 1,  2, '11:30', NULL),
            -- Warszawa - Lublin
            (51, 17, 4,  1, NULL,    '08:30'),
            (52, 17, 9,  2, '10:20', NULL),
            -- Lublin - Warszawa
            (53, 18, 9,  1, NULL,    '11:00'),
            (54, 18, 4,  2, '12:50', NULL);

        -- Pociągi
        INSERT OR IGNORE INTO trains (id, name, type) VALUES
            (1,  'IC 1234',  'IC'),
            (2,  'IC 5678',  'IC'),
            (3,  'EIP 101',  'EIP'),
            (4,  'EIP 202',  'EIP'),
            (5,  'IC 3344',  'IC'),
            (6,  'IC 7890',  'IC'),
            (7,  'TLK 501',  'TLK'),
            (8,  'TLK 502',  'TLK'),
            (9,  'IC 4411',  'IC'),
            (10, 'TLK 303',  'TLK');

        -- Wagony
        INSERT OR IGNORE INTO wagons (id, train_id, wagon_number, class, seat_count) VALUES
            -- IC 1234
            (1,  1, 1, 2, 60), (2,  1, 2, 1, 30),
            -- IC 5678
            (3,  2, 1, 2, 60), (4,  2, 2, 2, 60),
            -- EIP 101
            (5,  3, 1, 1, 45), (6,  3, 2, 1, 45),
            -- EIP 202
            (7,  4, 1, 1, 45), (8,  4, 2, 1, 45), (9,  4, 3, 1, 45),
            -- IC 3344
            (10, 5, 1, 2, 60), (11, 5, 2, 2, 60), (12, 5, 3, 1, 30),
            -- IC 7890
            (13, 6, 1, 2, 60), (14, 6, 2, 1, 30),
            -- TLK 501
            (15, 7, 1, 2, 80), (16, 7, 2, 2, 80),
            -- TLK 502
            (17, 8, 1, 2, 80), (18, 8, 2, 2, 80),
            -- IC 4411
            (19, 9, 1, 2, 60), (20, 9, 2, 1, 30),
            -- TLK 303
            (21, 10, 1, 2, 80), (22, 10, 2, 2, 80);

        -- Rozkłady jazdy
        INSERT OR IGNORE INTO schedules (id, route_id, train_id, departure_date, departure_time) VALUES
            (1,  1,  1,  '2025-08-01', '08:00'),
            (2,  2,  2,  '2025-08-01', '11:00'),
            (3,  3,  3,  '2025-08-01', '07:00'),
            (4,  4,  4,  '2025-08-01', '12:00'),
            (5,  5,  5,  '2025-08-01', '06:30'),
            (6,  6,  6,  '2025-08-01', '10:00'),
            (7,  7,  7,  '2025-08-01', '08:00'),
            (8,  8,  8,  '2025-08-01', '11:30'),
            (9,  9,  9,  '2025-08-02', '07:00'),
            (10, 10, 9,  '2025-08-02', '09:30'),
            (11, 11, 10, '2025-08-02', '09:00'),
            (12, 12, 10, '2025-08-02', '13:00'),
            (13, 13, 7,  '2025-08-02', '08:00'),
            (14, 14, 8,  '2025-08-02', '12:00'),
            (15, 15, 1,  '2025-08-02', '07:30'),
            (16, 16, 1,  '2025-08-02', '10:00'),
            (17, 17, 5,  '2025-08-03', '08:30'),
            (18, 18, 6,  '2025-08-03', '11:00');
    )";

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
