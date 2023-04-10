#pragma once
#include "Point.h"
#include "all.h"

// Function that compress all the games
void compress_all(int nb_thread = 1, double factor = 1.0);
/**
 * @brief Read a csv representing a trajectory and stock its as
 * succession of Point in a vector.
 * @param the name of the file.
 * @return Returns a vector of Point.
 */
std::vector<std::vector<Point> > load_game(const std::string);

/**
 * @brief List all the files in a directory.
 * @param dir_path the path of directory in wich the files are located.
 * @param regex_str regular expression to filter the wanted files.
 * @return A vector of string listing all the files in the directory.
 */
std::vector<std::string> list_files(const std::string dir_path, const std::string regex_str);

/**
 * @brief Compresses all the trajectories in a game using mdl
 * @param name The name of the directory
 * @param factor The compression factor
 */
void process_game(const std::string name, double factor);