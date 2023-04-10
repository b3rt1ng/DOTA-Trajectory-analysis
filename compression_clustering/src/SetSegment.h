/**
    @file SetSegment.h
    @brief Header file for SetSegment class
    */

#pragma once
#include "Segment.h"
#include "SegmentPlayer.h"

/**
    @brief A class representing a set of Segment objects
    */
class SetSegment {
   private:
    std::vector<SegmentPlayer>& segments;  ///< The vector of SegmentPlayer objects in the set

   public:
    /**
        @brief Construct a new SetSegment object
        @param segments The vector of SegmentPlayer objects to be stored in the set
        */
    SetSegment(std::vector<SegmentPlayer>& segments);

    /**
        @brief Get the vector of SegmentPlayer objects stored in the set
        @return std::vector<SegmentPlayer>& A reference to the vector of SegmentPlayer objects
        */
    std::vector<SegmentPlayer>& getSegments() const;

    /**
        @brief Calculate the mean segment of the set
        @return std::shared_ptr<Segment> A shared pointer to the mean segment
        */
    std::shared_ptr<Segment> meanSegment();
};