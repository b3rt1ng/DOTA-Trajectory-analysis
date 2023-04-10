/**
    @file MainSegment.h
    @brief This file declares the MainSegment class, which represents a main segment.
    */

#pragma once
#include "Segment.h"

/**

    @brief The MainSegment class represents a main segment.
    */
class MainSegment : public Segment {
   private:
    int id; /**< The segment id. */

   public:
    /**
        @brief Construct a new MainSegment object.
        @param p1 The first point of the segment.
        @param p2 The second point of the segment.
        @param _id The segment id.
        */
    MainSegment(Point p1, Point p2, int _id);

    /**
        @brief Get the segment id.
        @return The segment id.
        */
    int getSegmentID() const;

    /**
        @brief Set the segment id.
        @param _id The segment id to set.
        */
    void setID(int _id);

    /**
        @brief Overload the << operator to output the segment.
        @param stream The output stream.
        @param segment The segment to output.
        @return The output stream.
        */
    friend std::ostream& operator<<(std::ostream& stream, const MainSegment& segment);

    /**
        @brief Overload the == operator to compare two segments.
        @param other The other segment to compare.
        @return True if the two segments are equal, false otherwise.
        */
    bool operator==(const MainSegment& other) const;

    /**
        @brief Overload the << operator to output the segment to a file stream.
        @param ofs The file stream.
        @param s The segment to output.
        @return The file stream.
        */
    friend std::ofstream& operator<<(std::ofstream& ofs, MainSegment s);
};

namespace std {
template <>
struct hash<MainSegment> {
    const size_t operator()(const MainSegment& key) const {
        size_t h1 = hash<int>()(key.getSegmentID());
        return h1;
    }
};
}  // namespace std