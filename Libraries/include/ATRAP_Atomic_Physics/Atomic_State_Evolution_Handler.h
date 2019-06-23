#ifndef ATOMIC_STATE_EVOLUTION_HANDLER_H
#define ATOMIC_STATE_EVOLUTION_HANDLER_H

#include "ATRAP_base.h"
#include "ATRAP_EM.h"
#include "Prop_Defs.h"

namespace ase_hand_nms
{
    /* Exception for functions in this header */
    class ase_hand_exc {
    public:
        ase_hand_exc(std::string& msg): err_message{msg} {}
        ase_hand_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    enum class scatt_status {in_cycle, out_cycle};
    
    template <class Illum_T, class Decay_T>
    struct atomic_state_evolution_handler
    {
        std::vector<std::string> val_str;
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        Illum_T ill;
        Decay_T dec;
        
        double dt_nrm;
        double dt;
        
        double buffer;
        
        void fill(const boost::filesystem::path&);
        
        bool in_beam(const pdef_nms::state_t& pos_vel) const {return ill.in_beam(pos_vel.first, buffer);}
        void initialize(bool in_gr_state = true) {ill.initialize(in_gr_state);}
        scatt_status do_step(pdef_nms::state_t&);
    };
}

#include "Template_Impl/Atomic_State_Evolution_Handler.hpp"

#endif /* ATOMIC_STATE_EVOLUTION_HANDLER_H */

