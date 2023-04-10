#pragma once
#include "MainSegment.h"
#include "SegmentPlayer.h"

class Clustering {
   protected:
    int nb_cluster;               // Number of clusters to generate
    int max_iteration;            // Maximum number of iterations for the algorithm
    double angle_factor;          // Weight factor for angle similarity
    double perpendicular_factor;  // Weight factor for perpendicular similarity
    double parallel_factor;       // Weight factor for parallel similarity
    int nb_thread;                // Number of threads to use for parallel computation
    std::vector<SegmentPlayer> segments;  // A vector of SegmentPlayer objects representing the input segments
    std::mt19937 rng;             // A random number generator for the algorithm

   public:
    // Constructor
    Clustering(std::vector<SegmentPlayer>& _segments, int _nb_cluster, int _max_iteration, const double _angle_factor = 1,const double _perpendicular_factor = 1,const double _parallel_factor = 1 ,const int _nb_thread = 1);
    
    // Getter and Setter methods for private member variables
    std::vector<SegmentPlayer>& getSegments();
    int getNbCluster();
    void setNbCluster(int nb_cluster);
    int getNbIteration();
    void setNbIteration(int nb_iteration);
    double getAngleFactor();
    void setAngleFactor(double angle_factor);
    double getPerpendicularFactor();
    void setPerpendicularFactor(double perpendicular_factor);
    double getParallelFactor();
    void setParallelFactor(double parallel_factor);
    int getNbThread();
    void setNbThread(int nb_thread);

    // Method for generating a random number between min and max
    size_t randomGenerator(size_t min, size_t max);

    // Virtual method for generating the clusters
    virtual std::unordered_map<MainSegment, std::vector<SegmentPlayer>>getClusters() = 0;

};
