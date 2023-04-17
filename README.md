# DOTA 2 trajectory analysis

<img src="https://cdn-icons-png.flaticon.com/512/871/871499.png" align="right"
     alt="Logo DOTA" width="100" height="100">

This is a university project, carried out in the context of a scientific themed project.
The goal is to analyse a series of movements in order to detect strategies and repetitive parterns in serval dota games. 

In this README, you will find a brief description of the project with some of our final thoughts and results but if you want to know more details, and more in depth explanations about our implementation and calculations, you can refer to the [report](https://github.com/b3rt1ng/DOTA-Trajectory-analysis/blob/main/Rapport_projet_dota-1.pdf) wich is in french.

To achieve this, we proceeded with the following methods: 
* compression based on the principle MDL, discretization by clustering (K-means, K-medoids or Affinity Propagation)
* sequential extraction of frequent patterns.



<!-- With **[GitHub action]** Size Limit will post bundle size changes as a comment
in pull request discussion. -->

<!-- <p align="center">
<img src="https://raw.githubusercontent.com/andresz1/size-limit-action/master/assets/pr.png"
  alt="Size Limit comment in pull request about bundle size changes"
  width="686" height="289">
</p> -->

<br>

[GitHub action]: https://github.com/andresz1/size-limit-action
[Statoscope]:    https://github.com/statoscope/statoscope
[cult-img]:      http://cultofmartians.com/assets/badges/badge.svg
[cult]:          http://cultofmartians.com/tasks/size-limit-config.html

## The compression

Because the amount of data is very large, about 250 games were provided, with a ton of movements, we decided to compress the data in order to reduce the size of the data set.

The principle of the compression is based on the principle of minimum description length ([MDL](https://en.wikipedia.org/wiki/Minimum_description_length)). This principle consists of finding the most compact representation of the data without losing the relevance of the data. In our case, we will try to find the most compact representation of the trajectories of the players in the game.

You can refer to the [compression pipeline](https://github.com/b3rt1ng/DOTA-Trajectory-analysis/blob/main/compression_clustering/src/compressionPipe.cpp) for more details.

The results obtained during the compression illustrate the efficiency of the proposed method to identify the characteristic points of a trajectory. Indeed, this method allows us to reach a
average reduction of 67.9% of all our trajectories. These results highlight the importance of our approach for compressing trajectories, while maintaining their quality, as illustrated below.

<p align="center">
<img src="https://i.imgur.com/xbPMj9A.png"
  alt="Size Limit comment in pull request about bundle size changes"
  width="788" height="395">
</p>

It is possible to observe more finely the balance between conciseness and precision that allows this method in the following figure:

<p align="center">
<img src="https://i.imgur.com/jmLMM7E.png"
  alt="Size Limit comment in pull request about bundle size changes"
  width="500" height="500">
</p>

## The Clustering

### Setting up a distance calculation

In order to quantify how similar two trajectories are, we defined a distance method.

The distance used is the weighted sum of three distances: angular distance, perpendicular distance and parallel distance. The coefficients of this sum having a significant impact on the results of this part, it has been necessary to evaluate and find the most relevant parameterization.

check the [segment](https://github.com/b3rt1ng/DOTA-Trajectory-analysis/blob/main/compression_clustering/src/Segment.cpp) class implementation we made in C++ for more details.

### K-means

This clustering method has been our most easy and conveinent method to work with since this algorithm's complexity has been way lower than the affinity propagation algorithm we used later on.

<p align="center">
<img src="https://i.imgur.com/ESrtcAI.png"
  alt="kmeans various params"
  width="645" height="261">
</p>

On these results, it is possible to clearly distinguish the existing differences between each parametrizations. When its factor is equal to 0, an angular distance is critical to obtain  some results that are corectly covering the trajectories on the whole map. This is also the case for the perpendicular distance. Concerning the parallel distance, it would seem that it makes it possible to refine the grouping of segments with the same orientation and obtaining longer representative segments on the recurrent pieces of trajectory. This balancing is the most relevant here with the adoption of a factor 1/2 for the parallel distance.

From these observations, it appeared that a factor of 1/5 for the parallel distance and of 1 for the other two distances gave satisfactory results As illustrated here:

<p align="center">
<img src="https://i.imgur.com/RSZB1Xv.png"
  alt="kmeans on 5 5 1 params"
  width="640" height="480">
</p>

### K-medoid

it is possible to distinguish clearly the differences existing between each parametrizations. When its factor is equal to 0, the angular distance is critical to obtain results correctly covering the trajectories on the whole map. This is also the case for the perpendicular distance. Concerning the parallel distance, it would seem that it makes it possible to refine the grouping of segments with the same orientation and obtaining longer representative segments on the recurrent pieces of trajectory. This balancing is the most relevant here with the adoption of a factor 1/2 for the parallel distance.

<p align="center">
<img src="https://i.imgur.com/tHStQqW.png"
  alt="kmedoid various params"
  width="645" height="261">
</p>

From these observations, it appeared that a factor of 1/5 for the parallel distance and of 1 for the other two distances gave satisfactory results. As shown here:

<p align="center">
<img src="https://i.imgur.com/LM1dz3S.png"
  alt="kmedoid on 1 1 1 params"
  width="640" height="480">
</p>

## Affinity Propagation

For affinity propagation, results are globally quite similar and reflect more a notion of position than of trajectory partitions. Indeed, the segments are generally short and distributed over the whole map in a homogeneous way. Moreover, there is a tendency to obtain characteristic segments that are vertical, horizontal or diagonal.
However, it should be taken into account that these results are subject to the bias of the low number of parts. In the same way, this algorithm has its own parameters, such as damping, measures of acceptance, which affects the number of clusters found.
Here, it was chosen to use a damping of 0.9 with 60 iterations for each experiment.

<p align="center">
<img src="https://i.imgur.com/4Ove472.png"
  alt="propa various params"
  width="645" height="261">
</p>

Considering the results obtained, it appeared that it was quite relevant to use an angular factor of 0, giving this result:

<p align="center">
<img src="https://i.imgur.com/sDaSU8B.png"
  alt="propa on 0 1 1 params"
  width="640" height="480">
</p>

# addendum

We sketeched our algorithm in python before implementing it in C++, here are our take on those implementations:



| algo                 | python version         | C++ version             |
|----------------------|------------------------|-------------------------|
| kmeans               | [KMeansSegments.py](https://github.com/b3rt1ng/DOTA-Trajectory-analysis/blob/main/compression_clustering/clustering_python/KMeansSegments.py)      | [Kmeans.cpp](https://github.com/b3rt1ng/DOTA-Trajectory-analysis/blob/main/compression_clustering/src/Kmeans.cpp)              |
| kmedoid              | [KMedoid.py](https://github.com/b3rt1ng/DOTA-Trajectory-analysis/blob/main/compression_clustering/clustering_python/KMedoid.py)             | [Kmedoid.cpp](https://github.com/b3rt1ng/DOTA-Trajectory-analysis/blob/main/compression_clustering/src/Kmedoid.cpp)            |
| affinity propagatino | [AffinityPropagation.py](https://github.com/b3rt1ng/DOTA-Trajectory-analysis/blob/main/compression_clustering/clustering_python/AffinityPropagation.py) | [AffinityPropagation.cpp](https://github.com/b3rt1ng/DOTA-Trajectory-analysis/blob/main/compression_clustering/src/AffinityPropagation.cpp) |

Note: affinity propagation is a bit more complex to implement in python since one of the most interesting optimization is the use of parallelism. Tho numpy has been a life saver when it comes to matrix operations in the iterative process of the algorithm.
In C++, parallelism is way more convenient to implement thanks to [OpenMP](https://www.openmp.org/) and the vectorization of the operations are also affordable for a project like this one.
