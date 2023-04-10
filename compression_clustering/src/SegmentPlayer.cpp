#include "SegmentPlayer.h"

SegmentPlayer::SegmentPlayer(Point p1, Point p2, const std::string& _game_id, int _t1, int _t2, int _player_id, int _cluster_id)
    : Segment(p1, p2), game_id(_game_id), t1(_t1), t2(_t2), player_id(_player_id), cluster_id(_cluster_id) {}

const std::string& SegmentPlayer::getGameID() const {
    return game_id;
}

int SegmentPlayer::getPlayerID() const {
    return player_id;
}

int SegmentPlayer::getT1() const {
    return t1;
}

int SegmentPlayer::getT2() const {
    return t2;
}

int SegmentPlayer::getClusterID() const {
    return cluster_id;
}

void SegmentPlayer::setClusterID(int _cluster_id) {
    cluster_id = _cluster_id;
}

std::ostream& operator<<(std::ostream& stream, const SegmentPlayer& segment) {
    stream << "[" << segment.getP1() << ", " << segment.getP2() << ", game_id: " 
    << segment.getGameID() << ", t1: " << segment.getT1() << ", player_id: " << segment.getPlayerID() << "]";
    return stream;
}

bool SegmentPlayer::operator==(const SegmentPlayer& other) const {
  return game_id == other.game_id && t1 == other.t1 && player_id == other.player_id;
}

std::ofstream& operator<<(std::ofstream& ofs, SegmentPlayer s){
    ofs << s.game_id << "," << s.player_id  << "," << s.p1.getX() << "," << s.p1.getY() << "," << s.p2.getX() << "," << s.p2.getY() << "\n";
    return ofs;
}