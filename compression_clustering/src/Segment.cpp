#include "Segment.h"

Segment::Segment() {
}

Segment::Segment(Point _p1, Point _p2)
    : p1(_p1), p2(_p2) {
    length = p1.distance(p2);
}

Segment::Segment(const Segment& other)
    : p1(other.p1), p2(other.p2) {
    length = other.length;
}

Point Segment::getP1() const {
    return p1;
}

Point Segment::getP2() const {
    return p2;
}

double Segment::getLength() const {
    return length;
}

std::pair<double, double> Segment::getVector() const {
    std::pair<double, double> vect;
    double first = p2.getX() - p1.getX();
    double second = p2.getY() - p1.getY();
    vect.first = first;
    vect.second = second;
    return vect;
}

double norm(std::vector<double>& vect1, std::vector<double>& vect2) {
    double res = 0.0;
    std::vector<double> temp(2);
    for (size_t i = 0; i < vect1.size(); i++) {
        temp[i] = std::pow(vect1[i] - vect2[i], 2);
    }
    for (size_t i = 0; i < vect1.size(); i++) {
        res += temp[i];
    }
    return std::sqrt(res);
}

std::vector<double> Segment::allDistances(const Segment& segment2, double angle_factor, double perpendicular_factor, double parallel_factor) {


    std::vector<double> res(3, 1.0);
    res[0] = angle_factor;
    res[1] = perpendicular_factor;
    res[2] = parallel_factor;

    const Segment shorter = (getLength() > segment2.getLength()) ? Segment(segment2) : Segment(*this);
    const Segment longer = (getLength() > segment2.getLength()) ? Segment(*this) : Segment(segment2);
    
    Point si = longer.getP1();
    Point ei = longer.getP2();

    Point sj = shorter.getP1();
    Point ej = shorter.getP2();

    Point ps;
    Point pe;
    
    double sieiX = ei.getX() - si.getX();
    double sieiY = ei.getY() - si.getY();

    double sjejX = ej.getX() - sj.getX();
    double sjejY = ej.getY() - sj.getY();

    double sieiNorm = si.distance(ei);
    double sjejNorm = sj.distance(ej);

    // angle dist (res[0])
    double sieisjej = ( sieiX * sjejX ) + ( sieiY * sjejY );
    double tmp = sieisjej / ( sieiNorm * sjejNorm );
    double angle = std::acos(tmp);
    if(angle > M_PI) {
        angle = 2 * M_PI - angle;
    }
    if(angle > M_PI || angle < 0) { // Verification
        std::cout << "Error in angle: " << angle <<"\n";
        exit(1);
    }
    res[0] *= shorter.getLength();
    if(angle < M_PI / 2) {
        res[0] *= std::sin(angle);
    }

    // perpendicular dist (res[1]) & parallel dist (res[2])
    
    double squareNormSiEi = sieiNorm * sieiNorm;

    double sisjX = sj.getX() - si.getX();
    double sisjY = sj.getY() - si.getY();

    double sisjsiei = ( sisjX * sieiX ) + ( sisjY * sieiY );
    double u1 = sisjsiei / ( squareNormSiEi );

    double siejX = ej.getX() - si.getX();
    double siejY = ej.getY() - si.getY();

    double siejsiei = ( siejX * sieiX ) + ( siejY * sieiY );
    double u2 = siejsiei / ( squareNormSiEi );
    
    double psX = si.getX() + ( u1 * sieiX );
    double psY = si.getY() + ( u1 * sieiY );

    double peX = si.getX() + ( u2 * sieiX );
    double peY = si.getY() + ( u2 * sieiY );

    ps = Point(psX, psY);
    pe = Point(peX, peY);

    // perpendicular dist
    double l1perp = ps.distance(sj);
    double l2perp = pe.distance(ej);
    res[1] *= ( (l1perp * l1perp) + (l2perp * l2perp) ) / (l1perp + l2perp + 0.0000001);

    // parallel dist
    double l1par = ps.distance(si);
    double l2par = pe.distance(ei);
    res[2] *= ( ( l1par < l2par ) ? l1par : l2par );

    return res;
}

double Segment::distance(const Segment& segment2,
                        double angle_factor,
                        double perpendicular_factor,
                        double parallel_factor ) {
    auto temp = allDistances(segment2, angle_factor, perpendicular_factor, parallel_factor);
    double res = 0.0;
    for (size_t i = 0; i < temp.size(); i++) {
        res += temp[i];
    }
    return res;
}

std::ostream& operator<<(std::ostream& stream, const Segment& segment) {
    stream << "[" << segment.getP1() << ", " << segment.getP2() << "]";
    return stream;
}

bool Segment::operator==(const Segment& other) const {
    return p1 == other.getP1() && p2 == other.getP2();
}

std::ofstream& operator<<(std::ofstream& ofs, Segment s) {
    ofs << s.p1.getX() << "," << s.p1.getY() << "," << s.p2.getX() << "," << s.p2.getY() << "\n";
    return ofs;
}
