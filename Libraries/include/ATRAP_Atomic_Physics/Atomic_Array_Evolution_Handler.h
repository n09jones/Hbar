#ifndef ATOMIC_ARRAY_EVOLUTION_HANDLER_H
#define ATOMIC_ARRAY_EVOLUTION_HANDLER_H

#include "ATRAP_base.h"
#include "ATRAP_EM.h"
#include "ATRAP_Trap_Quantities.h"
#include "Prop_Defs.h"

namespace aae_hand_nms
{
    /* Exception for functions in this header */
    class aae_hand_exc {
    public:
        aae_hand_exc(std::string& msg): err_message{msg} {}
        aae_hand_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <class Illum_T, class Decay_T, class SSAH_T>
    struct atomic_array_evolution_handler
    {
        std::vector<std::string> val_str;
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        double t_i;
        pdef_nms::state_t pos_vel_i;
        
        double t_f;
        pdef_nms::state_t pos_vel_f;
        
        using vec_t = bvec_nms::basic_vec<>;
        using mat_t = std::array<vec_t, 3>;
        
        /* Basis : {a_mag_ub, pee_ub, P_lb}, new_bds = bds_A.old_bds + bds_b */
        mat_t bds_A;
        vec_t bds_b;
        
        Illum_T ill;
        Decay_T dec;
        SSAH_T ssah;
        
        double dt_nrm;
        double dt;
        
        double buffer;
        
        void fill(const boost::filesystem::path&, const std::array<int, 3>&);
        
        bool in_beam(const pdef_nms::state_t& pos_vel) const {return ill.in_beam(pos_vel.first, buffer);}
        void initialize(bool in_gr_st = true) {ill.initialize(in_gr_st);}
        
        template <ig_nms::interp_quant IQ, class KH_T>
        void do_steps_in_beam(KH_T&);
        
        template <ig_nms::interp_quant IQ, class KH_T>
        void do_steps_decay(KH_T&);
        
        template <class KH_T>
        void finish_and_add_to_final_states(KH_T&);
        
        template <ig_nms::interp_quant IQ, class KH_T>
        void build_params_arr(KH_T&);
    };
}

#include "Template_Impl/Atomic_Array_Evolution_Handler.hpp"

#endif /* ATOMIC_ARRAY_EVOLUTION_HANDLER_H */

