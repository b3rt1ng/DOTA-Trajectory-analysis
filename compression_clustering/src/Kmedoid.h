/**
 * @brief The Kmedoid class represents an implementation of the k-medoid algorithm.
 *
 * The k-medoid algorithm is a clustering algorithm that partitions a dataset into k clusters, where each cluster
 * is represented by its medoid (the data point that is the most centrally located within the cluster).
 * This implementation extends the Clustering class.
 */
#pragma once
#include "Clustering.h"

class Kmedoid : public Clustering {

   public:
    /**
     * @brief Constructor for the Kmedoid class.
     *
     * @param segment A vector of SegmentPlayer objects representing the dataset to be clustered.
     * @param nb_cluster The desired number of clusters.
     * @param nb_iter The maximum number of iterations for the algorithm.
     * @param _angle_factor A factor used to calculate the distance between two segments based on their angle.
     * @param _perpendicular_factor A factor used to calculate the distance between two segments based on their perpendicular distance.
     * @param _parallel_factor A factor used to calculate the distance between two segments based on their parallel distance.
     * @param _nb_thread The number of threads to use for the algorithm.
     */
    Kmedoid(std::vector<SegmentPlayer>& segment, int nb_cluster, int nb_iter, double _angle_factor, double _perpendicular_factor, double _parallel_factor, int _nb_thread);

    /**
     * @brief Method to genrerate random centers.
     *
     * @return A vector of SegmentPlayer objects representing the medoids of each cluster.
     */
    std::vector<SegmentPlayer> getCenters();

    /**
     * @brief Calculates the association between each segment and its nearest medoid.
     *
     * This function calculates the distance between each segment and each medoid, and assigns each segment to the medoid
     * with the smallest distance. The function returns an unordered map that maps each medoid to a pair containing a vector
     * of the segments that are associated with it, and the sum of distances between the medoid and its associated segments.
     *
     * @param centers A vector of SegmentPlayer objects representing the medoids of each cluster.
     * @return An unordered map that maps each medoid to a pair containing a vector of the segments that are associated with it,
     *         and the sum of distances between the medoid and its associated segments.
     */
    typedef std::pair<std::vector<SegmentPlayer>, double> Pair;
    std::unordered_map<SegmentPlayer, Pair> association(std::vector<SegmentPlayer>& centers);

    /**
     * @brief Calculates the distance between a given segment and all other segments in the same cluster.
     *
     * This function calculates the distance between a given segment and all other segments in the same cluster,
     * and returns the sum of these distances.
     *
     * @param cluster A vector of SegmentPlayer objects representing the segments in the same cluster as the given segment.
     * @param segment_index The index of the given segment in the cluster.
     * @return The sum of distances between the given segment and all other segments in the same cluster.
     */
    double distFromOthers(std::vector<SegmentPlayer>& cluster, size_t segment_index);

    /**
     * @brief Start the main loop of the k-medoid algorithm
     * @return An unordered map that map each center to the cluster assiociated to it.
    */
    std::unordered_map<SegmentPlayer, Pair> start();

    /**
     * @brief generates the clusters in the right format.
     * @return An unordered map that map each center as a MainSegment to its cluster.
    */
    std::unordered_map<MainSegment, std::vector<SegmentPlayer>> getClusters();

};