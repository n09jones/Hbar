#include <iostream>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

bool update_quants;

using EM_Quant_Gen = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;

using kh_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

using ill_t = ill_hand_nms::illumination_handler<pdef_nms::illum_type::TwoPh, EM_Quant_Gen>;
using dec_t = decay_nms::decay_handler<pdef_nms::illum_type::TwoPh, EM_Quant_Gen>;
using aseh_t = ase_hand_nms::atomic_state_evolution_handler<ill_t, dec_t>;

struct rad_finder
{
    double r_sq;

    double Z_min;
    double dZ;

    double vel_min;
    double dvel;

    mem_arr_nms::mem_arr<> Zeeman;
    mem_arr_nms::mem_arr<> vels;

    bvec_nms::basic_vec<> focus;
    bvec_nms::basic_vec<> khat;

    boost::filesystem::path Z_fn;
    boost::filesystem::path v_fn;

    rad_finder(bvec_nms::basic_vec<> focus_in, int k_idx, double r, std::string coil, std::string cunc, size_t idx) : Zeeman {10000000, 0.0}, vels {50000, 0.0}
    {
        r_sq = r*r;

        focus = focus_in;
        khat = {0.0, 0.0, 0.0};
        khat[k_idx] = 1.0;

        Z_min = -1.0e6;
        dZ = 1.0;

        vel_min = 0.0;
        dvel = 0.01;

	std::string out_str {coil + "_" + cunc + "_" + ((k_idx == 0) ? "x" : "z") + "_" + std::to_string(static_cast<int>((r*(1.0e6)) + 0.5)) + "_" + std::to_string(idx) + ".bin"};
	boost::filesystem::path dir {global_nms::large_storage_dir / boost::filesystem::path{"Propagation_Data"}};

	Z_fn = (dir / boost::filesystem::path{"Zeeman_" + out_str});
	v_fn = (dir / boost::filesystem::path{"velocity_" + out_str});
    }

    void operator() (kh_t& kh, aseh_t& aseh)
    {
        if(norm_sq(cross_pr(kh.pos_vel.first - focus, khat)) > r_sq) {return;}

	if(update_quants) {
		aseh.ill.update(kh.pos_vel);
		update_quants = false;
	}

	auto Z {aseh.ill.Zeeman_fact() - aseh.ill.det_0};
	auto v {norm(kh.pos_vel.second)};

	Zeeman[static_cast<size_t>((Z - Z_min)/dZ)] += kh.dt;
	vels[static_cast<size_t>((v - vel_min)/dvel)] += kh.dt;
    }

    void print()
    {
	print_to_file(Zeeman, Z_fn);
	print_to_file(vels, v_fn);
    }
};

template <ig_nms::interp_quant IQ>
void run_sim(kh_t& kh, aseh_t& aseh, std::vector<rad_finder>& rf_vec, size_t IC_idx)
{
    for(kh.t = 0.0; ((kh.t < kh.t_lim) && !(kh.out_of_bounds())); kh.t += kh.dt)
    {
	update_quants = true;
	for(auto& elem : rf_vec) {elem(kh, aseh);}
	kh.template do_step<IQ>();
    }
}

void pick_and_run_sum(kh_t& kh, aseh_t& aseh, std::vector<rad_finder>& rf_vec, size_t IC_idx)
{
    kh.pos_vel = kh.ICs[IC_idx].pos_vel;
    kh.reset_out_of_bnds();
    kh.t = 0.0;
    
    if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sc>(kh, aseh, rf_vec, IC_idx);
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sd>(kh, aseh, rf_vec, IC_idx);
    }
    else {
        throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
    }
}

void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    kh_t kh;
    aseh_t aseh;
    std::vector<rad_finder> rf_vec;

    kh.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    aseh.fill(jm_nms::get_opts_fn(argc, argv));

    std::vector<int> k_idx_vec {0, 2};
    std::vector<double> r_vec {100.e-6, 250.e-6, 0.001, 0.003};

    std::string coil;
    std::string cunc;

    std::vector<std::string> coil_search {"Straight_Oct", "Oct", "Quad"};
    std::vector<std::array<std::string, 2>> cunc_search {{{"Doppler", "DL"}}, {{"_cooled", "C"}}, {{"uncooled", "U"}}};

    for(auto str : coil_search) {
	if(kh.IC_fn.stem().string().find(str) != std::string::npos) {coil = str; break;}
    }

    for(auto strs : cunc_search) {
	if(kh.IC_fn.stem().string().find(strs[0]) != std::string::npos) {cunc = strs[1]; break;}
    }

std::cout << coil << " " << cunc << std::endl;


    for(size_t IC_idx = 0; IC_idx < kh.ICs.size(); ++IC_idx)
    {
        rf_vec.clear();

        for(int k_idx : k_idx_vec) {
            for(double r : r_vec) {
                rf_vec.push_back({aseh.ill.paths[0].focus, k_idx, r, coil, cunc, part_idx});
            }
        }

        auto wcts = std::chrono::system_clock::now();
        std::cout << "Starting simulation for particle " << (IC_idx+1) << " of " << kh.ICs.size() << std::endl;
        pick_and_run_sum(kh, aseh, rf_vec, IC_idx);
        std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
        std::cout << "Finished " << kh.t_lim << " s of simulation time in " << wctduration.count() << " s of real time" << std::endl;

        for(auto& elem : rf_vec) {elem.print();}
    }

}

int main(int argc, char ** argv)
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_t::output_files, kh_t::write_opt_file_template, kh_t::print_out_mem);
    
    return 0;
}

