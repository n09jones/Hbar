#include <random>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_EM.h"

int main(int argc, char ** argv)
{
    std::minstd_rand gen {};
    std::uniform_real_distribution<global_nms::default_data_type> distr(0.000, 0.001);
    unsigned t_seed = std::chrono::system_clock::now().time_since_epoch().count();
    gen.seed(t_seed);
    
    cg_nms::cart_coord_grid cg;
    std::vector<std::string> dummy;
    gqt_nms::fill_params(jm_nms::get_opts_fn(argc, argv), cg, dummy);
    
    std::vector<global_nms::default_data_type> coord_vec;
    for(global_nms::default_data_type z = cg.lo_lims[2] + 0.002; z < cg.up_lims[2] - 0.002; z += 0.001) {
        for(global_nms::default_data_type y = cg.lo_lims[1]; y < cg.up_lims[1]; y += 0.001) {
            for(global_nms::default_data_type x = cg.lo_lims[0]; x < cg.up_lims[0]; x += 0.001) {
                bvec_nms::basic_vec<> base_coords {x, y, z};
                bvec_nms::basic_vec<> ob_coords = {std::max(std::abs(x), std::abs(x)+0.001), std::max(std::abs(y), std::abs(y)+0.001), z};
                if(!cg.out_of_bds(ob_coords)) {
                    for(unsigned comp = 0; comp < 3; ++comp) {
                        coord_vec.push_back(base_coords[comp] + distr(gen));
                    }
                }
            }
        }
    }
    
    mem_arr_nms::mem_arr<> rand_coords {coord_vec.size()};
    std::copy(coord_vec.begin(), coord_vec.end(), rand_coords.begin());
    print_to_file(rand_coords, (global_nms::large_storage_dir / boost::filesystem::path{"interp_check_pts.bin"}));
}
