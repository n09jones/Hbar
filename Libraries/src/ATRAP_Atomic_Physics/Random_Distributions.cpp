#include <chrono>
#include "Random_Distributions.h"

std::minstd_rand distr_gen::gen {static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())};

std::uniform_real_distribution<global_nms::default_data_type> distr_gen::distr {0.0, 1.0};

