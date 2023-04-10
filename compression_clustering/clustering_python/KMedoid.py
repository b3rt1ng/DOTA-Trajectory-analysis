from cplusplus.clustering.Segment import *
import numpy as np
import random as rd
import sys
sys.path.insert(0, '..')


class KMedoid:
    """Class implementing the K-medoi algorithm
    Args:
        segments(list) : list of  segments; the data
        nbcluster(int) : the wanted number of clusters
    """

    def __init__(self, segments, nb_cluster):
        self._segments = segments
        self._nbcluster = nb_cluster

    def get_segments(self):
        return self._segments

    def get_nb_cluster(self):
        return self._nbcluster

    def get_centers(self):
        """function for picking random segments as centers

        Returns:
            list: list of segments as medoids centers
        """
        listlen = len(self._segments)
        setM = set()
        res = []
        for _ in range(self._nbcluster):
            random_index = rd.randint(0, listlen - 1)
            while random_index in setM:
                random_index = rd.randint(0, listlen - 1)
            setM.add(random_index)
            res.append(self._segments[random_index])
        return res

    def generate_medoid(self, centers):
        """Function that generate medoids: a medoid is a cluster of segment

        Args:
            centers (list[Segment]): list of centers of the medoids

        Returns:
            {center: [(segment, dist(center, segment)]}: a map associating each center to a list of segments
        """
        res = {center: [(center, 0)] for center in centers}
        for segment in self._segments:
            if segment in res:
                continue
            dist_from_center = np.Inf
            associated_center = None
            for center in centers:
                try:
                    dist = segment.distance(center)
                except:
                    print("center", center.get_p1().get_tuple(), center.get_p2().get_tuple())
                    print("center", segment.get_p1().get_tuple(), segment.get_p2().get_tuple())
                if dist < dist_from_center:
                    dist_from_center = dist
                    associated_center = center
            res[associated_center].append((segment, dist_from_center))
        return res

    def cost(self, medoids):
        """Function that compute the cost of the current clustirisation
            The cost is how much the center represent the cluster as a whole

        Args:
            medoids (_type_): map {center: [(segment, dist(center, segment)]}

        Returns:
            int : the cost 
        """
        res = 0
        for medoid in medoids:
            for i in range(len(medoids[medoid])):
                res += medoids[medoid][i][1]
        return res

    def kMedoids(self):
        """Function that partitions a list of segment into clusters

        Returns:
            {center: [(segment, dist(center, segment)]}: map of centers as key and the list of segments associated with it as value
        """
        centers = self.get_centers()
        medoids = self.generate_medoid(centers)
        current_cost = self.cost(medoids)
        res = medoids
        for center in centers:
            for point in self._segments:
                new_centers = [x if x != center else point for x in centers]
                new_medoids = self.generate_medoid(new_centers)
                new_cost = self.cost(new_medoids)
                if new_cost < current_cost:
                    current_cost = new_cost
                    res = new_medoids
        return res
