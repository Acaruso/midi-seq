#pragma once

#include <random>
#include <limits>

class RngService {
public:
    std::mt19937 mt_engine;

    RngService()
        : mt_engine(std::random_device{}())
    {}

    // get random int between `low` inclusive and `high` inclusive
    int getRand(int low, int high) {
        std::uniform_int_distribution<int> dist(low, high);
        return dist(mt_engine);
    }
};
