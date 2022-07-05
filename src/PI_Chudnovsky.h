#ifndef _PI_Chudnovsky_H_
#define _PI_Chudnovsky_H_

#include "BigFloat.h"
#include "Printer.h"

namespace _my_PI_
{

    /* WorkHouse */

    void pi_Chudnovsky_BS_WorkHouse(int d, int p);
    void pi_Chudnovsky_WorkHouse(int d, int p);

    /* PI Chudnovsky with binary splitting */

    int pi_Chudnovsky_BS(int, BigFloat &);
    int BS_worker(int, BigFloat &, BigFloat &, BigFloat &, int64_t, int64_t);

    /* PI Chudnovsky */

    int pi_Chudnovsky(int, BigFloat &);

}

#endif
