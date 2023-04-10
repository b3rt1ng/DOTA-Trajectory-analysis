import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Arrow
import sys


def get_color_array(n):
    """
    Cette fonction renvoie un tableau de n couleurs allant du bleu au rouge
    """
    # Définition des couleurs de base (bleu et rouge)
    color_start = np.array([0, 0, 1]) # bleu
    color_end = np.array([1, 0, 0]) # rouge
    
    # Calcul de la liste de couleurs intermédiaires
    color_array = []
    for i in range(n):
        # Calcul de la couleur intermédiaire
        color = (i/(n-1))*color_end + (1-(i/(n-1)))*color_start
        color_array.append(color)
    
    return color_array

def show_1(name, name_f="", name_img=""):
    print(name)

    data = np.genfromtxt(name , delimiter=',', skip_header=1)
    size = 0
    last = -1
    dico = {}
    
    for i in range(len(data)):
        if data[i][0] != last:
            last = data[i][0]
            dico[last] = size
            size += 1

    color_array = get_color_array(size)
    print("Number of transac: ",len(color_array))
    size = 6
    last = data[0][0]

    for i in range(len(data)):
        color = color_array[dico[data[i][0]]]
        x1, y1, x2, y2 = data[i][1], data[i][2], data[i][3], data[i][4]
        plt.plot([x1, x2], [y1, y2], color=color, linewidth=size)

        dx = x2 - x1
        dy = y2 - y1
        plt.arrow(x1, y1, dx, dy, head_width=size, head_length=size, fc=color, ec=color)
        if(last != data[i][0]):
            size -= 1
            last = data[i][0]
        #break

    plt.axis("square")
    xmin = 65
    xmax = 185
    ymin = 65
    ymax = 185
    ax = plt.gca()
    ax.set_xlim([xmin, xmax])
    ax.set_ylim([ymin, ymax])

    plt.title(name_f)
    plt.imshow(plt.imread("../Map_dota.jpg"), extent=(65, 185 ,65 ,185))

    if(name_img != ""):
        plt.savefig(name_img + ".png")
    plt.show()

    plt.close()

#def find_good_candidate

if __name__ == "__main__":
    #name = "29.csv"
    name = sys.argv[1] + ".csv"
    show_1("output/" + name, "", "../cplusplus/images/bad_seq2.png")

