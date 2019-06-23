#include "Atomic_States.h"

/* Value names and ranges */
namespace
{
    std::vector<int> F_vals(const std::map<std::string, int>&) {return std::vector<int>{0, 1};}
    std::vector<int> mF_vals(const std::map<std::string, int>& val_map)
    {
        std::vector<int> ret_vec;
        for(int mF = -val_map.at("F"); mF <= val_map.at("F"); ++mF) {ret_vec.push_back(mF);}
        return ret_vec;
    }
    
    std::vector<int> J_vals(const std::map<std::string, int>&) {return std::vector<int>{1, 3};}
    std::vector<int> mJ_vals(const std::map<std::string, int>& val_map)
    {
        std::vector<int> ret_vec;
        for(int mJ = -val_map.at("2J"); mJ <= val_map.at("2J"); mJ += 2) {ret_vec.push_back(mJ);}
        return ret_vec;
    }
    std::vector<int> mI_vals(const std::map<std::string, int>&) {return std::vector<int>{-1, 1};}
}

const asts_nms::nm_rng_t asts_nms::S_nm_rng {{"F", F_vals}, {"mF", mF_vals}};
const asts_nms::nm_rng_t asts_nms::P_nm_rng {{"2J", J_vals}, {"2mJ", mJ_vals}, {"2mI", mI_vals}};


/* ket class */
template <int N, const asts_nms::nm_rng_t * Nm_Rng>
void asts_nms::ket<N, Nm_Rng>::consistency_check()
{
    if(!std::is_permutation(val_map.begin(), val_map.end(), Nm_Rng->begin(), [](auto x, auto y) {return (x.first == y.first);}) ||
       (val_map.size() != Nm_Rng->size())) {throw asts_exc{"Nm_Rng doesn't match val_map"};}
    
    if((N != 1) && (N != 2)) {throw asts_exc{"N out of range"};}
    
    for(const auto& str_func : *Nm_Rng) {
        auto vec {str_func.second(val_map)};
        if(!std::count(vec.begin(), vec.end(), val_map.at(str_func.first))) {
            throw asts_exc{"Out of range"};
        }
    }
}

template <int N, const asts_nms::nm_rng_t * Nm_Rng>
std::vector<pid_nms::param_info> asts_nms::ket<N, Nm_Rng>::get_param_info()
{
    std::vector<pid_nms::param_info> param_info_vec;
    
    for(const auto& nm_func : *Nm_Rng)
    {
        param_info_vec.push_back
        ({
            ((N == 1) ? "Ground State " : "Excited State ") + nm_func.first,
            "",
            gqt_nms::fnc_true,
            [nm_func, this](std::string str) {gqt_nms::str_to_data(str, &((this->val_map)[nm_func.first]));}
        });
    }
    
    return param_info_vec;
}

template <int N, const asts_nms::nm_rng_t * Nm_Rng>
void asts_nms::ket<N, Nm_Rng>::fill(const boost::filesystem::path& opts_fn)
{
    val_map.clear();
    std::vector<std::string> val_str;
    gqt_nms::fill_params(opts_fn, *this, val_str);
}


template <int N, const asts_nms::nm_rng_t * Nm_Rng>
std::vector<asts_nms::ket<N, Nm_Rng>> asts_nms::ket<N, Nm_Rng>::basis(std::map<std::string, int> mp)
{
    if(mp.size() == Nm_Rng->size()) {return std::vector<ket>{{mp}};}
    
    std::vector<ket> ret_vec;
    auto nm_rng {(*Nm_Rng)[mp.size()]};

    for(auto num : nm_rng.second(mp)) {
        mp[nm_rng.first] = num;
        auto tmp_vec {basis(mp)};
        ret_vec.insert(ret_vec.end(), tmp_vec.begin(), tmp_vec.end());
    }
    
    return ret_vec;
}

/* States we want to use */
template struct asts_nms::ket<1, &asts_nms::S_nm_rng>;
template struct asts_nms::ket<2, &asts_nms::S_nm_rng>;
template struct asts_nms::ket<2, &asts_nms::P_nm_rng>;


