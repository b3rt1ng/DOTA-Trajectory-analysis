from warnings import filterwarnings
import numpy as np
from time import time
import matplotlib.pyplot as plt
import multiprocessing as mp
from multiprocessing import cpu_count
from multiprocessing import Array, Manager
import pickle

from sys import path
path.insert(0, '..')

filterwarnings("ignore", category=RuntimeWarning)


def get_shared_array(name: str, shape=None):
    mp_array = globals()[name]
    np_array = np.frombuffer(
        mp_array.get_obj(), dtype=np.dtype(mp_array.get_obj()._type_))
    if (shape is None) and (name + '_shape' in globals().keys()):
        shape = globals()[name + '_shape']
        shape = np.frombuffer(shape.get_obj(), dtype=int)
    if shape is not None:
        np_array = np_array.reshape(shape)
    return np_array


def make_shared_array(np_array: np.ndarray, name: str):
    mp_dtype = np.ctypeslib.as_ctypes(np_array.dtype.type())._type_
    mp_array = Array(typecode_or_type=mp_dtype,
                     size_or_initializer=int(np.prod(np_array.shape)))
    globals()[name] = mp_array
    shared_np_array = get_shared_array(name, shape=np_array.shape)
    shared_np_array[:] = np_array
    mp_array_shape = Array(typecode_or_type='l',
                           size_or_initializer=len(np_array.shape))
    globals()[name + '_shape'] = mp_array_shape
    shared_np_array = get_shared_array(name + '_shape')
    shared_np_array[:] = np_array.shape


class AffinityPropagation:
    def __init__(self, data: list, iterations: int = 100, damping: float = 0.9, fast: bool = False, cores: int = 1) -> None:
        """ Class implementing an affinity propagation algorithm.
            Once you instancied the an AffinityPropagation, you will need to call the start() method to process the data.
            Then you will be able to acess the clusters as a proprety of the AffinityPropagation object, calla for AffinityPropagation.clusters (key = representant: value = cluster).

            The structure returnred for the clusters is a distionary containing:
                as keys: the representant from a cluster
                as values: the elements of a cluster

            you can get access at the data you provided to this class as a proprety as a proprety of the AffinityPropagation 
            object (useful if you need to get the representant back)

        Args:
            data (list): 
            self._availability_matrix list of objects possessing a distance method so that algorithm can use it to compare any X elements to any Y elements in the data list.

            iterations (int, optional, default=10): 
            The number of iteration we want to proceed. Defaults to 10.

            damping (float, optional, default=0.5, range=(0.5, 1.0)): 
            The damping value is meant to maintain to a certain extent the previous iteration's values on 
            the processing matrixes (similarity and responsibility) so that the algorithm doesn't diverge because of
            some potential oscillations (especially on floats or big ints values that could lead to some outliers).

            fast (bool, optional, default=False):
            If set to True, the algorithm will use a numpy based calculation of the responsibility and availability matrixes.

            cores (int, optional, default=1):
            The number of cores you want to use to process the data. If set to 1, the algorithm will use only one core.
            This option will divide the calculation of the similarity matrix between the cores you provide.

        """

        if not (0.5 <= damping <= 1.0):
            raise Exception(
                "ERROR: The damping value needs to be between 0.5 and 1.0 (1 excluded)")

        cpus = cpu_count()
        if not 0 < cores <= cpus:
            raise Exception(
                f"ERROR: The number of cores needs to be at least 1 and not exceed the number of cores available on your machine ({cpus} cores detected)")

        # some timers meant for me to imporve the algorithm
        self._start, self._stop = 0, 0
        self._similarity_start, self._similarity_stop = 0, 0
        self._iterations_start, self._iterations_stop = 0, 0
        self._criterion_start, self._criterion_stop = 0, 0

        # some practical variables
        self._calculated = False
        self._loaded_similarity_matrix = False
        self._loaded_criterion_matrix = False
        self._iterations = iterations

        # we make sure that the data is a numpy array
        self._data = np.array(data) if type(data) == list else data
        self._data_length = self._data.shape[0]
        self._damping = damping
        self._number_of_clusters = 0
        self._clusters = {}
        self._fast = fast
        self._cores = cores

        # main variables used in the algorithm
        self._similarity_matrix = self.generate_blank_matrix()
        self._responsibility_matrix = self.generate_blank_matrix()
        self._availability_matrix = self.generate_blank_matrix()
        self._criterion_matrix = self.generate_blank_matrix()

        make_shared_array(self._similarity_matrix, name='similarity_matrix')

        # self._shm = shared_memory.SharedMemory(create=True, size=self._similarity_matrix.nbytes)
        # self._shm_name = self._shm.name
        # self._shared_similarity_matrix = np.ndarray(self._similarity_matrix.shape, dtype=self._similarity_matrix.dtype, buffer=self._shm.buf)
        # self._shared_similarity_matrix[:] = self._similarity_matrix[:]
        # print(self._shared_similarity_matrix)
        # print(self._similarity_matrix)
        # self._shared_similarity_matrix[0][0] = 1

    def generate_blank_matrix(self) -> list:
        """
            Generate a blank matrix of N*N, N being the amount of data we have to deal with (by blank, we mean a 0 filled matrix)
        """
        return np.zeros((self._data_length, self._data_length))

    def _trunk_process(self, trunk: tuple, manager: Manager) -> None:
        mat = get_shared_array('similarity_matrix')
        mini = float('inf')
        for i in range(trunk[0], trunk[1]):
            for k in range(self._data_length):
                distacne = -(self._data[i].distance(self._data[k], 1, 1, 1))
                if distacne < mini:
                    mini = distacne
                mat[i, k] = distacne  # -distacne
        manager[mini] = mini

    def _split(self, n: int) -> list:
        """Generate indices to split a list into approximately n slices.

        Args:
            n (int): how many slices we want to generate

        Returns:
            list: a list of tuples delimiting the slices
        """
        a = [i for i in range(len(self._data))]
        k, m = divmod(len(a), n)
        temp = (a[i*k+min(i, m):(i+1)*k+min(i+1, m)] for i in range(n))
        return [(i[0], i[-1]) for i in temp]

    def _process_similarity(self) -> None:
        """
            Calculate the similarity between each point and all the other data using their distance (or in general, quantify how different a point is from another)
        """
        trunks = self._split(self._cores)
        # used to gather the minimum values processed by each core
        result_manager = Manager().dict()
        jobs = []

        for trunk in trunks:
            p = mp.Process(target=self._trunk_process,
                           args=(trunk, result_manager,))
            jobs.append(p)
            p.start()

        for proc in jobs:
            proc.join()

        self._similarity_matrix = get_shared_array('similarity_matrix')

        # mini = float('inf')
        # for i in range(self._data_length):
        #     for k in range(self._data_length):
        #         distacne = -(self._data[i].distance(self._data[k],1,1,1))
        #         if distacne < mini:
        #             mini = distacne
        #         self._similarity_matrix[i, k] = distacne #-distacne

        np.fill_diagonal(self._similarity_matrix, min(result_manager.values()))

    def _process_responsibility(self) -> None:
        """
            Table of responsibility between each point and all the other potential candidates
        """
        if not self._fast:
            for i in range(self._responsibility_matrix.shape[0]):
                for k in range(self._responsibility_matrix.shape[1]):
                    v = self._similarity_matrix[i, :] + \
                        self._availability_matrix[i, :]
                    v[k] = -np.inf
                    v[i] = -np.inf
                    self._responsibility_matrix[i, k] = self._responsibility_matrix[i, k] * self._damping + (
                        1 - self._damping) * (self._similarity_matrix[i, k] - np.max(v))
        else:
            v = self._similarity_matrix + self._availability_matrix
            rows = np.arange(self._responsibility_matrix.shape[0])
            np.fill_diagonal(v, -np.inf)
            idx_max = np.argmax(v, axis=1)
            first_max = v[rows, idx_max]
            v[rows, idx_max] = -np.inf
            second_max = v[rows, np.argmax(v, axis=1)]
            max_matrix = np.zeros_like(
                self._responsibility_matrix) + first_max[:, None]
            max_matrix[rows, idx_max] = second_max
            new_val = self._similarity_matrix - max_matrix
            self._responsibility_matrix = self._responsibility_matrix * \
                self._damping + (1 - self._damping) * new_val

    def _process_availability(self) -> None:
        """
            Table of availability between each point and all the other potential candidates
        """
        if not self._fast:
            for i in range(self._data_length):
                for k in range(self._data_length):
                    v = np.array(self._responsibility_matrix[:, k])
                    if i != k:
                        v[i] = -np.inf
                        v[k] = -np.inf
                        v[v < 0] = 0
                        tmp = self._availability_matrix[i, k] * self._damping + (1-self._damping) * min(0, self._responsibility_matrix[k, k] + np.nansum(v))

                        self._availability_matrix[i, k] = tmp
                    else:
                        v[k] = -np.inf
                        v[v < 0] = 0
                        self._availability_matrix[k, k] = self._availability_matrix[k,
                                                                                    k] * self._damping + (1 - self._damping) * np.nansum(v)
        else:
            k_k_idx = np.arange(self._availability_matrix.shape[0])
            v = np.array(self._responsibility_matrix)
            v[v < 0] = 0
            np.fill_diagonal(v, 0)
            v = np.nansum(v, axis=0)
            v = v + self._responsibility_matrix[k_k_idx, k_k_idx]
            # v = np.nan_to_num(v)
            v = np.ones(self._availability_matrix.shape) * v
            v -= np.clip(self._responsibility_matrix, 0, np.inf)
            v[v > 0] = 0

            v_ = np.array(self._responsibility_matrix)
            np.fill_diagonal(v_, 0)
            v_[v_ < 0] = 0
            v[k_k_idx, k_k_idx] = np.nansum(v_, axis=0)

            self._availability_matrix = self._availability_matrix * \
                self._damping + (1 - self._damping) * v

    def _process_criterion(self) -> None:
        """
            The criterion matrix is the sum of the responsibility and availability matrix.
            The highest criterion value of each row is designated as the exemplar. 
            Rows that share the same exemplar are in the same cluster. (towardsdatascience)
        """
        self._criterion_start = time()
        self._criterion_matrix = self._responsibility_matrix + self._availability_matrix
        self._criterion_stop = time()

    def start(self) -> None:
        if self._calculated:
            raise Exception(
                "ERROR: The affinity propagation has already been calculated.")

        self._start = time()
        self._similarity_start = time()
        if (not self._loaded_similarity_matrix):
            # print("Calculating similarity matrix...")
            self._process_similarity()
        self._similarity_stop = time()
        # print(f"Similarity matrix calculated in {self._similarity_stop-self._similarity_start} seconds.")

        self._iterations_start = time()
        if not self._loaded_criterion_matrix:
            # print("Calculating criterion matrix...")
            # i = 0
            for _ in range(self._iterations):
                # i += 1
                # print(f"Iteration: {i}/{self._iterations}, {str((i/self._iterations)*100)[:4]}%", end="\r")
                self._process_responsibility()
                self._process_availability()

            self._process_criterion()
        self._iterations_stop = time()

        representants_indexes = np.argmax(self._criterion_matrix, axis=1)
        representants_indexes_unique = np.unique(representants_indexes)
        self._number_of_clusters = len(representants_indexes_unique)

        for i in representants_indexes_unique:
            self._clusters[self._data[i]] = []

        cpt = 0
        for i in representants_indexes:
            self._clusters[self._data[i]].append(self._data[cpt])
            cpt += 1

        self._stop = time()
        self._calculated = True

    @property
    def clusters(self) -> list:
        """
            Dictionary of clusters found by the algorithm
            The key is the representant of the cluster
            The value is the list of all the points in the cluster
        """
        if not self._calculated:
            raise Exception(
                "ERROR: The affinity propagation has not been calculated yet, please run the start() method before trying to get the clusters")
        return self._clusters

    @property
    def representants(self) -> list:
        """
            Return the representants of the clusters found by the algorithm
        """
        if self._number_of_clusters == 0:
            raise Exception(
                "ERROR: The affinity propagation has not been calculated yet, please run the start() method before trying to get the clusters representants")
        return self._clusters.keys()

    @property
    def data(self) -> list:
        return self._data

    @property
    def number_of_clusters(self) -> int:
        return self._number_of_clusters

    @property
    def iterations(self) -> int:
        return self._iterations

    @property
    def result(self) -> float:
        if not self._calculated:
            return None
        result = {"data": self._data}

    def __str__(self) -> str:
        if not self._calculated:
            calc = "Not calculated yet"
        else:
            calc = f"Whole process made in {self._stop - self._start} seconds\n\tSimilarity matrix made in {self._similarity_stop - self._similarity_start} seconds\n\tIterations made in {self._iterations_stop - self._iterations_start} seconds\n\tCriterion matrix made in {self._criterion_stop - self._criterion_start} seconds\n\tNumber of clusters = {self._number_of_clusters}\n\tNumber of iterations = {self._iterations}"
        method = "Fast" if self._fast else "Classic"
        return f"AffinityPropagation(\n\tData length = {self._data_length}\n\tNumber of iterations = {self._iterations}\n\tdamping = {self._damping}\n\t{calc}\n\tCalculation method = {method}\n\tNumber of cores = {self._cores}\n)"

    def save(self, path: str = "", filename: str = None) -> None:
        """save the result of the affinity propagation in a pickle file

        Args:
            path (str, optional): the patch where you want your file to be saved. Defaults to "" (execution directory).
            filename (str, optional): the name of the file you wanna save to. Defaults to None, will autogenerate a name.

        Raises:
            Exception: if the affinity propagation has not been calculated yet
        """
        if not self._calculated:
            raise Exception(
                "ERROR: The affinity propagation has not been calculated yet, please run the start() method before trying to save the result")
        if filename is None:
            filename = f"AffinityPropagation#{str(time())[:4]}.pickle"
            print(f"saved as \"{filename}\"")
        with open(path+filename, "wb") as file:
            pickle.dump(self._clusters, file)

    def save_similarity_matrix(self, path: str = "", filename: str = None) -> None:
        """save the similarity matrix in a pickle file

        Args:
            path (str, optional): the patch where you want your file to be saved. Defaults to "" (execution directory).
            filename (str, optional): the name of the file you wanna save to. Defaults to None, will autogenerate a name.

        Raises:
            Exception: if the affinity propagation has not been calculated yet
        """
        if not self._calculated:
            raise Exception(
                "ERROR: The affinity propagation has not been calculated yet, please run the start() method before trying to save the result")
        if filename is None:
            filename = f"SimilarityMatrix#{str(time())[:10]}"
            print(f"saved as \"{filename}\"")
        np.save(path+filename, self._similarity_matrix)

    def load_similarity_matrix(self, path: str) -> None:
        """load a similarity matrix from a npy

        Args:
            path (str): the path of the file you want to load

        Raises:
            Exception: if the file is not a pickle file
        """
        if not path.endswith(".npy"):
            raise Exception(
                "ERROR: The file you want to load is not a numpy .npy file")
        self._similarity_matrix = np.load(path, allow_pickle=True)

        self._loaded_similarity_matrix = True
        print("loaded similarity matrix")

    def save_criterion_matrix(self, path: str = "", filename: str = None) -> None:
        """save the criterion matrix in a pickle file

        Args:
            path (str, optional): the patch where you want your file to be saved. Defaults to "" (execution directory).
            filename (str, optional): the name of the file you wanna save to. Defaults to None, will autogenerate a name.

        Raises:
            Exception: if the affinity propagation has not been calculated yet
        """
        if not self._calculated:
            raise Exception(
                "ERROR: The affinity propagation has not been calculated yet, please run the start() method before trying to save the result")
        if filename is None:
            filename = f"criterionMatrix#{str(time())[:10]}"
            print(f"saved as \"{filename}\"")
        np.save(path+filename, self._criterion_matrix)

    def load_criterion_matrix(self, path: str) -> None:
        """load a criterion matrix from a npy

        Args:
            path (str): the path of the file you want to load

        Raises:
            Exception: if the file is not a pickle file
        """
        if not path.endswith(".npy"):
            raise Exception(
                "ERROR: The file you want to load is not a numpy .npy file")
        self._criterion_matrix = np.load(path, allow_pickle=True)

        self._loaded_criterion_matrix = True
        self._loaded_similarity_matrix = True
        print("loaded criterion matrix")


def plot_clusters(ap):
    img = plt.imread("Map_dota.jpg")
    clusters = ap.clusters
    plt.title(
        f"Affinity propagation found {ap.number_of_clusters} clusters for {len(ap.data)} data with {ap.iterations} iterations")
    for i in clusters:
        points = clusters[i]
        center = ap.data[i]
        plt.plot([point[0] for point in points], [point[1]
                 for point in points], 'o')
        plt.plot(center[0], center[1], 'o', markerfacecolor='r',
                 markeredgecolor='k', markersize=6)
        plt.text(center[0], center[1],
                 f"({round(center[0],2)}, {round(center[1],2)})", fontsize=10, color='black')
    plt.imshow(img, extent=[0, 200, 0, 200])
    plt.grid()
    plt.show()


if __name__ == "__main__":
    from random import choice

    percent = 0.5  # 50% of the data will be processed
    percent = 1-percent
    points = np.load("arr_segs_10_game.pkl.npy", allow_pickle=True)
    points = points[:len(points)-(round(len(points)*percent))]

    ap = AffinityPropagation(points, 100, 0.7, True, 8)

    print(ap)
    # ap.load_criterion_matrix("criterionMatrix#1679051395.npy")
    ap.start()
    print(ap)
    # ap.save_criterion_matrix()
    # print(len(ap.representants))

    i=0
    for segment in ap.representants:
        i+=1
        hexa = "#"
        for _ in range(6):
            hexa += choice("0123456789ABCDEF")

        # print(segment)
        plt.plot([segment.p1.x, segment.p2.x], [segment.p1.y,segment.p2.y], linestyle="-", linewidth=3, color=hexa)
        plt.text(segment.p1.x, segment.p1.y, f"rep#{i+1}", fontsize=10, color='black')
    # plt.imshow(plt.imread("../Map_dota.jpg"), extent=[65, 185, 65, 185])
    plt.title(
        f"Affinity propagation found {len(ap.representants)} clusters for {len(ap.data)} data with {ap.iterations} iterations")
    plt.show()

    # ap.save()
