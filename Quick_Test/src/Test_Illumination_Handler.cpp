#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using boost::filesystem::path;

struct observer
{
    double t0;
    std::vector<double> tP;
    
    void operator() (const typename ill_hand_nms::illumination_handler<pdef_nms::illum_type::LyAlph>::d_mat_t& dm, double t)
    {
        tP.push_back(t0 + t);
        tP.push_back(dm[ill_hand_nms::illumination_handler<pdef_nms::illum_type::LyAlph>::P]);
//        if(!(tP.size() % 20000)) {std::cout << "time, P : " << t << ", " << tP.back() << ", " << tP.size() << std::endl;}
    }
};

int main()
{
    try
    {
        auto opts_fn {global_nms::quick_test_dir / path{"Options_Files"} / path{"Illumination_Opts_File_2.txt"}};
        
        double dt {1.e-6};
        double ddt {dt/1000};
        pdef_nms::state_t pos_vel {{0, 0, 0}, {30, -20, 10}};
        
        ill_hand_nms::illumination_handler<pdef_nms::illum_type::LyAlph, emq_nms::one_EM_quants> ih_1;
        ih_1.fill(opts_fn);
        ih_1.initialize();
        observer obs;
        obs.t0 = 0.0;
        ih_1.integration_step(pos_vel, dt, [&obs](auto x, auto y) {obs(x, y);});
 
        ill_hand_nms::illumination_handler<pdef_nms::illum_type::LyAlph, emq_nms::one_EM_quants> ih_2;
        ih_2.fill(opts_fn);
        ih_2.initialize();
        ih_2.decay_threshold = -1;
        observer obs_2;
        for(double t = 0.0; t < dt; t += ddt) {
            obs_2.t0 = t;
            ih_2.integration_step(pos_vel, ddt, [&obs_2](auto x, auto y) {obs_2(x, y);});
        }
        
        auto arr_fn {global_nms::quick_test_dir / path{"tmp"} / path{"data.bin"}};
        auto arr_fn_2 {global_nms::quick_test_dir / path{"tmp"} / path{"data_2.bin"}};
        auto Rabi_det_fn {global_nms::quick_test_dir / path{"tmp"} / path{"params.bin"}};
        
        remove(arr_fn);
        remove(arr_fn_2);
        remove(Rabi_det_fn);
        
        mem_arr_nms::mem_arr<> arr {obs.tP.size()};
        std::copy(obs.tP.begin(), obs.tP.end(), arr.begin());
        print_to_file(arr, arr_fn);
        
        mem_arr_nms::mem_arr<> arr_2 {obs_2.tP.size()};
        std::copy(obs_2.tP.begin(), obs_2.tP.end(), arr_2.begin());
        print_to_file(arr_2, arr_fn_2);
        
        mem_arr_nms::mem_arr<> Rabi_det_arr {5*ih_1.Rabi_freq_Doppler_by_path.size()};
        for(size_t idx = 0; idx < ih_1.Rabi_freq_Doppler_by_path.size(); ++idx)
        {
            Rabi_det_arr[(5*idx) + 0] = std::get<0>(ih_1.Rabi_freq_Doppler_by_path[idx]).real();
            Rabi_det_arr[(5*idx) + 1] = std::get<0>(ih_1.Rabi_freq_Doppler_by_path[idx]).imag();
            Rabi_det_arr[(5*idx) + 2] = std::get<1>(ih_1.Rabi_freq_Doppler_by_path[idx]);
            Rabi_det_arr[(5*idx) + 3] = ih_1.FWHM;
            Rabi_det_arr[(5*idx) + 4] = ih_1.prob_decay_rate;
        }
        print_to_file(Rabi_det_arr, Rabi_det_fn);
    }
    catch (las_props_nms::las_props_exc& exc)
    {
        std::cout << exc.give_err() << std::endl;
    }
    
    return 0;
}
