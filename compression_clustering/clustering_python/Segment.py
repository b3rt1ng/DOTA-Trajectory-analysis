import numpy as np

from numba import jit

class Point:
    """Class representing a point in a two dimesion space
    Args:
        x(int): the abscisse
        y(int): the ordinate
    """

    def __init__(self, x, y):
        self._x = x
        self._y = y

    def get_x(self) -> int:
        return self._x

    def get_y(self) -> int:
        return self._y

    def get_tuple(self) -> tuple:
        return self._x, self._y

    @property
    def x(self) -> int:
        return self._x

    @property
    def y(self) -> int:
        return self._y

    def distance(self, p2) -> float:
        """Function computing the distance between two points

        Args:
            p2 (Point): the second point

        Returns:
            float: the distance
        """
        return np.linalg.norm(np.array(self.get_tuple()) - np.array(p2.get_tuple()))

    def __str__(self) -> str:
        return "(" + str(self._x) + ", " + str(self._y) + ")"


class Segment:
    """Class representing a segment
    Agrs
        p1(Point): the first point of the segment
        p2(Point): the second point of the segment
    """
    _p1: Point
    _p2: Point

    def __init__(self, p1: Point, p2: Point):
        self._p1 = p1
        self._p2 = p2

        adj = p1.distance(Point(p2.get_x(), p1.get_y()))
        hyp = p1.distance(p2)
        self._angle = np.arccos(adj / hyp) if hyp > 0 else 0

        self._length = hyp

    def get_p1(self) -> Point:
        return self._p1

    def get_p2(self) -> Point:
        return self._p2

    def get_angle(self) -> float:
        return self._angle

    def get_length(self) -> float:
        return self._length

    def get_vector(self) -> tuple:
        return (self._p2.get_x() - self._p1.get_x(), self._p2.get_y() - self._p1.get_y())

    @property
    def p1(self) -> Point:
        return self._p1

    @property
    def p2(self) -> Point:
        return self._p2

    @property
    def angle(self) -> float:
        return self._angle

    @property
    def length(self) -> float:
        return self._length


    def __str__(self) -> str:
        return str(self._p1) + " -> " + str(self._p2)

    def all_distances(self, segment2, angle_factor=1, perpendicular_factor=1, parallel_factor=0):
        """Helper function computing all the distances: the parallel, angular and perpendicular distance

        Args:
            segment2 (Segment): the second segment
            angle_factor (int, optional): defines the weight. Defaults to 1.
            perpendicular_factor (int, optional): defines the weight. Defaults to 1.
            parallel_factor (int, optional): defines the weight. Defaults to 1.

        Returns:
            (float, float, float): returns all three distances in a tuple
        """
        """ angle = abs(self.get_angle() - segment2.get_angle())
        if self.get_length() > segment2.get_length():
            longer, shorter = self, segment2
        else:
            longer, shorter = segment2, self
        angle_dist = shorter.get_length() * np.sin(angle)
        si = np.array(longer.get_p1().get_tuple())
        ei = np.array(longer.get_p2().get_tuple())
        sj = np.array(shorter.get_p1().get_tuple())
        ej = np.array(shorter.get_p2().get_tuple())
        vect_siei = ei - si
        vect_sisj = sj - si
        vect_siej = ej - si
        square_norm_vect_siei = np.nansum(np.power(vect_siei, 2))
        u1 = np.nansum(vect_siei * vect_sisj) / square_norm_vect_siei
        u2 = np.nansum(vect_siei * vect_siej) / square_norm_vect_siei
        ps = si + (u1 * vect_siei)
        pe = si + (u2 * vect_siei)
        l1p = np.linalg.norm(ps - sj)
        l2p = np.linalg.norm(pe - ej)
        perpendicular_dist = 0
        if l1p != 0 or l2p != 0:
            perpendicular_dist = ((l1p ** 2) + (l2p ** 2)) / (l1p + l2p)
        if np.linalg.norm(ps - si) > np.linalg.norm(pe - si):
            ps, pe = pe, ps
        l1para = np.linalg.norm(ps - si)
        l2para = np.linalg.norm(pe - ei)
        parallel_dist = min(l1para, l2para) """

        return dist_numba_seg(self.get_p1().get_tuple(), self.get_p2().get_tuple(), 
                    segment2.get_p1().get_tuple(), segment2.get_p2().get_tuple(), 
                    self.get_angle(), segment2.get_angle(), self.length, segment2.get_length(),
                    angle_factor, perpendicular_factor, parallel_factor)
        #return (perpendicular_dist * perpendicular_factor,
        #        parallel_dist * parallel_factor,
        #        angle_dist * angle_factor)

    def distance(self, segment2, angle_factor=1, perpendicular_factor=1, parallel_factor=1):
        return np.nansum(self.all_distances(segment2, angle_factor, perpendicular_factor, parallel_factor))



@jit(nopython=True)
def dist_numba_seg(p0_0, p0_1, p1_0, p1_1, angle0, angle1, len0, len1, angle_factor, perpendicular_factor, parallel_factor):
        angle = abs(angle0 - angle1)
        if len0 > len1:
            longer, shorter = (p0_0, p0_1), (p1_0, p1_1)
            lenlong, lenshort = len0, len1
        else:
            longer, shorter = (p1_0, p1_1), (p0_0, p0_1)
            lenlong, lenshort = len1, len0
        angle_dist = lenshort * np.sin(angle)
        si = np.array(longer[0]) #longer.get_p1().get_tuple())
        ei = np.array(longer[1]) #longer.get_p2().get_tuple())
        sj = np.array(shorter[0]) #shorter.get_p1().get_tuple())
        ej = np.array(shorter[1]) #shorter.get_p2().get_tuple())
        vect_siei = ei - si
        vect_sisj = sj - si
        vect_siej = ej - si
        square_norm_vect_siei = np.nansum(np.power(vect_siei, 2))
        u1 = np.nansum(vect_siei * vect_sisj) / (square_norm_vect_siei + 1e-10)
        u2 = np.nansum(vect_siei * vect_siej) / (square_norm_vect_siei + 1e-10)
        ps = si + (u1 * vect_siei)
        pe = si + (u2 * vect_siei)
        l1p = np.linalg.norm(ps - sj)
        l2p = np.linalg.norm(pe - ej)
        perpendicular_dist = 0
        if l1p != 0 or l2p != 0:
            perpendicular_dist = ((l1p ** 2) + (l2p ** 2)) / (l1p + l2p)
        if np.linalg.norm(ps - si) > np.linalg.norm(pe - si):
            ps, pe = pe, ps
        l1para = np.linalg.norm(ps - si)
        l2para = np.linalg.norm(pe - ei)
        parallel_dist = min(l1para, l2para)
        return (perpendicular_dist * perpendicular_factor,
                parallel_dist * parallel_factor,
                angle_dist * angle_factor)
