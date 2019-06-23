#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using boost::filesystem::path;

template <pdef_nms::illum_type IT>
void run_decay()
{
    std::string prefix {((pdef_nms::is_LyAlph<IT> ? "LyAlph" : "TwoPh"))};
    
    auto opts_fn {global_nms::quick_test_dir / path{"Options_Files"} / path{"Illumination_Opts_File_3.txt"}};
    
    decay_nms::decay_handler<IT, emq_nms::one_EM_quants> dh;
    dh.fill(opts_fn);
    
    size_t ionized_ct {0};
    
    std::map<asts_nms::S_gr, std::vector<global_nms::default_data_type>> decay_1ph_dv;
    std::map<asts_nms::S_gr, std::vector<global_nms::default_data_type>> decay_2ph_dv;
    
    for(const auto& Sket : asts_nms::S_gr::basis()) {
        for(int comp = 0; comp < 3; ++comp)
        {
            decay_1ph_dv[Sket].push_back(0.0);
            decay_2ph_dv[Sket].push_back(0.0);
        }
    }
    
    size_t trial_lim {100000};
    size_t prev_str_len {0};
    for(size_t trials = 0; trials < trial_lim; ++trials)
    {
        if(!(trials % 1000))
        {
            if(prev_str_len) {std::cout << std::string(prev_str_len, '\b');}
            std::string new_output {"trial " + std::to_string(trials) + " of " + std::to_string(trial_lim)};
            std::cout << new_output;
            prev_str_len = new_output.length();
            std::cout << std::flush;
        }
        dh.execute_decay_chain({{0, 0, 0}, {30, -20, 10}});
        
        if(dh.ionized)
        {
            ++ionized_ct;
            dh.ionized = false;
        }
        if(dh.one_ph_decays)
        {
            for(int comp = 0; comp < 3; ++comp) {
                decay_1ph_dv.at(dh.decay_st).push_back(dh.vel_kick[comp]);
            }
            dh.one_ph_decays = 0;
        }
        if(dh.two_ph_decays)
        {
            for(int comp = 0; comp < 3; ++comp) {
                decay_2ph_dv.at(dh.decay_st).push_back(dh.vel_kick[comp]);
            }
            dh.two_ph_decays = 0;
        }
    };
    
    std::cout << std::endl;
    
    for(const auto& Sket : asts_nms::S_gr::basis())
    {
        mem_arr_nms::mem_arr<> arr_1 {decay_1ph_dv.at(Sket).size()};
        auto fn_1 {global_nms::quick_test_dir / path{"tmp"} / path{prefix + "_dv_1ph_" + TQam_nms::ket_str_rep(Sket) + ".bin"}};
        std::copy(decay_1ph_dv.at(Sket).begin(), decay_1ph_dv.at(Sket).end(), arr_1.begin());
        remove(fn_1);
        print_to_file(arr_1, fn_1);
        
        mem_arr_nms::mem_arr<> arr_2 {decay_2ph_dv.at(Sket).size()};
        auto fn_2 {global_nms::quick_test_dir / path{"tmp"} / path{prefix + "_dv_2ph_" + TQam_nms::ket_str_rep(Sket) + ".bin"}};
        std::copy(decay_2ph_dv.at(Sket).begin(), decay_2ph_dv.at(Sket).end(), arr_2.begin());
        remove(fn_2);
        print_to_file(arr_2, fn_2);
    }
    
    mem_arr_nms::mem_arr<> arr {1};
    arr[0] = ionized_ct;
    auto fn_3 {global_nms::quick_test_dir / path{"tmp"} / path{prefix + "_Ionized_Count.bin"}};
    remove(fn_3);
    print_to_file(arr, fn_3);
}

int main()
{
    run_decay<pdef_nms::illum_type::LyAlph>();
    run_decay<pdef_nms::illum_type::TwoPh>();
    
    return 0;
}
