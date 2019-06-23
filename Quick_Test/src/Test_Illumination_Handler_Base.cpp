#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using boost::filesystem::path;

int main()
{
    auto opts_fn {global_nms::quick_test_dir / path{"Options_Files"} / path{"Illumination_Opts_File.txt"}};
    
    ill_hand_nms::illumination_handler<pdef_nms::illum_type::LyAlph, emq_nms::prompt_EM_quants> ih_1;
    ih_1.fill(opts_fn);
    
    ih_1.update({{-0.003, 100e-6, 0}, {10, 20, 30}});
    
    std::cout << "FWHM : " << ih_1.FWHM << std::endl;
    std::cout << "det_0 : " << ih_1.det_0 << std::endl;
    std::cout << "freq_drive : " << ih_1.freq_drive << std::endl;
    
    std::cout << "prob_decay_rate : " << ih_1.prob_decay_rate << std::endl << std::endl;
    
    std::cout << std::endl;
    
    for(size_t idx = 0; idx < ih_1.Rabi_freq_Doppler_by_path.size(); ++idx) {
        std::cout << "Rabi freq " << idx << " : " << std::get<0>(ih_1.Rabi_freq_Doppler_by_path[idx]) << std::endl;
        std::cout << "Detuning " << idx << " : " << std::get<1>(ih_1.Rabi_freq_Doppler_by_path[idx]) << std::endl;
        std::cout << std::endl;
    }
    
    ill_hand_nms::illumination_handler<pdef_nms::illum_type::TwoPh, emq_nms::prompt_EM_quants> ih_2;
    ih_2.fill(opts_fn);
    
    ih_2.update({{-0.003, 100e-6, 0}, {10, 20, 30}});
    
    std::cout << "FWHM : " << ih_2.FWHM << std::endl;
    std::cout << "det_0 : " << ih_2.det_0 << std::endl;
    std::cout << "freq_drive : " << ih_2.freq_drive << std::endl;
    
    std::cout << "prob_decay_rate : " << ih_2.prob_decay_rate << std::endl << std::endl;
    
    std::cout << std::endl;
    
    for(size_t idx = 0; idx < ih_2.Rabi_freq_Doppler_by_path.size(); ++idx) {
        std::cout << "Rabi freq " << idx << " : " << std::get<0>(ih_2.Rabi_freq_Doppler_by_path[idx]) << std::endl;
        std::cout << "Detuning " << idx << " : " << std::get<1>(ih_2.Rabi_freq_Doppler_by_path[idx]) << std::endl;
        std::cout << std::endl;
    }
    
    return 0;
}
