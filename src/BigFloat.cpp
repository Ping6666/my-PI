#include "BigFloat.h"

namespace _my_PI_
{

    /* constructors: PDS (passive data structure), POD (plain old data) */

    /**
     * default constructors, convenient one.
     * need to set precision afterwards
     */
    BigFloat::BigFloat() : sign(true), exp(0)
    {
        (*this).digits = std::vector<uint32_t>();
        (*this).digits.reserve(1);
        (*this).digits.push_back((uint32_t)0);
    }

    /**
     * default constructors
     */
    BigFloat::BigFloat(int64_t precision_) : precision(precision_), sign(true), exp(0)
    {
        (*this).digits = std::vector<uint32_t>();
        (*this).digits.reserve(1);
        (*this).digits.push_back((uint32_t)0);
    }

    /**
     * copy constructors
     */
    BigFloat::BigFloat(const BigFloat &n) : precision(n.precision), sign(n.sign), exp(n.exp)
    {
        (*this).digits = n.digits;
    }

    /**
     * copy assignment operator
     */
    BigFloat &BigFloat::operator=(const BigFloat &rhs_n)
    {
        (*this).precision = rhs_n.precision;
        (*this).sign = rhs_n.sign;
        (*this).exp = rhs_n.exp;
        (*this).digits = rhs_n.digits;

        return *this;
    }

    /**
     * move constructors
     */
    BigFloat::BigFloat(BigFloat &&n) : precision(n.precision), sign(n.sign), exp(n.exp)
    {
        (*this).digits = n.digits;
    }

    /**
     * move assignment operator
     */
    BigFloat &BigFloat::operator=(BigFloat &&rhs_n)
    {
        (*this).precision = rhs_n.precision;
        (*this).sign = rhs_n.sign;
        (*this).exp = rhs_n.exp;
        (*this).digits = rhs_n.digits;

        return *this;
    }

    /* other constructors */

    BigFloat::BigFloat(int64_t precision_, int64_t size, bool sign_, int64_t exp_)
        : precision(precision_), sign(sign_), exp(exp_)
    {
        (*this).digits = std::vector<uint32_t>();
        (*this).digits.reserve(size);
        for (int64_t i = 0; i < size; i++)
        {
            (*this).digits.push_back((uint32_t)0);
        }
    }

    BigFloat::BigFloat(int64_t precision_, uint32_t n, bool sign_, int64_t exp_)
        : precision(precision_), sign(true), exp(0)
    {
        if (n == 0)
        {
            return;
        }

        (*this).sign = sign_;
        (*this).exp = exp_;

        (*this).digits = std::vector<uint32_t>(2);
        fullAdder(n, 0, 0, (*this).digits[1], (*this).digits[0]);
        (*this).shrink_to_fit();
    }

    BigFloat::BigFloat(int64_t precision_, std::string str, bool sign_, int64_t exp_)
        : precision(precision_), sign(sign_), exp(exp_)
    {
        if (str.size() == 0)
        {
            *this = BigFloat(precision_, (uint32_t)0);
            return;
        }

        size_t iter;
        for (iter = 0; iter < str.size(); iter++)
        {
            if (str[iter] == '.')
            {
                break;
            }
        }

        iter = std::min(iter, str.size());

        // decimal front and back
        std::string dec_f, dec_b = "0";
        dec_f = str.substr(0, iter);
        if (iter < str.size())
        {
            dec_b = str.substr(iter + 1, str.size() - iter);
        }

        int append_l_f = (9 - ((int)dec_f.size() % 9)) % 9;
        for (int i = 0; i < append_l_f; i++)
        {
            dec_f = "0" + dec_f;
        }
        int append_l_b = (9 - ((int)dec_b.size() % 9)) % 9;
        for (int i = 0; i < append_l_b; i++)
        {
            dec_b = dec_b + "0";
        }

        int size = ((int)dec_f.size() / 9) + ((int)dec_b.size() / 9);
        exp -= ((int)dec_b.size() / 9);

        (*this).digits = std::vector<uint32_t>();
        (*this).digits.reserve(size);

        while (dec_f.size() > 0)
        {
            const char *tmp = (dec_f.substr(0, 9)).c_str() + '\0';
            dec_f = dec_f.substr(9, dec_f.size() - 9);

            (*this).digits.insert((*this).digits.begin(), (uint32_t)std::strtoul(tmp, NULL, 10));
        }

        while (dec_b.size() > 0)
        {
            const char *tmp = (dec_b.substr(0, 9)).c_str() + '\0';
            dec_b = dec_b.substr(9, dec_b.size() - 9);

            (*this).digits.insert((*this).digits.begin(), (uint32_t)std::strtoul(tmp, NULL, 10));
        }

        (*this).shrink_to_fit();
    }

    BigFloat::BigFloat(int64_t precision_, const char *str_, bool sign_, int64_t exp_)
    {
        std::string str = std::string(str_);
        *this = BigFloat(precision_, str, sign_, exp_);
    }

    /* setter */

    void BigFloat::set_sign(bool s)
    {
        (*this).sign = s;
    }

    void BigFloat::set_precision(int64_t p)
    {
        (*this).precision = p;
    }

    /* prefix operators */

    BigFloat &BigFloat::operator++()
    {
        // ++n
        *this += BigFloat((*this).precision, (uint32_t)1);
        return *this;
    }

    BigFloat &BigFloat::operator--()
    {
        // --n
        *this -= BigFloat((*this).precision, (uint32_t)1);
        return *this;
    }

    /* postfix operators */

    BigFloat BigFloat::operator++(int)
    {
        // n++
        BigFloat bigFloat = *this;
        ++(*this);
        return bigFloat;
    }

    BigFloat BigFloat::operator--(int)
    {
        // n--
        BigFloat bigFloat = *this;
        --(*this);
        return bigFloat;
    }

    /* assignment operators */

    BigFloat &BigFloat::operator+=(const BigFloat &rhs_n)
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        if (rhs_n.zero_check())
        {
            return *this;
        }

        if ((*this).sign != rhs_n.sign)
        {
            BigFloat neg_rhs_n = BigFloat(rhs_n);
            neg_rhs_n.sign = (*this).sign;
            *this -= neg_rhs_n;
            return *this;
        }

        BigFloat lhs_n_ = *this, rhs_n_ = rhs_n;

        // /* precision: truncation cut */

        // lhs_n_.truncation(2 * lhs_n_.precision + 1);
        // rhs_n_.truncation(2 * rhs_n_.precision + 1);

        /* += precondition check part */

        int64_t lhs_n_s_front = 0, lhs_n_s_back = 0, rhs_n_s_front = 0, rhs_n_s_back = 0;
        shrink_to_fit_size(lhs_n_, rhs_n_, lhs_n_s_front, lhs_n_s_back, rhs_n_s_front, rhs_n_s_back);
        lhs_n_.shrink_to_fit(lhs_n_s_front, lhs_n_s_back);
        rhs_n_.shrink_to_fit(rhs_n_s_front, rhs_n_s_back);

        /* important */
        // below statements should always be true
        // 1. lhs_n_.exp == rhs_n_.exp
        // 2. (int64_t)lhs_n_.digits.size() == (int64_t)rhs_n_.digits.size()

        /* += core part */

        int64_t total_length = (int64_t)lhs_n_.digits.size();
        BigFloat bigFloat = BigFloat(lhs_n_.precision, (int64_t)(total_length + 1),
                                     lhs_n_.sign, lhs_n_.exp);

        uint32_t carry = 0;
        int64_t idx = 0;
        for (idx = 0; idx < total_length && idx < (int64_t)lhs_n_.digits.size() && idx < (int64_t)rhs_n_.digits.size(); idx++)
        {
            fullAdder(lhs_n_.digits[idx], rhs_n_.digits[idx], carry, carry, bigFloat.digits[idx]);
        }

        while (carry != 0 && idx < total_length + 1)
        {
            fullAdder(bigFloat.digits[idx], 0, carry, carry, bigFloat.digits[idx]);
            idx++;
        }

        bigFloat.shrink_to_fit();
        *this = bigFloat;

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "+= take " << diff_t.count() << " sec." << std::endl;
        }

        return *this;
    }

    BigFloat &BigFloat::operator-=(const BigFloat &rhs_n)
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        if (rhs_n.zero_check())
        {
            return *this;
        }

        if ((*this).sign != rhs_n.sign)
        {
            BigFloat neg_rhs_n = BigFloat(rhs_n);
            neg_rhs_n.sign = (*this).sign;
            *this += neg_rhs_n;
            return *this;
        }

        bool swap = (*this).sign ? (*this < rhs_n) : (*this > rhs_n); // output sign check

        // while subtracting, make sure can always borrow some on lhs_n_
        // keep abs(lhs_n_) > abs(rhs_n_)

        BigFloat lhs_n_ = swap ? rhs_n : *this;
        BigFloat rhs_n_ = swap ? *this : rhs_n;

        // /* precision: truncation cut */

        // lhs_n_.truncation(2 * lhs_n_.precision + 1);
        // rhs_n_.truncation(2 * rhs_n_.precision + 1);

        /* -= precondition check part */

        int64_t lhs_n_s_front = 0, lhs_n_s_back = 0, rhs_n_s_front = 0, rhs_n_s_back = 0;
        shrink_to_fit_size(lhs_n_, rhs_n_, lhs_n_s_front, lhs_n_s_back, rhs_n_s_front, rhs_n_s_back);
        lhs_n_.shrink_to_fit(lhs_n_s_front, lhs_n_s_back);
        rhs_n_.shrink_to_fit(rhs_n_s_front, rhs_n_s_back);

        /* important */
        // below statements should always be true
        // 1. lhs_n_.exp == rhs_n_.exp
        // 2. (int64_t)lhs_n_.digits.size() == (int64_t)rhs_n_.digits.size()

        /* -= core part */

        int64_t total_length = (int64_t)lhs_n_.digits.size();
        BigFloat bigFloat = BigFloat(lhs_n_.precision, (int64_t)(total_length + 1),
                                     (swap ? !lhs_n_.sign : lhs_n_.sign), lhs_n_.exp);

        uint32_t carry = 0;
        int64_t idx = 0;
        for (idx = 0; idx < total_length && idx < (int64_t)lhs_n_.digits.size() && idx < (int64_t)rhs_n_.digits.size(); idx++)
        {
            fullSubtractor(lhs_n_.digits[idx], rhs_n_.digits[idx], carry, carry, bigFloat.digits[idx]);
        }

        while (carry != 0 && idx < total_length + 1)
        {
            fullSubtractor(bigFloat.digits[idx], 0, carry, carry, bigFloat.digits[idx]);
            idx++;
        }

        bigFloat.shrink_to_fit();
        *this = bigFloat;

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "-= take " << diff_t.count() << " sec." << std::endl;
        }

        return *this;
    }

    BigFloat &BigFloat::operator*=(const BigFloat &rhs_n)
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        if (rhs_n.zero_check())
        {
            *this = BigFloat((*this).precision, (uint32_t)0);
            return *this;
        }

        /* precision: truncation cut */

        BigFloat rhs_n_ = rhs_n;
        (*this).truncation(4 * (*this).precision + 1);
        rhs_n_.truncation(4 * rhs_n_.precision + 1);

        bool mul_sign = !((*this).sign ^ rhs_n_.sign);
        int64_t mul_exp = (*this).exp + rhs_n_.exp;

        /* *= core part */

        (*this).digits = convolution((*this).digits, rhs_n_.digits);
        (*this).sign = mul_sign;
        (*this).exp = mul_exp;

        (*this).truncation(2 * (*this).precision + 1);
        (*this).shrink_to_fit();

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "*= take " << diff_t.count() << " sec." << std::endl;
        }

        return *this;
    }

    BigFloat &BigFloat::operator/=(const BigFloat &rhs_n)
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        if (rhs_n.zero_check())
        {
            if (DEBUGGER)
            {
                std::cout << "ALERT: tremendous error, divide by 0." << std::endl;
            }
            throw std::runtime_error("ERROR: divide by 0.\n");
            return (*this);
        }

        /* make n_in to fall inside range (1, 10) */

        BigFloat rhs_mul, rhs_shift;               // sure will be set in shiftWorkHouse
        shiftWorkHouse(rhs_n, rhs_shift, rhs_mul); // rhs_n * rhs_mul = rhs_shift

        if (rhs_shift == BigFloat(rhs_shift.precision, (uint32_t)1))
        {
            // divide by 1
            (*this) *= rhs_mul;
            return (*this);
        }

        /* precision: truncation cut */

        (*this).truncation(4 * (*this).precision + 1);
        rhs_shift.truncation(4 * rhs_shift.precision + 1);

        /* /= core part */

        BigFloat rhs_n_rcp;                                          // sure will be set in reciprocal
        reciprocal(rhs_shift, rhs_n_rcp, 2 * (*this).precision + 1); // rhs_n_rcp = 1 / rhs_n
        (*this) *= rhs_mul * rhs_n_rcp;                              // (*this) * rhs_n_rcp = (*this) * (1 / rhs_n)

        (*this).truncation(2 * (*this).precision + 1);
        (*this).shrink_to_fit();

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "/= take " << diff_t.count() << " sec." << std::endl;
        }

        return *this;
    }

    /**
     * digit-wise shift left (aka. multiply 10 ^ rhs_n)
     */
    BigFloat &BigFloat::operator<<=(const int &rhs_n)
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        if (rhs_n == 0)
        {
            return *this;
        }
        else if (rhs_n < 0)
        {
            int neg_rhs_n = -1 * rhs_n;
            *this >>= neg_rhs_n;
            return *this;
        }

        // left shift
        (*this).exp += rhs_n / 9;
        int shift_digits = rhs_n % 9;

        int64_t shift_num = 1;
        for (int i = 0; i < shift_digits; i++)
        {
            shift_num *= 10;
        }

        int64_t carry_num = (int64_t)uint32_t_size / shift_num;

        (*this).shrink_to_fit(0, 1);

        uint32_t carry = 0;
        for (int idx = 0; idx < (int)(*this).digits.size(); idx++)
        {
            uint32_t remain_digits = (*this).digits[idx] % carry_num;
            uint32_t carry_digits = (*this).digits[idx] / carry_num;
            fullAdder(remain_digits * shift_num, 0, carry, carry, (*this).digits[idx]);
            carry += carry_digits;
        }

        (*this).shrink_to_fit();

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "<<= take " << diff_t.count() << " sec." << std::endl;
        }

        return *this;
    }

    /**
     * digit-wise shift right (aka. multiply 1 / 10 ^ rhs_n)
     */
    BigFloat &BigFloat::operator>>=(const int &rhs_n)
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        if (rhs_n == 0)
        {
            return *this;
        }
        else if (rhs_n < 0)
        {
            int neg_rhs_n = -1 * rhs_n;
            *this <<= neg_rhs_n;
            return *this;
        }

        // right shift
        (*this).exp -= rhs_n / 9;
        int shift_digits = rhs_n % 9;

        int64_t carry_num = 1;
        for (int i = 0; i < shift_digits; i++)
        {
            carry_num *= 10;
        }

        int64_t shift_num = (int64_t)uint32_t_size / carry_num;

        (*this).shrink_to_fit(1, 0);

        uint32_t carry = 0;
        for (int idx = (int)(*this).digits.size() - 1; idx >= 0; idx--)
        {
            uint32_t remain_digits = (*this).digits[idx] / carry_num;
            uint32_t carry_digits = (*this).digits[idx] % carry_num;
            fullAdder(remain_digits, 0, carry, carry, (*this).digits[idx]);
            carry += carry_digits * shift_num;
        }

        (*this).shrink_to_fit();

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << ">>= take " << diff_t.count() << " sec." << std::endl;
        }

        return *this;
    }

    /* arithmetic operators */

    BigFloat BigFloat::operator+(const BigFloat &rhs_n) const
    {
        BigFloat bigFloat = BigFloat(*this);
        bigFloat += rhs_n;
        return bigFloat;
    }

    BigFloat BigFloat::operator-(const BigFloat &rhs_n) const
    {
        BigFloat bigFloat = BigFloat(*this);
        bigFloat -= rhs_n;
        return bigFloat;
    }

    BigFloat BigFloat::operator*(const BigFloat &rhs_n) const
    {
        BigFloat bigFloat = BigFloat(*this);
        bigFloat *= rhs_n;
        return bigFloat;
    }

    BigFloat BigFloat::operator/(const BigFloat &rhs_n) const
    {
        BigFloat bigFloat = BigFloat(*this);
        bigFloat /= rhs_n;
        return bigFloat;
    }

    BigFloat BigFloat::operator<<(const int &rhs_n) const
    {
        BigFloat bigFloat = BigFloat(*this);
        bigFloat <<= rhs_n;
        return bigFloat;
    }

    BigFloat BigFloat::operator>>(const int &rhs_n) const
    {
        BigFloat bigFloat = BigFloat(*this);
        bigFloat >>= rhs_n;
        return bigFloat;
    }

    /* comparison operators */

    bool BigFloat::operator==(const BigFloat &rhs_n) const
    {
        if ((*this).sign != rhs_n.sign)
        {
            // sign inequality
            return false;
        }

        if ((*this).exp != rhs_n.exp)
        {
            // exp inequality
            return false;
        }

        uint32_t i = (*this).digits.size();
        while (i > 0)
        {
            if ((*this).digits[i - 1] != rhs_n.digits[i - 1])
            {
                return false;
            }
            i--;
        }

        return true;
    }

    bool BigFloat::operator!=(const BigFloat &rhs_n) const
    {
        return !(*this == rhs_n);
    }

    bool BigFloat::operator<(const BigFloat &rhs_n) const
    {
        if ((*this).sign == true && rhs_n.sign == false)
        {
            // *this > rhs_n
            return false;
        }
        else if ((*this).sign == false && rhs_n.sign == true)
        {
            // *this < rhs_n
            return true;
        }

        int64_t curr_dec_idx = std::max((int64_t)(*this).digits.size() + (*this).exp, (int64_t)rhs_n.digits.size() + rhs_n.exp);

        while (true)
        {
            int64_t this_idx = curr_dec_idx - (*this).exp;
            int64_t rhs_n_idx = curr_dec_idx - rhs_n.exp;
            uint32_t this_num = 0, rhs_num = 0;
            if ((this_idx <= (int64_t)(*this).digits.size()) && (this_idx > 0))
            {
                this_num = (*this).digits[this_idx - 1];
            }
            if ((rhs_n_idx <= (int64_t)rhs_n.digits.size()) && (rhs_n_idx > 0))
            {
                rhs_num = rhs_n.digits[rhs_n_idx - 1];
            }

            if (this_idx <= 0 && rhs_n_idx <= 0)
            {
                break;
            }

            if (this_num != rhs_num)
            {
                if (this_num > rhs_num)
                {
                    // abs(*this) > abs(rhs_n)
                    return (*this).sign ? false : true;
                }
                else if (this_num < rhs_num)
                {
                    // abs(*this) < abs(rhs_n)
                    return (*this).sign ? true : false;
                }
                break;
            }
            curr_dec_idx--;
        }

        return false;
    }

    bool BigFloat::operator>(const BigFloat &rhs_n) const
    {
        if ((*this).sign == true && rhs_n.sign == false)
        {
            // *this > rhs_n
            return true;
        }
        else if ((*this).sign == false && rhs_n.sign == true)
        {
            // *this < rhs_n
            return false;
        }

        int64_t curr_dec_idx = std::max((int64_t)(*this).digits.size() + (*this).exp, (int64_t)rhs_n.digits.size() + rhs_n.exp);

        while (true)
        {
            int64_t this_idx = curr_dec_idx - (*this).exp;
            int64_t rhs_n_idx = curr_dec_idx - rhs_n.exp;
            uint32_t this_num = 0, rhs_num = 0;
            if ((this_idx <= (int64_t)(*this).digits.size()) && (this_idx > 0))
            {
                this_num = (*this).digits[this_idx - 1];
            }
            if ((rhs_n_idx <= (int64_t)rhs_n.digits.size()) && (rhs_n_idx > 0))
            {
                rhs_num = rhs_n.digits[rhs_n_idx - 1];
            }

            if (this_idx <= 0 && rhs_n_idx <= 0)
            {
                break;
            }

            if (this_num != rhs_num)
            {
                if (this_num > rhs_num)
                {
                    // abs(*this) > abs(rhs_n)
                    return (*this).sign ? true : false;
                }
                else if (this_num < rhs_num)
                {
                    // abs(*this) < abs(rhs_n)
                    return (*this).sign ? false : true;
                }
                break;
            }
            curr_dec_idx--;
        }

        return false;
    }

    bool BigFloat::operator<=(const BigFloat &rhs_n) const
    {
        return !(*this > rhs_n);
    }

    bool BigFloat::operator>=(const BigFloat &rhs_n) const
    {
        return !(*this < rhs_n);
    }

    /* mathematics */
    /**
     * compute the invSqrt, UI part
     * */
    BigFloat BigFloat::invSqrt() const
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        if ((*this) <= BigFloat((*this).precision, (uint32_t)0))
        {
            if (DEBUGGER)
            {
                std::cout << "ALERT: tremendous error, invSqrt a number under or equal to 0." << std::endl;
            }
            throw std::runtime_error("ERROR: invSqrt by under or equal to 0.\n");
            return (*this);
        }

        /* invSqrt core part */

        BigFloat this_invSqrt;                                   // sure will be set in invSqrt
        invSqrt(*this, this_invSqrt, 2 * (*this).precision + 1); // this_invSqrt = 1 / (*this ^ 0.5)

        /* precision: truncation cut */

        this_invSqrt.truncation(2 * (*this).precision + 1);
        this_invSqrt.shrink_to_fit();

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "invSqrt UI take " << diff_t.count() << " sec." << std::endl;
        }

        return this_invSqrt;
    }

    /**
     * n_out = n_in ^ times
     * (aka. n_out = pow(n_in, times) in cmath)
     */
    BigFloat BigFloat::pow(int times) const
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        BigFloat n_out = BigFloat((*this).precision, (uint32_t)1);
        if (times == 0)
        {
            // do nothing
        }
        else if (times < 0)
        {
            times = -1 * times;
            for (int i = 0; i < times; i++)
            {
                n_out /= (*this);
            }
        }
        else
        {
            for (int i = 0; i < times; i++)
            {
                n_out *= (*this);
            }
        }

        /* precision: truncation cut */

        n_out.truncation(2 * n_out.precision + 1);
        n_out.shrink_to_fit();

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "pow take " << diff_t.count() << " sec." << std::endl;
        }

        return n_out;
    }

    /**
     * convert n_in to [1, 10), and n_in * n_mul = n_out
     *
     * @param n_in target number (should be positive number)
     * @param n_out shift number
     * @param n_mul shift multiplier
     */
    bool BigFloat::shiftWorkHouse(const BigFloat &n_in, BigFloat &n_out, BigFloat &n_mul) const
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        BigFloat n_in_ = n_in;
        n_in_.sign = true;

        if (n_in_.zero_check())
        {
            n_out = BigFloat(n_in.precision, (uint32_t)0);
            n_mul = BigFloat(n_in.precision, (uint32_t)0);
            return true;
        }

        /* speed up for digits shift */

        n_mul = BigFloat(n_in.precision, (uint32_t)1, n_in.sign);
        n_mul.exp = -1 * (int64_t)n_in.digits.size() - n_in.exp;
        n_in_.exp = -1 * (int64_t)n_in.digits.size();

        /* digit shift */

        BigFloat bigFloat_1 = BigFloat(n_in.precision, (uint32_t)1);
        BigFloat bigFloat_10 = BigFloat(n_in.precision, (uint32_t)10);

        int shifter = 0;
        if (n_in_ < bigFloat_1)
        {
            while (n_in_ < bigFloat_1)
            {
                n_in_ <<= 1;
                shifter++;
            }
        }
        else if (n_in_ >= bigFloat_10)
        {
            while (n_in_ >= bigFloat_10)
            {
                n_in_ >>= 1;
                shifter--;
            }
        }

        n_out = n_in_;
        n_mul <<= shifter;

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "shiftWorkHouse take " << diff_t.count() << " sec." << std::endl;
        }

        return true;
    }

    /**
     * compute the invSqrt, core part
     *
     * @param n_in the target need to invSqrt and need to > 0
     * @param n_out the invSqrt of input target
     * @param times recursion times of fast_invSqrt
     */
    bool BigFloat::invSqrt(const BigFloat &n_in, BigFloat &n_out, int times) const
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        /* cheat way to get a close guess on n_in's invSqrt for x_0 */

        BigFloat n_in_trunc = n_in;
        n_in_trunc.truncation(1);
        double d_n_in_invSqrt = (double)1 / (double)std::stod(n_in_trunc.to_string());
        d_n_in_invSqrt = std::pow(d_n_in_invSqrt, 0.5);

        /* Newton–Raphson invSqrt */

        fast_invSqrt(n_in, BigFloat(n_in.precision, std::to_string(d_n_in_invSqrt)), times, n_out);
        n_out.sign = n_in.sign;

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "invSqrt CORE take " << diff_t.count() << " sec." << std::endl;
        }

        return true;
    }

    /**
     * Newton–Raphson invSqrt:
     * X = X' + X' * ((1 - N * (X' ^ 2)) / 2)
     *
     * @param n_in the invSqrt target
     * @param x_0 initial value of the Newton–Raphson method
     * @param times recursion times
     * @return
     */
    bool BigFloat::fast_invSqrt(const BigFloat &n_in, const BigFloat &x_0, int times, BigFloat &x_out) const
    {
        if (times == 0)
        {
            x_out = x_0;
            return true;
        }

        bool checker = fast_invSqrt(n_in, x_0, times / 2, x_out);

        BigFloat n_mul_invSqrt = n_in * x_out * x_out;
        if (n_mul_invSqrt == BigFloat(n_in.precision, (uint32_t)1))
        {
            // speed up
            x_out = x_out;
        }
        else
        {
            x_out = x_out + x_out * ((BigFloat(n_in.precision, (uint32_t)1) - n_mul_invSqrt) / BigFloat(n_in.precision, (uint32_t)2));
        }

        return checker;
    }

    /**
     * compute the reciprocal (multiplicative inverse)
     * since case n_in == 1 is handled by /= function
     * therefore, guarantee 0 < n_out < 1 (n_out = 1 / abs(n_in))
     *
     * @param n_in the target need to reciprocal
     * @param n_out the reciprocal of input target
     * @param times recursion times of fast_reciprocal
     */
    bool BigFloat::reciprocal(const BigFloat &n_in, BigFloat &n_out, int times) const
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        /* cheat way to get a close guess on n_in's reciprocal for x_0 */

        BigFloat n_in_trunc = n_in;
        n_in_trunc.truncation(1);
        double d_n_in_rcp = (double)1 / (double)std::stod(n_in_trunc.to_string());

        /* Newton–Raphson division */

        fast_reciprocal(n_in, BigFloat(n_in.precision, std::to_string(d_n_in_rcp)), times, n_out);
        n_out.sign = n_in.sign;
        n_out.exp = -1 * ((int64_t)n_out.digits.size()); // guarantee 0 < n_out < 1

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "reciprocal take " << diff_t.count() << " sec." << std::endl;
        }

        return true;
    }

    /**
     * Newton–Raphson division:
     * X = X' + X' * (1 - N * X')
     *
     * @param n_in the reciprocal target
     * @param x_0 initial value of the Newton–Raphson method
     * @param times recursion times
     * @return
     */
    bool BigFloat::fast_reciprocal(const BigFloat &n_in, const BigFloat &x_0, int times, BigFloat &x_out) const
    {
        if (times == 0)
        {
            x_out = x_0;
            return true;
        }

        bool checker = fast_reciprocal(n_in, x_0, times / 2, x_out);

        BigFloat n_mul_rcp = n_in * x_out;
        if (n_mul_rcp == BigFloat(n_in.precision, (uint32_t)1))
        {
            // speed up
            x_out = x_out;
        }
        else
        {
            x_out = x_out + x_out * (BigFloat(n_in.precision, (uint32_t)1) - n_mul_rcp);
        }

        return checker;
    }

    /* arithmetic logic circuits */

    /**
     * 999999999s' complement (cpl)
     */
    bool BigFloat::complementer(uint32_t n_in, uint32_t &n_out) const
    {
        n_out = uint32_t_cpl - n_in;
        return true;
    }

    /**
     * fullAdder as the digital circuit in ALUs
     * a + b + c_in = c_out * base + s
     *
     * @param a input number
     * @param b input number
     * @param c_in input carry
     * @param c_out output carry
     * @param s output sum
     */
    bool BigFloat::fullAdder(uint32_t a, uint32_t b, uint32_t c_in, uint32_t &c_out, uint32_t &s) const
    {
        uint64_t sum = (uint64_t)a + (uint64_t)b + (uint64_t)c_in;
        c_out = sum / uint32_t_size;
        s = sum % uint32_t_size;

        return true;
    }

    /**
     * fullSubtractor as the digital circuit in ALUs
     * x - y - b_in = b_out * base + d
     *
     * @param x input number
     * @param y input number
     * @param b_in input borrow
     * @param b_out output borrow
     * @param d output difference
     */
    bool BigFloat::fullSubtractor(uint32_t x, uint32_t y, uint32_t b_in, uint32_t &b_out, uint32_t &d) const
    {
        uint32_t y_cpl, b_in_cpl, tmp_b_out;

        complementer(y, y_cpl);
        fullAdder(x, y_cpl, 1, b_out, d); // x - y = x + y_cpl + 1

        complementer(b_in, b_in_cpl);
        fullAdder(d, b_in_cpl, 1, tmp_b_out, d); // - b_in = + b_in_cpl + 1

        b_out += tmp_b_out; // should never overflow
        b_out = (b_out == 1) ? 1 : 0;

        return true;
    }

    /* other methods */

    /**
     * check if all digits are zero
     *
     * @return true: if all digits are zero; false: vice versa.
     */
    bool BigFloat::zero_check() const
    {
        bool checker = true;
        for (int i = 0; checker && i < (int)(*this).digits.size(); i++)
        {
            checker = ((int64_t)(*this).digits[i] == 0) ? true : false;
        }
        return checker;
    }

    bool BigFloat::sum_prec_check()
    {
        /* check if current precision is match the target */

        bool check = false;
        if ((*this).precision + (*this).exp + (int64_t)(*this).digits.size() < 0)
        {
            check = true;
        }

        return check;
    }

    /**
     * truncation current BigFloat
     *
     * @param length constain the size under the decimal point
     */
    bool BigFloat::truncation(int64_t length)
    {
        length = std::max(length, (int64_t)-1);
        length = (length == -1) ? ((*this).precision + 1) : length;

        if ((*this).exp >= 0)
        {
            // no need to truncation, *this is way too large
            return true;
        }

        int64_t dec_size = -1 * (*this).exp;
        if (dec_size < length)
        {
            // no need to truncation, condition fit
            return true;
        }

        int64_t dec_adj_size = std::min((int64_t)(*this).digits.size(), length);
        dec_adj_size = dec_size - dec_adj_size;

        // remove front digit (truncation)
        while (!(*this).digits.empty() && dec_adj_size > 0)
        {
            (*this).digits.erase((*this).digits.begin());
            (*this).exp++;
            dec_adj_size--;
        }

        // check size
        if ((*this).digits.empty())
        {
            if (DEBUGGER)
            {
                // if not all digits are removed, but reset occurse.
                std::cout << "ALERT: BigFloat vector truncation error." << std::endl;
            }

            // if this BigFloat is zero just reset
            *this = BigFloat((*this).precision, (uint32_t)0);
            return false;
        }

        return true;
    }

    /**
     * the vector digits example, s_front = 4; s_back = 3.
     * before: x x x 3 2 1 0 x x x x
     * after : o o o 3 2 1 0 o o o o (o fill with uint32_t(0))
     *
     * @param s_front fill front size
     * @param s_back fill back size
     */
    bool BigFloat::shrink_to_fit(int64_t s_front, int64_t s_back)
    {
        bool zero_check = (*this).zero_check();

        if (s_front == -1 && s_back == -1)
        {
            // check front zero
            while (!(*this).digits.empty() && (*this).digits.front() == 0)
            {
                (*this).digits.erase((*this).digits.begin());
                (*this).exp++;
            }

            // check back zero
            while (!(*this).digits.empty() && (*this).digits.back() == 0)
            {
                // (*this).digits.erase((*this).digits.end() - 1);
                (*this).digits.pop_back();
            }
        }
        else
        {
            // fill front with zero
            for (int64_t i = 0; i < s_front; i++)
            {
                (*this).digits.insert((*this).digits.begin(), (uint32_t)0);
                (*this).exp--;
            }

            // fill back with zero
            for (int64_t i = 0; i < s_back; i++)
            {
                // (*this).digits.insert((*this).digits.end(), (uint32_t)0);
                (*this).digits.push_back((uint32_t)0);
            }
        }

        // (*this).digits.shrink_to_fit();

        // check size
        if ((*this).digits.empty())
        {
            if (DEBUGGER && zero_check)
            {
                // if not all digits are zero, but reset occurse.
                std::cout << "ALERT: BigFloat vector shrink error." << std::endl;
            }

            // if this BigFloat is zero just reset
            *this = BigFloat((*this).precision, (uint32_t)0);
            return false;
        }

        return true;
    }

    bool BigFloat::shrink_to_fit_size(BigFloat &lhs_n_, BigFloat &rhs_n_, int64_t &lhs_n_s_front, int64_t &lhs_n_s_back, int64_t &rhs_n_s_front, int64_t &rhs_n_s_back) const
    {
        // compute s_front
        if (lhs_n_.exp > rhs_n_.exp)
        {
            lhs_n_s_front = lhs_n_.exp - rhs_n_.exp;
        }
        else if (lhs_n_.exp < rhs_n_.exp)
        {
            rhs_n_s_front = rhs_n_.exp - lhs_n_.exp;
        }

        // compute s_back
        if (((int64_t)lhs_n_.digits.size() + lhs_n_.exp) > ((int64_t)rhs_n_.digits.size() + rhs_n_.exp))
        {
            rhs_n_s_back = ((int64_t)lhs_n_.digits.size() + lhs_n_.exp) - ((int64_t)rhs_n_.digits.size() + rhs_n_.exp);
        }
        else if (((int64_t)lhs_n_.digits.size() + lhs_n_.exp) < ((int64_t)rhs_n_.digits.size() + rhs_n_.exp))
        {
            lhs_n_s_back = ((int64_t)rhs_n_.digits.size() + rhs_n_.exp) - ((int64_t)lhs_n_.digits.size() + lhs_n_.exp);
        }

        return true;
    }

    /**
     * convert BigFloat to string
     *
     * @return the string of BigFloat
     */
    std::string BigFloat::to_string() const
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        if (DEBUGGER)
        {
            std::cout << "\n==============\n\n";
            std::cout << "to_string\n";
            std::cout << "sign: " << (*this).sign << "\n";
            std::cout << "exp: " << (*this).exp << "\n";
            for (int iter = 0; iter < (int)(*this).digits.size(); iter++)
            {
                std::cout << "digits: " << iter << " " << (*this).digits[iter] << "\n";
            }
            std::cout << "\n==============\n\n";
        }

        int64_t size = (int64_t)(*this).digits.size();
        if (size == 0)
        {
            return "0";
        }

        int64_t deciaml_idx = (*this).exp;
        std::string str = "";

        if (deciaml_idx + size <= 0)
        {
            str += "0.";
            while (deciaml_idx + size < 0)
            {
                char tmp[10];
                sprintf(tmp, "%09d", 0);
                str += tmp;
                deciaml_idx += 1;
            }
        }

        for (uint32_t i = size; i != 0; i--)
        {
            if (((i - 1) == size - 1) && ((*this).exp + size > 0))
            {
                str += std::to_string((*this).digits[(i - 1)]);
            }
            else
            {
                char tmp[10];
                sprintf(tmp, "%09d", (*this).digits[(i - 1)]);
                str += tmp;
            }

            if ((i - 1) != 0 && (i - 1) + (*this).exp == 0)
            {
                str += ".";
            }
        }

        if (deciaml_idx >= 0)
        {
            while (deciaml_idx > 0)
            {
                char tmp[10];
                sprintf(tmp, "%09d", 0);
                str += tmp;
                deciaml_idx -= 1;
            }
            str += ".0";
        }

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_t = t2 - t1;
        if (TIMER)
        {
            std::cout << "to_string take " << diff_t.count() << " sec." << std::endl;
        }

        return (*this).sign ? str : (std::string("-") + str);
    }

}
