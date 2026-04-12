// DBManager.cpp

#include "DBManager.hpp"
#include <iostream>

DBManager::DBManager(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "Nie można otworzyć bazy: " << sqlite3_errmsg(db_) << std::endl;
        db_ = nullptr;
    } else {
        std::cout << "Połączono z bazą: " << db_path << std::endl;
    }
}

DBManager::~DBManager() {
    if (db_) {
        sqlite3_close(db_);
        std::cout << "Zamknięto połączenie z bazą." << std::endl;
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
        std::cerr << "Błąd tworzenia schematu: " << err << std::endl;
        sqlite3_free(err);
    } else {
        std::cout << "Schema utworzona." << std::endl;
    }
}

void DBManager::seedData() {
    const char* sql = R"(
        INSERT OR IGNORE INTO stations (id, name, city) VALUES
            (1, 'Kraków Główny', 'Kraków'),
            (2, 'Katowice', 'Katowice'),
            (3, 'Wrocław Główny', 'Wrocław'),
            (4, 'Warszawa Centralna', 'Warszawa'),
            (5, 'Gdańsk Główny', 'Gdańsk');

        INSERT OR IGNORE INTO routes (id, name, distance_km) VALUES
            (1, 'Kraków - Wrocław', 280),
            (2, 'Wrocław - Kraków', 280),
            (3, 'Warszawa - Gdańsk', 340);

        INSERT OR IGNORE INTO route_stops (id, route_id, station_id, stop_number, arrival_time, departure_time) VALUES
            (1,  1, 1, 1, NULL,    '08:00'),
            (2,  1, 2, 2, '08:45', '08:50'),
            (3,  1, 3, 3, '10:30', NULL),
            (4,  2, 3, 1, NULL,    '11:00'),
            (5,  2, 2, 2, '12:40', '12:45'),
            (6,  2, 1, 3, '13:30', NULL),
            (7,  3, 4, 1, NULL,    '07:00'),
            (8,  3, 5, 2, '10:00', NULL);

        INSERT OR IGNORE INTO trains (id, name, type) VALUES
            (1, 'IC 1234', 'IC'),
            (2, 'IC 5678', 'IC'),
            (3, 'EIP 101', 'EIP');

        INSERT OR IGNORE INTO wagons (id, train_id, wagon_number, class, seat_count) VALUES
            (1, 1, 1, 2, 60),
            (2, 1, 2, 1, 30),
            (3, 2, 1, 2, 60),
            (4, 2, 2, 2, 60),
            (5, 3, 1, 1, 45),
            (6, 3, 2, 1, 45);

        INSERT OR IGNORE INTO schedules (id, route_id, train_id, departure_date, departure_time) VALUES
            (1, 1, 1, '2025-08-01', '08:00'),
            (2, 2, 2, '2025-08-01', '11:00'),
            (3, 3, 3, '2025-08-02', '07:00');
    )";

    char* err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "Błąd seedowania: " << err << std::endl;
        sqlite3_free(err);
    } else {
        std::cout << "Dane seed wgrane." << std::endl;
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
