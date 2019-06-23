#ifndef GRID_QUANTITY_TEMPLATE_HPP
#define GRID_QUANTITY_TEMPLATE_HPP

#include <iostream>
#include "Coord_Grid.h"
#include "Grid_Data.h"


/* Useful function for param_info objects */
template <class U>
void gqt_nms::str_to_data(std::string str, U* data) {
    std::stringstream ss;
    ss.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    ss << str;
    ss >> *data;
}


/* Constructor */
template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::grid_quantity_template(const boost::filesystem::path& opts_fn, size_t tot_parts, size_t part_idx)
{
    fill_contents(opts_fn, tot_parts, part_idx);
}


/* Parameter loading functions */
template <class U>
void gqt_nms::fill_params(const boost::filesystem::path& opts_fn, U& params, std::vector<std::string>& val_str)
{
    const std::vector<pid_nms::param_info> param_info_vec {params.get_param_info()};
    val_str.resize(param_info_vec.size(), "");
    std::string * val_str_ptr {val_str.data()};
    
    if(param_info_vec.empty()) {return;}
    
    std::vector<pow_nms::mob_desc> desc_vec;
    for(auto& elem : param_info_vec)
    {
        desc_vec.push_back({std::get<0>(elem), [val_str_ptr](){return (boost::program_options::value<std::string>(val_str_ptr)->default_value("")->implicit_value(""));}, ""});
        val_str_ptr++;
    }
    
    auto vm {pow_nms::get_file_opt(opts_fn, desc_vec)};
    
    val_str_ptr = val_str.data();
    for(auto& elem : param_info_vec)
    {
        if(("" == *val_str_ptr) && std::get<2>(elem)()) {
            throw gqt_exc{(std::string{"The value "} + std::get<0>(elem) + std::string{" should have been set."}).c_str()};
        }
        std::get<3>(elem)(*(val_str_ptr++));
    }
    
    params.post_read_ops(vm);
}

template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
void gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::fill_in_params(const boost::filesystem::path& opts_fn)
{
    fill_params(opts_fn, in_params, in_val_str);
}

template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
void gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::fill_coord_grid(const boost::filesystem::path& opts_fn)
{
    auto cg_fn {((in_params.coord_grid_deduced && Constructing) ? in_params.coord_grid_src() : opts_fn)};
    fill_params(cg_fn, coord_grid, cg_val_str);
}

template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
void gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::fill_out_params(const boost::filesystem::path& opts_fn)
{
    fill_params(opts_fn, out_params, out_val_str);
}


/* Write to options file functions */
template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
template <class U>
void gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::app_opt_file(const boost::filesystem::path& opts_fn, U& params, std::vector<std::string> val_str)
{
    if((params.header() == "") && params.get_param_info().empty()) {return;}
    
    std::ofstream ofs {opts_fn.string(), std::ios_base::app};
    ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    
    ofs << std::string(params.header().length()+4, '#') << std::endl << "# " << params.header() << " #" << std::endl << std::string(params.header().length()+4, '#') << std::endl << std::endl;
    
    for(size_t idx = 0; idx < params.get_param_info().size(); ++idx) {
        if(std::get<1>(params.get_param_info()[idx]).length()) {
            if(idx) {ofs << std::endl;}
            ofs << "# " << std::get<1>(params.get_param_info()[idx]) << std::endl;
        }
        ofs << std::get<0>(params.get_param_info()[idx]) << " = " << (val_str.size() ? val_str[idx] : "") << std::endl;
    }
    ofs << std::endl << std::endl << std::endl;
}

template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
void gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::write_opt_file(const boost::filesystem::path& opts_fn)
{
    if(exists(opts_fn)) {throw gqt_exc{"Options file already exists"};}
    
    app_opt_file(opts_fn, in_params, in_val_str);
    app_opt_file(opts_fn, coord_grid, cg_val_str);
    app_opt_file(opts_fn, out_params, out_val_str);
}

template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
void gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::write_opt_file_template(const boost::filesystem::path& opts_fn)
{
    if(exists(opts_fn)) {throw gqt_exc{"Options file already exists"};}
    
    std::string reply;
    while (!(In_Params::coord_grid_deduced)) {
        std::cout << "Cartesian grid (enter C) or file grid (enter F)?" << std::endl;
        std::cin >> reply;
        if((reply == "C") || (reply == "F")) {break;}
    }
    
    grid_quantity_template<In_Params, cg_nms::cart_coord_grid, Data, Out_Params, Constructing> tmp_cart;
    grid_quantity_template<In_Params, cg_nms::file_coord_grid, Data, Out_Params, Constructing> tmp_file;
    
    tmp_cart.app_opt_file(opts_fn, tmp_cart.in_params);
    if(!(In_Params::coord_grid_deduced) && (reply == "C")) {
        tmp_cart.app_opt_file(opts_fn, tmp_cart.coord_grid);
    }
    else if(!(In_Params::coord_grid_deduced) && (reply == "F")) {
        tmp_file.app_opt_file(opts_fn, tmp_file.coord_grid);
    }
    tmp_cart.app_opt_file(opts_fn, tmp_cart.out_params);
}


/* General filling function */
template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
void gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::fill_contents(const boost::filesystem::path& opts_fn, size_t tot_parts, size_t part_idx)
{
    fill_in_params(opts_fn);
    fill_coord_grid(opts_fn);
    fill_out_params(opts_fn);
    data.import_data(out_params.output_dir, in_params.grads, Constructing, prt_nms::partition{coord_grid.grid_size(), tot_parts, part_idx});
}


/* Print data */
template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
template <bool, class>
void gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::print_contents(bool no_opt_file)
{
    data.print_data(out_params.output_dir);
    if(!(data.part.get_part_idx()) && !no_opt_file) {
        write_opt_file(out_params.output_dir / global_nms::default_opts_fn);
    }
}


/* Static functions */

template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
void gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::print_out_mem(const boost::filesystem::path& opts_fn)
{
    grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing> tmp;
    tmp.fill_in_params(opts_fn);
    
    size_t file_heads = (tmp.in_params.grads ? 4 : 1);
    size_t components = (std::is_arithmetic<typename Data::stored_type::value_type>::value ? 1 : 3);
    size_t elem_size = sizeof(typename Data::stored_type::fund_type);
    
    size_t grid_pts;
    auto cg_fn {(tmp.in_params.coord_grid_deduced ? tmp.in_params.coord_grid_src() : opts_fn)};
    if(cg_nms::gives_cart_grid(cg_fn))
    {
        grid_quantity_template<In_Params, cg_nms::cart_coord_grid, Data, Out_Params> cart_tmp;
        cart_tmp.fill_coord_grid(cg_fn);
        grid_pts = cart_tmp.coord_grid.grid_size();
    }
    else
    {
        grid_quantity_template<In_Params, cg_nms::file_coord_grid, Data, Out_Params> file_tmp;
        file_tmp.fill_coord_grid(cg_fn);
        grid_pts = file_tmp.coord_grid.grid_size();
    }
    
    size_t tot_mem {file_heads*components*grid_pts*elem_size};
    print_memory(tot_mem);
}


template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing>
std::vector<std::pair<boost::filesystem::path, bool>> gqt_nms::grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing>::output_files(const boost::filesystem::path& opts_fn)
{
    grid_quantity_template<In_Params, Coord_Grid, Data, Out_Params, Constructing> tmp;
    tmp.fill_in_params(opts_fn);
    tmp.fill_out_params(opts_fn);
    
    return tmp.data.output_files(tmp.out_params.output_dir, tmp.in_params.grads);
}

#endif /* GRID_QUANTITY_TEMPLATE_HPP */
