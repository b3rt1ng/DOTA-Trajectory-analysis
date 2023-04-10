import numpy as np
import pandas as pd
import sys
sys.path.insert(0,'..')
from cplusplus.clustering.Segment import *
from Mdl import *
import time

import os

def load_game(name: str):
    tmp = pd.read_csv(name)
    cols = [
        'tick',
        'x0','y0',
        'x1','y1',
        'x2','y2',
        'x3','y3',
        'x4','y4',
        'x5','y5',
        'x6','y6',
        'x7','y7',
        'x8','y8',
        'x9','y9',
    ]
    res = tmp[cols].tail(-1) # 1st position is not relevant
    return res

def process_trajectory(traj, i):
    lambda_pts = lambda x: Point(x['x' + str(i)], x['y' + str(i)])
    bis = traj.copy()
    bis['t'] = bis.apply(lambda_pts, axis=1)
    tmp = bis['t'].tolist()
    res, r2 = atp(list(tmp))
    traj["pc"+str(i)] = [ (True if i in r2 else False) for i in range(len(traj))]
    return res #Optional return

def load_and_process_game(name: str):
    game = load_game(name)

    for i in range(10):
        process_trajectory(game, i)
    
    pickle_name = (name[:-4]+ "_with_mdl_pc.pkl")
    save_DF_pickle(game, pickle_name)
    print(pickle_name)

    return game, pickle_name

def save_DF_pickle(df, name):
    df.to_pickle(name)

def load_DF_pickle(name):
    return pd.read_pickle(name)

def list_dir_extension(dir_path: str, ext: str):
    res = []
    for file in os.listdir(dir_path):
        if file.endswith(ext):
            res.append((dir_path+file))
    return res

def compression_rate_game(df):
    res = 0
    for i in range(10):
        res += df['pc' + str(i)].value_counts()[True]
    return 1 - ((res / len(df)) / 10)

def mean_compression_rates(dir_path):
    l = list_dir_extension("./coordinates/", "with_mdl_pc.pkl")
    res = 0
    for i in l:
        res += compression_rate_game(load_DF_pickle(i))
    return res / len(l)

if __name__ == '__main__':
    
    print("CSV to mdl-ed pickle files")

    dir_path = "./coordinatesV2/"
    ext = ".csv"

    files = list_dir_extension(dir_path, ext)
    nb_f = len(files)

    print("\nThere are", nb_f,"to compress")

    counter = 0
    print("Game", counter, "/", nb_f)
    tot_t = 0
    
    for name in files:
        t1 = time.time()
        load_and_process_game(name)
        t2 = time.time()
        counter += 1
        print("\nGame", counter, "/", nb_f, " time taken:", t2-t1, "s")
        tot_t += (t2-t1)
        print("Estimated time remaining:", (tot_t / counter) * (nb_f-counter), "s\n")

        if counter > 10:
            break

    print("\nAll done !!!")
    print("mean compression rate: ", mean_compression_rates("./coordinatesV2/"))




