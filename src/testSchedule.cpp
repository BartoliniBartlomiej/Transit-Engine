#include <iostream>
#include <string>
#include <sqlite3.h>
#include <iomanip>

class RozkladJazdy {
private:
    sqlite3* db;

public:
    // Konstruktor: Otwiera połączenie z bazą danych
    RozkladJazdy(const std::string& db_path) {
        int rc = sqlite3_open(db_path.c_str(), &db);
        if (rc) {
            std::cerr << "Nie można otworzyć bazy danych: " << sqlite3_errmsg(db) << std::endl;
            db = nullptr;
        } else {
            std::cout << "Połączono z bazą danych." << std::endl;
        }
    }

    // Destruktor: Zamyka połączenie z bazą
    ~RozkladJazdy() {
        if (db) {
            sqlite3_close(db);
        }
    }

    // Metoda wyświetlająca rozkład dla konkretnego ID trasy (route_id)
    void wyswietlTrase(int route_id) {
        if (!db) {
            std::cerr << "Brak połączenia z bazą." << std::endl;
            return;
        }

        // Najpierw pobierzmy nazwę trasy, żeby ładnie ją wyświetlić
        std::string route_name = pobierzNazweTrasy(route_id);
        if (route_name.empty()) {
            std::cerr << "Nie znaleziono trasy o ID: " << route_id << std::endl;
            return;
        }

        std::cout << "\n============================================\n";
        std::cout << " ROZKŁAD JAZDY DLA TRASY: " << route_name << " (ID: " << route_id << ")\n";
        std::cout << "============================================\n";
        std::cout << std::left 
                  << std::setw(5)  << "Nr" 
                  << std::setw(25) << "Stacja" 
                  << std::setw(15) << "Przyjazd" 
                  << std::setw(15) << "Odjazd" << "\n";
        std::cout << "--------------------------------------------\n";

        // Zapytanie SQL łączące route_stops z stations
        const char* sql = R"(
            SELECT rs.stop_number, s.name, rs.arrival_time, rs.departure_time
            FROM route_stops rs
            JOIN stations s ON rs.station_id = s.id
            WHERE rs.route_id = ?
            ORDER BY rs.stop_number ASC;
        )";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Błąd podczas przygotowywania zapytania: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        // Bindowanie ID trasy do zapytania (znak '?')
        sqlite3_bind_int(stmt, 1, route_id);

        // Wykonywanie zapytania wiersz po wierszu
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            int stop_number = sqlite3_column_int(stmt, 0);
            
            // Bezpieczne pobieranie tekstów (obsługa NULL)
            const unsigned char* text_name = sqlite3_column_text(stmt, 1);
            const unsigned char* text_arr = sqlite3_column_text(stmt, 2);
            const unsigned char* text_dep = sqlite3_column_text(stmt, 3);

            std::string stacja   = text_name ? reinterpret_cast<const char*>(text_name) : "Nieznana";
            std::string przyjazd = text_arr  ? reinterpret_cast<const char*>(text_arr)  : "---";
            std::string odjazd   = text_dep  ? reinterpret_cast<const char*>(text_dep)  : "---";

            std::cout << std::left 
                      << std::setw(5)  << stop_number 
                      << std::setw(25) << stacja 
                      << std::setw(15) << przyjazd 
                      << std::setw(15) << odjazd << "\n";
        }

        if (rc != SQLITE_DONE) {
            std::cerr << "Błąd podczas odczytu danych: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt);
        std::cout << "============================================\n\n";
    }

private:
    // Metoda pomocnicza do wyciągania nazwy trasy na podstawie ID
    std::string pobierzNazweTrasy(int route_id) {
        std::string name = "";
        const char* sql = "SELECT name FROM routes WHERE id = ?";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, route_id);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                const unsigned char* text = sqlite3_column_text(stmt, 0);
                if (text) name = reinterpret_cast<const char*>(text);
            }
            sqlite3_finalize(stmt);
        }
        return name;
    }
};

int main() {
    // Zakładam, że po wykonaniu skryptu seed.sql, Twoja baza nazywa się np. "trains.db"
    std::string sciezka_do_bazy = "db/train.db";
    
    RozkladJazdy rozklad(sciezka_do_bazy);

    // Przetestujmy dla kilku przykładowych tras z Twojego pliku seed.sql
    // 1: Kraków - Wrocław (Katowice, Opole)
    rozklad.wyswietlTrase(1);

    // 19: Warszawa - Wrocław (Łódź, Częstochowa, Opole)
    rozklad.wyswietlTrase(19);

    // 25: Kraków - Gdańsk 
    rozklad.wyswietlTrase(25);

    return 0;
}