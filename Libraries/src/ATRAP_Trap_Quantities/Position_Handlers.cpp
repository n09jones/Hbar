#include "Position_Handlers.h"

pos_nms::position::position()
{
    trap_radius = 0.0341;
    trap_radius_sq = trap_radius*trap_radius;
    out_of_bounds = false;
}

