#include <iostream>
#include <fstream>

#include "vec3.h"
#include "color.h"

using namespace std;

int main(int argc, char const *argv[])
{

    // Image
    ofstream o("./image.ppm", ios::out);
    const int img_width = 256;
    const int img_height = 256;

    const int total_pixel = img_height * img_width;

    // Render
    if (o.is_open())
    {
        o << "P3\n"
          // P3 means color in ASCII
          << img_width << ' ' << img_height << "\n255" << endl;

        for (int j = 0; j < img_height; ++j)
        {
            for (int i = 0; i < img_width; ++i)
            {
                clog << "\rProgress:" << j * img_width + i << "(Pixels Rendered) / " << total_pixel << flush;
                color pixel_col = color(double(i) / (img_width - 1), double(j) / (img_height - 1), 0);
                write_color(o, pixel_col);
            }
        }
    }
    
    o.close();
    clog << "\nDone." << endl;
}
