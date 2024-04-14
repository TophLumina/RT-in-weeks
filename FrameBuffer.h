#pragma once

#include "vec3.h"
#include <functional>

template <typename T>
class FrameBuffer
{
public:
    unsigned int width;
    unsigned int height;
    T **data;
    std::function<double(const T&, const T&)> diff;  // only used for the denoiser

    FrameBuffer() : width(0), height(0), data(nullptr) {}

    FrameBuffer(unsigned int width, unsigned int height, T value = T(), std::function<double(const T&, const T&)> diff = [](const T& a, const T& b)-> double {return 0;}) : width(width), height(height), diff(diff)
    {
        data = new T *[height];
        for (int i = 0; i < height; ++i)
        {
            data[i] = new T[width];
            std::fill(data[i], data[i] + width, value);
        }
    }

    FrameBuffer(const FrameBuffer &other) : width(other.width), height(other.height), diff(other.diff)
    {
        data = new T *[height];
        for (int i = 0; i < height; ++i)
        {
            data[i] = new T[width];
            std::copy(other.data[i], other.data[i] + width, data[i]);
        }
    }

    FrameBuffer &operator=(const FrameBuffer &other)
    {
        if (this != &other)
        {
            for (int i = 0; i < height; ++i)
                delete[] data[i];
            delete[] data;

            width = other.width;
            height = other.height;
            diff = other.diff;

            data = new T *[height];
            for (int i = 0; i < height; ++i)
            {
                data[i] = new T[width];
                std::copy(other.data[i], other.data[i] + width, data[i]);
            }
        }

        return *this;
    }

    FrameBuffer &operator=(FrameBuffer &&other) noexcept
    {
        if (this != &other)
        {
            for (int i = 0; i < height; ++i)
                delete[] data[i];
            delete[] data;

            width = other.width;
            height = other.height;
            diff = other.diff;

            data = other.data;

            other.width = 0;
            other.height = 0;
            other.data = nullptr;
        }

        return *this;
    }

    FrameBuffer(FrameBuffer &&other) noexcept : width(other.width), height(other.height), data(other.data), diff(other.diff)
    {
        other.width = 0;
        other.height = 0;
        other.data = nullptr;
    }

    ~FrameBuffer()
    {
        if (data)
        {
            for (int i = 0; i < height; ++i)
                delete[] data[i];
            delete[] data;
        }
    }
};