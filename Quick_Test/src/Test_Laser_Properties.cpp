#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using boost::filesystem::path;

int main()
{
    las_props_nms::laser_properties lp_1 {global_nms::quick_test_dir / path{"Options_Files"} / path{"Laser_Properties_Opts_File_1.txt"}};
    las_props_nms::laser_properties lp_2;
    
    lp_2.fill_laser_properties(global_nms::quick_test_dir / path{"Options_Files"} / path{"Laser_Properties_Opts_File_2.txt"});
    
    std::cout << std::boolalpha;
    
    std::cout << "*** FIRST OPTIONS FILE ***" << std::endl;
    std::cout << "det -- Expected : -34.0e6, Actual : " << lp_1.det << std::endl;
    std::cout << "B_0_det -- Expected : 0.0, Actual : " << lp_1.B_0_det << std::endl;
    std::cout << "FWHM -- Expected : 247, Actual : " << lp_1.FWHM << std::endl;
    std::cout << "pulsed? -- Expected : true, Actual : " << lp_1.pulsed << std::endl;
    std::cout << "pulse_len -- Expected : 150e-9, Actual : " << lp_1.pulse_len << std::endl;
    std::cout << "rep_rate -- Expected : 30, Actual : " << lp_1.rep_rate << std::endl;
    
    std::cout << std::endl << std::endl;
    
    std::cout << "*** SECOND OPTIONS FILE ***" << std::endl;
    std::cout << "det -- Expected : 78.0e9, Actual : " << lp_2.det << std::endl;
    std::cout << "B_0_det -- Expected : 0.78, Actual : " << lp_2.B_0_det << std::endl;
    std::cout << "FWHM -- Expected : 1.e3, Actual : " << lp_2.FWHM << std::endl;
    std::cout << "pulsed? -- Expected : false, Actual : " << lp_2.pulsed << std::endl;
    
    std::cout << std::endl << std::endl;
    
    return 0;
}
