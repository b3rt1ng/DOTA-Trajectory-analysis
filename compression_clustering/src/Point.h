/**
    @class Point
    Represents a point with two coordinates x and y.
    */
#pragma once
#include "all.h"
class Point {
   private:
    double x;  ///< The x-coordinate of the point.
    double y;  ///< The y-coordinate of the point.

   public:
    /**
        @brief Default constructor.
        Constructs a point at the origin (0, 0).
        */
    Point();

    /**
        @brief Parameterized constructor.
        Constructs a point with given x and y coordinates.
        @param x The x-coordinate of the point.
        @param y The y-coordinate of the point.
        */
    Point(double x, double y);

    /**
        @brief Copy constructor.
        Constructs a point by copying another point.
        @param other The point to be copied.
        */
    Point(const Point& other);

    /**
        @brief Gets the x-coordinate of the point.
        @return The x-coordinate of the point.
        */
    double getX() const;

    /**
        @brief Gets the y-coordinate of the point.
        @return The y-coordinate of the point.
        */
    double getY() const;

    /**
        @brief Gets the tuple representation of the point.
        @return A vector containing the x and y coordinates of the point.
        */
    std::vector<double> getTuple();

    /**
        @brief Calculates the Euclidean distance between two points.
        @param other The other point to which the distance is to be calculated.
        @return The Euclidean distance between the two points.
        */
    double distance(Point& other);

    /**
        @brief Overloaded stream insertion operator.
        Inserts the point into the output stream in the format (x, y).
        @param stream The output stream to insert the point into.
        @param other The point to be inserted into the output stream.
        @return The modified output stream.
        */
    friend std::ostream& operator<<(std::ostream& stream, const Point& other);

    /**
        @brief Overloaded equality operator.
        Determines whether two points are equal.
        @param point The point to compare with.
        @return True if the two points are equal, false otherwise.
        */
    bool operator==(const Point& point) const;
};

namespace std {
template <>
struct hash<Point> {
    const size_t operator()(const Point& point) const {
        size_t xHash = std::hash<double>()(point.getX());
        size_t yHash = std::hash<double>()(point.getY()) << 1;
        return xHash ^ yHash;
    }
};
}  // namespace std