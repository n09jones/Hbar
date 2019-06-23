#include "ATRAP_base.h"
#include "ATRAP_EM.h"

void fix_coil(const boost::filesystem::path& fn_in, const boost::filesystem::path& fn_out)
{
    mem_arr_nms::mem_arr<> raw_coil {global_nms::coil_dir / boost::filesystem::path{"Old_Coil_Defs"} / fn_in};
    mem_arr_vec_nms::mem_arr_vec<> raw_coil_vec {cvc_nms::coil_to_vec(raw_coil)};
    
    mem_arr_vec_nms::mem_arr_vec<> looped_coil_vec {raw_coil_vec.arr_len() + 1};
    for(unsigned comp = 0; comp < 3; ++comp) {
        for(size_t idx = 0; idx < raw_coil_vec.arr_len(); ++idx) {
            looped_coil_vec[comp][idx] = raw_coil_vec[comp][idx];
        }
        looped_coil_vec[comp][raw_coil_vec.arr_len()] = raw_coil_vec[comp][0];
    }
    
    mem_arr_vec_nms::mem_arr_vec<> fin_coil_vec;
    fin_coil_vec[0] = 0.001*looped_coil_vec[1];
    fin_coil_vec[1] = -0.001*looped_coil_vec[2];
    fin_coil_vec[2] = -0.001*looped_coil_vec[0];
    
    print_to_file(cvc_nms::vec_to_coil(fin_coil_vec), (global_nms::coil_dir / fn_out));
}

void straighten_oct(const boost::filesystem::path& fn_in, const boost::filesystem::path& fn_out)
{
    mem_arr_nms::mem_arr<> raw_coil {global_nms::coil_dir / fn_in};
    mem_arr_vec_nms::mem_arr_vec<> raw_coil_vec {cvc_nms::coil_to_vec(raw_coil)};
    
    std::vector<std::array<global_nms::default_data_type,3>> straight_vec;
    for(size_t k = 0; k < raw_coil_vec.arr_len(); ++k) {
        if(std::abs(raw_coil_vec[2][k]) > 0.09) {
            straight_vec.push_back({{raw_coil_vec[0][k], raw_coil_vec[1][k], raw_coil_vec[2][k]}});
        }
    }
    
    mem_arr_vec_nms::mem_arr_vec<> fin_coil_vec {straight_vec.size()};
    for(size_t k = 0; k < straight_vec.size(); ++k) {
        for(unsigned comp = 0; comp < 3; ++comp) {
            fin_coil_vec[comp][k] = straight_vec[k][comp];
        }
    }
    
    print_to_file(cvc_nms::vec_to_coil(fin_coil_vec), (global_nms::coil_dir / fn_out));
}

int main()
{
    fix_coil("H-QuadAll.txt", "STD_Quad_Coil.bin");
    fix_coil("H-Octupole-All.txt", "STD_Oct_Coil.bin");
    straighten_oct("STD_Oct_Coil.bin", "Straight_Oct_Coil.bin");
    
    return 0;
}
