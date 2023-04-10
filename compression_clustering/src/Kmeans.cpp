#include "Kmeans.h"
#include <omp.h>
#include "SetSegment.h"

Kmeans::Kmeans(std::vector<SegmentPlayer>& _segments, int _nbCluster, int _nbIter, double _angle_factor, double _perpendicular_factor, double _parallel_factor, int _nb_thread)
    : Clustering(_segments, _nbCluster, _nbIter, _angle_factor, _perpendicular_factor, _parallel_factor, _nb_thread) {
    min_x = DBL_MAX;
    max_x = DBL_MIN;
    min_y = DBL_MAX;
    max_y = DBL_MIN;

    for (const SegmentPlayer& seg : segments) {
        double x = seg.getP1().getX();
        double y = seg.getP1().getY();
        if (x < min_x)
            min_x = x;
        if (x > max_x)
            max_x = x;
        if (y < min_y)
            min_y = y;
        if (y > max_y)
            max_y = y;
        x = seg.getP2().getX();
        y = seg.getP2().getY();
        if (x < min_x)
            min_x = x;
        if (x > max_x)
            max_x = x;
        if (y < min_y)
            min_y = y;
        if (y > max_y)
            max_y = y;
    }
}

std::vector<Segment> Kmeans::getCenters() {
    if ((size_t)nb_cluster > segments.size())
        nb_cluster = segments.size();
    std::vector<Segment> res;
    res.reserve(nb_cluster);
    std::unordered_set<int> picked;
    for (int i = 0; i < nb_cluster; i++) {
        size_t random_index = randomGenerator(0, segments.size() - 1);
        while (picked.count(random_index) != 0) {
            random_index = randomGenerator(0, segments.size() - 1);
        }
        picked.insert(random_index);
        res.push_back(segments[random_index]);
    }
    return res;
}

std::vector<std::vector<SegmentPlayer>> Kmeans::association(std::vector<Segment>& centers) {
    std::vector<std::vector<SegmentPlayer>> res(nb_cluster, std::vector<SegmentPlayer>());

    std::vector<int> cluster_ids(segments.size(), -1);

#pragma omp parallel for
    for (int i = 0; i < (int)segments.size(); i++) {
        SegmentPlayer segment = segments[i];
        int argmin = -1;
        double min = DBL_MAX;
        for (int c = 0; c < nb_cluster; c++) {
            double dist = segment.distance(centers[c], angle_factor, perpendicular_factor, parallel_factor);
            if (dist < min) {
                min = dist;
                argmin = c;
            }
        }
#pragma omp critical
        {
            cluster_ids[i] = argmin;
            segments[i].setClusterID(argmin);
        }
    }

    for (int i = 0; i < (int)segments.size(); i++) {
        res[cluster_ids[i]].push_back(segments[i]);
    }
    return res;
}

std::vector<Segment> Kmeans::computeCentroids(std::vector<std::vector<SegmentPlayer>>& clusters) {
    std::vector<Segment> res(nb_cluster, Segment());

#pragma omp parallel for
    for (size_t i = 0; i < clusters.size(); i++) {
        std::vector<SegmentPlayer> cluster = clusters[i];
        SetSegment set_segment(cluster);
        std::shared_ptr<Segment> center = set_segment.meanSegment();
#pragma omp critical
        {
            if (center == nullptr) {
                // the cluster is empty so we pick a random segment within the limits
                double x0 = ((double)rand() / RAND_MAX) * (max_x - min_x) + min_x;
                double y0 = ((double)rand() / RAND_MAX) * (max_y - min_y) + min_y;
                double x1 = ((double)rand() / RAND_MAX) * (max_x - min_x) + min_x;
                double y1 = ((double)rand() / RAND_MAX) * (max_y - min_y) + min_y;
                Segment random_center(Point(x0, y0), Point(x1, y1));
                center = std::make_shared<Segment>(random_center);
            }
            res[i] = *center;
        }
    }
    return res;
}

////// --------------------------------------------------------------------- Start
std::vector<std::vector<SegmentPlayer>> Kmeans::start() {
    omp_set_num_threads(nb_thread);

    int iteration = 0;
    std::vector<std::vector<SegmentPlayer>> clusters;
    clusters.reserve(nb_cluster);
    std::vector<Segment> centers = getCenters();
    std::vector<Segment> old_centers(1, Segment(Point(0, 0), Point(0, 0)));
    std::cout << "\n\nKmeans start\n\n";

    while (iteration < max_iteration && centers != old_centers) {
        std::cout << "iteration " << iteration << std::endl;

        clusters = association(centers);
        old_centers = centers;
        centers = computeCentroids(clusters);
        iteration++;
    }

    return clusters;
}

void verifying_cluster_att(std::unordered_map<MainSegment, std::vector<SegmentPlayer>> clusters, double angle_factor, double perpendicular_factor, double parallel_factor) {
    std::unordered_set<MainSegment> keys;
    for (const auto& cluster : clusters) {
        keys.insert(cluster.first);
    }

    for (std::pair<const MainSegment, std::vector<SegmentPlayer>>& cluster : clusters) {
        int original_id = cluster.first.getSegmentID();
        MainSegment main = cluster.first;
        for (SegmentPlayer seg : cluster.second) {
            double min = seg.distance(main, angle_factor, perpendicular_factor, parallel_factor);
            double dist = -1.0;
            MainSegment c = main;
            for (const MainSegment& key : keys) {
                dist = seg.distance(key, angle_factor, perpendicular_factor, parallel_factor);
                if (dist < min) {
                    min = dist;
                    c = key;
                }
            }
            if (std::abs((min - seg.distance(main, angle_factor, perpendicular_factor, parallel_factor))) > 0.0000000001) {
                std::cout << "dist = " << min << " ; seg dist cluster = " << seg.distance(cluster.first) << "\n";
                std::cout << "c = " << c << "\n";
                std::cout << "main = " << cluster.first << "\n";
                std::cout << "seg = " << seg << "\n";
                std::cout << std::abs((min - seg.distance(c))) << "\n";
                std::cout << (std::abs((min - seg.distance(seg))) > 0.1) << "\n";

                std::cout << "Error in cluster assauce\n";
                exit(1);
            }
            if (seg.getClusterID() != original_id) {
                std::cout << "Error ORIGINAL U KNOW ID STUFF\n";
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

std::unordered_map<MainSegment, std::vector<SegmentPlayer>> Kmeans::getClusters() {
    std::unordered_map<MainSegment, std::vector<SegmentPlayer>> res;
    auto clusters = start();
    auto centers = computeCentroids(clusters);

    std::unordered_map<Segment, int> ids;
    for (int i = 0; i < (int)centers.size(); i++) {
        ids[centers[i]] = i;
    }

    clusters = association(centers);
    for (int i = 0; i < nb_cluster; i++) {
        Segment center = centers[i];
        auto main_center = std::make_shared<MainSegment>(center.getP1(), center.getP2(), i);
        res[*main_center] = clusters[i];
    }

    std::cout << "nb cluster = " << res.size() << "\n";
    verifying_cluster_att(res, angle_factor, perpendicular_factor, parallel_factor);

    return res;
}