from cplusplus.clustering.Segment import *
import numpy as np
import sys
sys.path.insert(0, '..')

# from numba import jit
# from numba.typed import List


class MDL:
    """Class representing Minimum Description Lenght(MDL) principle
    Args:
        points(list[Point]): list of points representing the original trajectory
    """

    def __init__(self, points):
        self._points = points
        self._cst = 1e-10

    def ldh(self, index1: int, index2: int):
        """Fonction computing the L(D|H) i.e the preciness
        Args:
            points(list[Point]): list of points representing the original trajectory
            index1 (int): the index of the first point
            index2 (int): the index of the second point
        Returns:
            float: the L(D|H)
        """
        res = 0
        segmentC = Segment(self._points[index1], self._points[index2])
        for k in range(index1, index2 - 1):
            segmentK = Segment(self._points[k], self._points[k + 1])
            dist_perp, _, dist_angle = segmentC.all_distances(segmentK)
            res += np.log2(dist_perp + 1e-10)
            res += np.log2(dist_angle + 1e-10)
        return res

    def lh(self, index1: int, index2: int):
        dist = self._points[index1].distance(self._points[index2])
        return np.log2(dist + 1e-10)

    def mdl_par(self, index1: int, index2: int):
        res = self.lh(self._points, index1, index2)
        res += self.ldh(self._points, index1, index2)
        return res

    def mdl_nopar(self, index1: int, index2: int):
        res = 0
        for i in range(index1, index2):
            res += self.lh(self._points, i, i+1)
        return res

    def atp(self, compression=False):
        """Funtion implementing the Approximate Trajectory Partionning

            Args:
                points(list[Point]): list of points representing the original trajectory
                compression (bool, optional): Parameter to define if we want to get the compression ratio. Defaults to False.

            Returns:
                list[Point]: a compressed trajectory of points
                list[int]: a list of the index of calculated carateristic points from arg list ~~~reformuler
        """
        res = list()
        resIndex = list()
        res.append(self._points[0])
        resIndex.append(0)
        points_size = len(self._points)
        start, lenght = 0, 1
        while (start + lenght < points_size):
            curr = start + lenght

            if self._points[start].distance(self._points[curr]) == 0:
                lenght += 1
                continue

            # if ldh(self._points ,start, curr) > 0:
            if self.mdl_par(self._points, start, curr) > self.mdl_nopar(self._points, start, curr):
                res.append(self._points[curr - 1])
                resIndex.append(curr - 1)
                start = curr - 1
                lenght = 1
            else:
                lenght += 1
        res.append(self._points[-1])
        resIndex.append(len(self._points)-1)
        if compression:
            return res, resIndex, len(res) / len(self._points)
        return res, resIndex
