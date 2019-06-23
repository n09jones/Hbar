#ifndef INTERPOLATORS_HPP
#define INTERPOLATORS_HPP

#include <future>

/* interpolator_in_params class */
template <ig_nms::interp_type IT>
boost::filesystem::path interp_nms::interpolator_in_params<IT>::coord_grid_src()
{
    return (interp_quant_src_dirs.begin()->second / global_nms::default_opts_fn);
}

template <ig_nms::interp_type IT>
std::string interp_nms::interpolator_in_params<IT>::header()
{
    std::map<ig_nms::interp_type, std::string> it_to_str {{ig_nms::interp_type::zeroth, "ZERO-ORDER"}, {ig_nms::interp_type::lin, "LINEARLY"}, {ig_nms::interp_type::quad, "QUADRATICALLY"}, {ig_nms::interp_type::trilin, "TRILINEARLY"}};
    
    return (it_to_str.at(IT) + std::string{" INTERPOLATED QUANTITY SOURCES"});
}

template <ig_nms::interp_type IT>
std::vector<pid_nms::param_info> interp_nms::interpolator_in_params<IT>::get_param_info()
{
    std::vector<std::pair<std::string, ig_nms::interp_quant>> str_to_interp_quant {{"BdB", ig_nms::interp_quant::BdB}, {"B", ig_nms::interp_quant::B}, {"Bnorm", ig_nms::interp_quant::Bnorm}, {"acc_1Sc", ig_nms::interp_quant::acc_1Sc}, {"acc_1Sd", ig_nms::interp_quant::acc_1Sd}, {"acc_2Sc", ig_nms::interp_quant::acc_2Sc}, {"acc_2Sd", ig_nms::interp_quant::acc_2Sd}};
    
    std::vector<pid_nms::param_info> param_info_vec;
    
    for(size_t idx = 0; idx < str_to_interp_quant.size(); ++idx)
    {
        param_info_vec.push_back
        ({
            std::string{"Interpolation Source Directory "} + str_to_interp_quant[idx].first,
            ((!idx) ? "All directory paths are relative to \"large_storage_dir\" in Global_Info.h. Exclude directories for unused quantities." : ""),
            gqt_nms::fnc_false,
            [str_to_interp_quant, idx, this](std::string str){if("" != str){(this->interp_quant_src_dirs)[str_to_interp_quant[idx].second] = (global_nms::large_storage_dir / boost::filesystem::path{str});}}
            
        });
    }
    
    return param_info_vec;
}

template <ig_nms::interp_type IT>
void interp_nms::interpolator_in_params<IT>::post_read_ops(boost::program_options::variables_map)
{
    grads = false;
    
    std::vector<std::string> dummy;
    ig_nms::ig_in_params tmp_in_params;
    gqt_nms::fill_params(coord_grid_src(), tmp_in_params, dummy);
    
    it = tmp_in_params.it;
    if(IT != it) {throw interp_exc{"Improper interpolation type loaded"};}
}

/* sub_interp_data function */
template <ig_nms::interp_quant IQ, ig_nms::interp_type IT>
void interp_nms::sub_interp_data<IQ, IT>::import_data(const interpolator_in_params<IT>& iip, const cg_nms::cart_coord_grid& cg)
{
    if(!iip.interp_quant_src_dirs.count(IQ)) {return;}

    grid_idx_loc = cg.grid_size();
    
std::cout << "starting " << static_cast<int>(IQ) << std::endl;
    ig.fill_contents(iip.interp_quant_src_dirs.at(IQ) / global_nms::default_opts_fn);
std::cout << "ending " << static_cast<int>(IQ) << std::endl;
    if((iip.it != IT) || (iip.it != ig.in_params.it) || (cg != ig.coord_grid)) {
        throw interp_exc{"Interpolation types must match"};
    }
}

/* interp_data */
template <ig_nms::interp_type IT>
void interp_nms::interp_data<IT>::import_data(const interp_nms::interpolator_in_params<IT>& iip, const cg_nms::cart_coord_grid& cg)
{
    std::vector<std::future<void>> futures;

    futures.push_back(std::async(&interp_data<IT>::template sub_interp_data<ig_nms::interp_quant::BdB, IT>::import_data, this, iip, cg));
    futures.push_back(std::async(&interp_data<IT>::template sub_interp_data<ig_nms::interp_quant::B, IT>::import_data, this, iip, cg));
    futures.push_back(std::async(&interp_data<IT>::template sub_interp_data<ig_nms::interp_quant::Bnorm, IT>::import_data, this, iip, cg));
    futures.push_back(std::async(&interp_data<IT>::template sub_interp_data<ig_nms::interp_quant::acc_1Sc, IT>::import_data, this, iip, cg));
    futures.push_back(std::async(&interp_data<IT>::template sub_interp_data<ig_nms::interp_quant::acc_1Sd, IT>::import_data, this, iip, cg));
    futures.push_back(std::async(&interp_data<IT>::template sub_interp_data<ig_nms::interp_quant::acc_2Sc, IT>::import_data, this, iip, cg));
    futures.push_back(std::async(&interp_data<IT>::template sub_interp_data<ig_nms::interp_quant::acc_2Sd, IT>::import_data, this, iip, cg));

    for(auto& e : futures) {e.get();}
}

/* interpolator class */
template <ig_nms::interp_type IT>
interp_nms::interpolator<IT>::interpolator(const boost::filesystem::path& opts_fn, size_t tot_parts, size_t part_idx)
{
    fill_contents(opts_fn, tot_parts, part_idx);
}

template <ig_nms::interp_type IT>
void interp_nms::interpolator<IT>::fill_contents(const boost::filesystem::path& opts_fn, size_t tot_parts, size_t part_idx)
{
    this->fill_in_params(opts_fn);
    this->fill_coord_grid(opts_fn);
    this->fill_out_params(opts_fn);
    (this->data).import_data(this->in_params, this->coord_grid);
    
    this->init_grid_position(this->coord_grid);
}

#endif /* INTERPOLATORS_HPP */

