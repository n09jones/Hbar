#ifndef ILLUMINATION_HANDLER_H
#define ILLUMINATION_HANDLER_H

#include <boost/numeric/odeint.hpp>
#include "ATRAP_base.h"
#include "Prop_Defs.h"
#include "Atomic_States.h"
#include "Local_Fields.h"
#include "Laser_Properties.h"
#include "Laser_Paths.h"
#include "TQ_Array_Manager.h"

namespace ill_hand_nms
{
    template <pdef_nms::illum_type IT, class EM_Quant_Gen = emq_nms::grid_EM_quants<>>
    struct illumination_handler_base :
    loc_fld_nms::local_EM_fields<IT, EM_Quant_Gen>,
    las_props_nms::laser_properties,
    las_path_nms::path_mngr
    {
        asts_nms::S_gr gr_state;
        typename std::conditional<pdef_nms::is_LyAlph<IT>, asts_nms::P_ex, asts_nms::S_ex>::type ex_state;
        
        /* Arrays for state evolution */
        TQam_nms::arr_mngr<> decay_rt_1ph_fact;
        TQam_nms::arr_mngr<> decay_rt_2ph_fact;
        TQam_nms::arr_mngr<> ion_rt_fact;
        
        using Rabi_fact_t = typename std::conditional<pdef_nms::is_LyAlph<IT>,
        std::array<std::complex<global_nms::default_data_type>, 3>,
        std::array<std::complex<global_nms::default_data_type>, 1>>::type;
        
        TQam_nms::arr_mngr<Rabi_fact_t> Rabi_freq_fact;
        
        TQam_nms::arr_mngr<> Zeeman_fact;
        TQam_nms::arr_mngr<> AC_Stark_fact;
        TQam_nms::arr_mngr<> DC_Stark_fact;
        
        /* Laser FWHM in Hz */
        global_nms::default_data_type FWHM;
        /* Drive (1 or 2 times the laser frequency) detuning from resonance at B = 0 */
        global_nms::default_data_type det_0;
        /* Laser frequency (times 2 if 1S-2S) */
        global_nms::default_data_type freq_drive;
        
        /* Rate of loss thru decay + ionization */
        global_nms::default_data_type prob_decay_rate;
        
        /* Vector of tuples {Rabi frequency, detuning (omega_laser - omega_atom), accumulated_phase} */
        using Rabi_det_t = std::vector<std::pair<std::complex<global_nms::default_data_type>, global_nms::default_data_type>>;
        Rabi_det_t Rabi_freq_Doppler_by_path;
        
        /* Compute Doppler shift */
        global_nms::default_data_type Doppler_shift(const bvec_nms::basic_vec<>&, const las_path_nms::laser_path&);
        
        void fill(const boost::filesystem::path&);
        void update(const pdef_nms::state_t&);
        void update_derived_quants(const pdef_nms::state_t&);
    };
    
    template <pdef_nms::illum_type IT, class EM_Quant_Gen = emq_nms::grid_EM_quants<>>
    struct illumination_handler :
    illumination_handler_base<IT, EM_Quant_Gen>
    {
        using d_mat_t = std::array<global_nms::default_data_type, 4>;
        enum dmat_comp {u, v, w, P};
        
        d_mat_t d_mat;
        std::vector<global_nms::default_data_type> acc_phases;
        global_nms::default_data_type decay_threshold;
        global_nms::default_data_type excitation_threshold;
        
        void fill(const boost::filesystem::path opts_fn) {
            illumination_handler_base<IT, EM_Quant_Gen>::fill(opts_fn);
        }
        
        void initialize(bool = true);
        void operator() (const d_mat_t&, d_mat_t&, const double);
        
        using null_obs_t = boost::numeric::odeint::null_observer;
        template <class Func = null_obs_t>
        double integration_step(const pdef_nms::state_t&, double, Func&& = null_obs_t{});
        
        bool is_excited() {
            return ((d_mat[P] > 0.0) && ((d_mat[w] + d_mat[P])/(2.0*d_mat[P]) > excitation_threshold));
        }
    };
}

#include "Template_Impl/Illumination_Handler.hpp"

#endif /* ILLUMINATION_HANDLER_H */

