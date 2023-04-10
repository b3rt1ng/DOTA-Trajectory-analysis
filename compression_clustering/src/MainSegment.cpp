#include "MainSegment.h"

MainSegment::MainSegment(Point p1, Point p2, int _id)
    : Segment(p1, p2), id(_id) {}

int MainSegment::getSegmentID() const {
    return id;
}

void MainSegment::setID(int _id) {
    id = _id;
}

bool MainSegment::operator==(const MainSegment& other) const {
    return p1 == other.getP1() && p2 == other.getP2() && id == other.id;
}

std::ostream& operator<<(std::ostream& stream, const MainSegment& segment) {
    stream << "[" << segment.getP1() << ", " << segment.getP2() << ", ID: " << segment.getSegmentID() << "]";
    return stream;
}

std::ofstream& operator<<(std::ofstream& ofs, MainSegment s) {
    ofs << s.id << "," << s.p1.getX() << "," << s.p1.getY() << "," << s.p2.getX() << "," << s.p2.getY() << " id:" << s.getSegmentID() << "\n";
    return ofs;
}
