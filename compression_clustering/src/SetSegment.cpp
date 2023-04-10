#include "SetSegment.h"

SetSegment::SetSegment(std::vector<SegmentPlayer>& _segments)
    : segments(_segments){};
std::vector<SegmentPlayer>& SetSegment::getSegments() const {
    return segments;
}

std::shared_ptr<Point> meanPoint(std::vector<Point>& points) {
    double mean_x = 0.0f;
    double mean_y = 0.0f;

    for (auto point : points) {
        mean_x += point.getX();
        mean_y += point.getY();
    }
    mean_x /= points.size();
    mean_y /= points.size();
    std::shared_ptr<Point> mean = std::make_shared<Point>(Point(mean_x, mean_y));
    return mean;
}

std::shared_ptr<Segment> SetSegment::meanSegment() {
    if (segments.size() == 0)
        return nullptr;
    std::vector<Point> firsts;
    std::vector<Point> seconds;

    for (Segment segment : segments) {
        firsts.push_back(segment.getP1());
        seconds.push_back(segment.getP2());
    }
    std::shared_ptr<Point> mean_first = meanPoint(firsts);
    std::shared_ptr<Point> mean_second = meanPoint(seconds);
    std::shared_ptr<Segment> mean = std::make_shared<Segment>( Segment(*mean_first, *mean_second));
    return mean;
}