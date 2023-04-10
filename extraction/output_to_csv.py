import os
import sys

def write_to_csv(content, file_name):
    with open(file_name, "w") as f:
        f.write(content)

def parsing_output(file_name, map_coord):

    with open(file_name, "r") as f:
        lines = f.readlines()
        nb_lines = len(lines)
    
    count = 0
    # iterate over lines
    for i in range(nb_lines):
        line = lines[i].split(" ")
        res = "t,x0,y0,x1,y1\n"
        t = 0
        count_trans = 0
        for j in range(len(line)):
            word = line[j]
            #print("word: " + word)
            if word == "-1":
                #res += "\n"
                continue
            elif word == "" or word[0] == "#":
                break
            elif word[0] == "<":
                t = int(word[1:-1])
                count_trans += 1
            else:
                res += str(t) + ","
                res += map_coord[word]
    
        #write_to_csv(res, "output/" + str(count) + "_" + str(count_trans) + ".csv")
        write_to_csv(res, "output/" + str(count) + ".csv")
        count += 1

def parsing_map_coord(file_name):
    with open(file_name, "r") as f:
        lines = f.readlines()
        nb_lines = len(lines)
    map_coord = {}
    for i in range(nb_lines):
        line = lines[i].split(",")
        map_coord[line[0]] = line[1] + "," + line[2] + "," + line[3] + "," + line[4]
    
    return map_coord

if __name__ == "__main__":
    #empty output folder
    for file in os.listdir("output"):
        os.remove("output/" + file)
    
    #cluster_file = "../cplusplus/experiments_results/kmeans/km_300_2/"
    cluster_file = sys.argv[1]
    
    cluster_file += "clusters.csv"

    map_coord = parsing_map_coord(cluster_file)

    parsing_output("output.txt", map_coord)
    