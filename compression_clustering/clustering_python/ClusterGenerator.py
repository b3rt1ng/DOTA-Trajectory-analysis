import matplotlib.pyplot as plt
from numpy import random as rd
from numpy import concatenate as concat

#notes: this is a class that generates RANDOMS clusters of points with some potential spacing between them OR NOT.

class ClusterGenerator:
    def __init__(self, amount: int, distance: int):
        """The cluster generator will make some n amounts of cluster with a dispatch from m (the givent distance) for it's center

        Args:
            amount (int): the number of clusters you wanna make
            distance (int): the distance that the points will be appart from it's center
        """
        self._amount = amount
        self._distance = distance
        self._data = []
        self._size = None

    def set_size(self, size: int) -> None:
        self._size = size

    def __str__(self) -> str:
        size = f"size={self._size}" if self._size != None else "size will be rendomly generated between 1 and 100"
        return f"ClusterGenerator(amount={self._amount}, distance={self._distance}, {size})"

    def generate_cluster(self, center, distance, size):
        return rd.uniform(center[0], center[0] + distance, size=(size,)), rd.normal(center[1], distance, size=(size,))

    def generate(self) -> list:
        """ generate the clusters

        Returns:
            array: a list of points in the numpy array format
        """
        result = []
        for _ in range(self._amount): result.append(self.generate_cluster(rd.uniform(0, 100, size=(2,)), self._distance, (self._size if self._size else rd.randint(1, 100))))
        self._data = concat(result, axis=1)
        return self._data

    def generate_tuples(self) -> list:
        data = self.generate()
        result = []
        for i in range(len(data[0])):
            result.append((data[0][i], data[1][i]))
        return result

    def plot(self) -> None:
        if self._data != []:
            plt.scatter(self._data[0], self._data[1])
            plt.show()
        else:
            print("No data to plot")


if __name__ == '__main__':
    generator = ClusterGenerator(10, 2)
    generator.set_size(10)
    generator.generate()
    print(generator)
    generator.plot()