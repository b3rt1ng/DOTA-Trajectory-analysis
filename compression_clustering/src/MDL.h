/**
    @file MDL.h
    @brief Header file for the MDL class.
    */
#pragma once
#include "Segment.h"

/**
    @brief The MDL class computes the Minimum Description Length (MDL) principle on a set of points.
    */
class MDL {
   private:
    std::vector<Point>& points;  ///< The set of points to be analyzed. */
    double cst;                  ///< The constant term of the MDL formula. */

   public:
    /**
        @brief Construct a new MDL object.
        @param points A reference to the vector of points to be analyzed.
        */
    MDL(std::vector<Point>& points);

    /**
        @brief Get the vector of points.
        @return A reference to the vector of points.
        */
    std::vector<Point>& getPoints() const;

    /**
        @brief Compute the L(D|H).
        @param index1 The index of the first point.
        @param index2 The index of the second point.
        @return The L(D|H).
        */
    double ldh(int index1, int index2);

    /**
        @brief Compute the L(H).
        @param index1 The index of the first point.
        @param index2 The index of the second point.
        @return The L(H).
        */
    double lh(int index1, int index2);

    /**
        @brief Compute the MDL cost of the line segment defined by two points with a codebook.
        @param index1 The index of the first point.
        @param index2 The index of the second point.
        @return The MDL cost of the line segment defined by the two points with a codebook.
        */
    double mdl_par(int index1, int index2);

    /**
        @brief Compute the MDL cost of the line segment defined by two points.
        @param index1 The index of the first point.
        @param index2 The index of the second point.
        @return The MDL cost of the line segment defined by the two points.
        */
    double mdl_nopar(int index1, int index2);

    /**
        @brief Apply the MDL algorithm to the set of points.
        @return A vector of integers representing the partition of the points.
        */
    std::vector<int> atp();
};