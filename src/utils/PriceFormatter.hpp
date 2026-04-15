#pragma once
#include <sstream>
#include <iomanip>

inline std::string formatPrice(double price) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << price;
    ss << " PLN";
    return ss.str();
}