#ifndef RANDOM_DISTRIBUTIONS_H
#define RANDOM_DISTRIBUTIONS_H

#include <random>
#include "ATRAP_base.h"

namespace distr_gen
{
    extern std::minstd_rand gen;
    extern std::uniform_real_distribution<global_nms::default_data_type> distr;
    
    inline global_nms::default_data_type rand() {return distr(gen);}
    
    template <size_t N, class Func>
    auto inline rand(Func&& func, const std::array<std::array<global_nms::default_data_type, 2>, N>& x_lims, global_nms::default_data_type y_bd)
    {
        std::array<global_nms::default_data_type, N> x;
        
        do {
            for(size_t dim = 0; dim < N; ++dim) {
                x[dim] = x_lims[dim][0] + ((x_lims[dim][1] - x_lims[dim][0])*distr(gen));
            }
        } while(func(x) < y_bd*distr(gen));
        
        return x;
    }
}

#endif /* RANDOM_DISTRIBUTIONS_H */

