#ifndef DECAY_HANDLER_H
#define DECAY_HANDLER_H

#include "ATRAP_base.h"
#include "Prop_Defs.h"
#include "Atomic_States.h"
#include "Local_Fields.h"
#include "Laser_Paths.h"
#include "TQ_Array_Manager.h"

namespace decay_nms
{
    /* Exception for functions in this header */
    class decay_exc {
    public:
        decay_exc(const std::string& msg): err_message{msg} {}
        decay_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    using theta_phi_t = std::array<global_nms::default_data_type, 2>;
    using dpm_coeff_t = std::array<global_nms::default_data_type, 6>;
    
    enum class decay_ch {dec_1ph, dec_2ph, dec_ion};
    
    global_nms::default_data_type em_unnormed_PDF(const dpm_coeff_t&, const theta_phi_t&);
    global_nms::default_data_type em_unnormed_PDF_bd(const dpm_coeff_t&);
    
    template <class U, class Func>
    U pick(const std::vector<U>&, Func&&);
    
    bvec_nms::basic_vec<> theta_phi_vec(const theta_phi_t&);
    bvec_nms::basic_vec<> norm_em_recoil(const dpm_coeff_t&);
    
    template <pdef_nms::illum_type IT, class EM_Quant_Gen = emq_nms::grid_EM_quants<>>
    struct decay_handler :
    loc_fld_nms::local_EM_fields<IT, EM_Quant_Gen>,
    las_path_nms::path_mngr
    {
        /* States */
        asts_nms::S_gr decay_st;
        typename std::conditional<pdef_nms::is_LyAlph<IT>, asts_nms::P_ex, asts_nms::S_ex>::type ex_state;
        
        /* Decay statistics*/
        size_t one_ph_decays;
        size_t two_ph_decays;
        bool ionized;
        
        /* Most recent velocity kick */
        bvec_nms::basic_vec<> vel_kick;
        
        /* Arrays for decay */
        TQam_nms::arr_mngr<> decay_rt_1ph_fact;
        TQam_nms::arr_mngr<> decay_rt_2ph_fact;
        TQam_nms::arr_mngr<> ion_rt_fact;
        
        std::map<asts_nms::S_gr, TQam_nms::arr_mngr<>> trans_freqs;
        
        std::map<asts_nms::S_gr, TQam_nms::arr_mngr<>> br_ratios_1ph;
        std::map<asts_nms::S_gr, TQam_nms::arr_mngr<>> br_ratios_2ph;
        
        std::map<asts_nms::S_gr, TQam_nms::arr_mngr<dpm_coeff_t>> DPM_coeffs_1ph;
        std::map<asts_nms::S_gr, TQam_nms::arr_mngr<dpm_coeff_t>> DPM_coeffs_2ph;
        
        TQam_nms::arr_mngr<> two_ph_E_distr;
        
        /* Derived quantity of interest */
        std::map<asts_nms::S_gr, global_nms::default_data_type> vel_kick_mags;
        std::map<decay_ch, global_nms::default_data_type> br_ratios;
        
        /* Fill structure and update its contents for new fields */
        void fill(const boost::filesystem::path&);
        void update(const pdef_nms::state_t&);
        void update_derived_quants(const pdef_nms::state_t&);
        
        /* Manages decays of different types */
        void execute_decay_chain(const pdef_nms::state_t&);
        
        decay_ch choose_channel();
        void execute_one_ph_decay(const pdef_nms::state_t&);
        void execute_two_ph_decay();
    };
}

#include "Template_Impl/Decay_Handler.hpp"

#endif /* DECAY_HANDLER_H */

