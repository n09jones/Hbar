#include <iostream>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

double time_time;

constexpr pdef_nms::illum_type it_const {pdef_nms::illum_type::TwoPh};

using EM_Quant_Gen = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;

using kh_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

using ill_t = ill_arr_hand_nms::illumination_array_handler<it_const, EM_Quant_Gen>;
using dec_t = decay_arr_nms::decay_array_handler<it_const, EM_Quant_Gen>;
using ssah_t = st_sum_arr_hand_nms::st_sum_arr_hand;
using aaeh_t = aae_hand_nms::atomic_array_evolution_handler<ill_t, dec_t, ssah_t>;

using func_ptr_t = void (*)(kh_t&, aaeh_t&);
func_ptr_t next_func {nullptr};

template <ig_nms::interp_quant IQ> void run_reg(kh_t& kh, aaeh_t& aaeh);
template <ig_nms::interp_quant IQ> void in_beam_func(kh_t& kh, aaeh_t& aaeh);
template <ig_nms::interp_quant IQ> void decaying_func(kh_t& kh, aaeh_t& aaeh);

template <ig_nms::interp_quant IQ>
void run_reg(kh_t& kh, aaeh_t& aaeh)
{
    for(; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && !(aaeh.in_beam(kh.pos_vel))); kh.t += kh.dt) {
while(kh.t >= time_time) {std::cout << "t = " << kh.t << ", " << prop_num << " of " << poss_num << std::endl; ++time_time;}
        kh.template do_step<IQ>();
    }
    if((kh.t >= kh.t_lim) || kh.out_of_bounds()) {next_func = nullptr;}
    else 
    {
        next_func = in_beam_func<IQ>;
        aaeh.initialize();
    }
}

template <ig_nms::interp_quant IQ>
void in_beam_func(kh_t& kh, aaeh_t& aaeh)
{
    ++(aaeh.ill.beam_passes);
    
    aaeh.do_steps_in_beam<IQ>(kh);
    aaeh.ill.check_for_excitation();    
    
    if((kh.t >= kh.t_lim) || kh.out_of_bounds() || (aaeh.ill.active_members == 0)) {next_func = nullptr;}
    else if(aaeh.ill.decaying_idxs.size())
    {
        next_func = decaying_func<IQ>;
        aaeh.initialize(false);
    }
    else {next_func = run_reg<IQ>;}
}

template <ig_nms::interp_quant IQ>
void decaying_func(kh_t& kh, aaeh_t& aaeh)
{
    aaeh.do_steps_decay<IQ>(kh);

    if((kh.t >= kh.t_lim) || kh.out_of_bounds() || (aaeh.ill.active_members == 0)) {next_func = nullptr;}
    else if(aaeh.in_beam(kh.pos_vel)) {next_func = in_beam_func<IQ>;}
    else {next_func = run_reg<IQ>;}
}

void pick_and_run_sum(kh_t& kh, aaeh_t& aaeh, size_t IC_idx)
{
    kh.pos_vel = kh.ICs[IC_idx].pos_vel;
    kh.reset_out_of_bnds();
    kh.t = 0.0;
    if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
        for(next_func = run_reg<ig_nms::interp_quant::acc_1Sc>; next_func;) {next_func(kh, aaeh);}
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        for(next_func = run_reg<ig_nms::interp_quant::acc_1Sd>; next_func;) {next_func(kh, aaeh);}
    }
    else {
        throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
    }
}

void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    kh_t kh;
    aaeh_t aaeh;
    std::array<int, 3> det_lims {{-50000, 50000, 50}};
    
    kh.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    std::cout << "Loaded" << std::endl;
    std::cout <<  "Minimum B : " << kh.Bnorm_min << std::endl;
    
    for(size_t IC_idx = 0; IC_idx < kh.ICs.size(); ++IC_idx)
    {
time_time = 0.0;
     	aaeh.fill(jm_nms::get_opts_fn(argc, argv), det_lims);
        auto wcts = std::chrono::system_clock::now();
        std::cout << "Starting simulation for particle " << (IC_idx+1) << " of " << kh.ICs.size() << std::endl;

        pick_and_run_sum(kh, aaeh, IC_idx);
        aaeh.finish_and_add_to_final_states(kh);

        std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
        std::cout << "Finished " << kh.t_lim << " s of simulation time in " << wctduration.count() << " s of real time" << std::endl;
    }
    
    kh.print_contents();
}

int main(int argc, char ** argv)
{
try
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_t::output_files, kh_t::write_opt_file_template, kh_t::print_out_mem);
}
catch(gqt_nms::gqt_exc& exc) {
std::cout << exc.give_err() << std::endl;
}    
    return 0;
}

