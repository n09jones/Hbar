#ifndef COMPILE_TIME_FUNCTIONS_HPP
#define COMPILE_TIME_FUNCTIONS_HPP

namespace ctf_nms
{
    template <class U>
    constexpr U ct_min(U x, U y) {return ((x < y) ? x : y);}
    
    template <class U>
    constexpr U ct_max(U x, U y) {return ((x > y) ? x : y);}
    
    template <class U>
    constexpr U ct_abs(U x) {return ((x >= 0) ? x : -x);}
    
    constexpr double ct_pow(double base, int exp, double acc = 1)
    {
        if(exp < 0) {return ct_pow(base, exp+1, acc/base);}
        else if(exp > 0) {return ct_pow(base, exp-1, acc*base);}
        else {return acc;}
    }
}

#endif /* COMPILE_TIME_FUNCTIONS_HPP */
