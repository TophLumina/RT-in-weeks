#include "FrameBuffer.h"
#include "color.h"
#include <cmath>

class Denoiser
{
public:
    Denoiser(int kernel_size) : kernel_size(kernel_size) {}

    template <typename... Args>
    void denoise(FrameBuffer<color> &src_color, const FrameBuffer<Args> &...buffers)
    {
        FrameBuffer<color> result = src_color;

        for (int i = 0; i < src_color.height; ++i)
        {
            for (int j = 0; j < src_color.width; ++j)
            {
                // For each pixel, search its neighbors and use it to weight the denoising
                double weight_sum = 0;
                for (int k = -kernel_size; k <= kernel_size; ++k)
                {
                    for (int l = -kernel_size; l <= kernel_size; ++l)
                    {
                        if (k == 0 && l == 0)
                        {
                            weight_sum += 1;
                            continue;
                        }
                        
                        int x = i + k;
                        int y = j + l;

                        if (x >= 0 && x < src_color.height && y >= 0 && y < src_color.width)
                        {
                            // Use the G-buffers to weight the denoising
                            double weight = 0;
                            (..., (weight += buffers.diff(buffers.data[i][j], buffers.data[x][y])));
                            weight = std::exp(-pow(weight, 2) / 2);
                            weight_sum += weight;
                            
                            result.data[i][j] += weight * src_color.data[x][y];
                        }
                    }
                }
                result.data[i][j] /= weight_sum;
            }
        }

        src_color = result;
    }

private:
    int kernel_size = 1;
};