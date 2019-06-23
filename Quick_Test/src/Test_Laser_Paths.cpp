#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using boost::filesystem::path;

template <class U>
void print_array(const U& arr)
{
    std::cout << "(";
    for(size_t idx = 0; idx < arr.size(); ++idx)
    {
        if(idx) {std::cout << ", ";}
        std::cout << arr[idx];
    }
    std::cout << ")";
}

int main()
{
    las_path_nms::path_mngr pm {global_nms::quick_test_dir / path{"Options_Files"} / path{"Laser_Path_Manager_Opts_File.txt"}};
    
    std::cout << "Paths -- Expected : 2, Actual : " << pm.paths.size() << std::endl;
    std::cout << std::endl;
    
    
    
    std::cout << "*** PATH 1 ***" << std::endl;
    std::cout << "one_way_P -- Expected : 1.3e-9, Actual : " << pm.paths[0].one_way_P << std::endl;
    std::cout << "unit_wave_vector -- Expected : (1, 0, 0), Actual : ";
    print_array(pm.paths[0].unit_wave_vector); std::cout << std::endl;
    std::cout << "focus -- Expected : (0, 0, 0), Actual : ";
    print_array(pm.paths[0].focus); std::cout << std::endl;
    std::cout << "waist -- Expected : 250e-6, Actual : " << pm.paths[0].waist << std::endl;
    std::cout << "pol -- Expected : ((0, 0), (0, 0), (0, 1)), Actual : ";
    print_array(pm.paths[0].pol); std::cout << std::endl;
    
    std::cout << std::endl << std::endl;
    
    std::cout << "*** PATH 2 ***" << std::endl;
    std::cout << "one_way_P -- Expected : 15, Actual : " << pm.paths[1].one_way_P << std::endl;
    std::cout << "unit_wave_vector -- Expected : (0.259, -0.519, 0.815), Actual : ";
    print_array(pm.paths[1].unit_wave_vector); std::cout << std::endl;
    std::cout << "focus -- Expected : (0, 0, 0.015), Actual : ";
    print_array(pm.paths[1].focus); std::cout << std::endl;
    std::cout << "waist -- Expected : 0.003, Actual : " << pm.paths[1].waist << std::endl;
    std::cout << "pol -- Expected : ((0.707, 0), (0, 0.707), (0, 0)), Actual : ";
    print_array(pm.paths[1].pol); std::cout << std::endl;
    
    std::cout << std::endl << std::endl;
    
    bvec_nms::basic_vec<> pos_1 {-0.00339837, 0.000639619, 0.000407401};
    if(!pm.in_beam(pos_1, 0.0015)) {
        throw las_path_nms::las_path_exc{"Unexpected in-beam test evaluation"};
    }
    
    bvec_nms::basic_vec<> pos_2 {-0.00339837, 0.00139872, 0.000890902};
    if(!pm.in_beam(pos_2, 0.001)) {
        throw las_path_nms::las_path_exc{"Unexpected in-beam test evaluation"};
    }
    
    if(pm.in_beam(pos_2, 0.0003)) {
        throw las_path_nms::las_path_exc{"Unexpected in-beam test evaluation"};
    }
    
    std::cout << "*** INTENSITY TEST ***" << std::endl;
    bvec_nms::basic_vec<> pos_3 {-0.00339837, 0.0000492113, 0.0000313448};
    std::cout << "Laser 1 Intensity -- Expected : 0.011875, Actual : " << pm.paths[0].intensity(pos_3) << std::endl;
    std::cout << "Laser 2 Intensity -- Expected : 0.706474, Actual : " << pm.paths[1].intensity(pos_3) << std::endl;
    
    return 0;
}
