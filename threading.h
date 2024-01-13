#pragma once

// MultiThread
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>

using namespace std;

class threading
{
public:
    static void threading_indicator(int total_lines)
    {
        while (thread_finished < total_lines)
        {
            clog << "\rLines Rendered: " << thread_finished << " / " << total_lines << flush;
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }

    // Line Counter for MultiThreading
    static atomic<int> thread_finished;
};

atomic<int> threading::thread_finished(0);