#ifndef PHYSICAL_VALUES_SI_H
#define PHYSICAL_VALUES_SI_H

#include <cmath>
#include "Global_Info.h"
#include "Template_Impl/Compile_Time_Functions.hpp"

namespace pvsi_nms
{
    using namespace ctf_nms;
    
    constexpr global_nms::default_data_type c {299792458};
    constexpr global_nms::default_data_type h {6.626070040e-34};
    constexpr global_nms::default_data_type hbar {h/(2*M_PI)};
    constexpr global_nms::default_data_type mu_0 {(4.0e-7)*M_PI};
    constexpr global_nms::default_data_type k_B {1.38064852e-23};
    
    constexpr global_nms::default_data_type q_fund {1.6021766208e-19};
    constexpr global_nms::default_data_type alpha_FS {7.2973525664e-3};
    
    constexpr global_nms::default_data_type mass_e {9.10938356e-31};
    constexpr global_nms::default_data_type mass_p {1.672621898e-27};
    constexpr global_nms::default_data_type red_mass_e {mass_e*mass_p/(mass_e + mass_p)};
    
    constexpr global_nms::default_data_type a0 {hbar/(red_mass_e*c*alpha_FS)};
    
    constexpr global_nms::default_data_type mu_B {9.274009994e-24};
    constexpr global_nms::default_data_type g_L {1 - (mass_e/mass_p)};
    constexpr global_nms::default_data_type g_positron_bare {2.00231930436182};
    constexpr global_nms::default_data_type g_positron(unsigned N)
    {
        return g_positron_bare*
        (1 - (ct_pow(alpha_FS/N, 2)/3) + ((alpha_FS*ct_pow(alpha_FS/N, 3)/2)*((1./(2.*N)) - (2./3.)))
         + (alpha_FS*ct_pow(alpha_FS/N, 2)/(4*M_PI)) + (ct_pow(alpha_FS/N, 2)*mass_e/(2*mass_p)));
    }
    
    constexpr global_nms::default_data_type mu_N {5.050783699e-27};
    constexpr global_nms::default_data_type g_pbar_bare {-5.585694702};
    constexpr global_nms::default_data_type g_pbar(unsigned N)
    {
        return g_pbar_bare*(1 - (ct_pow(alpha_FS/N, 2)/3) + ((ct_pow(alpha_FS/N, 2)*mass_e/(6*mass_p))*((3 + (4*1.793))/(1 + 1.793))));
    }
    
    /*
     Hydrogen energy levels and splittings
     See https://arxiv.org/pdf/1107.3101.pdf and
     "A critical compilation of experimental data on spectral lines and energy levels of hydrogen, deuterium, and tritium" by A.E Kramida
     */
    inline global_nms::default_data_type E_centroid(int N, int L, int half_J)
    {
        std::array<int, 3> st_arr {{N, L, half_J}};
        
        if(st_arr == std::array<int, 3>{{1, 0, 1}}) {return 0;}
        else if(st_arr == std::array<int, 3>{{2, 0, 1}}) {return (E_centroid(1, 0, 1) + (2466061413187035*h));}
        else if(st_arr == std::array<int, 3>{{2, 1, 1}}) {return (E_centroid(2, 0, 1) - (1057847000*h));}
        else if(st_arr == std::array<int, 3>{{2, 1, 3}}) {return (E_centroid(2, 1, 1) + (10969130000*h));}
        else {return 0;}
    }
    
    constexpr global_nms::default_data_type S_HFS(unsigned N)
    {
        if(1 == N) {return 1420405751.7667*h;}
        else if(2 == N) {return 177556834.3*h;}
        else {return 0;}
    }
}

#endif /* PHYSICAL_VALUES_SI_H */
