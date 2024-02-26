#pragma once

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

#include <stdlib.h>
#include <iostream>

#include <functional>
#include "FrameBuffer.h"

class rtw_image
{
public:
    rtw_image() : data(nullptr) {}

    rtw_image(const char *image_filename)
    {
        auto filename = std::string(image_filename);

        // Searching image file
        if (load(filename))
            return;
        if (load("img/" + filename))
            return;

        std::cerr << "ERROR:: " << image_filename << " FILE NOT FOUND." << std::endl;
    }

    template <typename T>
    rtw_image(FrameBuffer<T> fb, int _bytes_per_pixel, std::function<void(T, unsigned char*)> trans)
        : bytes_per_pixel(_bytes_per_pixel), image_height(fb.height), image_width(fb.width), bytes_per_line(image_width * bytes_per_pixel)
    {
        if (load(fb, trans))
            return;

        std::cerr << "ERROR:: FRAMEBUFFER TO IMG FAILED." << std::endl;
    }

    ~rtw_image()
    {
        STBI_FREE(data);
    }

    void saveasPNG(std::string filename) const
    {
        auto tmp = stbi_write_png(filename.c_str(), image_width, image_height, bytes_per_pixel, data, bytes_per_line);
        // error log at image output
        if (tmp == 0)
            std::cerr << "ERROR:: IMAGE TO PNG FAILED." << std::endl;
    }

    // TODO:: support PPM image in camera class.
    void saveasPPM(std::string filename) const
    {
        
    }

    int width() const { return data == nullptr ? 0 : image_width; }
    int height() const { return data == nullptr ? 0 : image_height; }

    const unsigned char *pixel_data(int x, int y) const
    // Return address of three bytes of pixel(x, y) (or magenta if no data)
    {
        static unsigned char magenta[] = {255, 0, 255};
        if (data == nullptr)
            return magenta;

        x = clamp(x, 0, image_width);
        y = clamp(y, 0, image_height);
        return data + y * bytes_per_line + x * bytes_per_pixel;
    }

private:
    int bytes_per_pixel = 3;
    unsigned char *data;
    int image_width;
    int image_height;
    int bytes_per_line;

    bool load(const std::string filename)
    {
        // load image from file, return true if succeed
        auto n = bytes_per_pixel;
        data = stbi_load(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
        bytes_per_line = image_width * bytes_per_pixel;
        return data != nullptr;
    }

    template <typename T>
    bool load(const FrameBuffer<T> fb, const std::function<void(T, unsigned char*)> trans)  // Any data from framebuffer to img, should have provide a function to convert that specific data to unsigned char* first
    {
        // load image from framebuffer, return true if succeed
        unsigned char *pixel_data = new unsigned char[bytes_per_pixel];

        data = new unsigned char[image_width * image_height * bytes_per_pixel];
        int data_index = 0;

        for (int i = 0; i < image_height; ++i)
        {
            for (int j = 0; j < image_width; ++j)
            {
                trans(fb.data[i][j], pixel_data);
                for (int k = 0; k < bytes_per_pixel; ++k)
                    data[data_index++] = pixel_data[k];
            }
        }

        delete[] pixel_data;
        return data != nullptr;
    }

    static int clamp(int x, int low, int high)
    // Clamp to [low, high)
    {
        if (x < low)
            return low;
        if (x >= high)
            return high - 1;
        return x;
    }
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif