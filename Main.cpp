#include "src/PI_Chudnovsky.h"
#include "src/BBP_Formula.h"

using namespace _my_PI_;

int main()
{
    int64_t digits = 50000;
    int64_t precision = digits / 9;

    std::cout << "\nStart\n\n";
    auto t1 = std::chrono::high_resolution_clock::now();

    pi_Chudnovsky_BS_WorkHouse(digits, precision);
    // digits: 1000, loop times: 8, time spend: 1.02016 sec.

    // pi_Chudnovsky_WorkHouse(digits, precision);
    // digits: 1000, loop times: 71, time spend: 1.7704 sec.

    // BBP_WorkHouse(digits, precision);
    // digits: 1000, loop times: 999, time spend: 347.726  sec.

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_t = t2 - t1;
    if (true || TIMER)
    {
        std::cout << "all work take " << diff_t.count() << " sec." << std::endl;
    }

    std::cout << "\nEND\n";

    return 0;
}
