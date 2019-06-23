#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

int main()
{
    size_t len {1000000};
    
    mem_arr_nms::mem_arr<> uniform_rand {len};
    mem_arr_nms::mem_arr<> gaussian {len};
    mem_arr_nms::mem_arr<> ellipsoid {2*len};
    
    auto func_gaussian {[](auto x) {return std::exp(-std::pow((x[0] - 3.14)/2.82, 2)/2);}};
    auto func_ellipsoid {[](auto x) {return ((17.5*x[0]*x[0]) + (7.2*x[1]*x[1]));}};
    
    for(size_t idx = 0; idx < len; ++idx)
    {
        uniform_rand[idx] = distr_gen::rand();
        gaussian[idx] = distr_gen::rand<1>(func_gaussian, {{{{-10, 10}}}}, 1)[0];
        
        auto rand_ell_coords {distr_gen::rand<2>(func_ellipsoid, {{{{-1, 1}}, {{-2, 2}}}}, (17.5 + (4*7.2)) + 1)};
        ellipsoid[(2*idx)+0] = rand_ell_coords[0];
        ellipsoid[(2*idx)+1] = rand_ell_coords[1];
    }
    
    auto dir {global_nms::quick_test_dir / boost::filesystem::path{"tmp"}};
    
    print_to_file(uniform_rand, (dir / boost::filesystem::path{"uniform_rand.bin"}));
    print_to_file(gaussian, (dir / boost::filesystem::path{"gaussian_rand.bin"}));
    print_to_file(ellipsoid, (dir / boost::filesystem::path{"ellipsoid_rand.bin"}));
    
    return 0;
}
