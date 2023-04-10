#include "clusteringPipe.h"
#include "Clustering.h"
#include "Kmeans.h"
#include "Segment.h"
#include "SegmentPlayer.h"
#include "compressionPipe.h"
#include "AffinityPropagation.h"
#include "Kmedoid.h"

struct game_point {
    int t;
    std::shared_ptr<Point> pt;
};

struct seg_with_id {
    int id;
    SegmentPlayer seg;

    seg_with_id(int _id, SegmentPlayer _seg) : id(_id), seg(_seg) {}
};

std::vector< std::vector< std::shared_ptr<game_point> >> load_game_with_pc(const std::string& name, 
                const int start_id = 0, const int end_id = 10);

void process_game(const std::string& name, std::vector<SegmentPlayer>& segments, 
                const int start_id = 0, const int end_id = 10);

std::vector<SegmentPlayer> process_all(const std::string& dir_path, const std::string& regex_str, const int nb_games, 
                const int start_id = 0, const int end_id = 10);

void write_clusters(const std::vector<MainSegment>& keys, const std::string dir_path);

void write_infos(const int nb_clusters, const int nb_iterations, const int nb_games, 
                const double angle_factor, const double perpendicular_factor, const double parallel_factor, 
                const std::string dir_parth, const std::string method, const int start_id, const int end_id);

void write_group_clusters(std::unordered_map<MainSegment, std::vector<SegmentPlayer>>& clusters, const std::string dir_path);

void write_gameS_v2(std::unordered_map<MainSegment, std::vector<SegmentPlayer>>& clusters, const std::string dir_path, 
                    double angle_factor, double perpendicular_factor, double parallel_factor,
                    const int start_id, const int end_id);

void verifying_cluster(std::unordered_map<MainSegment, std::vector<SegmentPlayer>> clusters, double angle_factor, double perpendicular_factor, double parallel_factor);

// --------------------------------------------------------------------------------

std::vector< std::vector< std::shared_ptr<game_point> >> load_game_with_pc(const std::string& name, const int start_id, const int end_id) {
    std::vector< std::vector< std::shared_ptr<game_point> >> cols(10, std::vector< std::shared_ptr<game_point> >());
    std::vector<std::string> row;
    std::string line, word;
    std::fstream file(name, std::ios::in);

    if(file.is_open()) {
        getline(file, line); // 1st line
        getline(file, line); // loading line

        int counterTick = 0;
        while(getline(file, line)) {
            row.clear();
            std::stringstream str(line);

            while(getline(str, word,',')) {
                row.push_back(word);
            }
            int counterPlayer = 0;
            for(int i = 3; i < 52 ; i += 4) {
                if(counterPlayer >= 10)
                    break;
                if(row[row.size()-10+counterPlayer] == "1") {
                    float x = std::stof(row[i]);
                    float y = std::stof(row[i+1]);
                    std::shared_ptr<Point> pt(new Point(x, y));
                    std::shared_ptr<game_point> gp(new game_point);
                    gp->t = counterTick;
                    gp->pt = pt;
                    cols[counterPlayer].push_back(gp);
                }
                counterPlayer += 1;
            }
            counterTick += 30;
        }
        file.close();

        std::vector< std::vector< std::shared_ptr<game_point> >> res(end_id - start_id, std::vector< std::shared_ptr<game_point> >());

        int counter = 0;
        for(int i = start_id; i < end_id; i++) {
            res[i-start_id] = cols[i];
            counter += res[i-start_id].size();
        }
        std::cout << "counter: " << counter << "\n";

        return res;
    }
    return {};
}

// --------------------------------------------------------------------------------

void process_game(const std::string& name, std::vector<SegmentPlayer>& segments, const int start_id, const int end_id) {
    
    std::string game_id(name.substr(std::string("mdl_trajectories").size(), name.length()-std::string("mdl_trajectories").size()-std::string(".csv").size()));
    std::vector< std::vector< std::shared_ptr<game_point> >> game = load_game_with_pc(name, start_id, end_id);
    int player_id = start_id;
    for(const std::vector< std::shared_ptr<game_point> >& player : game) {
        for(int i = 0; i < (int)player.size()-1; i++) {
            auto gp1 = (player[i]->pt);
            auto gp2 = (player[i+1]->pt);
            Point p1(gp1->getX(), gp1->getY());
            Point p2(gp2->getX(), gp2->getY());
            int t1 = player[i]->t;
            int t2 = player[i+1]->t;
            SegmentPlayer sp(p1, p2, game_id, t1, t2, player_id);
            segments.push_back(sp);
        }
        if(player_id >= end_id && player_id < start_id) {
            std::cout << "player_id: " << player_id << "\n";
            std::cout << "Supposed to be between " << start_id << " and " << end_id << "\n";
            exit(1);
        }
        player_id++;
    }
}

// --------------------------------------------------------------------------------

std::vector<SegmentPlayer> process_all(const std::string& dir_path, const std::string& regex_str, const int nb_games, const int start_id, const int end_id) {
    std::vector<std::string> files = list_files(dir_path, regex_str);
    std::cout << "Files: " << files.size() << "\n";
    std::vector<SegmentPlayer> segments;
    int counter = 1;
    for(auto file : files) {
        if(counter > nb_games) break; // TODO: remove this
        std::cout << "\r" << file << " " << counter++ << "/" << files.size() << std::flush;
        process_game(file, segments, start_id, end_id);
        
    }
    std::cout << "game processed\n" << std::flush;
    return segments;
}

// --------------------------------------------------------------------------------


void clustering(const int nb_clusters,const int nb_iter, const int nb_games, std::string method, double angle_factor, double perpendicular_factor, double parallel_factor, int nb_threads, const int start_id, const int end_id) {
    std::cout << "Clustering\n";
    //std::string dir_path = "mdl_trajectories";
    std::string dir_path = "mdl_trajectories";
    std::string regex_str = ".*\\.csv";

    std::cout << "Processing all files in " << dir_path << "\n";
    std::vector<SegmentPlayer> segments = process_all(dir_path, regex_str, nb_games, start_id, end_id);
    
    std::cout << "Segments: " << segments.size() << "\n";

    std::shared_ptr<Clustering> cl;
    if(method == "kmeans")
        cl = std::shared_ptr<Clustering>(new Kmeans(segments, nb_clusters, nb_iter, angle_factor, perpendicular_factor, parallel_factor, nb_threads));
    else if(method == "propa")
        cl = std::shared_ptr<Clustering>(new AffinityPropagation(segments, nb_iter, angle_factor, perpendicular_factor, parallel_factor, nb_threads));
    else if(method == "kmedoid")
        cl = std::shared_ptr<Clustering>(new Kmedoid(segments, nb_clusters, nb_iter, angle_factor, perpendicular_factor, parallel_factor, nb_threads));
    else {
        std::cout << "Unknown method: " << method << "\n";
        exit(1);
    }
    //(Kmeans(segments, nb_clusters, nb_iter, angle_factor, perpendicular_factor, parallel_factor, nb_threads));
    //Kmeans cl(segments, nb_clusters, nb_iter, angle_factor, perpendicular_factor, parallel_factor, nb_threads);
    
    std::unordered_map<MainSegment, std::vector<SegmentPlayer>> clusters = cl->getClusters();
    
    std::vector<MainSegment> keys;
    for(auto const& x : clusters) {
        keys.push_back(x.first);
    }

    if(method == "kmeans")
        verifying_cluster(clusters, angle_factor, perpendicular_factor, parallel_factor);

    namespace fs = boost::filesystem;

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::string date = std::to_string(tm.tm_year + 1900) + "_" + std::to_string(tm.tm_mon + 1) + "_" + std::to_string(tm.tm_mday) + "_" +std::to_string(tm.tm_hour) + "H_" + std::to_string(tm.tm_min) + "M" + std::to_string(tm.tm_sec) + "S";

    std::string dir_path_results = "clustering_results/" + date;

    fs::create_directory(dir_path_results);

    std::cout << "NB clusters: " << clusters.size() << "\n";
    write_clusters(keys, dir_path_results);

    write_infos(nb_clusters, nb_iter, nb_games, angle_factor, perpendicular_factor, parallel_factor, dir_path_results, method, start_id, end_id);

    write_group_clusters(clusters, dir_path_results);

    std::cout << "Writing gamessSSSSSSSSSSSSSSSSSSSSS\n";
    write_gameS_v2(clusters, dir_path_results, angle_factor, perpendicular_factor, parallel_factor, start_id, end_id);
    std::cout << "END GAMESSSSSSSSSSSSSSSSSSSSSSS\n";


    std::cout << "Clustering done\n";
}

// --------------------------------------------------------------------------------

void write_clusters(const std::vector<MainSegment>& keys, const std::string dir_path) {
    std::ofstream myfile;
    myfile.open (dir_path + "/clusters.csv");
    myfile << "id,x0,y0,x1,y1\n";
    for(auto key : keys) {
        myfile << key.getSegmentID() << "," << key.getP1().getX() << "," << key.getP1().getY() << "," << key.getP2().getX() << "," << key.getP2().getY() << "\n";
    }
    myfile.close();
}

// --------------------------------------------------------------------------------

void write_infos(const int nb_clusters, const int nb_iterations, const int nb_games, 
                const double angle_factor, const double perpendicular_factor, const double parallel_factor, 
                const std::string dir_parth, const std::string method, const int start_id, const int end_id) {
    std::ofstream myfile;
    myfile.open(dir_parth + "/infos.txt");
    myfile << "method: " << method << "\n";
    myfile << "nb_clusters: " << nb_clusters << "\n";
    myfile << "nb_iterations: " << nb_iterations << "\n";
    myfile << "nb_games: " << nb_games << "\n";
    myfile << "angle_factor: " << angle_factor << "\n";
    myfile << "perpendicular_factor: " << perpendicular_factor << "\n";
    myfile << "parallel_factor: " << parallel_factor << "\n";
    myfile << "start_id: " << start_id << "\n";
    myfile << "end_id: " << end_id << "\n";
    myfile.close();
}

// --------------------------------------------------------------------------------

void write_group_clusters(std::unordered_map<MainSegment, std::vector<SegmentPlayer>>& clusters, const std::string dir_path) {
    std::ofstream myfile;
    myfile.open (dir_path + "/group_clusters.csv");
    myfile << "id,x0,y0,x1,y1\n";
    for(const auto & cluster : clusters) {
        int id = cluster.first.getSegmentID();
        for(const auto & seg : cluster.second) {
            myfile << id << "," << seg.getP1().getX() << "," << seg.getP1().getY() << "," << seg.getP2().getX() << "," << seg.getP2().getY() << "\n";
        }        
    }
    myfile.close();
}

// --------------------------------------------------------------------------------

void write_gameS_v2(std::unordered_map<MainSegment, std::vector<SegmentPlayer>>& clusters, const std::string dir_path, 
                    const double angle_factor, const double perpendicular_factor, const double parallel_factor,
                    const int start_id, const int end_id ) {
    std::cout << "\nWriting games\n";

    bool debug = false;

    // I need to make group of games and for each segment link the corresponding MainSegment
    std::unordered_map<std::string, std::unordered_map<int, std::vector<std::shared_ptr<seg_with_id>>>> games;

    for(const auto & cluster : clusters) {
        for(const auto & seg : cluster.second) {
            int id = cluster.first.getSegmentID();
            std::shared_ptr<seg_with_id> seg_id(new seg_with_id(id, seg));

            /* if(id != seg.getClusterID()) {
                std::cout << "Error in cluster attribution (clusterIDAAAAAAAAA\n";
                exit(1);
            }
            if(seg_id->seg.getClusterID() != seg.getClusterID() || seg_id->id != id) {
                std::cout << "Error in cluster attribution (clusterIDDDDDDDDDDDDDDDDDDDDDd\n";
                exit(1);
            } */
            
            // if game is not in games, create it and initialize it
            if(games.find(seg.getGameID()) == games.end()) {
                games[seg.getGameID()] = std::unordered_map<int, std::vector<std::shared_ptr<seg_with_id>>>();
                for(int i = 0; i < end_id - start_id; i++) {
                    games[seg.getGameID()][i] = std::vector<std::shared_ptr<seg_with_id>>();
                }
            }
            
            games[seg.getGameID()][seg.getPlayerID() - start_id].push_back(seg_id);
        }
    }

    // Check if all games have correct number of players
    for(auto const& game : games) {
        if( (int) game.second.size() != (end_id - start_id)) {
            std::cout << "ERROR Game " << game.first << " has " << game.second.size() << " players\n";
            exit(1);
        }
        // check if players are in order
        for(int i = 0; i < end_id - start_id; i++) {
            if(game.second.find(i) == game.second.end()) {
                std::cout << "Game " << game.first << " has no player " << i + start_id << "\n";
                exit(1);
            }
            for(auto const& s : game.second.at(i)) {
                if(s->seg.getPlayerID() != i + start_id) {
                    std::cout << "Game " << game.first << " has a player lost\n";
                    exit(1);
                }
            }
        }
    }
    std::cout << "All games have the correct number of players and are well placed\n\n";

    // Key set
    std::unordered_set<MainSegment> keys;
    for(const auto& x : clusters) {
        keys.insert(x.first);
    }

    // check if assignations are still valid
    if(debug) {
        for(const std::pair<const std::string, std::unordered_map<int, std::vector<std::shared_ptr<seg_with_id>>>>& game : games) {
            for(const std::pair<const int, std::vector<std::shared_ptr<seg_with_id> > >& player : game.second) {
                for(std::shared_ptr<seg_with_id> s : player.second) {
                    SegmentPlayer seg = s->seg;
                    int id = s->id;
                    int cluster_id = seg.getClusterID();

                    MainSegment key = MainSegment(Point(-1.,-1.), Point(-1.,-1.), -1);
                    if(key.getSegmentID() != -1) {
                        std::cout << "Error in key initialization\n";
                        exit(1);
                    }
                    double min_dist = DBL_MAX;
                    for(const MainSegment& k : keys) {
                        double dist = seg.distance(k, angle_factor, perpendicular_factor, parallel_factor);
                        if(dist < min_dist) {
                            min_dist = dist;
                            key = k;
                        }
                    }
                    if(key.getSegmentID() != id) {
                        std::cout << "current key: " << key << "\n";
                        std::cout << "Game " << game.first << " has FUCKED UP HAAAAARD\n";
                        std::cout << "s->id: " << s->id << "\n";
                        std::cout << "key.getSegmentID(): " << key.getSegmentID() << "\n";
                        std::cout << "seg cluster id: " << s->seg.getClusterID() << "\n";

                        std::cout << "cluster id: " << cluster_id << "\n";
                        std::cout << "seg: " << seg << "\n";
                        
                        exit(1);
                    }
                }
            }
        }
    }

    std::cout << "All games still have correct assignations\n\n";

    for(const auto & game : games) {
        const int tot = end_id - start_id;
        std::cout << "tot: " << tot << "\n";

        std::vector<std::vector<std::shared_ptr<seg_with_id>>> game_v(tot, std::vector<std::shared_ptr<seg_with_id>>());
        for(const auto& player : game.second) {
            // I need player.second to be pushed in game_v but with the corresponding index corresponding to the player id  
            for(const auto& s : player.second) {
                game_v[player.first].push_back(s);
            }
            if(player.first > 9 || player.first < 0) {
                std::cout << "Player id " << player.first << " error\n";
                exit(1);
            }
        }
        
        // Now I have a vector of vector of segments, I need to sort them by time (here)
        for(std::vector<std::shared_ptr<seg_with_id>>& player : game_v) {
            std::sort(player.begin(), player.end(), 
                [](std::shared_ptr<seg_with_id>& a, std::shared_ptr<seg_with_id>& b) {
                        return (a->seg.getT1()) < (b->seg.getT1());
                    });
        }

        //std::cout << "\nAfter sorting\n";

        // check if all players are sorted
        for(int player = start_id; player < end_id; player++) {
            for(int i = 0; i < (int) game_v[player-start_id].size()-1; i++) {
                if(game_v[player - start_id][i]->seg.getT1() > game_v[player - start_id][i+1]->seg.getT1()) {
                    std::cout << "Player " << player << " is not sorted\n";
                    exit(1);
                }
            }
        }

        // now check if seg_with_id id is still correct
        if(debug) {
            for(int player = 0; player < end_id - start_id; player++) {
                for(int i = 0; i < (int) game_v[player].size(); i++) {
                    double dist = DBL_MAX;
                    MainSegment r = MainSegment(Point(0,0), Point(0,0), 0);
                    for(const MainSegment& key : keys) {
                        double d = game_v[player][i]->seg.distance(key, angle_factor, perpendicular_factor, parallel_factor);
                        if(d < dist) {
                            dist = d;
                            r = key;
                        }
                    }
                    if(r.getP1() == Point(0,0) && r.getP2() == Point(0,0)) {
                        std::cout << "Error in seg_with_id id\n";
                        exit(1);
                    }
                    if(game_v[player][i]->id != r.getSegmentID()) {
                        MainSegment normal = MainSegment(Point(-69.0,0), Point(0,0), 0);
                        for(const MainSegment& key : keys) {
                            if(key.getSegmentID() == game_v[player][i]->id) {
                                normal = key;
                                break;
                            }
                        }
                        std::cout << "Error in seg_with_id id?????\n";
                        std::cout << "game_v[player][i]->id: " << game_v[player][i]->id << "\n";
                        std::cout << "r.getSegmentID(): " << r.getSegmentID() << "\n";
                        std::cout << "dist r: " << dist << "\n";
                        std::cout << "dist normal: " << game_v[player][i]->seg.distance(normal, angle_factor, perpendicular_factor, parallel_factor) << "\n";
                        std::cout << "normal: " << normal << "\n";
                        std::cout << "r: " << r << "\n";
                        exit(1);
                    }
                    bool found = false;
                    for(const SegmentPlayer& x : clusters[r]) {
                        if(x == game_v[player][i]->seg) {
                            found = true;
                            break;
                        }
                    }
                    if(!found) {
                        std::cout << "Error in WTFFFFF\n";
                        exit(1);
                    }
                }
            }
        }

        std::cout << "Writing game " << game.first << "\n";

        std::ofstream file;
        file.open(dir_path + "/" + game.first + ".csv");
        file << "t,";
        for(int player = start_id; player < end_id -1 ; player++) {
            file << "p" << player << ",";
        }
        file << "p" << end_id - 1 << "\n";

        //int minTick = game_v[0][0]->seg.getT1();
        int minTick = 0;

        int maxTick = game_v[0][game_v[0].size()-1]->seg.getT1();
        for(int player = 1; player < end_id - start_id; player++) {
            int tick = game_v[player][game_v[player].size()-1]->seg.getT1();
            if(tick > maxTick) maxTick = tick;
        }

        // Now I can write the game
        // I need to iterate over the players and write the segments but I need to fill gap of time with previous segment
        // I need to keep track of the current segment for each player, by index ?
        std::vector<int> current_seg_index(end_id - start_id, 0);

        for(int tick = minTick; tick <= maxTick; tick += 30) {
            file << tick;
            for(int player = 0; player < end_id - start_id; player++) {
                if(game_v[player][current_seg_index[player]]->seg.getT1() == tick && current_seg_index[player] < (int) game_v[player].size()-1) {
                    current_seg_index[player]++;
                } 
                file << "," << game_v[player][current_seg_index[player]]->id;
                
            }
            file << "\n";
        }

        file.close();

        std::cout << "Game " << game.first << " written\n";
    }

    std::cout << "Games written\n";
}

// --------------------------------------------------------------------------------

void verifying_cluster(std::unordered_map<MainSegment, std::vector<SegmentPlayer>> clusters, double angle_factor, double perpendicular_factor, double parallel_factor) {
    std::unordered_set<MainSegment> keys;
    for(const auto & cluster : clusters) {
        keys.insert(cluster.first);
    }
    for(std::pair<const MainSegment, std::vector<SegmentPlayer>>& cluster : clusters) {
        for( SegmentPlayer seg : cluster.second) {
            double min = DBL_MAX;
            double dist = 0;
            MainSegment c = cluster.first;
            for(const MainSegment& key : keys) {
                dist = seg.distance(key, angle_factor, perpendicular_factor, parallel_factor);
                if(dist < min) {
                    min = dist;
                    c = key;
                }
            }
            if(std::abs(( min - seg.distance(c, angle_factor, perpendicular_factor, parallel_factor))) > 0.0000000001) {
                std::cout << "dist = " << min << " ; seg dist cluster = " << seg.distance(cluster.first) << "\n";
                std::cout << "c = " << c << "\n";
                std::cout << "main = " << cluster.first << "\n";
                std::cout << "seg = " << seg << "\n";
                std::cout << std::abs((min - seg.distance(c))) << "\n";
                std::cout << (std::abs((min - seg.distance(seg))) > 0.1) << "\n";
                
                std::cout << "Error in cluster\n";
                exit(1);
            } 
            if(seg.getClusterID() != c.getSegmentID()) {
                std::cout << "Error in cluster attribution (clusterID ?????0000000000?\n";
                std::cout << "seg = " << seg << "\n";
                std::cout << "seg clusterID = " << seg.getClusterID() << "\n";
                std::cout << "main = " << cluster.first << "\n";
                std::cout << "c = " << c << "\n";
                exit(1);
            }
        }
    }
    std::cout << "Cluster attribution verified\n";

}