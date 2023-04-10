#include <omp.h>
#include "compressionPipe.h"
#include "clusteringPipe.h"

// run the compression of the games use script or ./bin/main compression <nb_thread_to_use>?
// or ./bin/main compression <nb_thread_to_use>?
// and then run the bash command :
//    for i in `ls -d coordinates/*/ | sed 's/coordinates\///' | sed 's/\///'`; do
//        paste -d "," coordinates/$i.csv coordinates/$i/*  | sed 's/\r//' > mdl_trajectories/mdl_$i.csv
//    done

// run the clustering : ./bin/main clustering <methods: "kmeans", ?> <nb_clusters> <nb_iter> <nb_games> <nb_thread_to_use>? <angle_factor>? <perpendicular_factor>? <parallel_factor>? <start_id>? <end_id>?

int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            std::cout << "Here are the ways to use this program:\n";
            std::cout << "\t1) ./bin/main compression : To run the compression of the games\n";
            std::cout << "\t2) ./bin/main clustering : To run the clustering of the games\n";
            std::cout << "\t\t2.1) ./bin/main clustering nb_clusters <nb_iter>? <nb_games>? <nb_thread_to_use>? <angle_factor>? <perpendicular_factor>? <parallel_factor>? : To run the clustering of the games with the given parameters\n";
            std::cout << "\t\t\tBy default, nb_iter = 10, nb_games = 10, angle_factor = 1.0, perpendicular_factor = 1.0, parallel_factor = 1.0, nb_thread = 1\n";
        } else if (strcmp(argv[1], "compression") == 0) {
            int arg_nb_thread = 1;
            double arg_factor = 1.0;
            if (argc > 2) {
                arg_nb_thread = atoi(argv[2]);
                if (arg_nb_thread > omp_get_max_threads())
                    arg_nb_thread = omp_get_max_threads();
            }
            if (argc > 3)
                arg_factor = atof(argv[3]);

            compress_all(arg_nb_thread, arg_factor);
        } else if (strcmp(argv[1], "clustering") == 0) {
            std::string arg_method = "kmeans";
            int arg_nb_clusters = 5;
            int arg_nb_iter = 10;
            int arg_nb_games = 10;
            int arg_start_id = 0;
            int arg_end_id = 10;
            double arg_angle_factor = 1.0;
            double arg_perpendicular_factor = 1.0;
            double arg_parallel_factor = 1.0;
            int arg_nb_thread = 1;
            if (argc > 2)
                arg_method = argv[2];
            if (argc > 3)
                arg_nb_clusters = atoi(argv[3]);
            if (argc > 4)
                arg_nb_iter = atoi(argv[4]);
            if (argc > 5)
                arg_nb_games = atoi(argv[5]);
            if (argc > 6) {
                arg_nb_thread = atoi(argv[6]);
                if (arg_nb_thread > omp_get_max_threads())
                    arg_nb_thread = omp_get_max_threads();
            }
            if (argc > 7)
                arg_angle_factor = atof(argv[7]);
            if (argc > 8)
                arg_perpendicular_factor = atof(argv[8]);
            if (argc > 9)
                arg_parallel_factor = atof(argv[9]);
            if (argc > 10)
                arg_start_id = atoi(argv[10]);
            if (argc > 11)
                arg_end_id = atoi(argv[11]);
            std::cout << "Starting clustering\n";
            std::cout << "method = " << arg_method << "\n";
            std::cout << "nb_clusters = " << arg_nb_clusters << "\n";
            std::cout << "nb_iter = " << arg_nb_iter << "\n";
            std::cout << "nb_games = " << arg_nb_games << "\n";
            std::cout << "nb_thread = " << arg_nb_thread << "\n\n";
            std::cout << "angle_factor = " << arg_angle_factor << ", perpendicular_factor = " << arg_perpendicular_factor << ", parallel_factor = " << arg_parallel_factor << "\n\n";
            clustering(arg_nb_clusters, arg_nb_iter, arg_nb_games, arg_method, arg_angle_factor, arg_perpendicular_factor, arg_parallel_factor, arg_nb_thread, arg_start_id, arg_end_id);
            std::cout << "Exiting clustering\n";
        }
    }

    std::cout << "\nDone\n";
}
