#include "MDL.h"
#include "compressionPipe.h"
#include <omp.h>

using namespace std;


void compress_all(int nb_threads, double factor) {
    if(nb_threads > 10)
        nb_threads = 10;
    omp_set_num_threads(nb_threads);
    std::cout << "Starting mdl processing on games\n";
    std::cout << "factor = " << factor << "\n";

    std::vector<std::string> files = list_files("coordinates", "(coord)*(.csv)");
    int nb_files = files.size();
    int counter = 0;
    double tot_t = 0;

    //#pragma omp parallel for schedule(dynamic)
    for(auto f: files) {
        std::clock_t t0 = std::clock();
        process_game(f, factor);
        std::clock_t t1 = std::clock();
        double chrono =  (t1 - t0) / (double)CLOCKS_PER_SEC ;
        tot_t += chrono;
        //("Estimated time remaining:", (tot_t / counter) * (nb_f-counter), "s\n")
        
        std::cout << "\nTime taken for " << f << " compression: " << chrono << "s" << std::endl;
        std::cout << "\nEstimated time remaining: " << (tot_t / counter) * (nb_files - counter) << "s\n" << std::endl;
        counter += 1;
    }


}

std::vector< std::vector< Point >> load_game(const std::string name) {
    std::vector< std::vector< Point >> cols(10, std::vector< Point >());
    std::vector<std::string> row;
    std::string line, word;

    std::fstream file(name, std::ios::in);

    if(file.is_open()) {
        getline(file, line); // 1st line
        getline(file, line); // loading line

        while(getline(file, line)) {
            row.clear();

            std::stringstream str(line);

            while(getline(str, word,',')) {
                row.push_back(word);
            }

            //std::cout << "nb cols: " << row.size() << std::endl;

            int counter = 0;
            for(int i = 3; i < 42 ; i += 4) {
                double x = std::stof(row[i]);
                double y = std::stof(row[i+1]);
                Point pt = Point(x, y);
                
                cols[counter++].push_back(pt);
            }
        }
        return cols;
    }

    return {};
}

std::vector<std::string> list_files(const std::string dir_path, const std::string regex_str) {
    std::vector<std::string> r;

    std::regex reg(regex_str);
    
    for( boost::filesystem::directory_iterator itr(dir_path); 
            itr != boost::filesystem::directory_iterator(); 
            ++itr) {
        if( boost:: filesystem::is_regular_file(itr->status()) 
            && std::regex_search(itr->path().filename().string(), reg) )
            r.push_back(itr->path().string());
    }
    return r;
}

void process_game(const std::string name, double factor) {
    namespace fs = boost::filesystem;
    std::string new_name = name.substr(0, name.length()-4);
    fs::path new_folder(new_name);
    std::vector<std::vector<Point>> game;

    //#pragma omp critical
    //{
        if(fs::exists(new_folder) && fs::is_directory(new_folder)) {
            fs::remove_all(new_folder);
        }
        fs::create_directory(new_folder);

        game = load_game(name);
    //}

    std::vector<std::vector<int>> games(10, std::vector<int>());

    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < (int)game.size(); ++i) {
        std::vector<Point> cols;
        for(int j = 0; j < (int)game[i].size(); ++j) {
            Point point = game[i][j];
            Point point_new = Point(point.getX() * factor, point.getY() * factor);
            cols.push_back(point_new);
        }

        MDL mdl(cols);
        std::vector<int> traj_comp = mdl.atp();

        #pragma omp critical
            games[i] = traj_comp;
    }

    int total = 0;
    int total2 = 0;
    for(int  i = 0; i < (int)games.size(); ++i) {
        total += games[i].size();
        if(games[i].size() == 0 || games[i].size() == 1 || games[i].size() == 2)
            std::cout << "empty traj: " << games[i].size() << std::endl;
        total2 += game[i].size();
    }
    std::cout << "total original: " << total2 << std::endl;
    std::cout << "total: " << total << std::endl;
    std::cout << "compression: " << (double)total / (double)total2 << std::endl;



    for(int i = 0; i < 10; ++i) {
        std::ofstream my_file(name.substr(0, name.length()-4) + "/pc" + std::to_string(i) + ".csv");
        my_file << "pc" << i << "\n";
        my_file << "0\n";

        int counter = 0;
        for(int j = 0; j < (int)game[i].size(); ++j) {
            if( games[i][counter] == j ) {
                my_file << "1\n";
                counter++;
            } else {
                my_file << "0\n";
            }
        }
        //std::cout << "counter: " << counter << std::endl;   
        my_file.close();
        //exit(0);
    }
    


    std::cout << "processed game:  " << name << std::endl;
}