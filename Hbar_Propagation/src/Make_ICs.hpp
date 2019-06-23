const global_nms::default_data_type pos_r {0.001};
const global_nms::default_data_type pos_hL {0.015};

const global_nms::default_data_type pos_T_K {20.0};
const global_nms::default_data_type pos_density {5.0e13};

constexpr size_t IC_num {1000};
using kh_t = kin_hand_nms::kinematic_handler<true, kin_hand_nms::simple_integrator, IC_num>;

bvec_nms::basic_vec<> rand_position()
{
    auto func {
        [pos_r = pos_r, pos_hL = pos_hL](auto x) {
            bvec_nms::basic_vec<> y {x[0]/pos_r, x[1]/pos_r, (x[2] - pos_off_z)/pos_hL};
            return ((norm_sq(y) <= 1) ? 1 : -1);
        }
    };
    
    auto x {distr_gen::rand<3>(func, {{{{-pos_r, pos_r}}, {{-pos_r, pos_r}}, {{-pos_hL + pos_off_z, pos_hL + pos_off_z}}}}, 0.5)};
    
    return bvec_nms::basic_vec<>{x[0], x[1], x[2]};
}

template <class KH_T>
bvec_nms::basic_vec<> rand_velocity(KH_T& kh)
{
    std::array<global_nms::default_data_type, 1> Bnorm_arr {{0.0}};
    kh.template EM_quant<ig_nms::interp_quant::Bnorm>(Bnorm_arr, kh.pos_vel.first);

    using namespace pvsi_nms;
    auto const_coeff {(mass_p + mass_e)/(2*k_B*pos_T_K)};
    auto ang_rot_freq {q_fund*pos_density*mu_0*c*c/(2*Bnorm_arr[0])};
    auto ang_rot_vel {ang_rot_freq*cross_pr(kh.pos_vel.first, (-1.0)*bvec_nms::unit_vec(2))};

    auto func {
        [const_coeff](auto v) {
            return std::exp(-const_coeff*std::inner_product(v.begin(), v.end(), v.begin(), 0.0));
        }
    };
    
    double ax_v_lim {300.0};
    double tr_v_lim {ax_v_lim + (q_fund*pos_density*mu_0*c*c*pos_r/(2*kh.Bnorm_min))};
    auto v {distr_gen::rand<3>(func, {{{{-tr_v_lim, tr_v_lim}}, {{-tr_v_lim, tr_v_lim}}, {{-ax_v_lim, ax_v_lim}}}}, 1.0)};

    return (bvec_nms::basic_vec<>{v[0], v[1], v[2]} + ang_rot_vel);
}

template <ig_nms::interp_quant IQ, class KH_T>
void run_sim(KH_T& kh)
{
    for(kh.t = 0.0; ((kh.t < kh.t_lim) && !(kh.out_of_bounds())); kh.t += kh.dt) {kh.template do_step<IQ>();}
}

boost::filesystem::path out_fn(const boost::filesystem::path& sing_fn, bool ini)
{
    auto out_dir {sing_fn.parent_path()};
    auto out_stem {sing_fn.stem().string()};
    auto out_ext {sing_fn.extension().string()};

    return (out_dir / boost::filesystem::path{out_stem + (ini ? "_init" : "_fin") + out_ext});
}

void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    kh_t kh;
    kh.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    std::cout << "Loaded" << std::endl;
    
    std::string status_str;
    size_t ct_tot_steps {0};
    std::vector<st_sum_nms::state_summary> ini_sts;
    std::vector<st_sum_nms::state_summary> fin_sts;
    while(ini_sts.size() < kh.part.get_elems_in_part()/kh.part.get_chunk_size())
    {
        ++ct_tot_steps;
        kh.reset_out_of_bnds();
        auto tmp_pos {rand_position()};
	kh.pos_vel.first = tmp_pos;
        auto tmp_vel {rand_velocity(kh)};
	kh.pos_vel.second = tmp_vel;
        
        if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
            run_sim<ig_nms::interp_quant::acc_1Sc>(kh);
        }
        else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
            run_sim<ig_nms::interp_quant::acc_1Sd>(kh);
        }
        else {
            throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
        }
        
        if(!(kh.out_of_bounds()))
        {
	    ini_sts.push_back({});
	    ini_sts.back().pos_vel = {tmp_pos, tmp_vel};
	    ini_sts.back().E = kh.eh.get_gr_E(ini_sts.back().pos_vel);

	    fin_sts.push_back({});
	    fin_sts.back().pos_vel = kh.pos_vel;
	    fin_sts.back().E = kh.eh.get_gr_E(fin_sts.back().pos_vel);

            status_str = "Completed " + std::to_string(ini_sts.size()) + " of " + std::to_string(kh.part.get_elems_in_part()/kh.part.get_chunk_size());
            status_str += " (in " + std::to_string(ct_tot_steps) + " total steps)";
            std::cout << status_str << std::endl;
        }
    }
    
    std::cout << std::endl;

    st_sum_nms::print_state_summary(ini_sts, out_fn(kh.out_fn, true), kh.part);
    st_sum_nms::print_state_summary(fin_sts, out_fn(kh.out_fn, false), kh.part);
}

std::vector<std::pair<boost::filesystem::path, bool>> output_files(const boost::filesystem::path& opts_fn)
{
    return std::vector<std::pair<boost::filesystem::path, bool>>{
	{out_fn(kh_t::output_files(opts_fn)[0].first, true), false},
	{out_fn(kh_t::output_files(opts_fn)[0].first, false), false}
    };
}

void print_out_mem(const boost::filesystem::path& opts_fn)
{
    std::cout << "For each of the two output files : " << std::endl;
    kh_t::print_out_mem(opts_fn);
}

int main(int argc, char ** argv)
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, output_files, kh_t::write_opt_file_template, print_out_mem);
    
    return 0;
}



