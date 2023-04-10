
import random
import sys
import matplotlib.pyplot as plt

sys.path.append("../")

from cplusplus.clustering.Segment import *
from cplusplus.clustering.SetSegment import *

class KMeansSegments:

    def __init__(self, k, list_segments):
        self._k = k
        self._list_segments = list_segments

    def get_k(self):
        return self._k

    def get_list_segments(self):
        return self._list_segments

    def set_k(self, k):
        self._k = k

    def set_list_segments(self, list_segments):
        self._list_segments = list_segments

    def initialize_centroids(self):
        return random.sample(self._list_segments, self._k)
    
    def assign_clusters(self, centroids):
        clusters = [[] for _ in range(self._k)]
        for segment in self._list_segments:
            distances = np.array([segment.distance(centroid) for centroid in centroids])
            clusters[distances.argmin()].append(segment)
        return clusters
            
    def calculate_centroids(self, clusters):
        centroids = []
        for cluster in clusters:
            set_segments = SetSegment(cluster)
            centroid = set_segments.mean_segment()
            if centroid == None:
                centroid = random.sample(self._list_segments, 1)[0]
            centroids.append(centroid)
        return centroids

    def calculate(self, max_iteration=100):
        iteration = 0
        centroids = self.initialize_centroids()
        centroids_old = None

        while max_iteration > iteration and centroids != centroids_old:
            print("iteration: ", iteration)
            clusters = self.assign_clusters(centroids)
            centroids_old = centroids
            centroids = self.calculate_centroids(clusters)
            iteration += 1
        
        return (centroids, clusters)




if __name__ == "__main__":

    data = []

    for i in range(10):
        data.append(Segment(Point(random.randint(0,10), random.randint(0,10)), Point(random.randint(0,10), random.randint(0,10))))
    
    print("List Segments:")
    for row in data:
        print(str(row))

    print("")

    kmeans = KMeansSegments(5, data)
    
    centroids = kmeans.initialize_centroids()

    print("Initial Centroids:")
    print('\n'.join(map(str, centroids)))

    print("")

    clusters = kmeans.assign_clusters(centroids)

    print("Clusters:")
    for row in clusters:
        print('\n'.join(map(str, row)))
        print("")

    centroids = kmeans.calculate_centroids(clusters)

    print("Centroids:")
    print('\n'.join(map(str, centroids)))

    print("")

    res = kmeans.calculate()

    print("Results:")
    print("  Centroids:")
    print('\n'.join(map(str, res[0])))
    print("")

    print("  Clusters:")
    for row in res[1]:
        print('\n'.join(map(str, row)))
        print("")

    centroids = kmeans.calculate_centroids(res[1])
    print("  Next Centroids:")
    print('\n'.join(map(str, res[0])))

    for i in range(len(res[1])):
        hexa = "#"
        for _ in range(6):
            hexa += random.choice("0123456789ABCDEF")

        for segment in res[1][i]:
            plt.plot([segment.get_p1().get_x(), segment.get_p2().get_x()], [segment.get_p1().get_y(), segment.get_p2().get_y()], linestyle="-", color=hexa)

        plt.plot([centroids[i].get_p1().get_x(), centroids[i].get_p2().get_x()], [centroids[i].get_p1().get_y(), centroids[i].get_p2().get_y()], 'bo', linestyle="--", color=hexa)

    #plt.plot([0,5], [10,15], 'bo', linestyle="-", color="#0F0055")
    plt.show()