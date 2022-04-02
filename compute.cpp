#include <stdlib.h>
#include <assert.h>
#include <immintrin.h>

#include "compute.h"

const int    ITER_MAX = 256;
const double R2_MAX   = 100.0;

const uint32_t COLOR = 0xFFFFFFFF;
const uint32_t BACKGROUND = 0x0;

static void mandelbrot_noAVX(uint32_t* dst,
                      const double tl_x, const double tl_y,
                      const double br_x, const double br_y,
                      const int   width, const int  height);

static void mandelbrot_AVX(uint32_t* dst,
                      const double tl_x, const double tl_y,
                      const double br_x, const double br_y,
                      const int   width, const int  height);

void mandelbrot(uint32_t* dst, bool enableAVX,
                const double tl_x, const double tl_y,
                const double br_x, const double br_y,
                const int   width, const int  height)
{
        if(enableAVX)
                mandelbrot_AVX  (dst, tl_x, tl_y, br_x, br_y, width, height);
        else
                mandelbrot_noAVX(dst, tl_x, tl_y, br_x, br_y, width, height);               
}

static void mandelbrot_noAVX(uint32_t* dst,
                      const double tl_x, const double tl_y,
                      const double br_x, const double br_y,
                      const int   width, const int  height)
{
        assert(dst);

        const double dx = (br_x - tl_x) / width;
        const double dy = (br_y - tl_y) / height;

        double y_0 = tl_y;
        
        for(int iy = 0; iy < height; iy++)
        {
                double x_0 = tl_x;
                
                for(int ix = 0; ix < width; ix++)
                {
                        double x = x_0;
                        double y = y_0;

                        int iter = 1;
                        for(; iter <= ITER_MAX; iter++)
                        {
                                double x2 = x * x;
                                double y2 = y * y;
                                double xy = x * y;

                                double r2 = x2 + y2;

                                if(r2 > R2_MAX)
                                        break;

                                x = x_0 + x2 - y2;
                                y = y_0 + 2 * xy;
                        }

                        dst[iy * width + ix] = (iter & 0x1) ? COLOR : BACKGROUND;

                        x_0 += dx;
                }

                y_0 += dy;
        }
}

static void mandelbrot_AVX(uint32_t* dst,
                           const double tl_x, const double tl_y,
                           const double br_x, const double br_y,
                           const int   width, const int  height)
{
        assert(dst);

        const double dx = (br_x - tl_x) / width;
        const double dy = (br_y - tl_y) / height;

        // avx_dx               = (3*dx  | 2*dx  | 1*dx  | 0*dx )
        const __m256d avx_dx    = _mm256_set_pd(3 * dx, 2 * dx, 1 * dx, 0);
        // avx_r2max            = (R2MAX | R2MAX | R2MAX | R2MAX)
        const __m256d avx_r2max = _mm256_set_pd(R2_MAX, R2_MAX, R2_MAX, R2_MAX);
        
        double y_0 = tl_y;
        
        for(int iy = 0; iy < height; iy++)
        {
                double x_0 = tl_x;
                        
                for(int ix = 0; ix < width; ix += 4)
                {
                        // avx_x_0       = (x_0 + 3*dx | x_0 + 2*dx | x_0 + 1*dx | x_0 + 0*dx)
                        __m256d avx_x_0  = _mm256_add_pd(_mm256_set_pd(x_0, x_0, x_0, x_0), avx_dx);
                        // avx_y_0       = (y_0        | y_0        | y_0        | y_0       )
                        __m256d avx_y_0  = _mm256_set_pd(y_0, y_0, y_0, y_0);
                                                
                        // avx_x         = (x_0 + 3*dx | x_0 + 2*dx | x_0 + 1*dx | x_0 + 0*dx)
                        __m256d avx_x    = avx_x_0;
                        // avx_y         = (y_0        | y_0        | y_0        | y_0       )
                        __m256d avx_y    = avx_y_0;

                        // avx_iter      = (0          | 0          | 0          | 0         )
                        __m256i avx_iter = _mm256_setzero_si256();
                        for(int i = 0; i < ITER_MAX; i++)
                        {
                                // avx_x2      = (x_4^2       | x_3^2      | x_2^2      | x_1^2      )
                                __m256d avx_x2 = _mm256_mul_pd(avx_x, avx_x);
                                // avx_y2      = (y_4^2       | y_3^2      | y_2^2      | y_1^2      )
                                __m256d avx_y2 = _mm256_mul_pd(avx_y, avx_y);
                                // avx_r2      = (r_4^2       | r_3^2      | r_2^2      | r_1^2      )
                                __m256d avx_r2 = _mm256_add_pd(avx_x2, avx_y2);

                                // cmp         = (-1    if r_4^2 <  r2_max | -1    if r_4^2 <  r2_max | -1    if r_4^2 <  r2_max | -1    if r_4^2 <  r2_max)
                                //                 0    if r_4^2 >= r2_max |  0    if r_4^2 >= r2_max |  0    if r_4^2 >= r2_max |  0    if r_4^2 >= r2_max)
                                __m256d cmp    = _mm256_cmp_pd(avx_r2, avx_r2max, _CMP_LT_OQ);
                                // mask        = (1***b if r_4^2 <  r2_max | *1**b if r_4^2 <  r2_max | **1*b if r_4^2 <  r2_max | ***1b if r_4^2 <  r2_max)
                                //               (0***b if r_4^2 >= r2_max | *0**b if r_4^2 >= r2_max | **0*b if r_4^2 >= r2_max | ***0b if r_4^2 >= r2_max)
                                int mask = _mm256_movemask_pd(cmp);
                                if(!mask)
                                        break;

                                // avx_iter    = (avx_iter_4 - cmp_4       | avx_iter_3 - cmp_3       | avx_iter_2 - cmp_2       | avx_iter_1 - cmp_1      )
                                avx_iter       = _mm256_sub_epi64(avx_iter, _mm256_castpd_si256(cmp));

                                __m256d avx_xy = _mm256_mul_pd(avx_x, avx_y);
                                
                                //  x = (x_0 + x2) - y2
                                avx_x = _mm256_sub_pd(_mm256_add_pd(avx_x_0, avx_x2), avx_y2);
                                //  y = y_0 + (xy + xy)
                                avx_y = _mm256_add_pd(avx_y_0, _mm256_add_pd(avx_xy, avx_xy));
                        }

                        for(int i = 0; i < 4; i++)
                        {
                                // *ptr_iter       = (iter_1 | iter_2 | iter_3 | iter_4)
                                uint64_t* ptr_iter = (uint64_t*) &avx_iter;
                                
                                dst[iy * width + ix + i] = (ptr_iter[i] & 0x1) ? COLOR : BACKGROUND;
                        }

                        x_0 += 4 * dx;
                }

                y_0 += dy;
        }
}

