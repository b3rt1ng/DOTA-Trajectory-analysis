#pragma once

#include "all.h"

void clustering(const int arg_nb_clusters, const int arg_nb_iter, const int nb_games, const std::string method, const double angle_factor = 1.0, const double perpendicular_factor = 1.0, const double parallel_factor = 1.0, const int nb_thread = 1, const int start_id = 0, const int end_id = 10);

