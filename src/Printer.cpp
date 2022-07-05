#include "Printer.h"

namespace _my_PI_
{

    /* printer */

    /**
     * print the computational result of pi
     */
    void printByLength(std::string str, int length)
    {
        length = std::max(length, 0);
        if (length == 0)
        {
            std::cout << "\n==============\n\n";
            std::cout << "PI result\n";
            std::cout << str << std::endl;
            std::cout << "\n==============\n\n";
            return;
        }

        /* find decimal point */

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

        /* fit size */

        int append_l_f = (length - ((int)dec_f.size() % length)) % length;
        for (int i = 0; i < append_l_f; i++)
        {
            dec_f = "0" + dec_f;
        }
        int append_l_b = (length - ((int)dec_b.size() % length)) % length;
        for (int i = 0; i < append_l_b; i++)
        {
            dec_b = dec_b + "0";
        }

        /* print */

        std::cout << "\n==============\n\n";
        std::cout << "PI result\n";
        for (int i = 0; i < (int)dec_f.size(); i += 50)
        {
            std::cout << dec_f.substr(i, 50) << std::endl;
        }
        std::cout << "." << std::endl;
        for (int i = 0; i < (int)dec_b.size(); i += 50)
        {
            std::cout << dec_b.substr(i, 50) << std::endl;
        }
        std::cout << "\n==============\n\n";
    }

    void printBBP(std::string str)
    {
        std::cout << "\n==============\n\n";
        std::cout << "BBP result\n";
        std::cout << str << std::endl;
        std::cout << "\n==============\n\n";
    }

    /**
     * output the compute details
     *
     * @param d digits
     * @param p precision
     * @param t loop time
     */
    void printSetting(int64_t d, int64_t p, int t)
    {
        std::cout << "\n==============\n\n";
        std::cout << "setting\n";
        std::cout << "digits: " << d << std::endl;
        std::cout << "precision: " << p << std::endl;
        std::cout << "loop time: " << t << std::endl;
        std::cout << "\n==============\n\n";
    }

}
