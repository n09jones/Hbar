#ifndef TQ_ARRAY_MANAGER_HPP
#define TQ_ARRAY_MANAGER_HPP

/* File name functions */
template <int M, const asts_nms::nm_rng_t * Nm_Rng>
std::string TQam_nms::ket_str_rep(const asts_nms::ket<M, Nm_Rng>& k)
{
    std::string ret_str {std::to_string(M) + ((Nm_Rng == &asts_nms::S_nm_rng) ? "S" : "P")};
    for(const auto& key_val : *Nm_Rng) {
        ret_str += "_" + key_val.first + (k(key_val.first) < 0 ? "_n" : "_") + std::to_string(std::abs(k(key_val.first)));
    }
    return ret_str;
}

template <class... Ket_Ts>
boost::filesystem::path TQam_nms::tqg_fn(std::string str, const Ket_Ts&... kets_in)
{
    std::vector<std::string> ket_str_vec {ket_str_rep(kets_in)...};
    for(auto elem : ket_str_vec) {str += "_" + elem;}
    str += ".bin";
    
    return (global_nms::trans_quant_dir / boost::filesystem::path{str});
}

/* arr_mngr functions */
template <class U>
template <class... Ket_Ts>
void TQam_nms::arr_mngr<U>::fill(const std::string& fn_str, const Ket_Ts&... kets_in)
{
    if(tqg_fn(fn_str, kets_in...).string() == stored_fn) {return;}

    stored_fn = tqg_fn(fn_str, kets_in...).string();
    mem_arr_nms::mem_arr<> tmp_arr {tqg_fn(fn_str, kets_in...)};
    auto it {tmp_arr.begin()};
    
    dx = *(it++);
    quants.clear();
    while(it < tmp_arr.end()) {quants.push_back(extract_from_it<U>(it));}
}

template <>
inline void TQam_nms::arr_mngr<global_nms::default_data_type>::update_quant(global_nms::default_data_type x)
{
    size_t x_idx {static_cast<size_t>(x/dx)};
    auto lambda {(x/dx) - x_idx};
    
    quant = ((1 - lambda)*quants[x_idx + 0]) + (lambda*quants[x_idx + 1]);
}

template <>
inline void TQam_nms::arr_mngr<std::array<std::complex<global_nms::default_data_type>, 1>>::update_quant(global_nms::default_data_type x)
{
    size_t x_idx {static_cast<size_t>(x/dx)};
    auto lambda {(x/dx) - x_idx};
    
    quant[0] = ((1 - lambda)*quants[x_idx + 0][0]) + (lambda*quants[x_idx + 1][0]);
}

template <>
inline void TQam_nms::arr_mngr<std::array<std::complex<global_nms::default_data_type>, 3>>::update_quant(global_nms::default_data_type x)
{
    size_t x_idx {static_cast<size_t>(x/dx)};
    auto lambda {(x/dx) - x_idx};
    
    for(int comp = 0; comp < 3; ++comp) {
        quant[comp] = ((1 - lambda)*quants[x_idx + 0][comp]) + (lambda*quants[x_idx + 1][comp]);
    }
}

template <>
inline void TQam_nms::arr_mngr<std::array<global_nms::default_data_type, 6>>::update_quant(global_nms::default_data_type x)
{
    size_t x_idx {static_cast<size_t>(x/dx)};
    auto lambda {(x/dx) - x_idx};
    
    for(int comp = 0; comp < 6; ++comp) {
        quant[comp] = ((1 - lambda)*quants[x_idx + 0][comp]) + (lambda*quants[x_idx + 1][comp]);
    }
}

#endif /* TQ_ARRAY_MANAGER_HPP */

