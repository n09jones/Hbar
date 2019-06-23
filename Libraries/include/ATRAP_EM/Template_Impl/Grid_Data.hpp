#ifndef GRID_DATA_HPP
#define GRID_DATA_HPP

template <par_arr_nms::storage STORAGE, gd_nms::quant QUANT, class T>
void gd_nms::grid_data<STORAGE, QUANT, T>::import_data(const boost::filesystem::path& output_dir, bool grads, bool constructing, prt_nms::partition part_in)
{
    part = part_in;
    auto init_func {par_arr_nms::init_par_arr<STORAGE, (quant_is_scalar(QUANT) ? par_arr_nms::arr_type::scalar : par_arr_nms::arr_type::vector), T>};
    
    std::vector<std::string> keys {base_strs(QUANT, grads)};
    std::vector<std::pair<boost::filesystem::path, bool>> fns {output_files(output_dir, grads)};
    
    for(size_t idx = 0; idx < keys.size(); ++idx) {
        data_map[keys[idx]] = std::move(init_func(fns[idx].first, true, constructing, part));
    }
}


template <par_arr_nms::storage STORAGE, gd_nms::quant QUANT, class T>
template <par_arr_nms::storage Ret>
tf_nms::enable_if_tt<Ret == par_arr_nms::storage::mem> gd_nms::grid_data<STORAGE, QUANT, T>::print_data(const boost::filesystem::path& output_dir)
{
    for(auto& pr : data_map) {
        boost::filesystem::path fn {fio_nms::part_file_name({base_str_to_fn(output_dir, pr.first), !quant_is_scalar(QUANT)}, part).first};
        print_to_file(pr.second, fn);
    }
}


template <par_arr_nms::storage STORAGE, gd_nms::quant QUANT, class T>
std::vector<std::pair<boost::filesystem::path, bool>> gd_nms::grid_data<STORAGE, QUANT, T>::output_files(const boost::filesystem::path& output_dir, bool grads)
{
    std::vector<std::pair<boost::filesystem::path, bool>> ret_vec;
    
    for(const auto& str : base_strs(QUANT, grads)) {
        ret_vec.push_back({base_str_to_fn(output_dir, str), !quant_is_scalar(QUANT)});
    }
    
    return ret_vec;
}

#endif /* GRID_DATA_HPP */

