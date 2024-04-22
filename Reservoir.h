#pragma once

#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <vector>


template <typename T, typename F = double>
struct ReservoirItem
{
    T value;
    F weight;
};

enum class ReservoirMixMethod
{
    Mixture,
    Majority,
};

template <typename T, typename F = double>
class Reservoir
{
public:
    Reservoir(int size)
    {
        static_assert(std::is_floating_point<F>::value, "F must be a floating point type");
        data.resize(size);
    }

    // Reservoir can be constructed from a list of Reservoirs which are the same type, but with different weights
    template <typename... Reservoirs>
    Reservoir(int size, std::vector<F> weights = std::vector<F>(), ReservoirMixMethod mix_method = ReservoirMixMethod::Mixture, const Reservoirs &...reservoirs)
    {
        static_assert(std::is_floating_point<F>::value, "F must be a floating point type");

        // Check if the size of the mixture reservoir is larger than the total number of elements in the input reservoirs
        int total_elements = 0;
        for (const auto &r : {reservoirs...})
        {
            total_elements += r.data.size();
        }
        if (size <= total_elements)
            throw std::runtime_error("The size of a mixture reservoir must be larger than the total number of elements in the input reservoirs");

        // Check if the number of weights is the same as the number of reservoirs
        if (weights.size() != sizeof...(reservoirs))
        {
            weights = std::vector<F>(sizeof...(reservoirs), 1.0 / sizeof...(reservoirs));
        }
        std::vector<Reservoir<T, F>> all_reservoirs = {reservoirs...};

        F sum_reservoirs_weight = 0;
        for (const auto &w : weights)
        {
            sum_reservoirs_weight += w;
        }

        switch (mix_method)
        {
        case ReservoirMixMethod::Mixture:
            // Construct the mixture reservoir strictly based on the weights of the reservoirs and the weights of the items in the reservoirs
            // Select an reservoir based on the weights
            data.resize(size);
            for (int i = 0; i < size; ++i)
            {
                F random_num1 = random_double() * sum_reservoirs_weight;
                F sum1 = 0;
                for (int j = 0; j < sizeof...(reservoirs); ++j)
                {
                    sum1 += weights[j];
                    if (random_num1 < sum1)
                    {
                        // Select an item from the reservoir based on the weights of current reservoir
                        F random_num2 = random_double() * all_reservoirs[j].total_weight;
                        F sum2 = 0;

                        for (const auto &d : all_reservoirs[j].data)
                        {
                            sum2 += d.weight;
                            if (random_num2 < sum2)
                            {
                                // Add the selected item to the target reservoir, and weight it by the weight of the reservoir multiplied by the weight of the item
                                data[i] = {d.value, weights[j] * d.weight};
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            break;
        case ReservoirMixMethod::Majority:
            // Construct the mixture reservoir by the overall majority weight of the items in the reservoirs
            std::vector<ReservoirItem<T, F>> all_items_weighted;
            for (int i = 0; i < weights.size(); ++i)
            {
                for (const auto &d : all_reservoirs[i].data)
                {
                    all_items_weighted.push_back({d.value, weights[i] * d.weight});
                }
            }
            std::sort(all_items_weighted.begin(), all_items_weighted.end(), [](const ReservoirItem<T, F> &a, const ReservoirItem<T, F> &b)
                      { return a.weight > b.weight; });

            data = all_items_weighted;
            data.resize(size);
            break;
        }

        update_total_weight();
    }

    ~Reservoir() = default;

    const ReservoirItem<T, F> &operator[](int index) const
    {
        return data[index];
    }

    ReservoirItem<T, F> &operator[](int index)
    {
        return data[index];
    }

    template <typename Tp = T, typename Fp = F>
    void emplace_back(const Tp &&value, const Fp weight)
    {
        data.emplace_back(ReservoirItem<Tp, Fp>{std::forward<Tp>(value), weight});
        total_weight += weight;
    }

    template <typename Tp = T, typename Fp = F>
    void push_back(const ReservoirItem<Tp, Fp> &&item)
    {
        data.push_back(std::forward(item));
        total_weight += item.weight;
    }

    template <typename Tp = T, typename Fp = F>
    void push_back(const Tp &&value, const Fp weight)
    {
        emplace_back(std::forward<Tp>(value), weight);
    }

    void clear()
    {
        data.clear();
        total_weight = 0;
    }

    const int size() const
    {
        return data.size();
    }

    const F Total_Weight() const
    {
        return total_weight;
    }

    // Serve a Item from the Reservoir according to the weights
    const ReservoirItem<T, F>& Serve() const
    {
        F random_num = random_double() * total_weight;
        F sum = 0;
        for (int i = 0; i < data.size(); i++)
        {
            sum += data[i].weight;
            if (sum > random_num)
            {
                return data[i];
            }
        }
    }

private:
    std::vector<ReservoirItem<T, F>> data;
    F total_weight = 0;

    void update_total_weight()
    {
        total_weight = 0;
        for (const auto &d : data)
        {
            total_weight += d.weight;
        }
    }
};