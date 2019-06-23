#ifndef INTERPOLATORS_H
#define INTERPOLATORS_H

#include "ATRAP_base.h"
#include "ATRAP_EM.h"
#include "Interpolation_Grids.h"
#include "Interpolation_Data.h"
#include "Position_Handlers.h"

namespace interp_nms
{
    /* Exception for functions in this header */
    class interp_exc {
    public:
        interp_exc(std::string& msg): err_message{msg} {}
        interp_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    constexpr ig_nms::interp_type default_interp_type {ig_nms::interp_type::lin};
    
    /* interpolator components */
    template <ig_nms::interp_type IT = default_interp_type>
    struct interpolator_in_params
    {
        static const bool coord_grid_deduced {true};
        boost::filesystem::path coord_grid_src();
        
        std::string header();
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        bool grads;
        ig_nms::interp_type it;
        std::map<ig_nms::interp_quant, boost::filesystem::path> interp_quant_src_dirs;
    };
    
    template <ig_nms::interp_quant IQ, ig_nms::interp_type IT = default_interp_type>
    class sub_interp_data
    {
    private:
        ig_nms::interp_grid<IQ, par_arr_nms::storage::map, (true || (IQ != ig_nms::interp_quant::BdB))> ig;
        
    public:
        size_t grid_idx_loc;
        const global_nms::default_data_type * ptr;
        std::array<global_nms::default_data_type, ig_nms::enum_val(IQ)*ig_nms::enum_val(IT)> loc_arr;
        
        void import_data(const interpolator_in_params<IT>&, const cg_nms::cart_coord_grid&);
        
        template <bool Use_Arr>
        const global_nms::default_data_type * update_interp_info(size_t grid_idx)
        {
            if(grid_idx != grid_idx_loc)
            {
                grid_idx_loc = grid_idx;
                ptr = ig.data.arr.data() + (grid_idx_loc*ig_nms::enum_val(IQ)*ig_nms::enum_val(IT));
                
                if(Use_Arr) {std::copy(ptr, ptr + (ig_nms::enum_val(IQ)*ig_nms::enum_val(IT)), loc_arr.begin());}
            }
            
            return (Use_Arr ? loc_arr.data() : ptr);
        }
    };
    
    template <ig_nms::interp_type IT = default_interp_type>
    struct interp_data :
    sub_interp_data<ig_nms::interp_quant::BdB, IT>, sub_interp_data<ig_nms::interp_quant::B, IT>,
    sub_interp_data<ig_nms::interp_quant::Bnorm, IT>, sub_interp_data<ig_nms::interp_quant::acc_1Sc, IT>,
    sub_interp_data<ig_nms::interp_quant::acc_1Sd, IT>, sub_interp_data<ig_nms::interp_quant::acc_2Sc, IT>,
    sub_interp_data<ig_nms::interp_quant::acc_2Sd, IT>
    {
        void import_data(const interpolator_in_params<IT>&, const cg_nms::cart_coord_grid&);
    };
    
    struct null_out_params
    {
        std::string header() {return "";}
        std::vector<pid_nms::param_info> get_param_info() {return std::vector<pid_nms::param_info>{};}
        void post_read_ops(boost::program_options::variables_map) {}
        boost::filesystem::path output_dir {};
    };
    
    /* interpolator class */
    template <ig_nms::interp_type IT = default_interp_type>
    struct interpolator :
    gqt_nms::grid_quantity_template<interpolator_in_params<IT>, cg_nms::cart_coord_grid, interp_data<IT>, null_out_params, true>,
    pos_nms::grid_position<IT>
    {
        interpolator() {}
        interpolator(const boost::filesystem::path&, size_t = 1, size_t = 0);
        void fill_contents(const boost::filesystem::path&, size_t = 1, size_t = 0);
        
        template <ig_nms::interp_quant IQ, bool Use_Arr, bool Pay_Attention_To_Out_Of_Bounds = true>
        void calc_trap_quant(std::array<global_nms::default_data_type, ig_nms::enum_val(IQ)>& out_arr, bvec_nms::basic_vec<> pos_new)
        {
            this->update_position_info(pos_new);
            if((this->out_of_bounds) && Pay_Attention_To_Out_Of_Bounds) {return;}
            auto ptr {this->data.sub_interp_data<IQ, IT>::template update_interp_info<Use_Arr>(this->grid_idx)};
            
            for (auto& elem : out_arr)
            {
                elem = 0;
                for(auto mon_elem : this->monomials) {elem += mon_elem*(*(ptr++));}
            }
        }
    };
}

#include "Template_Impl/Interpolators.hpp"

#endif /* INTERPOLATORS_H */
