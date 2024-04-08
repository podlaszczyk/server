#pragma once

#include <functional>

struct Data
{
    double pressure;
    double temperature;
    double velocity;

    struct Hash
    {
        size_t operator()(const Data& data) const
        {
            size_t hash = 17;
            hash = hash * 31 + std::hash<double>()(data.pressure);
            hash = hash * 31 + std::hash<double>()(data.temperature);
            hash = hash * 31 + std::hash<double>()(data.velocity);
            return hash;
        }
    };

    bool operator==(const Data& other) const
    {
        return (pressure == other.pressure) && (temperature == other.temperature) && (velocity == other.velocity);
    }
};

struct Config
{
    int frequency = 10;
    bool debug = false;
};
