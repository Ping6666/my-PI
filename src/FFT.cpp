#include "FFT.h"

namespace _my_PI_
{

    /* Cooley–Tukey FFT algorithm */

    /**
     * implement convolution, x * y.
     *
     * @param x inputs std::vector<uint32_t>
     * @param y inputs std::vector<uint32_t>
     * @return output std::vector<uint32_t>
     */
    std::vector<uint32_t> convolution(std::vector<uint32_t> x, std::vector<uint32_t> y)
    {
        /* compute potential length in time domain */

        // length in time domain (the max size after convolution)
        int length_t = (int)x.size() + (int)y.size();

        /* compute length in frequency domain */

        // make length_f be the min number is pow of 2 that larger than 3 * length_t
        // and exp_size be the root of length_f
        int length_f = 1, exp_size = 0;
        while (length_f < 3 * length_t)
        {
            length_f = length_f << 1;
            exp_size++;
        }

        /* convert to FFT and shrink to fit the size */

        FFT x_fft = FFT(x);
        x_fft.shrink_to_fit(length_f, true);
        FFT y_fft = FFT(y);
        y_fft.shrink_to_fit(length_f, true);

        /* convert to frequency domain */

        x_fft.dif(0, exp_size);
        y_fft.dif(0, exp_size);

        /* pointwise product */

        FFT z = x_fft.pointwise_product(y_fft); // z = x_fft * y_fft

        /* convert back to time domain */

        z.dit(0, exp_size);

        return z.to_uint32_t();
    }

    /* constructors */

    FFT::FFT(bool domain_) : domain(domain_)
    {
        (*this).values = std::vector<std::complex<double>>();
        (*this).values.reserve(0);
    }

    FFT::FFT(int n, bool domain_) : domain(domain_)
    {
        (*this).values = std::vector<std::complex<double>>();
        (*this).values.reserve(n);
    }

    FFT::FFT(std::vector<uint32_t> convert_v, bool domain_) : domain(domain_)
    {
        int n = convert_v.size();

        (*this).values = std::vector<std::complex<double>>();
        (*this).values.reserve(3 * n);

        /* could cause round-off error */
        // mapping discrete numbers to continuous numbers (still discrete numbers)

        for (int i = 0; i < n; i++)
        {
            uint32_t num = convert_v[i];
            (*this).values.push_back(num % 1000);
            (*this).values.push_back((num / 1000) % 1000);
            (*this).values.push_back((num / 1000000) % 1000);
        }
    }

    /* converter */

    std::vector<uint32_t> FFT::to_uint32_t()
    {
        int diff_n = (int)(*this).values.size() % 3;
        (*this).shrink_to_fit(diff_n);

        size_t n = (*this).values.size() / 3;
        std::vector<uint32_t> convert_v = std::vector<uint32_t>();
        convert_v.reserve(n);

        /* prevent round-off error (like std::round) */
        // mapping continuous numbers to discrete numbers

        // add 0.5 when converting to uint32_t
        double rounding = 0.5;

        uint32_t carry = 0;
        for (size_t i = 0; i < n; i++)
        {
            uint32_t tmp = 0, number = 0;

            // first 3-digits
            number = (uint32_t)((*this).values[3 * i + 0].real() + rounding);
            tmp += (carry + number) % 1000;
            carry = (carry + number) / 1000;

            // second 3-digits
            number = (uint32_t)((*this).values[3 * i + 1].real() + rounding);
            tmp += ((carry + number) % 1000) * 1000;
            carry = (carry + number) / 1000;

            // third 3-digits
            number = (uint32_t)((*this).values[3 * i + 2].real() + rounding);
            tmp += ((carry + number) % 1000) * 1000000;
            carry = (carry + number) / 1000;

            convert_v.push_back(tmp);
        }

        return convert_v;
    }

    /* FFT (fast Fourier transform) */

    /**
     * radix-2 DIF FFT (decimation in frequency fast Fourier transform)
     *
     * @param base_idx complex numbers idx
     * @param n total length, n-th root of unity.
     */
    bool FFT::dif(size_t base_idx, int n)
    {
        if (1 << n == (int)(*this).values.size())
        {
            // first call this function (recursion start)
            if ((*this).domain == true)
            {
                // already in frequency domain
                return false;
            }
            (*this).domain = true;
        }

        if (n == 0)
        {
            return true;
        }

        /* implement N-point DFT (discrete Fourier transform) */
        // on both parts: even-indexed part, odd-indexed part.

        size_t N = (size_t)1 << n; // N = 2 ^ n
        size_t half_N = N >> 1;    // half = N / 2

        double sample_angle = 2 * pi_ / N;

        for (size_t i = 0; i < half_N; i++)
        {
            // complex roots of unity (aka. twiddle factors)
            double phase = sample_angle * i; // (2 * pi * i) / N
            std::complex<double> twiddle_factors = std::complex<double>(cos(phase), sin(phase));

            size_t j = i + half_N;
            std::complex<double> x_i, x_j;
            x_i = (*this).values[base_idx + i];
            x_j = (*this).values[base_idx + j];

            /* butterfly computation */

            (*this).values[base_idx + i] = (x_i + x_j);
            (*this).values[base_idx + j] = (x_i - x_j) * twiddle_factors;
        }

        /* implement half_N-point DFT twice (front / back part) */
        return ((*this).dif(base_idx, n - 1) && (*this).dif(base_idx + half_N, n - 1));
    }

    /**
     * radix-2 DIT FFT (decimation in time fast Fourier transform)
     *
     * @param base_idx complex numbers idx
     * @param n total length, n-th root of unity.
     */
    bool FFT::dit(size_t base_idx, int n)
    {
        if (1 << n == (int)(*this).values.size())
        {
            // first call this function (recursion start)
            if ((*this).domain == false)
            {
                // already in time domain
                return false;
            }
            (*this).domain = false;
        }

        if (n == 0)
        {
            return true;
        }

        size_t N = (size_t)1 << n; // N = 2 ^ n
        size_t half_N = N >> 1;    // half = N / 2

        /* implement half_N-point DFT twice (front / back part) */
        bool result = ((*this).dit(base_idx, n - 1) && (*this).dit(base_idx + half_N, n - 1));

        /* implement N-point DFT (discrete Fourier transform) */
        // on both parts: even-indexed part, odd-indexed part.

        double sample_angle = -2 * pi_ / N; // in order to make inverse

        for (size_t i = 0; i < half_N; i++)
        {
            // complex roots of unity (aka. twiddle factors)
            double phase = sample_angle * i; // (2 * pi (i + (N / 2))) / N = (-2 * pi * i) / N
            std::complex<double> twiddle_factors = std::complex<double>(cos(phase), sin(phase));

            /* FFT algo description */
            // pre_twiddle_factors  = sample_angle but not multiple on -1   = dif twiddle_factors
            // twiddle_factors      = sample_angle but     multiple on -1   = dit twiddle_factors
            // x[i]                 = (x[i] + (x[i + half_N] / pre_twiddle_factors)) / 2 = (x[i] + (x[i + half_N] * twiddle_factors)) / 2;
            // x[i + half_N]        = (x[i] - (x[i + half_N] / pre_twiddle_factors)) / 2 = (x[i] - (x[i + half_N] * twiddle_factors)) / 2;

            size_t j = i + half_N;
            std::complex<double> x_i, x_j;
            x_i = (*this).values[base_idx + i];
            x_j = (*this).values[base_idx + j] * twiddle_factors;

            /* butterfly computation */

            (*this).values[base_idx + i] = (x_i + x_j) / 2.0;
            (*this).values[base_idx + j] = (x_i - x_j) / 2.0;
        }

        return result;
    }

    /**
     * implement pointwise product core part of convolution, return (*this) * x.
     *
     * @param x inputs FFT
     * @return output FFT
     */
    FFT FFT::pointwise_product(FFT &x)
    {
        if (((*this).domain == false || x.domain == false) || ((int)(*this).values.size() != (int)x.values.size()))
        {
            if (DEBUGGER)
            {
                std::cout << "ALERT: FFT vector pointwise product precondition wrong." << std::endl;
                std::cout << "\t domain: " << (*this).domain << " " << x.domain << std::endl;
                std::cout << "\t size: " << (*this).values.size() << " " << x.values.size() << std::endl;
            }

            // (*this) or x
            // 1. not in frequency domain
            // 2. std::vector size was not align

            return FFT();
        }

        size_t n = (*this).values.size();
        FFT z = FFT((int)n, true); // default set to frequency domain

        for (size_t i = 0; i < n; i++)
        {
            z.values.push_back((*this).values[i] * x.values[i]);
        }

        return z;
    }

    /**
     * the vector digits example, s_back = 3.
     * before: x x x 3 2 1 0
     * after : o o o 3 2 1 0 (o fill with std::complex<double>(0, 0))
     *
     * @param s_back fill back size
     */
    bool FFT::shrink_to_fit(int64_t s_back, bool d_size)
    {
        s_back = d_size ? (s_back - (int)(*this).values.size()) : s_back;

        auto complex_zero = std::complex<double>(0, 0);

        if (s_back == -1)
        {
            // check back zero
            while (!(*this).values.empty() && (*this).values.back() == complex_zero)
            {
                // (*this).values.erase((*this).digits.end() - 1);
                (*this).values.pop_back();
            }
        }
        else
        {
            // fill back with zero
            for (int64_t i = 0; i < s_back; i++)
            {
                // (*this).values.insert((*this).values.end(), std::complex<double>(0, 0));
                (*this).values.push_back(complex_zero);
            }
        }

        // (*this).values.shrink_to_fit();

        // check size
        if ((*this).values.empty())
        {
            if (DEBUGGER)
            {
                std::cout << "ALERT: FFT vector shrink error." << std::endl;
            }
            *this = FFT(0);
            return false;
        }

        return true;
    }

}
