#ifndef INTERPOLATION_GRIDS_HPP
#define INTERPOLATION_GRIDS_HPP

/* Define interp_data functions */

template <ig_nms::interp_quant IQ, par_arr_nms::storage Storage, bool Tmp_Copy>
void ig_nms::interp_data<IQ, Storage, Tmp_Copy>::import_data(const boost::filesystem::path& output_dir, bool, bool constructing, prt_nms::partition part_in)
{
    part = part_in;
    arr = std::move(par_arr_nms::init_par_arr<Storage, par_arr_nms::arr_type::scalar>(output_files(output_dir).front().first, Tmp_Copy, constructing, part));
}


template <ig_nms::interp_quant IQ, par_arr_nms::storage Storage, bool Tmp_Copy>
template <par_arr_nms::storage Ret>
tf_nms::enable_if_tt<Ret == par_arr_nms::storage::mem> ig_nms::interp_data<IQ, Storage, Tmp_Copy>::print_data(const boost::filesystem::path& output_dir)
{
    boost::filesystem::path fn {fio_nms::part_file_name(output_files(output_dir).front(), part).first};
    print_to_file(arr, fn);
}


template <ig_nms::interp_quant IQ, par_arr_nms::storage Storage, bool Tmp_Copy>
std::vector<std::pair<boost::filesystem::path, bool>> ig_nms::interp_data<IQ, Storage, Tmp_Copy>::output_files(const boost::filesystem::path& output_dir, bool)
{
    std::map<interp_quant, boost::filesystem::path> out_fn_map {{interp_quant::BdB, "BdB.bin"}, {interp_quant::B, "B.bin"}, {interp_quant::Bnorm, "Bnorm.bin"}, {interp_quant::acc_1Sc, "acc_1Sc.bin"}, {interp_quant::acc_1Sd, "acc_1Sd.bin"}, {interp_quant::acc_2Sc, "acc_2Sc.bin"}, {interp_quant::acc_2Sd, "acc_2Sd.bin"}};
    
    return std::vector<std::pair<boost::filesystem::path, bool>>{{(output_dir/out_fn_map.at(IQ)), false}};
}


/* Define interp_grid functions */

template <ig_nms::interp_quant IQ, par_arr_nms::storage Storage, bool Tmp_Copy, bool Constructing>
ig_nms::interp_grid<IQ, Storage, Tmp_Copy, Constructing>::interp_grid(const boost::filesystem::path& opts_fn, size_t tot_parts, size_t part_idx)
{
    fill_contents(opts_fn, tot_parts, part_idx);
}

template <ig_nms::interp_quant IQ, par_arr_nms::storage Storage, bool Tmp_Copy, bool Constructing>
void ig_nms::interp_grid<IQ, Storage, Tmp_Copy, Constructing>::fill_contents(const boost::filesystem::path& opts_fn, size_t tot_parts, size_t part_idx)
{
    this->fill_in_params(opts_fn);
    this->fill_coord_grid(opts_fn);
    this->fill_out_params(opts_fn);
    
    size_t nums_per_grid_pt {ig_nms::enum_val((this->in_params).it)*ig_nms::enum_val(IQ)};
    size_t grid_pts {(this->coord_grid).grid_size()};
    (this->data).import_data((this->out_params).output_dir, false, Constructing, prt_nms::partition{grid_pts*nums_per_grid_pt, tot_parts, part_idx, nums_per_grid_pt});
}

template <ig_nms::interp_quant IQ, par_arr_nms::storage Storage, bool Tmp_Copy, bool Constructing>
void ig_nms::interp_grid<IQ, Storage, Tmp_Copy, Constructing>::print_out_mem(const boost::filesystem::path& opts_fn)
{
    interp_grid<IQ, Storage, Tmp_Copy, true> tmp;
    tmp.fill_in_params(opts_fn);
    tmp.fill_coord_grid(opts_fn);
    
    size_t tot_mem {tmp.coord_grid.grid_size()*ig_nms::enum_val(tmp.in_params.it)*ig_nms::enum_val(IQ)*sizeof(typename interp_data<IQ, Storage, Tmp_Copy>::stored_type::fund_type)};
    gqt_nms::print_memory(tot_mem);
}

#endif /* INTERPOLATION_GRIDS_HPP */

