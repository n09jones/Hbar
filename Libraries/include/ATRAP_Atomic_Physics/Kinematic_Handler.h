#ifndef KINEMATIC_HANDLER_H
#define KINEMATIC_HANDLER_H

#include <boost/numeric/odeint.hpp>
#include "ATRAP_base.h"
#include "ATRAP_EM.h"
#include "EM_Quants.h"
#include "Prop_Defs.h"
#include "State_Summary.h"

namespace kin_hand_nms
{
    /* Exception for functions in this header */
    class kin_hand_exc {
    public:
        kin_hand_exc(std::string& msg): err_message{msg} {}
        kin_hand_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    /* Custom integrator */
    struct simple_integrator
    {
        simple_integrator() : acc_set {false}, acc {0, 0, 0} {}
        
        bool acc_set;
        bvec_nms::basic_vec<> acc;
        
        template <class F>
        void do_step(F&&, pdef_nms::state_t&, double, double);
    };
    
    /* kinematic_handler */
    template <bool Symplectic, class Step_T, size_t Init_Num = 0, class EM_Quant_Gen = emq_nms::grid_EM_quants<>>
    struct kinematic_handler :
    EM_Quant_Gen
    {
        std::string header() {return "KINEMATIC PARAMETERS";}
        std::vector<std::string> val_str;
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        static void write_opt_file_template(const boost::filesystem::path&);
        static void print_out_mem(const boost::filesystem::path&);
        static std::vector<std::pair<boost::filesystem::path, bool>> output_files(const boost::filesystem::path&);
        
        Step_T stepper;
        en_hand_nms::energy_handler<EM_Quant_Gen> eh;
        
        boost::filesystem::path IC_fn;
        boost::filesystem::path out_fn;
        
        prt_nms::partition part;
        std::vector<st_sum_nms::state_summary> ICs;
        std::vector<st_sum_nms::state_summary> final_states;
        
        
        asts_nms::S_gr gr_state;
        
        double t;
        double dt;
        double t_lim;
        
        pdef_nms::state_t pos_vel;
        
        void fill(const boost::filesystem::path&, size_t, size_t);
        
        void reset_out_of_bnds() {EM_Quant_Gen::reset_out_of_bnds();}
        bool out_of_bounds() {return EM_Quant_Gen::out_of_bounds();}
        template <ig_nms::interp_quant>
        void do_step();
        
        template <ig_nms::interp_quant, bool is_Symplectic = Symplectic, class = std::enable_if_t<is_Symplectic>>
        void ODE_sys(const bvec_nms::basic_vec<>&, bvec_nms::basic_vec<>&);
        template <ig_nms::interp_quant, bool is_Symplectic = Symplectic, class = std::enable_if_t<!is_Symplectic>, class = void>
        void ODE_sys(const pdef_nms::state_t&, pdef_nms::state_t&, const double);
        
        void print_contents();
    };
}

#include "Template_Impl/Kinematic_Handler.hpp"

#endif /* KINEMATIC_HANDLER_H */

