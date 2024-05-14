#pragma once

#include "rtweekend.h"

#include <functional>
#include <vector>

template <typename T, typename F = double>
struct ReservoirItem
{
    T value;
    F weight;
};

template <typename T, typename F = double>
class Reservoir
{
public:
    unsigned int capacity;
    unsigned int items_sofar = 0;
    F weight_sofar = 0;
    std::vector<ReservoirItem<T, F>> data;

    Reservoir(unsigned int size) : capacity(size) {}

    // For RIS, Weight should be target_distribution_val / proposal_distribution_val
    void update(const T &value, F weight)
    {
        ++items_sofar;
        weight_sofar += weight;

        if (data.size() < capacity)
        {
            data.push_back({value, weight});
        }
        else
        {
            unsigned int idx = random_int(0, capacity - 1);
            double xi = random_double();
            if (xi < weight / weight_sofar)
            {
                data[idx] = {value, weight};
            }
        }
    }

    F ProbabilisticWeight(std::function<F(T sample)> target_distribution) const
    {
        F weight_sum = 0;
        for (const auto &item : data)
        {
            weight_sum += item.weight * target_distribution(item.value);
        }

        F result = 0;
        for (const auto &item : data)
        {
            if (item.weight > 0)
            {
                result += item.weight / target_distribution(item.value);
            }
        }

        return result / weight_sum * weight_sofar / items_sofar;
    }
};