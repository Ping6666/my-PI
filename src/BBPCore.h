#ifndef _BBP_Core_H_
#define _BBP_Core_H_

#include <cstdint>

namespace _my_PI_
{

    /* DEBUGGER */

    const bool DEBUGGER = false;
    // const bool DEBUGGER = true;

    /* TIMER */

    // const bool TIMER = false;
    const bool TIMER = true;

    /* BASE CONST */

    const double pi_ = 3.14159265358979323846; // M_PI in cmath

    const uint32_t uint32_t_size = 1000000000;
    const uint32_t uint32_t_cpl = 999999999;

}

#endif
