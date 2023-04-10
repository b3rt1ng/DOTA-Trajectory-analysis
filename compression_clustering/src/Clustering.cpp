#include "Clustering.h"

Clustering::Clustering(std::vector<SegmentPlayer>& _segments, int _nbCluster, int _nbiter, double _angle_factor, double _perpendicular_factor, double _parallel_factor, int _nb_thread)
    : nb_cluster(_nbCluster), max_iteration(_nbiter), angle_factor(_angle_factor), perpendicular_factor(_perpendicular_factor), parallel_factor(_parallel_factor), nb_thread(_nb_thread) {
    rng.seed(std::random_device()());
    segments = std::vector<SegmentPlayer>(_segments);
}

std::vector<SegmentPlayer>& Clustering::getSegments() {
    return segments;
}

int Clustering::getNbCluster() {
    return nb_cluster;
}

int Clustering::getNbIteration() {
    return max_iteration;
}

double Clustering::getAngleFactor() {
    return angle_factor;
}

double Clustering::getPerpendicularFactor() {
    return perpendicular_factor;
}

double Clustering::getParallelFactor() {
    return parallel_factor;
}

int Clustering::getNbThread() {
    return nb_thread;
}

void Clustering::setNbCluster(int _nb_cluster) {
    nb_cluster = _nb_cluster;
}

void Clustering::setNbIteration(int max) {
    max_iteration = max;
}

void Clustering::setAngleFactor(double _angle_factor) {
    angle_factor = _angle_factor;
}

void Clustering::setPerpendicularFactor(double _perpendicular_factor) {
    perpendicular_factor = _perpendicular_factor;
}

void Clustering::setParallelFactor(double _parallel_factor) {
    parallel_factor = _parallel_factor;
}

void Clustering::setNbThread(int _nb_thread) {
    nb_thread = _nb_thread;
}

size_t Clustering::randomGenerator(size_t min, size_t max) {
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return dist(rng);
}