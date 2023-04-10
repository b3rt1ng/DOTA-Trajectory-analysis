/**
 * @brief The SegmentPlayer class represents a line segment in a game played by a specific player.
 *
 * It extends the Segment class and adds a player ID and a game ID to it.
 * It can also be assigned to a specific cluster by setting the cluster ID.
 */
#pragma once
#include "Segment.h"
#include "all.h"

class SegmentPlayer : public Segment {
   private:
    std::string game_id;  ///< The ID of the game this segment belongs to.
    int t1;               ///< The start time of the segment.
    int t2;               ///< The end time of the segment.
    int player_id;        ///< The ID of the player.
    int cluster_id;       ///< The ID of the cluster this segment belongs to, initialized to -1.

   public:
    /**
     * @brief Constructs a new SegmentPlayer object with the given parameters.
     * @param p1 The starting point of the segment.
     * @param p2 The ending point of the segment.
     * @param _game_id The ID of the game this segment belongs to.
     * @param _t1 The start time of the segment.
     * @param _t2 The end time of the segment.
     * @param _player_id The ID of the player.
     * @param _cluster_id The ID of the cluster this segment belongs to, initialized to -1.
     */
    SegmentPlayer(Point p1, Point p2, const std::string& _game_id, int _t1, int _t2, int _player_id, int _cluster_id = -1);

    /**
     * @brief Returns the game ID of this segment.
     * @return The game ID of this segment.
     */
    const std::string& getGameID() const;

    /**
     * @brief Returns the start time of this segment.
     * @return The start time of this segment.
     */
    int getT1() const;

    /**
     * @brief Returns the end time of this segment.
     * @return The end time of this segment.
     */
    int getT2() const;

    /**
     * @brief Returns the player ID of this segment.
     * @return The player ID of this segment.
     */
    int getPlayerID() const;

    /**
     * @brief Returns the cluster ID of this segment.
     * @return The cluster ID of this segment.
     */
    int getClusterID() const;

    /**
     * @brief Sets the cluster ID of this segment to the given value.
     * @param _cluster_id The ID of the cluster to assign to this segment.
     */
    void setClusterID(int _cluster_id);

    /**
     * @brief Overloads the output stream operator to print this segment to the given stream.
     * @param stream The output stream to print to.
     * @param segment The segment to print.
     * @return The output stream with the printed segment.
     */
    friend std::ostream& operator<<(std::ostream& stream, const SegmentPlayer& segment);

    /**
     * @brief Compares this segment to another segment for equality.
     * @param other The segment to compare to.
     * @return true if the segments are equal, false otherwise.
     */
    bool operator==(const SegmentPlayer& other) const;

    /**
     * @brief Overloads the output file stream operator to write this segment to the given file stream.
     * @param ofs The output file stream to write to.
     * @param s The segment to write.
     * @return The output file stream with the written segment
     */
    friend std::ofstream& operator<<(std::ofstream& ofs, SegmentPlayer s);
};

namespace std {
template <>
struct hash<SegmentPlayer> {
    const size_t operator()(const SegmentPlayer& key) const {
        return hash<std::string>()(key.getGameID());
    }
};
}  // namespace std