#include "BBP_Formula.h"

namespace _my_PI_
{

    /* WorkHouse */

    void BBP_WorkHouse(int d, int p)
    {
        /* PI */

        struct BBP_struct BBP_F = BBP_struct(1, 1, 16, 8, {4, 0, 0, -2, -1, -1}); // PI
        // struct BBP_struct BBP_F = BBP_struct(0.5, 1, 2, 1, {1}); // ln(2)

        BigFloat bbp_n = BigFloat(p, (uint32_t)0);

        // very very very slow
        // not sure the k upper bound is right or not
        for (int k = 0; k < p * 9 * 16 / BBP_F.b; k++)
        {
            BigFloat bbp_n_part_k = BigFloat(p, (uint32_t)0);
            BBP(p, BBP_F.s, BBP_F.b, BBP_F.m, BBP_F.A, k, bbp_n_part_k);
            bbp_n += bbp_n_part_k;
        }

        const BigFloat bf_d = BigFloat(p, std::to_string(BBP_F.d));
        bbp_n *= bf_d;
        bbp_n.truncation();

        printBBP(bbp_n.to_string());
        printSetting(d, p, p * 9 * 16 / BBP_F.b);
    }

    /* BBP (Bailey–Borwein–Plouffe) */

    /**
     * this will only compute one item in the given BBP (see the def. in wikipedia)
     *
     * @param p precision
     * @param s exponent (aka. significant part)
     * @param b exponent
     * @param m significand of current k
     * @param A coefficients in numerator w.r.t denominator
     * @param k current index k
     */
    void BBP(int p, int s, int b, int m, std::vector<int> A, int k, BigFloat &bbp_n_part_k)
    {
        bbp_n_part_k = BigFloat(p, (uint32_t)0);

        /* const */

        const BigFloat bf_m = BigFloat(p, std::to_string(m));
        const BigFloat bf_k = BigFloat(p, std::to_string(k));
        const BigFloat bf_b_k = BigFloat(p, std::to_string(b)).pow(k);

        for (int i = 0; i < (int)A.size(); i++)
        {
            // current idx = i + 1
            BigFloat term_i = BigFloat(p, (uint32_t)1);
            const BigFloat bf_idx = BigFloat(p, std::to_string(i + 1));
            const BigFloat const_n = bf_m * bf_k + bf_idx; // (m * k + i)

            for (int idx_s = 0; idx_s < s; idx_s++)
            {
                term_i /= const_n; // 1 / ((m * k + i) ^ s)
            }

            bool sign = (A[i] >= 0) ? true : false;
            int num = (A[i] >= 0) ? A[i] : -1 * A[i];
            const BigFloat bf_a_i = BigFloat(p, std::to_string(num), sign);

            term_i *= bf_a_i; // a_i  / ((m * k + i) ^ s)
            bbp_n_part_k += term_i;

            /* status print */

            if (true || DEBUGGER)
            {
                std::cout << "\n==============\n\n";
                std::cout << "bf_a_i: " << bf_a_i.to_string() << std::endl;
                std::cout << "const_n: " << const_n.to_string() << std::endl;
                std::cout << "term_i: " << term_i.to_string() << std::endl;
                std::cout << "\n==============\n\n";
            }
        }

        bbp_n_part_k /= bf_b_k; // [sum{0 to size of A} a_i  / ((m * k + i) ^ s)] / (b ^ k)

        std::cout << "\nBBP summation part, current idx: " << k << "\n\n";
        std::cout << "bbp_n_part_k: " << bbp_n_part_k.to_string() << std::endl;
    }

}
