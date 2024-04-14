#pragma once

#include <functional>
#include <utility>
#include <vector>

template <typename T>
class FrameBuffer
{
public:
    unsigned int width;
    unsigned int height;
    std::vector<std::vector<T>> data;
    std::function<double(const T&, const T&)> diff;  // only used for the denoiser

    FrameBuffer() : width(0), height(0), data(), diff([](const T& a, const T& b)-> double {return 0;}) {}

    FrameBuffer(unsigned int width, unsigned int height, T value = T(), std::function<double(const T&, const T&)> diff = [](const T& a, const T& b)-> double {return 0;}) : width(width), height(height), diff(diff)
    {
        data.resize(height, std::vector<T>(width, value));
    }

    FrameBuffer(const FrameBuffer &other) : width(other.width), height(other.height), diff(other.diff)
    {
        data.resize(height, std::vector<T>(width));
        for (int i = 0; i < height; ++i)
            std::copy(other.data[i].begin(), other.data[i].end(), data[i].begin());
    }

    FrameBuffer &operator=(const FrameBuffer &other)
    {
        if (this != &other)
        {
            width = other.width;
            height = other.height;
            diff = other.diff;

            data.resize(height, std::vector<T>(width));

            for (int i = 0; i < height; ++i)
                std::copy(other.data[i].begin(), other.data[i].end(), data[i].begin());
        }

        return *this;
    }

    FrameBuffer &operator=(FrameBuffer &&other) noexcept
    {
        if (this != &other)
        {
            width = other.width;
            height = other.height;

            diff = std::move(other.diff);
            data = std::move(other.data);

            other.width = 0;
            other.height = 0;
            other.data.clear();
        }

        return *this;
    }

    FrameBuffer(FrameBuffer &&other) noexcept : width(other.width), height(other.height), data(std::move(other.data)), diff(std::move(other.diff))
    {
        other.width = 0;
        other.height = 0;
        other.data.clear();
    }

    ~FrameBuffer() = default;
};