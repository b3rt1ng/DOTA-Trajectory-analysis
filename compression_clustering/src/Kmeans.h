/**
    @brief The Kmeans class represents an implementation of the k-means algorithm.
    The k-means algorithm is a clustering algorithm that partitions a dataset into k clusters, where each cluster
    is represented by its centroid (the mean of the data points in the cluster).
    This implementation extends the Clustering class.
    */
#pragma once
#include "Clustering.h"

class Kmeans : public Clustering {
   private:
    double min_x, max_x, min_y, max_y;

   public:
    /**
     * @brief Constructor for the Kmeans class.
     *
     * @param segment A vector of SegmentPlayer objects representing the dataset to be clustered.
     * @param nb_cluster The desired number of clusters.
     * @param nb_iteration The maximum number of iterations for the algorithm.
     * @param angle_factor A factor used to calculate the distance between two segments based on their angle.
     * @param perpendicular_factor A factor used to calculate the distance between two segments based on their perpendicular distance.
     * @param parallel_factor A factor used to calculate the distance between two segments based on their parallel distance.
     * @param nb_thread The number of threads to use for the algorithm.
     */
    Kmeans(std::vector<SegmentPlayer>& segment, int nb_cluster, int nb_iteration, double angle_factor = 1.0, double perpendicular_factor = 1.0, double parallel_factor = 1.0, int nb_thread = 1);

    /**
     * @brief Getter method to retrieve the centroids of each cluster.
     *
     * @return A vector of Segment objects representing the centroids of each cluster.
     */
    std::vector<Segment> getCenters();

    /**
     * @brief Calculates the association between each segment and its nearest centroid.
     *
     * This function calculates the distance between each segment and each centroid, and assigns each segment to the centroid
     * with the smallest distance. The function returns a vector of vectors of SegmentPlayer objects, where each vector of
     * SegmentPlayer objects represents the segments associated with a centroid.
     *
     * @param centers A vector of Segment objects representing the centroids of each cluster.
     * @return A vector of vectors of SegmentPlayer objects, where each vector of SegmentPlayer objects represents the segments
     *         associated with a centroid.
     */
    std::vector<std::vector<SegmentPlayer>> association(std::vector<Segment>& centers);

    /**
     * @brief Calculates the centroids of each cluster.
     *
     * This function takes a vector of vectors of SegmentPlayer objects, where each vector of SegmentPlayer objects represents
     * the segments associated with a centroid, and calculates the centroid of each cluster.
     *
     * @param clusters A vector of vectors of SegmentPlayer objects.
     * @return A vector of Segment objects representing the centroids of each cluster.
     */
    std::vector<Segment> computeCentroids(std::vector<std::vector<SegmentPlayer>>& clusters);

    /**
     * @brief Starts the k-means algorithm.
     *
     * This function initializes the centroids, assigns the segments to the closest centroid, recalculates the centroids,
     * and repeats the process until convergence or the maximum number of iterations is reached. The function returns a
     * vector of vectors of SegmentPlayer objects, where each vector of SegmentPlayer objects represents the segments
     * associated with a centroid.
     *
     * @return A vector of vectors of SegmentPlayer objects.
     */
    std::vector<std::vector<SegmentPlayer>> start();
    
    /**
     * @brief generates the clusters in the right format.
     * @return An unordered map that map each center as a MainSegment to its cluster.
     */
    std::unordered_map<MainSegment, std::vector<SegmentPlayer>> getClusters();
};