#include <iostream>
#include "ATRAP_base.h"

bool is_visible(decltype(mapped_arr_nms::mapped_arr<>{}.cbegin()) it, size_t idx, global_nms::default_data_type z0)
{
	global_nms::default_data_type sum {0.0};
	std::vector<global_nms::default_data_type> offset {0.0, 0.0, z0};

	for(size_t k = 0; k < 3; ++k) {
		if(k != idx) {sum += (it[k] - offset[k])*(it[k] - offset[k]);}
	}

	return (sum < (0.001)*(0.001));
}

void process(std::string fn_stem, global_nms::default_data_type z0)
{
	boost::filesystem::path fn {global_nms::large_storage_dir / boost::filesystem::path{fn_stem}};
	mapped_arr_nms::mapped_arr<> arr {fn, false};

	std::vector<std::vector<global_nms::default_data_type>> visible {{}, {}, {}};
	auto it {arr.cbegin()};
	
	for(auto it = arr.cbegin(); it < arr.cend(); it += 17)
	{
		for(size_t idx = 0; idx < 3; ++idx)
		{
			if(is_visible(it, idx, z0))
			{
				for(size_t k = 0; k < 17; ++k) {visible[idx].push_back(it[k]);}
			}
		}
	}

	for(size_t idx = 0; idx < 3; ++idx)
	{
		mem_arr_nms::mem_arr<> tmp {visible[idx].size()};
		std::copy(visible[idx].cbegin(), visible[idx].cend(), tmp.begin());

		std::vector<std::string> str_vec {"x", "y", "z"};
		std::string out_fn_end {fn.stem().string() + "_" + str_vec[idx] + fn.extension().string()};
		boost::filesystem::path out_fn {fn.parent_path() / boost::filesystem::path{out_fn_end}};

		print_to_file(tmp, out_fn);
	}
}

int main(int argc, char ** argv)
{
	std::string dir {argv[1]};
	std::vector<std::string> trap {"Oct", "Quad", "Straight_Oct"};
	std::vector<global_nms::default_data_type> z0_vec {0.0226, 0.0, 0.0};
	std::vector<std::string> cunc_vec {"DL"};

	for(size_t trap_idx = 0; trap_idx < trap.size(); ++trap_idx) {
		for(size_t cunc_idx = 0; cunc_idx < cunc_vec.size(); ++cunc_idx) {
			std::cout << "Starting to process " << trap[trap_idx] << "_" << cunc_vec[cunc_idx] << ".bin" << std::endl;
			process((dir + "/" + trap[trap_idx] + "_" + cunc_vec[cunc_idx] + ".bin"), z0_vec[trap_idx]);
		}
	}

	return 0;
}
