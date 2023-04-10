import sys

# python3 traj2seq_2.py <traj.csv> <taille découpage>

def traj2seq(csv_file, str_traj_succ):
    
    with open(csv_file, "r") as f:
        f.readline()
        lines = f.readlines()
        nb_lines = len(lines)

    res = ""
    nb_traj_succ = int(str_traj_succ)
    counter = 0
    for i in range(nb_lines//nb_traj_succ):
        for j in range(nb_traj_succ):
            res += "<" + str(j) + "> "

            #cols = lines[i*nb_traj_succ+j].split(",")
            cols = lines[counter].split(",")
            counter += 1
            for k in range (1, len(cols)):
                col = cols[k]
                res += col.strip() + " "

            res += "-1 "
        res += "-2\n"

    for i in range(nb_lines%nb_traj_succ):
        res += "<" + str(i) + "> "

        cols = lines[counter].split(",")
        counter += 1
        for k in range (1, len(cols)):
            col = cols[k]
            res += col.strip() + " "
        if(i == (nb_lines%nb_traj_succ)-1):
            res += "-2\n"
        else:
            res += "-1 "

    return res

def traj2seq_player(csv_file, str_traj_succ, player):
    
    with open(csv_file, "r") as f:
        f.readline()
        lines = f.readlines()
        nb_lines = len(lines)

    res = ""
    nb_traj_succ = int(str_traj_succ)
    counter = 0

    for i in range(nb_lines//nb_traj_succ):
        for j in range(nb_traj_succ):
            res += "<" + str(j) + "> "

            #cols = lines[i*nb_traj_succ+j].split(",")
            cols = lines[counter].split(",")
            counter += 1

            res += cols[player+1].strip() + " "
            #for k in range (1, len(cols)):
            #    col = cols[k]
            #    res += col.strip() + " "

            res += "-1 "
        res += "-2\n"

    for i in range(nb_lines%nb_traj_succ):
        res += "<" + str(i) + "> "

        cols = lines[counter].split(",")
        counter += 1

        res += cols[player+1].strip() + " "

        #cols = lines[i].split(",")
        """ for k in range (1, len(cols)):
            col = cols[k]
            res += col.strip() + " " """
        
        res += "-1 "
    res += "-2\n"

    return res


if __name__ == "__main__":
    
    print(traj2seq(sys.argv[1], sys.argv[2]))
    #print(traj2seq_player(sys.argv[1], sys.argv[2], int(sys.argv[3])))
