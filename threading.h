#pragma once

// MultiThread
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>

#include "rtweekend.h"
#include "sphere.h"
#include "hittable_list.h"
#include "material.h"

using namespace std;

class threading // now served as a helper function lib to achieve parallelism, might be reconstructed into thread pool someday
{
public:
    static void threading_indicator(int total_lines)
    {
        while (thread_finished < total_lines)
        {
            clog << "\rLines Rendered: " << thread_finished << " / " << total_lines << flush;
            this_thread::sleep_for(chrono::microseconds(500));
        }
    }

    // Line Counter for MultiThreading
    static atomic<int> thread_finished;
};

atomic<int> threading::thread_finished(0);