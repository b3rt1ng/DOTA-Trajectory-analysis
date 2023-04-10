/**

    @file Segment.h
    @brief Header file for the Segment class.
    */

#pragma once

#include <iostream>
#include <vector>
#include "Point.h"

/**

    @brief A class representing a line segment in 2D space.
    */
class Segment {
   protected:
    Point p1;      /**< The first endpoint of the segment. */
    Point p2;      /**< The second endpoint of the segment. */
    double length; /**< The length of the segment. */

   public:
    /**
        @brief Default constructor for the Segment class.
        */
    Segment();

    /**
        @brief Constructor for the Segment class.
        @param _p1 The first endpoint of the segment.
        @param _p2 The second endpoint of the segment.
        */
    Segment(Point _p1, Point _p2);

    /**
        @brief Copy constructor for the Segment class.
        @param other The segment to copy.
        */
    Segment(const Segment& other);

    /**
        @brief Returns the first endpoint of the segment.
        @return The first endpoint of the segment.
        */
    Point getP1() const;

    /**
        @brief Returns the second endpoint of the segment.
        @return The second endpoint of the segment.
        */
    Point getP2() const;

    /**
        @brief Returns the length of the segment.
        @return The length of the segment.
        */
    double getLength() const;

    /**
        @brief Returns the vector representation of the segment.
        @return A pair representing the x and y components of the vector.
        */
    std::pair<double, double> getVector() const;

    /**
        @brief Returns a vector containing the distances between each endpoint of the segment and the corresponding endpoint of another segment.
        @param other The other segment to compare to.
        @param angle_factor The weight to give to the angle between the segments.
        @param perpendicular_factor The weight to give to the perpendicular distance between the segments.
        @param parallel_factor The weight to give to the parallel distance between the segments.
        @return A vector containing the distances between each endpoint of the segment and the corresponding endpoint of the other segment.
        */
    std::vector<double> allDistances(const Segment& other, double angle_factor = 1, double perpendicular_factor = 1, double parallel_factor = 1);

    /**
        @brief Returns the distance between the segment and another segment.
        @param other The other segment to compare to.
        @param angle_factor The weight to give to the angle between the segments.
        @param perpendicular_factor The weight to give to the perpendicular distance between the segments.
        @param parallel_factor The weight to give to the parallel distance between the segments.
        @return The distance between the segment and the other segment.
        */
    double distance(const Segment& ohter, double angle_factor = 1, double perpendicular_factor = 1, double parallel_factor = 1);

    /**
        @brief Overloaded output stream operator for the Segment class.
        @param stream The output stream to write to.
        @param segment The segment to output.
        @return The output stream.
        */
    friend std::ostream& operator<<(std::ostream& stream, const Segment& segment);

    /**
        @brief Overloaded equality operator for the Segment class.
        @param The other segment to compare to.
        @return true if the segments are equal, false otherwise.
        */
    bool operator==(const Segment&) const;

    /**
        @brief Overloaded output stream operator for the Segment class.
        @param ofs The file to write to.
        @param s The segment to write.
        @return The output file stream.
    */
    friend std::ofstream& operator<<(std::ofstream& ofs, Segment s);
};

namespace std {
template <>
struct hash<Segment> {
    const size_t operator()(const Segment& s) const {
        size_t hash1 = hash<Point>{}(s.getP1());
        size_t hash2 = hash<Point>{}(s.getP2()) << 1;
        return hash1 ^ hash2;
    }
};
}  // namespace std