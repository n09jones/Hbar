#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_EM.h"
#include "ATRAP_Trap_Quantities.h"

template <ig_nms::interp_type IT, ig_nms::interp_quant IQ, bool Use_Arr>
void test_interp(interp_nms::interpolator<IT>& intp, const cg_nms::file_coord_grid& cg)
{
    if(!intp.in_params.interp_quant_src_dirs.count(IQ)){return;}
    
    std::map<ig_nms::interp_quant, std::string> iq_to_str {{ig_nms::interp_quant::BdB, "BdB"}, {ig_nms::interp_quant::B, "B"}, {ig_nms::interp_quant::Bnorm, "Bnorm"}, {ig_nms::interp_quant::acc_1Sc, "acc_1Sc"}, {ig_nms::interp_quant::acc_1Sd, "acc_1Sd"}, {ig_nms::interp_quant::acc_2Sc, "acc_2Sc"}, {ig_nms::interp_quant::acc_2Sd, "acc_2Sd"}};
    
    std::map<ig_nms::interp_type, std::string> it_to_str {{ig_nms::interp_type::zeroth, "zeroth"}, {ig_nms::interp_type::lin, "lin"}, {ig_nms::interp_type::quad, "quad"}, {ig_nms::interp_type::trilin, "trilin"}};

    size_t arr_len = cg.grid_size();
    mem_arr_nms::mem_arr<> arr {ig_nms::enum_val(IQ)*arr_len};
    auto ptr {arr.data()};
    std::array<global_nms::default_data_type, ig_nms::enum_val(IQ)> tmp_arr {};

    std::cout << "Start the clock" << std::endl;    
    auto wcts = std::chrono::system_clock::now();
    
    for(size_t k = 0; k < arr_len; ++k)
    {
        intp.template calc_trap_quant<IQ, Use_Arr, false>(tmp_arr, cg.coords(k));
        for(auto elem : tmp_arr) {*(ptr++) = elem;}
    }

    std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
    std::cout << "For " << iq_to_str.at(IQ) << " : " << wctduration.count() << " seconds [Wall Clock]" << std::endl;
    
    print_to_file(arr, (global_nms::large_storage_dir / boost::filesystem::path{it_to_str.at(IT) + std::string("_interp_test")} / boost::filesystem::path{std::string{"interpolated_"} + iq_to_str.at(IQ) + std::string{".bin"}}));
}

template <ig_nms::interp_type IT, bool Use_Arr>
int pseudo_main(int argc, char ** argv)
{
std::cout << "Getting coord grid" << std::endl;
    cg_nms::file_coord_grid cg;
    std::vector<std::string> dummy;
    gqt_nms::fill_params(jm_nms::get_opts_fn(argc, argv), cg, dummy);
    
std::cout << "Firing up the interpolator" << std::endl;
    interp_nms::interpolator<IT> intp {jm_nms::get_opts_fn(argc, argv)};
    
    std::cout << "Starting" << std::endl;
    
    test_interp<IT, ig_nms::interp_quant::BdB, Use_Arr>(intp, cg);
    test_interp<IT, ig_nms::interp_quant::B, Use_Arr>(intp, cg);
    test_interp<IT, ig_nms::interp_quant::Bnorm, Use_Arr>(intp, cg);
    test_interp<IT, ig_nms::interp_quant::acc_1Sc, Use_Arr>(intp, cg);
    test_interp<IT, ig_nms::interp_quant::acc_1Sd, Use_Arr>(intp, cg);
    test_interp<IT, ig_nms::interp_quant::acc_2Sc, Use_Arr>(intp, cg);
    test_interp<IT, ig_nms::interp_quant::acc_2Sd, Use_Arr>(intp, cg);
    
    std::cout << "Ending" << std::endl;
    
    return 0;
}


int main(int argc, char ** argv)
{
try{
std::cout << "Figuring out it" << std::endl;
    interp_nms::interpolator_in_params<ig_nms::interp_type::lin> iip;
    std::vector<std::string> dummy;
    gqt_nms::fill_params(jm_nms::get_opts_fn(argc, argv), iip, dummy);
std::cout << "It determined" << std::endl;
    switch (iip.it)
    {
        case ig_nms::interp_type::zeroth :
            std::cout << "zeroth" << std::endl;
            return pseudo_main<ig_nms::interp_type::zeroth, true>(argc, argv);
        
        case ig_nms::interp_type::lin :
            std::cout << "lin" << std::endl;
            return pseudo_main<ig_nms::interp_type::lin, true>(argc, argv);
            
        case ig_nms::interp_type::quad :
            std::cout << "quad" << std::endl;
            return pseudo_main<ig_nms::interp_type::quad, true>(argc, argv);
            
        case ig_nms::interp_type::trilin :
            std::cout << "trilin" << std::endl;
            return pseudo_main<ig_nms::interp_type::trilin, true>(argc, argv);
    }
    
}
catch(fio_nms::fio_exc& exc)
{
    std::cout << exc.give_err() << std::endl;
}
    
    return 0;
}
