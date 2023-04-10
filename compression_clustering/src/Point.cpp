#include "Point.h"

Point::Point() {
    x = 0.0f;
    y = 0.0f;
}

Point::Point(double _x, double _y)
    : x(_x), y(_y) {}

Point::Point(const Point& other) {
    x = other.getX();
    y = other.getY();
}

double Point::getX() const {
    return x;
}

double Point::getY() const {
    return y;
}

std::vector<double> Point::getTuple() {
    std::vector<double> tuple = {x, y};
    return tuple;
}

double Point::distance(Point& p2) {
    double square = std::pow(x - p2.getX(), 2) + std::pow(y - p2.getY(), 2);
    return std::sqrt(square);
}

std::ostream& operator<<(std::ostream& stream, const Point& point) {
    stream << "(" << point.getX() << ", " << point.getY() << ")";
    return stream;
}

bool Point::operator==(const Point& other) const {
    return x == other.getX() && y == other.getY();
}
