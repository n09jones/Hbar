#include "State_Summary.h"

st_sum_nms::state_summary::serial_ss_t st_sum_nms::state_summary::to_arr() const
{
    serial_ss_t arr;
    
    size_t idx {0};
    
    for(int comp = 0; comp < 3; ++comp) {arr[idx++] = pos_vel.first[comp];}
    for(int comp = 0; comp < 3; ++comp) {arr[idx++] = pos_vel.second[comp];}
    
    arr[idx++] = t_end;
    arr[idx++] = E;
    
    arr[idx++] = N;
    arr[idx++] = F;
    arr[idx++] = mF;
    
    arr[idx++] = one_photon_decays;
    arr[idx++] = two_photon_decays;
    arr[idx++] = (ionized ? 1.0 : 0.0);
    
    arr[idx++] = (out_of_bounds ? 1.0 : 0.0);
    
    arr[idx++] = beam_passes;
    arr[idx++] = t_in_beam;
    
    return arr;
}


std::vector<st_sum_nms::state_summary> st_sum_nms::extract_state_summaries(const boost::filesystem::path& arr_fn, prt_nms::partition part)
{
    if((part.get_chunk_size() != std::tuple_size<state_summary::serial_ss_t>::value) && !(part.is_def_init())) {
        throw st_sum_exc{"Partition chunk size should match the size of the state summary arrays"};
    }
    
    std::vector<state_summary> smry_vec;
    mapped_arr_nms::mapped_arr<> arr_in {arr_fn, false, part};
    
    for(auto it = arr_in.cbegin(); it < arr_in.cend(); it += std::tuple_size<state_summary::serial_ss_t>::value)
    {
        state_summary tmp;
        size_t idx {0};
        
        for(int comp = 0; comp < 3; ++comp) {tmp.pos_vel.first[comp] = it[idx++];}
        for(int comp = 0; comp < 3; ++comp) {tmp.pos_vel.second[comp] = it[idx++];}
        
        tmp.t_end = it[idx++];
        tmp.E = it[idx++];
        
        tmp.N = std::lround(it[idx++]);
        tmp.F = std::lround(it[idx++]);
        tmp.mF = std::lround(it[idx++]);
        
        tmp.one_photon_decays = std::lround(it[idx++]);
        tmp.two_photon_decays = std::lround(it[idx++]);
        tmp.ionized = (it[idx++] > 0.5);
        
        tmp.out_of_bounds = (it[idx++] > 0.5);
        
        tmp.beam_passes = std::lround(it[idx++]);
        tmp.t_in_beam = it[idx++];
        
        smry_vec.push_back(tmp);
    }
    
    return smry_vec;
}

void st_sum_nms::print_state_summary(const std::vector<st_sum_nms::state_summary>& smry_vec, const boost::filesystem::path& fn, prt_nms::partition part)
{
    mem_arr_nms::mem_arr<> arr {smry_vec.size()*std::tuple_size<state_summary::serial_ss_t>::value};
    
    auto it {arr.begin()};
    for(const auto& smry : smry_vec) {
        for(auto elem : smry.to_arr()) {*(it++) = elem;}
    }
    
    if(part.is_def_init()) {part = prt_nms::partition{1, 1, 0, 1};}
    print_to_file(arr, fio_nms::part_file_name({fn, false}, part).first);
}

