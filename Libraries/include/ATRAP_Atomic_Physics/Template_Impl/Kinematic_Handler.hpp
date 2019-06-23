#ifndef KINEMATIC_HANDLER_HPP
#define KINEMATIC_HANDLER_HPP

#include "Template_Options_File_Printer.h"

/* Custom integrator */
template <class F>
inline void kin_hand_nms::simple_integrator::do_step(F&& sys, pdef_nms::state_t& pos_vel, double /* t */, double dt)
{
    if(!acc_set)
    {
        acc_set = true;
        sys(pos_vel.first, acc);
    }
    
    pos_vel.first += (dt*pos_vel.second) + (0.5*dt*dt*acc);
    pos_vel.second += 0.5*dt*acc;
    
    sys(pos_vel.first, acc);
    
    pos_vel.second += 0.5*dt*acc;
}


/* kinematic_handler */
template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
void kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::fill(const boost::filesystem::path& opts_fn, size_t tot_parts, size_t part_idx)
{
    this->initialize(opts_fn);
    gqt_nms::fill_params(opts_fn, *this, val_str);
    gr_state.fill(opts_fn);
    eh.fill(opts_fn);
    
    size_t chunk_size {std::tuple_size<st_sum_nms::state_summary::serial_ss_t>::value};
    if(Init_Num)
    {
        ICs.clear();
        part = prt_nms::partition{Init_Num*chunk_size, tot_parts, part_idx, chunk_size};
    }
    else
    {
        part = prt_nms::partition{fio_nms::data_len<global_nms::default_data_type>(IC_fn), tot_parts, part_idx, chunk_size};
        ICs = st_sum_nms::extract_state_summaries(IC_fn, part);
    }
    
    final_states.clear();
}

template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
void kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::write_opt_file_template(const boost::filesystem::path& opts_fn)
{
    kinematic_handler tmp_kh;
    EM_Quant_Gen::write_opt_file_template(opts_fn);
    tofp_nms::app_opt_file(opts_fn, tmp_kh);
    tofp_nms::app_opt_file(opts_fn, tmp_kh.gr_state);
}

template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
void kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::print_out_mem(const boost::filesystem::path& opts_fn)
{
    if(Init_Num) {
        size_t chunk_size {std::tuple_size<st_sum_nms::state_summary::serial_ss_t>::value};
        gqt_nms::print_memory(Init_Num*chunk_size*sizeof(global_nms::default_data_type));
        return;
    }
    
    kinematic_handler tmp_kh;
    gqt_nms::fill_params(opts_fn, tmp_kh, tmp_kh.val_str);
    gqt_nms::print_memory(fio_nms::data_len<global_nms::default_data_type>(tmp_kh.IC_fn)*sizeof(global_nms::default_data_type));
}

template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
std::vector<std::pair<boost::filesystem::path, bool>> kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::output_files(const boost::filesystem::path& opts_fn)
{
    kinematic_handler tmp_kh;
    gqt_nms::fill_params(opts_fn, tmp_kh, tmp_kh.val_str);
    return std::vector<std::pair<boost::filesystem::path, bool>>{{tmp_kh.out_fn, false}};
}

template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
std::vector<pid_nms::param_info> kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::get_param_info()
{
    std::vector<pid_nms::param_info> ret_vec
    {
        {
            "Time Per Step",
            "In seconds",
            gqt_nms::fnc_true,
            [this](std::string str){gqt_nms::str_to_data(str, &(this->dt));}
        },
        {
            "Total Simulated Time",
            "In seconds",
            gqt_nms::fnc_true,
            [this](std::string str){gqt_nms::str_to_data(str, &(this->t_lim));}
        }
    };
    
    if(!Init_Num)
    {
        ret_vec.push_back
        ({
            "Initial Condition File Path",
            "Relative to \"large_storage_dir\" in Global_Info.h.",
            gqt_nms::fnc_true,
            [this](std::string str){this->IC_fn = (global_nms::large_storage_dir / boost::filesystem::path{str});}
        });
    }
    
    ret_vec.push_back
    ({
        "Output File Path",
        "Relative to \"large_storage_dir\" in Global_Info.h.",
        gqt_nms::fnc_true,
        [this](std::string str){this->out_fn = (global_nms::large_storage_dir / boost::filesystem::path{str});}
    });
    
    return ret_vec;
}

template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
void kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::post_read_ops(boost::program_options::variables_map)
{
    if(dt <= 0) {throw kin_hand_exc{"Time per step must be positive"};}
    if(t_lim < 10.0*dt) {throw kin_hand_exc{"Total simulated time must be at least 10 times the time per step"};}
}

template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
template <ig_nms::interp_quant IQ>
void kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::do_step()
{
    auto ODE_sys_lambda {
        [this](const auto& x, auto& dxdt ,auto... args) {(this->template ODE_sys<IQ>)(x, dxdt, args...);}
    };
    stepper.do_step(ODE_sys_lambda, pos_vel, t, dt);
}


/* ODE_sys functions */
template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
template <ig_nms::interp_quant IQ, bool, class>
inline void kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::ODE_sys(const bvec_nms::basic_vec<>& q, bvec_nms::basic_vec<>& dpdt)
{
    this->template EM_quant<IQ>(dpdt, q);
}

template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
template <ig_nms::interp_quant IQ, bool, class, class>
inline void kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::ODE_sys(const pdef_nms::state_t& x, pdef_nms::state_t& dxdt, const double t)
{
    dxdt.first = x.second;
    this->template EM_quant<IQ>(dxdt.second, x.first, t);
}

template <bool Symplectic, class Step_T, size_t Init_Num, class EM_Quant_Gen>
void kin_hand_nms::kinematic_handler<Symplectic, Step_T, Init_Num, EM_Quant_Gen>::print_contents()
{
    st_sum_nms::print_state_summary(final_states, out_fn, part);
}

#endif /* KINEMATIC_HANDLER_HPP */

