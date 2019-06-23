#ifndef STATE_SUMMARY_H
#define STATE_SUMMARY_H

#include "ATRAP_base.h"
#include "Prop_Defs.h"

namespace st_sum_nms
{
    /* Exception for functions in this header */
    class st_sum_exc {
    public:
        st_sum_exc(std::string& msg): err_message{msg} {}
        st_sum_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    struct state_summary
    {
        pdef_nms::state_t pos_vel;
        
        double t_end;
        global_nms::default_data_type E;
        
        int N;
        int F;
        int mF;
        
        size_t one_photon_decays;
        size_t two_photon_decays;
        bool ionized;
        
        bool out_of_bounds;
        
        size_t beam_passes;
        double t_in_beam;
        
        using serial_ss_t = std::array<global_nms::default_data_type, 17>;
        serial_ss_t to_arr() const;
    };
    
    std::vector<state_summary> extract_state_summaries(const boost::filesystem::path&, prt_nms::partition = prt_nms::partition{});
    void print_state_summary(const std::vector<state_summary>&, const boost::filesystem::path&, prt_nms::partition = prt_nms::partition{});
}

#endif /* STATE_SUMMARY_H */

