#include "PI_Chudnovsky.h"

namespace _my_PI_
{

    /* WorkHouse */

    void pi_Chudnovsky_BS_WorkHouse(int d, int p)
    {
        BigFloat PI_n;
        int times = pi_Chudnovsky_BS(p, PI_n);
        std::string PI_str = PI_n.to_string();

        printByLength(PI_str, 0);
        printSetting(d, p, times);
    }

    void pi_Chudnovsky_WorkHouse(int d, int p)
    {
        BigFloat PI_n;
        int times = pi_Chudnovsky(p, PI_n);
        std::string PI_str = PI_n.to_string();

        printByLength(PI_str, 0);
        printSetting(d, p, times);
    }

    /* PI Chudnovsky with binary splitting */

    /*

    // p = 1000, ratio = 4
    //  time: 1.02016 sec, layer: 8.

    //  p = 10000, ratio = 4
    // time: 16.9836 sec, layer: 12.

    // p = 50000, ratio = 4
    // time: 229.893 sec, layer: 14.

    // p = 100000, ratio = 4, correct: 51 % (maybe is overflow)
    // time: 984.935 sec, layer: 15.

    // p = 500000, ratio = 4, correct: 10 % (maybe is overflow)
    // time: 17015.4 sec, layer: 17.

    */

    /**
     * caller for PI_BS.
     *
     * BUG here!! Do not know why the ratio is diff, when p is diff.
     * If ratio is given too small will output the wrong answer.
     */
    int pi_Chudnovsky_BS(int p, BigFloat &pi_n)
    {
        int ratio = 4;
        BigFloat P, Q, T;
        int times = BS_worker(p, P, Q, T, 0, (uint64_t)(p + 1));

        if (true || DEBUGGER)
        {
            std::cout << "\n==============\n\n";
            std::cout << "PI final part\n";
            std::cout << "P " << P.to_string() << std::endl;
            std::cout << "Q " << Q.to_string() << std::endl;
            std::cout << "T " << T.to_string() << std::endl;
            std::cout << "\n==============\n\n";
        }

        BigFloat const_numerator = BigFloat(ratio * p, (uint32_t)426880) / BigFloat(ratio * p, (uint32_t)10005).invSqrt();
        BigFloat const_denominator = BigFloat(ratio * p, (uint32_t)13591409);

        pi_n = ((const_numerator * Q) / (const_denominator * Q + T));

        pi_n.set_precision(p);
        pi_n.truncation();

        return times;
    }

    /**
     * compute PI by Chudnovsky algorithm with binary splitting (BS).
     * Since B will always be 1, therefore excluded from the recursion.
     *
     * @param p is precision
     * @param a is left bound
     * @param b is right bound
     */
    int BS_worker(int p, BigFloat &P, BigFloat &Q, BigFloat &T, int64_t a, int64_t b)
    {
        if (b - a == 1)
        {
            int i = b;
            const BigFloat bf_i = BigFloat(p, std::to_string(i));

            if (i == 0)
            {
                P = BigFloat(p, (uint32_t)1);
                Q = BigFloat(p, (uint32_t)1);
            }
            else
            {
                P = (((BigFloat(p, (uint32_t)2) * bf_i) - BigFloat(p, (uint32_t)1)) *
                     ((BigFloat(p, (uint32_t)6) * bf_i) - BigFloat(p, (uint32_t)1)) *
                     ((BigFloat(p, (uint32_t)6) * bf_i) - BigFloat(p, (uint32_t)5)));
                Q = bf_i.pow(3) * BigFloat(p, "10939058860032000");
            }

            BigFloat A = BigFloat(p, "13591409") + BigFloat(p, "545140134") * bf_i;
            T = A * P;

            if (i % 2 == 1)
            {
                T.set_sign(false);
            }

            return 1;
        }

        int64_t m = (a + b) / 2; // middle point
        BigFloat P_a_m, P_m_b, Q_a_m, Q_m_b, T_a_m, T_m_b;
        int i_a_m = BS_worker(p, P_a_m, Q_a_m, T_a_m, a, m); // (a, m)
        int i_m_b = BS_worker(p, P_m_b, Q_m_b, T_m_b, m, b); // (m, b)

        P = P_a_m * P_m_b;
        Q = Q_a_m * Q_m_b;
        T = Q_m_b * T_a_m + P_a_m * T_m_b;

        int layer = std::max(i_a_m, i_m_b) + 1;
        std::cout << "\nPI summation part, current layer: " << layer << "\n\n";

        return layer;
    }

    /* PI Chudnovsky */

    /**
     * compute PI by Chudnovsky algorithm,
     * this will take a huge amount of computation time.
     *
     * @param p is precision
     */
    int pi_Chudnovsky(int p, BigFloat &pi_n)
    {
        pi_n = BigFloat(p, (uint32_t)0);

        BigFloat a = BigFloat(p, (uint32_t)0);
        BigFloat b = BigFloat(p, (uint32_t)0);

        BigFloat term_a = BigFloat(p, (uint32_t)1);
        BigFloat term_b = term_a * BigFloat(p, (uint32_t)0);

        a += term_a;
        b += term_b;

        int i = 0; // loop_time

        BigFloat const_num = BigFloat(p, (uint32_t)24, false) / BigFloat(p, (uint32_t)640320).pow(3);

        // loop time in (p / 2 + 1, p] will match correct answer
        while (true)
        {
            i += 1;

            /* compute state diff */

            BigFloat state_diff_num = ((((BigFloat(p, (uint32_t)2) * BigFloat(p, std::to_string(i))) - BigFloat(p, (uint32_t)1)) *
                                        ((BigFloat(p, (uint32_t)6) * BigFloat(p, std::to_string(i))) - BigFloat(p, (uint32_t)1)) *
                                        ((BigFloat(p, (uint32_t)6) * BigFloat(p, std::to_string(i))) - BigFloat(p, (uint32_t)5))) /
                                       BigFloat(p, std::to_string(i)).pow(3));

            /* compute state */

            term_a *= (const_num * state_diff_num);
            term_b = term_a * BigFloat(p, std::to_string(i));

            /* summation */

            a += term_a;
            b += term_b;

            /* status print */

            std::cout << "\nPI summation part, current idx: " << i << "\n\n";
            if (DEBUGGER)
            {
                std::cout << "\n==============\n\n";
                std::cout << "term_a: " << term_a.to_string() << std::endl;
                std::cout << "term_b: " << term_b.to_string() << std::endl;
                std::cout << "a: " << a.to_string() << std::endl;
                std::cout << "b: " << b.to_string() << std::endl;
                std::cout << "\n==============\n\n";
            }

            /* check fitting the precision or upper bound of loop times */

            if (term_a.sum_prec_check() || i > p)
            {
                break;
            }
        }

        if (true || DEBUGGER)
        {
            std::cout << "\n==============\n\n";
            std::cout << "PI final part\n";
            std::cout << "a " << a.to_string() << std::endl;
            std::cout << "b " << b.to_string() << std::endl;
            std::cout << "\n==============\n\n";
        }

        pi_n = ((BigFloat(p, (uint32_t)426880) * (BigFloat(p, (uint32_t)1) / BigFloat(p, (uint32_t)10005).invSqrt())) / ((BigFloat(p, (uint32_t)13591409) * a) + (BigFloat(p, (uint32_t)545140134) * b)));
        pi_n.truncation();

        return i;
    }

}
