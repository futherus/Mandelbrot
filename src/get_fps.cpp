#include <chrono>
#include <stdlib.h>
#include <math.h>
#include "get_fps.h"

int get_fps()
{
        static std::chrono::time_point<std::chrono::steady_clock> prev{};

        auto cur = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = cur - prev;

        prev = cur;

        double time = elapsed.count();

        return (int) fabs(1 / time);
}
