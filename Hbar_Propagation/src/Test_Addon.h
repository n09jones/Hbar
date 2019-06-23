#include <cassert>
#include "ATRAP_base.h"

std::vector<double> records;

/*
 cycle_int :
 0 - In-cycle
 1 - Intermediate
 2 - Ionized
 3 - Wrong state
 */
template <class STATE_T>
void add_to_record(double t, double cycle_int, STATE_T pos_vel, double mix_mag, double w, double P, double decay_thresh, double exc_thresh)
{
    records.push_back(t);
    records.push_back(cycle_int);
    for(int comp = 0; comp < 3; ++comp) {records.push_back(pos_vel.first[comp]);}
    for(int comp = 0; comp < 3; ++comp) {records.push_back(pos_vel.second[comp]);}
    records.push_back(mix_mag);
    records.push_back(w);
    records.push_back(P);
    records.push_back(decay_thresh);
    records.push_back(exc_thresh);
}

void print_record_record()
{
    mem_arr_nms::mem_arr<double> tmp_arr {records.size()};
    
    std::copy(records.begin(), records.end(), tmp_arr.begin());
    print_to_file(tmp_arr, OUTPUT_FILE_PATH);
    
    assert(0);
}
