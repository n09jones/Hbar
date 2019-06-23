#include <iostream>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

int bin_func(const std::array<double, 3>& lims, double x)
{
    if((x < lims[0]) || (x >= lims[1])) {return -1;}
    return static_cast<int>((x - lims[0])/lims[2]);
}

void func(boost::filesystem::path out_fn, boost::filesystem::path det_fn, boost::filesystem::path weights_fn, std::array<double, 3> lims)
{
    std::vector<std::array<double, 2>> det_weights {};
    
    mem_arr_nms::mem_arr<> arr_dets {det_fn};
    mem_arr_nms::mem_arr<> arr_weights {weights_fn};
    
    if(lims[2] < 0)
    {
        auto its {std::minmax_element(arr_dets.begin(), arr_dets.end())};
        lims[0] = *(its.first);
	lims[1] = *(its.second);
	lims[2] = 1.0;
    }

    for(size_t idx = 0; idx < arr_dets.size(); ++idx) {
        det_weights.push_back({{arr_dets[idx], arr_weights[idx]}});
    }
    
    std::vector<std::array<double, 2>> hist;
    for(double lim = lims[0]; lim < lims[1]; lim += lims[2]) {hist.push_back({{lim + (lims[2]/2.0), 0.0}});}
    
    for(const auto& elem : det_weights)
    {
        int idx {bin_func(lims, elem[0])};
        
        if(idx != -1) {hist[idx][1] += elem[1];}
    }
    
    mem_arr_nms::mem_arr<> out {2*hist.size()};
    for(size_t idx = 0; idx < hist.size(); ++idx)
    {
        out[(2*idx) + 0] = hist[idx][0];
        out[(2*idx) + 1] = hist[idx][1];
    }
    
    print_to_file(out, out_fn);
}

int main(int argc, char ** argv)
{
    std::array<double, 3> lims;

    if(argc == 2) {lims = {{-1, -1, -1}};}
    else {lims = {{::atof(argv[2]), ::atof(argv[3]), ::atof(argv[4])}};}

    std::string base {argv[1]};

boost::filesystem::path dir {"/n/home11/njones/PFC_3"};
boost::filesystem::path out_fn {dir / boost::filesystem::path{base + ".bin"}};
boost::filesystem::path det_fn {dir / base / boost::filesystem::path{"Zeeman_shifts.bin"}};
boost::filesystem::path weights_fn {dir / base / boost::filesystem::path{"Zeeman_exp_mask.bin"}};
    
    func(out_fn, det_fn, weights_fn, lims);
    return 0;
}
