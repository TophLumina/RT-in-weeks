#include "FrameBuffer.h"
#include "color.h"
#include "ThreadPool.hpp"

#include <cmath>
#include <future>
#include <queue>

class Denoiser
{
public:
    Denoiser(double kernal_radius, int samplers, ThreadPool &pool) : kernal_radius(kernal_radius), samplers(samplers), pool(pool) {}

    template <typename... Args>
    void denoise(FrameBuffer<color> &src_color, const FrameBuffer<Args> &...buffers)
    {
        FrameBuffer<color> result = src_color;

        for (int i = 0; i < src_color.height; ++i)
        {
            for (int j = 0; j < src_color.width; ++j)
            {
                futures.push(pool.Submit([&](int i, int j)
                {
                    // For each pixel, search its neighbors and use it to weight the denoising
                    double weight_sum = 0;

                    for (int s = 0; s < samplers; ++s)
                    {
                        vec2d offsets = Math::Vector::random_disk(kernal_radius);
                        vec2i coords = vec2d(i, j) + offsets + 0.5;

                        if (coords.x >= 0 && coords.x < src_color.height && coords.y >= 0 && coords.y < src_color.width)
                        {
                            if (i == coords.x && j == coords.y)
                            {
                                continue;
                            }

                            // Use the G-buffers to weight the denoising
                            double weight = 0;
                            (..., (weight += buffers.diff(buffers.data[i][j], buffers.data[coords.x][coords.y])));
                            weight = std::exp(-weight);

                            weight_sum += weight;

                            result.data[i][j] += weight * src_color.data[coords.x][coords.y];
                        }
                    }
                    result.data[i][j] /= weight_sum;
                }, i, j));
            }
        }

        while (!futures.empty())
        {
            futures.front().get();
            futures.pop();
        }

        src_color = result;
    }

private:
    double kernal_radius;
    int samplers;
    ThreadPool &pool;
    std::queue<std::future<void>> futures;
};