from cplusplus.clustering.Segment import *
import numpy as np


class SetSegment:

    def __init__(self, set_segment: set):
        self._set_segment = set_segment

    def get_segments(self):
        return self._set_segment

    def mean_vector(self):
        vectors = np.array(
            list(map(lambda s: s.get_vector(), self._set_segment)))
        vector_mean = np.mean(vectors, 0)
        return vector_mean

    def mean_segment(self):
        if len(self._set_segment) == 0:
            return None
        
        firsts = np.array( list( map( lambda s: s.get_p1().get_tuple() , self._set_segment ) ) )
        seconds = np.array( list( map( lambda s: s.get_p2().get_tuple() , self._set_segment ) ) )
        mean_first = np.mean(firsts, 0) 
        mean_second = np.mean(seconds, 0)
        p1 = Point(mean_first[0], mean_first[1])
        p2 = Point(mean_second[0], mean_second[1])
        return Segment(p1, p2)

    def medoidSeg(self):
        minVal = int('inf')
        minS = None
        for s1 in self._set_segment:
            tot = 0
            for s2 in self._set_segment:
                tot += s1.distance(s2)
            if minVal > tot:
                minVal = tot
                minS = s1
        return minS

    def medoidSegFast(self):
        meanS = self.meanSeg(self._set_segment)
        minS = None
        minVal = int('inf')

        for s in self._set_segment:
            dist = s.distance(meanS)
            if (dist < minVal):
                minS = s
                minVal = dist
        return minS
