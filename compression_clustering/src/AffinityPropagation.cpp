#include "AffinityPropagation.h"
#include "SetSegment.h"
#include <omp.h>
#include <vector>
#include <algorithm>
#include <limits>
#include <chrono>
using namespace std::chrono;

#include <iostream>
#include <boost/multi_array.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/operation.hpp>
#define BOOST_NO_CXX11_TEMPLATE_ALIASES

namespace ublas = boost::numeric::ublas;

void print_matrix(std::vector<std::vector<double>> matrix);
void print_matrix_boost(boost::multi_array<double, 2> matrix);

AffinityPropagation::AffinityPropagation(std::vector<SegmentPlayer>& _segments, int _nbIter, double _angle_factor, double _perpendicular_factor, double _parallel_factor, int _nb_thread)
    : Clustering(_segments, 0, _nbIter, _angle_factor, _perpendicular_factor, _parallel_factor, _nb_thread) {}


std::vector<std::vector<double>> AffinityPropagation::generateMatrix(int size) {
    std::vector<std::vector<double>> matrix(size, std::vector<double>(size, 0));
    return matrix;
}

std::vector<std::vector<SegmentPlayer>> AffinityPropagation::start() {

    /* std::vector<SegmentPlayer> tmp_segs;
    for(int i = 0; i < 7; i++) {
        tmp_segs.push_back(segments[i]);
    }
    segments = tmp_segs; */
    omp_set_num_threads(nb_thread);
    std::vector<std::vector<SegmentPlayer>> res;
    const double damping = 0.9;


    typedef boost::multi_array<double, 2> array_2d;

    const double inf = std::numeric_limits<double>::infinity();
    size_t n = segments.size();
    //n = 5;

    array_2d A(boost::extents[n][n]);  // responsibilities
    array_2d R(boost::extents[n][n]);  // availabilities
    array_2d S(boost::extents[n][n]);  // similarities
    array_2d tmp(boost::extents[n][n]);  // temporary array

    ublas::vector<int> I(n);  // indices of the maximum element in each row
    ublas::vector<double> Y(n);  // maximum element in each row
    ublas::vector<double> Y2(n);  // second-largest element in each row
    ublas::vector<double> dA(n);  // diagonal of A

    // initialize
    double minimum = inf;
    #pragma omp parallel for
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j <= i; j++) {
            A[i][j] = 0;
            R[i][j] = 0;
            A[j][i] = 0;
            R[j][i] = 0;
            double dist = -segments[i].distance(segments[j], angle_factor, perpendicular_factor, parallel_factor);
            S[i][j] = dist;
            S[j][i] = dist;
            if (dist < minimum) {
                minimum = dist;
            }
        }
    }

    

    // diagonal of S set to minimum value
    for (size_t i = 0; i < n; i++) {
        S[i][i] = minimum;
        //S[i][i] = - DBL_MAX;
    }

    /* for(size_t i = 0; i < n; i++) {
        S[i][i] = -22;
    }
    S[0][1] = -7;
    S[1][0] = -7;
    S[0][2] = -6;
    S[2][0] = -6;
    S[0][3] = -12;
    S[3][0] = -12;
    S[0][4] = -17;
    S[4][0] = -17;

    S[1][2] = -17;
    S[2][1] = -17;
    S[1][3] = -17;
    S[3][1] = -17;
    S[1][4] = -22;
    S[4][1] = -22;

    S[2][3] = -18;
    S[3][2] = -18;
    S[2][4] = -21;
    S[4][2] = -21;

    S[3][4] = -3;
    S[4][3] = -3; */




    //print_matrix_boost(S);
    //std::cout << "similarity matrix: " << std::endl;
    //print_matrix_boost(R);
    //std::cout << "responsibility matrix: " << std::endl;
    //print_matrix_boost(A);
    //std::cout << "availability matrix: " << std::endl;
    //std::cin.get();

        
    double total_time = 0;

    // main loop
    for (int it = 0; it < max_iteration; ++it) {
        std::clock_t t0 = std::clock();
        std::cout << "--------------------------------------" << std::endl;
        #pragma omp parallel
        {
        #pragma omp single
        {
        // tmp = A + S; compute responsibilities
        std::transform(A.data(), A.data() + A.num_elements(), S.data(), tmp.data(), std::plus<double>());        
        }
        //#pragma omp parallel for
        #pragma omp for schedule(static)
        for (size_t i = 0; i < n; i++) {
            auto begin = tmp.data() + i * n;
            auto end = begin + n;
            auto max_it = std::max_element(begin, end);
            I[i] = max_it - begin;
            Y[i] = *max_it;
            *max_it = -inf;
            Y2[i] = *std::max_element(begin, end); 
        }

        // tmp = Rnew
        //#pragma omp parallel for
        #pragma omp for schedule(static)
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                tmp[i][j] = S[i][j] - Y[i];
            }
            tmp[i][I[i]] = S[i][I[i]] - Y2[i];
        }
        
        //  # Damping
        //#pragma omp parallel for
        #pragma omp for schedule(static)
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                tmp[i][j] = tmp[i][j] * (1 - damping);
                R[i][j] = R[i][j] * damping;
                R[i][j] = R[i][j] + tmp[i][j];
            }
        }

        //print_matrix_boost(R);
        //std::cout << "responsibility matrix: " << std::endl;
        //std::cin.get();

        // np.maximum(R, 0, tmp)
        //#pragma omp parallel for
        #pragma omp for schedule(static)
        for(size_t i = 0; i < n; i++) {
            for(size_t j = 0; j < n; j++) {
                tmp[i][j] = std::max(R[i][j], 0.0);
            }
        }

        // tmp.flat[:: n_samples + 1] = R.flat[:: n_samples + 1]
        #pragma omp for schedule(static)
        for (size_t i = 0; i < n; ++i) {
            tmp[i][i] = R[i][i];
        }


         // Update A and D
        // tmp -= np.sum(tmp, axis=0)
        //#pragma omp parallel for
        #pragma omp for schedule(static)
        for (std::size_t j = 0; j < n; ++j) {
            double sum = 0;
            for (std::size_t i = 0; i < n; ++i) {
                sum += tmp[i][j];
            }
            for (std::size_t i = 0; i < n; ++i) {
                tmp[i][j] -= sum;
            }
        }   

        // dA = np.diag(tmp).copy()
        //boost::numeric::ublas::vector<double> dA(n);
        #pragma omp for schedule(static)
        for (size_t i = 0; i < n; ++i) {
            dA[i] = tmp[i][i];
        }

        // tmp.clip(0, np.inf, tmp)
        #pragma omp for schedule(static)
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                tmp[i][j] = std::max(tmp[i][j], 0.0);
            }
        }

        // tmp.flat[:: n_samples + 1] = dA
        #pragma omp for schedule(static)
        for (size_t i = 0; i < n; ++i) {
            tmp[i][i] = dA[i];
        }

        // Damping
        #pragma omp for schedule(static)
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                tmp[i][j] = (1 - damping) * tmp[i][j];
                A[i][j] = A[i][j] * damping;
                A[i][j] = A[i][j] - tmp[i][j];
            }
        }

        //print_matrix_boost(A);
        //std::cout << "availibility matrix: " << std::endl;
        //std::cin.get();
        
        }
        std::clock_t t1 = std::clock();
        double chrono =  (t1 - t0) / (double)CLOCKS_PER_SEC ;
        total_time += chrono / nb_thread;
        std::cout << "time taken: " << chrono / nb_thread << std::endl;
        std::cout << "iteration " << it << " done" << std::endl;
        std::cout << "estimated time: " << (total_time / it) * (max_iteration - it) << "s" << std::endl;
    }
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Iterations done" << std::endl;

    //array_2d E(boost::extents[n][n]);
    std::vector<int> centers(n, -1);

    #pragma omp parallel for
    for(size_t i = 0; i < n; i++){
        int imax = -1;
        double max = -DBL_MAX;
        for(size_t j = 0; j < n; j++){
            tmp[i][j] = R[i][j] + A[i][j];
            if(tmp[i][j] > max){
                max = tmp[i][j];
                imax = j;
            }
        }
        centers[i] = imax;
    }

    std::cout << "centers: ----------------- " << std::endl;
    // Unique centers
    std::sort(centers.begin(), centers.end());
    centers.erase(std::unique(centers.begin(), centers.end()), centers.end());
    std::cout << std::endl;

    std::cout << "\nNB centers: " << centers.size() << std::endl;
    //std::cin.get();

    res = std::vector<std::vector<SegmentPlayer>>(centers.size());
    clusters = std::unordered_map<MainSegment, std::vector<SegmentPlayer>>(centers.size());

    for(int i = 0; (size_t) i < centers.size(); i++){
        double ind = centers[i];
        double val = tmp[ind][ind];
        for(size_t j = 0; j < n; j++){
            if(tmp[ind][j] == val){
                res[i].push_back(segments[j]);
            }
        }
        //SetSegment setSegment = SetSegment(res[i]);
        //Segment segment = *setSegment.meanSegment();
        //MainSegment mainSegment = MainSegment(segment.getP1(), segment.getP2(), i);
        MainSegment mainSegment = MainSegment(segments[ind].getP1(), segments[ind].getP2(), i);
        clusters[mainSegment] = res[i];
    }

    return res;
}

std::unordered_map<MainSegment, std::vector<SegmentPlayer>> AffinityPropagation::getClusters(){
        start();
        return clusters;
}


void print_matrix(std::vector<std::vector<double>> matrix) {
    //pretty print matrix evenly spaced

    std::cout << "matrix size " << matrix.size() << std::endl;
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix.size(); j++) {
            std::cout << std::setw(10) << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}


void print_matrix_boost(boost::multi_array<double, 2> matrix) {
    //pretty print matrix evenly spaced

    std::cout << "matrix size " << matrix.size() << std::endl;
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix.size(); j++) {
            std::cout << std::setw(10) << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}