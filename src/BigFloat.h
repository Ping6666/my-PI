#ifndef _BIGFLOAT_H_
#define _BIGFLOAT_H_

#include <iostream>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "BBPCore.h"
#include "FFT.h"

namespace _my_PI_
{

    // if want to check on const BigFloat just add some method in this class

    class BigFloat
    {
    public:
        /* constructors: PDS (passive data structure), POD (plain old data) */

        BigFloat();                            // default constructors
        BigFloat(int64_t);                     // default constructors
        BigFloat(const BigFloat &);            // copy constructors
        BigFloat &operator=(const BigFloat &); // copy assignment operator
        BigFloat(BigFloat &&);                 // move constructors
        BigFloat &operator=(BigFloat &&);      // move assignment operator

        /* other constructors */

        BigFloat(int64_t, int64_t, bool = true, int64_t = 0);
        BigFloat(int64_t, uint32_t, bool = true, int64_t = 0);
        BigFloat(int64_t, std::string, bool = true, int64_t = 0);
        BigFloat(int64_t, const char *, bool = true, int64_t = 0);

        /* setter */

        void set_sign(bool);
        void set_precision(int64_t);

        /* prefix operators */

        BigFloat &operator++(); // ++n
        BigFloat &operator--(); // --n

        /* postfix operators */

        BigFloat operator++(int); // n++
        BigFloat operator--(int); // n--

        /* assignment operators */

        BigFloat &operator+=(const BigFloat &);
        BigFloat &operator-=(const BigFloat &);
        BigFloat &operator*=(const BigFloat &);
        BigFloat &operator/=(const BigFloat &);
        BigFloat &operator<<=(const int &);
        BigFloat &operator>>=(const int &);

        /* arithmetic operators */

        BigFloat operator+(const BigFloat &) const;
        BigFloat operator-(const BigFloat &) const;
        BigFloat operator*(const BigFloat &) const;
        BigFloat operator/(const BigFloat &) const;
        BigFloat operator<<(const int &) const;
        BigFloat operator>>(const int &) const;

        /* comparison operators */

        bool operator==(const BigFloat &) const;
        bool operator!=(const BigFloat &) const;
        bool operator<(const BigFloat &) const;
        bool operator>(const BigFloat &) const;
        bool operator<=(const BigFloat &) const;
        bool operator>=(const BigFloat &) const;

        /* mathematics */

        BigFloat invSqrt() const;
        BigFloat pow(int) const;

        /* other methods */

        bool sum_prec_check();
        bool truncation(int64_t = -1);
        std::string to_string() const;

    private:
        int64_t precision;            // the number of digits under decimal point
        bool sign;                    // true: positive; false: negative.
        int64_t exp;                  // exponent
        std::vector<uint32_t> digits; // numbers

        /* mathematics */

        bool shiftWorkHouse(const BigFloat &, BigFloat &, BigFloat &) const;
        bool invSqrt(const BigFloat &, BigFloat &, int) const;
        bool fast_invSqrt(const BigFloat &, const BigFloat &, int, BigFloat &) const;
        bool reciprocal(const BigFloat &, BigFloat &, int) const;
        bool fast_reciprocal(const BigFloat &, const BigFloat &, int, BigFloat &) const;

        /* arithmetic logic circuits */

        bool complementer(uint32_t, uint32_t &) const;
        bool fullAdder(uint32_t, uint32_t, uint32_t, uint32_t &, uint32_t &) const;
        bool fullSubtractor(uint32_t, uint32_t, uint32_t, uint32_t &, uint32_t &) const;

        /* other methods */

        bool zero_check() const;
        bool shrink_to_fit(int64_t = -1, int64_t = -1);
        bool shrink_to_fit_size(BigFloat &, BigFloat &, int64_t &, int64_t &, int64_t &, int64_t &) const;
    };

}

#endif
