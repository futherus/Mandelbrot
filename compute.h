#ifndef COMPUTE_H
#define COMPUTE_H

#include <stdint.h>
#include <stdbool.h>

void mandelbrot(uint32_t* dst, bool enableAVX,
                const double tl_x, const double tl_y,
                const double br_x, const double br_y,
                const int   width, const int  height);

#endif // COMPUTE_H
