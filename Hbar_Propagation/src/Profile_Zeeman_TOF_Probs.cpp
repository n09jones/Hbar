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

    double Ez_E_min;
    double dEz_E;

    double KE_Ez_E_min;
    double KE_dEz_E;

    double Ez_min;
    double dEz;

    double vz_min;
    double dvz;

    double z_min;
    double dz;

    mem_arr_nms::mem_arr<> Zeeman;
    mem_arr_nms::mem_arr<> vels;

    mem_arr_nms::mem_arr<> Ez_E;
    mem_arr_nms::mem_arr<> KE_Ez_E;
    std::array<std::array<std::array<double, 303>, 401>, 1001> Ez_vz_z;
    std::array<std::array<double, 3001>, 3021> zKE_Ez_E;


    bvec_nms::basic_vec<> focus;
    bvec_nms::basic_vec<> khat;

    boost::filesystem::path Z_fn;
    boost::filesystem::path v_fn;
    boost::filesystem::path Ez_E_fn;
    boost::filesystem::path KE_Ez_E_fn;
    boost::filesystem::path zKE_Ez_E_fn;
    boost::filesystem::path Ez_vz_z_fn;


    rad_finder(bvec_nms::basic_vec<> focus_in, int k_idx, double r, std::string coil, std::string cunc, size_t idx) : Zeeman {10000000, 0.0}, vels {50000, 0.0}, Ez_E {3001, 0.0}, KE_Ez_E {3001, 0.0}
    {
        r_sq = r*r;

        focus = focus_in;
        khat = {0.0, 0.0, 0.0};
        khat[k_idx] = 1.0;

        Z_min = -1.0e6;
        dZ = 1.0;

        vel_min = 0.0;
        dvel = 0.01;

        Ez_E_min = -1.0;
        dEz_E = 0.001;

        KE_Ez_E_min = -1.0;
        KE_dEz_E = 0.001;

	Ez_min = -0.1;
	dEz = 0.001;

        vz_min = -200.;
        dvz = 1.0;

        z_min = -0.151;
        dz = 0.001;

	std::string out_str {coil + "_" + cunc + "_" + ((k_idx == 0) ? "x" : "z") + "_" + std::to_string(static_cast<int>((r*(1.0e6)) + 0.5)) + "_" + std::to_string(idx) + ".bin"};
	boost::filesystem::path dir {global_nms::large_storage_dir / boost::filesystem::path{"Propagation_Data_4"}};

	Z_fn = (dir / boost::filesystem::path{"Zeeman_" + out_str});
	v_fn = (dir / boost::filesystem::path{"velocity_" + out_str});
	Ez_E_fn = (dir / boost::filesystem::path{"Ez_E_" + out_str});
	KE_Ez_E_fn = (dir / boost::filesystem::path{"KE_Ez_E_" + out_str});
	zKE_Ez_E_fn = (dir / boost::filesystem::path{"zKE_Ez_E_" + out_str});
        Ez_vz_z_fn =(dir / boost::filesystem::path{"Ez_vz_z_" + out_str});
    }

    void operator() (kh_t& kh, aseh_t& aseh)
    {
        if(norm_sq(cross_pr(kh.pos_vel.first - focus, khat)) > r_sq) {return;}

	if(update_quants) {
		aseh.ill.update(kh.pos_vel);
		update_quants = false;
	}

	auto Z {aseh.ill.Zeeman_fact() - aseh.ill.det_0};
	auto v {norm(cross_pr(kh.pos_vel.second, khat))};

        auto z {inner_pr(kh.pos_vel.first, khat)};
        auto vz {inner_pr(kh.pos_vel.second, khat)};
        auto E {kh.eh.get_gr_E(kh.pos_vel)};
        auto Ez {E - (0.5*(pvsi_nms::mass_e + pvsi_nms::mass_p)*v*v/pvsi_nms::k_B)};

	auto KE_rat {vz*vz/norm_sq(kh.pos_vel.second)};

	Zeeman[static_cast<size_t>((Z - Z_min)/dZ)] += kh.dt;
	vels[static_cast<size_t>((v - vel_min)/dvel)] += kh.dt;

        Ez_E[static_cast<size_t>(((Ez/E) - Ez_E_min)/dEz_E)] += kh.dt;
        KE_Ez_E[static_cast<size_t>((KE_rat - KE_Ez_E_min)/KE_dEz_E)] += kh.dt;
        zKE_Ez_E[static_cast<size_t>((z - z_min)/(dz/10.))][static_cast<size_t>((KE_rat - KE_Ez_E_min)/KE_dEz_E)] += kh.dt;
        Ez_vz_z[static_cast<size_t>((Ez - Ez_min)/dEz)][static_cast<size_t>((vz - vz_min)/dvz)][static_cast<size_t>((z - z_min)/dz)] += kh.dt;
    }

    void print()
    {
	print_to_file(Zeeman, Z_fn);
	print_to_file(vels, v_fn);
        print_to_file(Ez_E, Ez_E_fn);
        print_to_file(KE_Ez_E, KE_Ez_E_fn);

        size_t idx {0};
        mem_arr_nms::mem_arr<> Ez_vz_z_mem_arr {(Ez_vz_z.size())*(Ez_vz_z[0].size())*(Ez_vz_z[0][0].size())};
        for(const auto& lev1 : Ez_vz_z) {
	    for(const auto& lev2 : lev1) {
	        for(const auto& lev3 : lev2) {
		    Ez_vz_z_mem_arr[idx++] = lev3;
                }
	    }
	}
        print_to_file(Ez_vz_z_mem_arr, Ez_vz_z_fn);

        idx = 0;
        mem_arr_nms::mem_arr<> zKE_Ez_E_mem_arr {(zKE_Ez_E.size())*(zKE_Ez_E[0].size())};
        for(const auto& lev1 : zKE_Ez_E) {
	    for(const auto& lev2 : lev1) {
		zKE_Ez_E_mem_arr[idx++] = lev2;
	    }
	}
        print_to_file(zKE_Ez_E_mem_arr, zKE_Ez_E_fn);

    }
};

template <ig_nms::interp_quant IQ>
void run_sim(kh_t& kh, aseh_t& aseh, std::vector<rad_finder>& rf_vec, size_t IC_idx)
{
double t_next {1.0};
    for(kh.t = 0.0; ((kh.t < kh.t_lim) && !(kh.out_of_bounds())); kh.t += kh.dt)
    {
while(kh.t >= t_next) {std::cout << "t = " << kh.t << std::endl; t_next += 1.0;}
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
//    std::vector<double> r_vec {100.e-6, 250.e-6, 0.001, 0.003};
    std::vector<double> r_vec {0.001};

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

       for(int k_idx : k_idx_vec) {
            for(double r : r_vec) {
                rf_vec.push_back({aseh.ill.paths[0].focus, k_idx, r, coil, cunc, part_idx});
            }
	}

    for(size_t IC_idx = 0; IC_idx < kh.ICs.size(); ++IC_idx)
    {
        auto wcts = std::chrono::system_clock::now();
        std::cout << "Starting simulation for particle " << (IC_idx+1) << " of " << kh.ICs.size() << std::endl;
        pick_and_run_sum(kh, aseh, rf_vec, IC_idx);
        std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
        std::cout << "Finished " << kh.t_lim << " s of simulation time in " << wctduration.count() << " s of real time" << std::endl;
    }
        for(auto& elem : rf_vec) {elem.print();}

}

int main(int argc, char ** argv)
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_t::output_files, kh_t::write_opt_file_template, kh_t::print_out_mem);
    
    return 0;
}

