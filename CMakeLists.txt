cmake_minimum_required(VERSION 3.15)
project(TicketReservationSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_POLICY_DEFAULT_CMP0135 NEW) # Usuwa warning z FetchContent

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Szukamy SQLite3 w systemie (na macOS zwykle jest wbudowany lub z brew)
find_package(SQLite3 REQUIRED)

# Includy (żeby nie pisać pełnych ścieżek w #include)
include_directories(src)

# Pliki źródłowe (bez main.cpp, bo on pójdzie do executable)
file(GLOB_RECURSE SOURCES 
    "src/db/*.cpp"
    "src/logic/*.cpp"
    "src/cli/*.cpp"
)

# Główny plik wykonywalny
add_executable(railway src/main.cpp ${SOURCES})

# Linkowanie bibliotek
target_link_libraries(railway PRIVATE SQLite3::SQLite3)

# --- KONFIGURACJA TESTÓW (Google Test) ---
# FetchContent pozwala pobrać GTest automatycznie podczas kompilacji
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Wyłącza instalację gtesta w systemie, budujemy go tylko lokalnie dla projektu
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Dodajemy plik z testami (na razie przykładowy)
add_executable(run_tests tests/ReservationTests.cpp ${SOURCES})
target_link_libraries(run_tests GTest::gtest_main SQLite3::SQLite3)

add_executable(run_tests_DB tests/DBManagerTest.cpp ${SOURCES})
target_link_libraries(run_tests_DB GTest::gtest_main SQLite3::SQLite3)

include(GoogleTest)
gtest_discover_tests(run_tests)
gtest_discover_tests(run_tests_DB)