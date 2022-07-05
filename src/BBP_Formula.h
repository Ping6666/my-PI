#ifndef _BBP_FORMULA_H_
#define _BBP_FORMULA_H_

#include "BigFloat.h"
#include "Printer.h"

namespace _my_PI_
{

    struct BBP_struct
    {
        double d;
        int s, b, m;
        std::vector<int> A;

        BBP_struct(double d_, int s_, int b_, int m_, std::vector<int> A_)
            : d(d_), s(s_), b(b_), m(m_)
        {
            A = A_;
        }
    };

    /* WorkHouse */

    void BBP_WorkHouse(int, int);

    /* BBP (Bailey–Borwein–Plouffe) */

    void BBP(int, int, int, int, std::vector<int>, int, BigFloat &);

}

#endif
