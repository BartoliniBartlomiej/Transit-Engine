# 🚂 Railway Reservation System — Dokumentacja projektu

> Projekt portfolio w C++/SQLite. Nacisk na smart pointery, zarządzanie pamięcią i relacje między obiektami.

---

## Cel projektu

Zbudować działające CLI do rezerwacji biletów kolejowych, które:
- modeluje prawdziwe relacje między obiektami (ownership, sharing, obserwacja)
- pokazuje świadome użycie `unique_ptr`, `shared_ptr`, `weak_ptr`
- persystuje dane w SQLite
- ma czytelną architekturę nadającą się do pokazania na rozmowie kwalifikacyjnej

---

## Struktura katalogów (do wypełnienia samodzielnie)

```
railway/
├── src/
│   ├── models/          # Station, Train, Wagon, Route, Passenger, Reservation
│   ├── db/              # DBManager — warstwa SQLite
│   ├── logic/           # Router (Dijkstra), ReservationService
│   └── cli/             # Interfejs użytkownika w terminalu
├── include/
├── tests/
├── db/
│   └── railway.db
└── CMakeLists.txt
```

---

## Model domenowy — klasy

### `Station`
Reprezentuje stację kolejową.

```
id, name, city
połączona z innymi stacjami przez Route
```

**Pytanie do przemyślenia:** Kto powinien być właścicielem listy tras wychodzących ze stacji?

---

### `Train`
Pociąg kursujący na trasie.

```
id, name, type (IC / TLK / EIP)
posiada wagony (Wagon[])
przypisany do Route
```

**Kluczowa decyzja:** `Train` jest **jedynym właścicielem** swoich wagonów.
→ Naturalne miejsce na `unique_ptr<Wagon>`.

---

### `Wagon`
Wagon należący do konkretnego pociągu.

```
id, wagon_number, class (1/2), seat_count
należy do dokładnie jednego Train
```

**Pytanie:** Czy `Wagon` potrzebuje wskaźnika z powrotem do `Train`? Jakie są konsekwencje?

---

### `Route`
Trasa przejazdu (np. Warszawa → Kraków).

```
id, lista stacji pośrednich, distance_km, duration_min
może być współdzielona przez wiele pociągów
```

→ Naturalne miejsce na `shared_ptr<Route>`.

---

### `Passenger`
Pasażer kupujący bilet.

```
id, name, email
ma historię rezerwacji
```

**Pytanie:** Czy `Passenger` powinien trzymać `shared_ptr<Reservation>` czy `weak_ptr<Reservation>`?
Zastanów się: kto jest właścicielem rezerwacji — pasażer czy system?

---

### `Reservation`
Pojedyncza rezerwacja biletu.

```
id, passenger_id, train_id, wagon_id, seat_number, date, status
```

**Pułapka cykli referencji:**
```
Reservation → shared_ptr<Passenger>
Passenger   → shared_ptr<Reservation>   ← cykl! wyciek pamięci
```
Rozwiązanie: jedno z połączeń musi być `weak_ptr`.

---

## Relacje i ownership — mapa

```
                    ┌─────────────────────────────────┐
                    │           RouteGraph             │
                    │   shared_ptr<Station>[]          │
                    └────────────────┬────────────────┘
                                     │
                              shared_ptr<Route>
                                     │
                    ┌────────────────▼────────────────┐
                    │              Train               │
                    │   unique_ptr<Wagon>[]            │
                    └────────────────┬────────────────┘
                                     │
                              shared_ptr<Reservation>
                                     │
                    ┌────────────────▼────────────────┐
                    │           Reservation            │
                    │   weak_ptr<Passenger>  ◄─────────┼── shared_ptr<Passenger>
                    └─────────────────────────────────┘
```

---

## Smart pointery — podsumowanie użycia

| Pointer | Gdzie | Dlaczego |
|---|---|---|
| `unique_ptr<Wagon>` | `Train` posiada wagony | Jeden właściciel, brak potrzeby współdzielenia |
| `shared_ptr<Route>` | Wiele pociągów na tej samej trasie | Kilka obiektów potrzebuje dostępu |
| `shared_ptr<Station>` | Graf stacji w `RouteGraph` | Stacje współdzielone między trasami |
| `weak_ptr<Passenger>` | `Reservation` → `Passenger` | Przerywa cykl referencji |
| raw pointer (opcjonalnie) | Iteracja po grafie | Do nauki różnicy — nigdy do ownership |

---

## Schemat bazy danych SQLite

```sql
CREATE TABLE stations (
    id      INTEGER PRIMARY KEY,
    name    TEXT NOT NULL,
    city    TEXT NOT NULL
);

CREATE TABLE routes (
    id           INTEGER PRIMARY KEY,
    name         TEXT NOT NULL,
    distance_km  INTEGER,
    duration_min INTEGER
);

CREATE TABLE route_stations (
    route_id   INTEGER REFERENCES routes(id),
    station_id INTEGER REFERENCES stations(id),
    stop_order INTEGER NOT NULL,
    PRIMARY KEY (route_id, station_id)
);

CREATE TABLE trains (
    id       INTEGER PRIMARY KEY,
    name     TEXT NOT NULL,
    type     TEXT CHECK(type IN ('IC','TLK','EIP')),
    route_id INTEGER REFERENCES routes(id)
);

CREATE TABLE wagons (
    id           INTEGER PRIMARY KEY,
    train_id     INTEGER REFERENCES trains(id),
    wagon_number INTEGER NOT NULL,
    class        INTEGER CHECK(class IN (1,2)),
    seat_count   INTEGER NOT NULL
);

CREATE TABLE passengers (
    id    INTEGER PRIMARY KEY,
    name  TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL
);

CREATE TABLE reservations (
    id           INTEGER PRIMARY KEY,
    passenger_id INTEGER REFERENCES passengers(id),
    train_id     INTEGER REFERENCES trains(id),
    wagon_id     INTEGER REFERENCES wagons(id),
    seat_number  INTEGER NOT NULL,
    travel_date  TEXT NOT NULL,
    status       TEXT CHECK(status IN ('active','cancelled'))
);
```

---

## Warstwa DBManager

Klasa odpowiedzialna za całą komunikację z SQLite.

```cpp
// Szkic interfejsu — do zaimplementowania samodzielnie
class DBManager {
public:
    explicit DBManager(const std::string& db_path);
    ~DBManager();

    // Przykładowe metody do przemyślenia:
    std::vector<Station>     getAllStations();
    std::optional<Train>     getTrainById(int id);
    bool                     saveReservation(const Reservation& r);
    std::vector<Reservation> getReservationsForPassenger(int passenger_id);

private:
    sqlite3* db_;   // raw pointer — SQLite API jest w C
                    // Pytanie: czy można to opakować w unique_ptr z custom deleter?
};
```

**Do przemyślenia:** Jak obsłużyć błędy SQLite? Wyjątki czy kody błędów?

---

## Logika — Router (Dijkstra)

Graf stacji trzymany w pamięci jako:
```cpp
std::unordered_map<int, std::shared_ptr<Station>> stations_;
std::unordered_map<int, std::vector<std::weak_ptr<Route>>> adjacency_;
//                                   ^
//                          Pytanie: dlaczego weak_ptr tutaj ma sens?
```

Algorytm Dijkstry zwraca listę stacji pośrednich i łączny czas przejazdu.

---

## CLI — przykładowe komendy

```
> list stations
> search Warsaw Krakow 2025-08-01
> book <train_id> <wagon_id> <seat> <passenger_id>
> cancel <reservation_id>
> history <passenger_id>
```

---

## Etapy implementacji (sugerowana kolejność)

- [ ] **Etap 1** — Klasy modeli bez SQL, proste testy w `main.cpp`
- [ ] **Etap 2** — DBManager + schemat SQLite, seedowanie danych testowych
- [ ] **Etap 3** — RouteGraph + Dijkstra (tylko w pamięci)
- [ ] **Etap 4** — ReservationService — logika rezerwacji z obsługą konfliktów miejsc
- [ ] **Etap 5** — CLI — pętla główna, parsowanie komend
- [ ] **Etap 6** — Testy jednostkowe (opcjonalnie: Google Test)

---

## Rzeczy warte pokazania na rozmowie

- Świadomy wybór `unique_ptr` vs `shared_ptr` vs `weak_ptr` — umieć uzasadnić każdy
- Unikanie cykli referencji przez `weak_ptr`
- RAII w `DBManager` — zasoby zwalniane w destruktorze
- Custom deleter dla `sqlite3*` w smart pointerze
- Separacja warstw: modele / DB / logika / CLI

---

## Przydatne zasoby

- [cppreference — smart pointers](https://en.cppreference.com/w/cpp/memory)
- [SQLite C API](https://www.sqlite.org/capi3ref.html)
- Scott Meyers, *Effective Modern C++* — rozdziały 18-22 (unique/shared/weak ptr)
