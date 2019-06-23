#ifndef TQ_ARRAY_MANAGER_H
#define TQ_ARRAY_MANAGER_H

#include "ATRAP_base.h"
#include "Atomic_States.h"

namespace TQam_nms
{
    /* File name functions */
    template <int M, const asts_nms::nm_rng_t * Nm_Rng>
    std::string ket_str_rep(const asts_nms::ket<M, Nm_Rng>&);
    
    template <class... Ket_Ts>
    boost::filesystem::path tqg_fn(std::string str, const Ket_Ts&...);
    
    /* Fill helper function template */
    using it_t = decltype(mem_arr_nms::mem_arr<>{}.begin());
    
    template <class U>
    U extract_from_it(it_t&);
    
    /* Class for managing arrays of transition quantities */
    template <class U = global_nms::default_data_type>
    struct arr_mngr
    {
	std::string stored_fn;
        global_nms::default_data_type dx;
        std::vector<U> quants;
        U quant;
        
        template <class... Ket_Ts>
        void fill(const std::string&, const Ket_Ts&...);
        
        void update_quant(global_nms::default_data_type);
        U operator() () {return quant;}
        U operator() (global_nms::default_data_type x) {update_quant(x); return quant;}
    };
}

#include "Template_Impl/TQ_Array_Manager.hpp"

#endif /* TQ_ARRAY_MANAGER_H */

