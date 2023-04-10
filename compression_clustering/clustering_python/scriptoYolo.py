import numpy as np
import pandas as pd
import sys

from AffinityPropagation import *

sys.path.insert(0,'..')
from cplusplus.clustering.Segment import *

sys.path.insert(0,'../mdl')
from csv_to_mdl_pickle import *

def get_segs(df):
    segs = []
    for i in range(10):
        pc_mdl = df.query( 'pc' + str(i) + ' == True' )
        #print('len MDL'+str(i), len(pc_mdl))
        #print(pc_mdl)
        point_maker = lambda x: Point(x['x'+str(i)], x['y'+str(i)])
        tmp = pc_mdl.apply(point_maker, axis=1).tolist()
        #print("leeeeeen tmp", len(tmp))
        for i in range(len(tmp)-1):
            segs.append( Segment(tmp[i], tmp[i+1]) )

    #print('segs of a game: ', len(segs))
    return segs

def process_games(l):
    res = []
    for i in l:
        print(i)
        d = pd.read_csv(i, delimiter=',').tail(-1)
        res.extend( get_segs(d))
        
    return res

l = ['../cplusplus/mdl_trajectories/mdl_coord_test.csv']

res = process_games(l)


titre = np.array(res)
np.save("arr_segs_"+str(len(l))+"_bis_game.pkl", titre)