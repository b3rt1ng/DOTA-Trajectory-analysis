import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Arrow
import pandas as pd


def show_cluster(dir, idi, color):
    name = dir + '/group_clusters.csv'
    df = pd.read_csv(name, delimiter=',')
    seg_group = df.query("id == " + str(idi))
    print("group size: ", len(seg_group))
    for i in range(0, len(seg_group)):
        x1 = seg_group.iloc[i].x0
        y1 = seg_group.iloc[i].y0
        x2 = seg_group.iloc[i].x1
        y2 = seg_group.iloc[i].y1
        # plt.plot([x1, x2], [y1, y2], color=color, linewidth=1.5)

        dx = x2 - x1
        dy = y2 - y1
        plt.arrow(x1, y1, dx, dy, head_width=1.8,
                  head_length=1.8, fc=color, ec=color)


def show_2(dir1, dir2, name_f="", name_img=""):
    name = dir + '/clusters.csv'
    data = np.genfromtxt(name, delimiter=',', skip_header=1)
    plt.subplot(1, 2, 1)

    for i in range(len(data)):
        x1, y1, x2, y2 = data[i][1], data[i][2], data[i][3], data[i][4]
        plt.plot([x1, x2], [y1, y2], color='white', linewidth=2)

        dx = x2 - x1
        dy = y2 - y1
        plt.arrow(x1, y1, dx, dy, head_width=2,
                  head_length=2, fc='white', ec='white')

    plt.axis("square")
    plt.title(name)
    plt.imshow(plt.imread("../Map_dota.jpg"), extent=(65, 185, 65, 185))

    name = dir2 + '/clusters.csv'
    data = np.genfromtxt(name, delimiter=',', skip_header=1)

    plt.subplot(1, 2, 2)

    for i in range(len(data)):
        x1, y1, x2, y2 = data[i][1], data[i][2], data[i][3], data[i][4]
        plt.plot([x1, x2], [y1, y2], color='white', linewidth=2)

        dx = x2 - x1
        dy = y2 - y1
        plt.arrow(x1, y1, dx, dy, head_width=2,
                  head_length=2, fc='white', ec='white')

    plt.axis("square")
    plt.title(name)
    plt.imshow(plt.imread("../Map_dota.jpg"), extent=(65, 185, 65, 185))

    if (name_img != ""):
        plt.savefig(name_img + ".png")
    # plt.show()

    plt.close()


def show_1(dir, name_f="", name_img="", show_nb=False):
    name = dir + '/clusters.csv'
    print(name)
    data = np.genfromtxt(name, delimiter=',', skip_header=1)

    for i in range(len(data)):
        x1, y1, x2, y2 = data[i][1], data[i][2], data[i][3], data[i][4]
        plt.plot([x1, x2], [y1, y2], color='white', linewidth=2)

        dx = x2 - x1
        dy = y2 - y1
        plt.arrow(x1, y1, dx, dy, head_width=2,
                  head_length=2, fc='white', ec='white')

    plt.axis("square")

    if show_nb:
        name_f += "\nnb clusters: " + str(len(data))

    plt.title(name_f)
    plt.imshow(plt.imread("../Map_dota.jpg"), extent=(65, 185, 65, 185))

    if (name_img != ""):
        plt.savefig(name_img + ".png")
    plt.show()

    plt.close()


def show_with_group(dir, idi, color, name_f="", name_img=""):
    name = dir + '/clusters.csv'
    data = np.genfromtxt(name, delimiter=',', skip_header=1)
    ii = idi

    show_cluster(dir, ii, color)

    for i in range(len(data)):
        x1, y1, x2, y2 = data[i][1], data[i][2], data[i][3], data[i][4]
        plt.plot([x1, x2], [y1, y2], color='white', linewidth=2)

        dx = x2 - x1
        dy = y2 - y1
        plt.arrow(x1, y1, dx, dy, head_width=2,
                  head_length=2, fc='white', ec='white')
        if data[i][0] == ii:
            plt.plot([x1, x2], [y1, y2], color='black', linewidth=3)

            plt.arrow(x1, y1, dx, dy, head_width=4,
                      head_length=4, fc='black', ec='black')

    plt.axis("square")
    plt.title(name_f)
    plt.imshow(plt.imread("../Map_dota.jpg"), extent=(65, 185, 65, 185))

    if (name_img != ""):
        plt.savefig(name_img + ".png")
    plt.show()

    plt.close()


# dir = 'clustering_results/2023_3_31_17H_46M57S'
# dir = 'experiments_results/kmeans/11__'
# dirBis = 'experiments_results/kmed/11__'

names = [
    '1_1_1_1',
    '2_1_1_0',
    '3_1_0_1',
    '4_0_1_1',
    '5_1_1_2',
    '6_1_2_1',
    '7_2_1_1',
    '8_2_2_1',
    '9_2_1_2',
    '10_1_2_2',
    # '11_4_4_1',
    # '12_4_1_4',
    # '13_1_4_4',
]

dist_factor = [
    [1, 1, 1],
    [1, 1, 0],
    [1, 0, 1],
    [0, 1, 1],
    [1, 1, 2],
    [1, 2, 1],
    [2, 1, 1],
    [2, 2, 1],
    [1, 2, 2],
    [2, 1, 2],
    # [4,4,1],
    # [4,1,4],
    # [1,4,4],
]

d_angle = "d\u03B8="
# d_perp = "d\u27C2="
d_perp = "dperp="
d_par = "d\u2225="
print(d_angle)
print(d_perp)
print(d_par)
dir = 'experiments_results/propa/'
method = "images/propa/propa"

# name_img = "images/kmed/kmed_212_show_cluster_long"
# dir = 'experiments_results/kmed/10__'

dir = 'experiments_results/kmed/1__'
dir = 'experiments_results/kmed/kmed_150_111'
dir = 'experiments_results/kmed/kmed_200_111'
dir = 'experiments_results/kmed/kmed_300_111'
dir = 'clustering_results/2023_4_6_23H_37M49S'

name_img = "images/kmed/kmed_long_traj"

name_f = d_angle + str(1) + "  " + d_perp + str(1) + "  " + d_par + str(1)
i = 66
# show_1(dir, name_f, name_img, True)
# quit()
show_with_group(dir, i, 'red', name_f, name_img)

for i in range(len(names)):
    name_f = d_angle + str(dist_factor[i][0]) + "  " + d_perp + str(
        dist_factor[i][1]) + "  " + d_par + str(dist_factor[i][2])
    print(name_f)
    show_1(dir + str(i+1) + "__", name_f, method + names[i], True)
    # print(dist_factor[i][0], dist_factor[i][1], dist_factor[i][2])

""" res = 0
for i in range(0, len(data)):
    res += np.sqrt((data[i][1] - data[i][3])**2 + (data[i][2] - data[i][4])**2)
print(res/len(data)) """
