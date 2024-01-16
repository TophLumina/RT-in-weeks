#pragma once

#include "vec3.h"

template <typename T>
class FrameBuffer
{
public:
    int width;
    int height;
    T **data;

    FrameBuffer() : width(0), height(0), data(nullptr) {}

    FrameBuffer(int _width, int _height) : width(_width), height(_height)
    {
        data = new T *[height];
        for (int i = 0; i < height; ++i)
            data[i] = new T[width];
    }

    FrameBuffer(const FrameBuffer &other) : width(other.width), height(other.height)
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
        width = other.width;
        height = other.height;

        data = new T *[height];
        for (int i = 0; i < height; ++i)
        {
            data[i] = new T[width];
            std::copy(other.data[i], other.data[i] + width, data[i]);
        }

        return *this;
    }

    FrameBuffer &operator=(FrameBuffer &&other) noexcept
    {
        if (this != other)
        {
            for (int i = 0; i < height; ++i)
                delete[] data[i];
            delete[] data;

            width = other.width;
            height = other.height;
            data = other.data;

            other.width = 0;
            other.height = 0;
            other.data = nullptr;
        }
    }

    FrameBuffer(FrameBuffer &&other) noexcept { *this = std::move(other); }

    ~FrameBuffer()
    {
        if (data != nullptr)
        {
            for (int i = 0; i < height; ++i)
                delete[] data[i];
            delete[] data;
        }
    }
};