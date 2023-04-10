#include "MDL.h"

MDL::MDL(std::vector<Point>& _points)
    : points(_points), cst(1e-10) {}

std::vector<Point>& MDL::getPoints() const{
    return points;
}    

double MDL::ldh(int index1, int index2) {
    double d_perp = 0;
    double d_ang = 0;
    Segment seg1(points[index1], points[index2]);
    for (int k = index1 + 1; k <= index2; k++) {
        const Segment seg2(points[index1], points[k]);
        auto dists = seg1.allDistances(seg2);
        d_ang += dists[0];
        d_perp += dists[1];
    }
    return log2(d_perp + cst) + log2(d_ang + cst);
}

double MDL::lh(int index1, int index2) {
    double dist = points[index1].distance(points[index2]);
    return log2(dist + cst);
}

double MDL::mdl_par(int index1, int index2) {
    double res = lh(index1, index2) + ldh(index1, index2);
    return res;
}

double MDL::mdl_nopar(int index1, int index2) {
    double dist = 0.0f;
    for (int i = index1; i < index2; i++)
        dist += points[i].distance(points[i + 1]);
    return log2(dist + cst);
}

std::vector<int> MDL::atp() {
    std::vector<int> res_index;
    res_index.push_back(0);
    int start = 0;
    int lenght = 1;
    int curr;
    while (start + lenght < (int) points.size()) {
        curr = start + lenght;
        if (points[start].distance(points[curr]) == 0) {
            lenght += 1;
            continue;
        }
        if (mdl_par(start, curr) > mdl_nopar(start, curr)) {
            res_index.push_back(curr - 1);
            start = curr - 1;
            lenght = 1;
        } else {
            lenght += 1;
        }
    }
    
    res_index.push_back(points.size() - 1);
    return res_index;
}