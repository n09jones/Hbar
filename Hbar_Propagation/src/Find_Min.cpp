#include <iostream>
#include "ATRAP_base.h"

int main(int argc, char ** argv)
{
	boost::filesystem::path fn {global_nms::large_storage_dir / boost::filesystem::path{argv[1]}};
	mapped_arr_nms::mapped_arr<> arr {fn, false};

	auto begin_it {std::find_if(arr.cbegin(), arr.cend(), [](auto x) {return (x > 0.0);})};
	auto it {std::min_element(begin_it, arr.cend(), [](auto x, auto y) {return ((x < y) && (x > 0.0));})};

	size_t grid_idx = it - arr.cbegin();
	size_t z_idx {grid_idx / (351*351)};
	grid_idx -= z_idx * (351*351);
	size_t y_idx {grid_idx / 351};
	grid_idx -= y_idx * 351;
	size_t x_idx = grid_idx;

	std::cout << std::setprecision(9);
	std::cout << "Minimum grid coordinates : ( ";
	std::cout << ((x_idx*0.2) - 35.0)/1000. << " , ";
	std::cout << ((y_idx*0.2) - 35.0)/1000. << " , ";
	std::cout << ((z_idx*0.2) - 151.0)/1000. << " )" << std::endl;
	std::cout << "Minimum value : " << *it << std::endl;

	return 0;
}
