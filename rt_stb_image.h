#pragma once

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
    #pragma warning(push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#include <stdlib.h>
#include <iostream>

class rtw_image
{
public:
    rtw_image() : data(nullptr) {}

    rtw_image(const char* image_filename)
    {
        auto filename = std::string(image_filename);
    
        // Searching image file
        if (load(filename))
            return;
        if (load("img/" + filename))
            return;

        std::cerr << "ERROR:: " << image_filename << " FILE NOT FOUND." << std::endl;
    }

    ~rtw_image()
    {
        STBI_FREE(data);
    }

    bool load(const std::string filename)
    {
        // load image from file, return true if succeed
        auto n = bytes_per_pixel;
        data = stbi_load(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
        bytes_per_line = image_width * bytes_per_pixel;
        return data != nullptr;
    }

    int width() const { return data == nullptr ? 0 : image_width; }
    int height() const { return data == nullptr ? 0 : image_height; }

    const unsigned char* pixel_data(int x, int y) const
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
    const int bytes_per_pixel = 3;
    unsigned char* data;
    int image_width;
    int image_height;
    int bytes_per_line;

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