#ifndef LOCAL_FIELDS_H
#define LOCAL_FIELDS_H

#include "ATRAP_base.h"
#include "Prop_Defs.h"
#include "EM_Quants.h"
#include "Laser_Paths.h"

namespace loc_fld_nms
{
    template <pdef_nms::illum_type IT, class EM_Quant_Gen = emq_nms::grid_EM_quants<>>
    struct local_EM_fields :
    EM_Quant_Gen
    {
        /* Data */
        global_nms::default_data_type Bnorm;
        bvec_nms::basic_vec<> B;
        bvec_nms::basic_vec<> E;
        global_nms::default_data_type E_sq;
        
        global_nms::default_data_type intensity;
        std::array<std::complex<global_nms::default_data_type>, 3> loc_pol;
        
        /* Load path-independent fields */
        void load_for_illumination(const pdef_nms::state_t&);
        void load_B_fields(const pdef_nms::state_t&);
        void compute_E_fields(const pdef_nms::state_t&);
        
        /* Load path-dependent fields */
        void load_per_path(const pdef_nms::state_t&, const las_path_nms::laser_path&);
        void compute_intensity(const pdef_nms::state_t&, const las_path_nms::laser_path&);
        void compute_loc_pol(const las_path_nms::laser_path&);
    };
}

#include "Template_Impl/Local_Fields.hpp"

#endif /* LOCAL_FIELDS_H */

