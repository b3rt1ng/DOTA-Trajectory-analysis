import sys

def traj2seq(csv_file, str_traj_succ):
    
    with open(csv_file, "r") as f:
        f.readline()
        lines = f.readlines()
        nb_lines = len(lines)

    res = ""
    nb_traj_succ = int(str_traj_succ)

    for i in range(nb_lines):
        if (nb_lines-i)//nb_traj_succ >= 1 :
            lim = nb_traj_succ
        else:
            lim = nb_lines-i
        for j in range(lim):
            res += "<" + str(j) + "> "

            cols = lines[i+j].split(",")
            for k in range (1, len(cols)):
                col = cols[k]
                res += col.strip() + " "
            
            res += "-1 "

        res += "-2\n"

    return res




if __name__ == "__main__":
    
    print(traj2seq(sys.argv[1], sys.argv[2]))