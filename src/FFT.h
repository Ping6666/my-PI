#ifndef _FFT_H_
#define _FFT_H_

#include <iostream>
#include <complex>
#include <vector>

#include "BBPCore.h"

namespace _my_PI_
{
    /* Cooley–Tukey FFT algorithm */

    std::vector<uint32_t> convolution(std::vector<uint32_t>, std::vector<uint32_t>);

    class FFT
    {
    public:
        /* constructors */

        FFT(bool = false);
        FFT(int, bool = false);
        FFT(std::vector<uint32_t>, bool = false);

        /* FFT (fast Fourier transform) */

        bool dif(size_t, int);
        bool dit(size_t, int);
        FFT pointwise_product(FFT &);

        /* converter */

        std::vector<uint32_t> to_uint32_t();

        /* other methods */

        bool shrink_to_fit(int64_t = -1, bool = false);

    private:
        bool domain = false; // true: frequency, false: time.
        std::vector<std::complex<double>> values;
    };

}

#endif
