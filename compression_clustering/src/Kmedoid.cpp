#include "Kmedoid.h"
#include <omp.h>

Kmedoid::Kmedoid(std::vector<SegmentPlayer>& _segments, int _nbCluster, int _nbIter, double _angle_factor, double _perpendicular_factor, double _parallel_factor, int _nb_thread)
    : Clustering(_segments, _nbCluster, _nbIter, _angle_factor, _perpendicular_factor, _parallel_factor, _nb_thread) {}

std::vector<SegmentPlayer> Kmedoid::getCenters() {
    if ((size_t)nb_cluster > segments.size())
        nb_cluster = segments.size();
    std::vector<SegmentPlayer> res;
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


typedef std::pair<std::vector<SegmentPlayer>, double> Pair;
std::unordered_map<SegmentPlayer, Pair> Kmedoid::association(std::vector<SegmentPlayer>& centers) {
    std::unordered_map<SegmentPlayer, Pair> res;
#pragma omp parallel for schedule(static)
    for (SegmentPlayer segment : segments) {
        double min_dist = DBL_MAX;
        SegmentPlayer associated_center(segment);
        for (SegmentPlayer center : centers) {
            if (center == segment)
                continue;
            double dist = center.distance(segment, angle_factor, perpendicular_factor, parallel_factor);
            if (dist < min_dist) {
                min_dist = dist;
                associated_center = center;
            }
        }
#pragma omp critical
        {
            res[associated_center].first.emplace_back(segment);
            res[associated_center].second += min_dist;
        }
    }
    return res;
}


double Kmedoid::distFromOthers(std::vector<SegmentPlayer>& cluster, size_t segment_index) {
    double res = 0.0f;
#pragma omp parallel for schedule(static) reduction(+ \
                                                    : res)
    for (size_t i = 0; i < cluster.size(); i++) {
        if (i == segment_index)
            continue;
        res += cluster[segment_index].distance(cluster[i], angle_factor, perpendicular_factor, parallel_factor);
    }
    return res;
}


std::unordered_map<SegmentPlayer, Pair> Kmedoid::start() {
    std::vector<SegmentPlayer> centers = getCenters();
    std::vector<SegmentPlayer> previous;
    std::unordered_map<SegmentPlayer, Pair> medoids = association(centers);
    int iteration = 0;
    while (max_iteration > 0) {
        std::cout << "iteration: " << iteration++ << "\n";
        std::vector<SegmentPlayer> new_centers;
        for (auto& [center, cluster] : medoids) {
            double min = cluster.second;
            int new_center = 0;
            for (size_t i = 0; i < cluster.first.size(); i++) {
                double dist = distFromOthers(cluster.first, i);
                if (dist < min) {
                    min = dist;
                    new_center = i;
                }
            }
            new_centers.emplace_back(cluster.first[new_center]);
        }
        if (new_centers == centers)
            break;
        centers = new_centers;
        medoids = association(centers);
        max_iteration--;
    }
    return medoids;
}

std::unordered_map<MainSegment, std::vector<SegmentPlayer>> Kmedoid::getClusters() {
    std::unordered_map<MainSegment, std::vector<SegmentPlayer>> res;
    std::unordered_map<SegmentPlayer, Pair> medoids = start();
    int center_index = 0;
    for (const auto& [center, cluster] : medoids) {
        MainSegment main_center(center.getP1(), center.getP2(), center_index);
        center_index += 1;
        res[main_center] = cluster.first;
    }
    return res;
}