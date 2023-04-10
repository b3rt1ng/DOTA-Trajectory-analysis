import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys
import os

def list_dir_extension(dir_path: str, ext: str):
    res = []
    for file in os.listdir(dir_path):
        if file.endswith(ext):
            res.append((dir_path+file))
    return res

def compression_rate():
    print("calculating compression rate...")
    res = []
    l = list_dir_extension("mdl_trajectories/", ".csv")
    for file in l:
        df = pd.read_csv(file, delimiter=',').tail(-1)
        for i in range(10):
            dc = df.copy()[['x' +str(i),'y' +str(i),'pc' +str(i)]]
            pc_mdl = df.query("pc" + str(i) + " == 1")
            comp_percentage = (1 - (len(pc_mdl)/len(df)) ) * 100
            res.append(comp_percentage)
    mean = np.mean(res)
    print("mean compression rate: ", mean, "%")
    return mean

compression_rate()

#df = pd.read_csv('mdl_trajectories/mdl_coord_test.csv', delimiter=',').tail(-1).head(100)
df = pd.read_csv('mdl_trajectories/mdl_coord_test.csv', delimiter=',').tail(-1)


dc = df.copy()[['x0','y0','pc0']]
x = dc.x0
y = dc.y0

pc_mdl = df.query("pc0 == 1")

print("nb pc0: ",len(pc_mdl))
print("nb pts: ",len(df))

comp_percentage = (1 - (len(pc_mdl)/len(df)) ) * 100
print("compression percentage: ", round(comp_percentage, 1), "%")

x_mdl = pc_mdl.x0
y_mdl = pc_mdl.y0

plt.subplot(1, 2, 1)
plt.plot(x, y, '-o', c='blue', label="original trajectory")
#plt.plot(x_mdl, y_mdl, '--*', c='orange', label="compressed trajectory")
plt.axis('square')
plt.legend()

plt.subplot(1, 2, 2)
plt.plot(x_mdl, y_mdl, '-o', c='orange', label="compressed trajectory (MDL), " + str(round(comp_percentage, 1)) + "% of the original trajectory")
plt.axis('square')
plt.legend()
#plt.imshow(plt.imread("../Map_dota.jpg"), extent=(65, 185 ,65 ,185))
plt.show()

df = pd.read_csv('mdl_trajectories/mdl_coord_test.csv', delimiter=',').tail(-1).head(100)


dc = df.copy()[['x0','y0','pc0']]
x = dc.x0
y = dc.y0

pc_mdl = df.query("pc0 == 1")

x_mdl = pc_mdl.x0
y_mdl = pc_mdl.y0

plt.plot(x, y, '-o', c='blue', label="original trajectory")
plt.plot(x_mdl, y_mdl, '--*', c='orange', label="compressed trajectory")
plt.axis('square')
plt.legend()

plt.show()





""" def f(df, i, color):
    dc = df.copy()[['x'+str(i),'y'+str(i),'pc'+str(i)]]
    #x = dc.x0
    #y = dc.y0

    pc_mdl = df.query("pc" + str(i) +" == 1").head(2)

    x_mdl = pc_mdl[['x'+str(i)]]
    y_mdl = pc_mdl[['y'+str(i)]]

    plt.plot(x_mdl, y_mdl, '--o', c=color)
for i in range(10):
    f(df, i, 'red')

f(df, 7, 'blue')
f(df, 1, 'blue')
f(df, 2, 'blue')
f(df, 3, 'blue')
f(df, 4, 'blue')
f(df, 0, 'blue')
 """

""" plt.legend()
#plt.imshow(plt.imread("../Map_dota.jpg"), extent=(65, 185 ,65 ,185))
plt.axis('square')
plt.show() """



""" resG = 0
resC = 0

for i in range(10):
    dc = df.copy()[['x'+str(i),'y'+str(i),'pc'+str(i)]]
    pc_mdl = df.query("pc" + str(i) +" == 1")
    # calculate the total distance of the compressed trajectory
    resC += len(pc_mdl)-1
    for i in range(0, len(pc_mdl)-1):
        resG += np.sqrt((pc_mdl.iloc[i].x0 - pc_mdl.iloc[i+1].x0)**2 + (pc_mdl.iloc[i].y0 - pc_mdl.iloc[i+1].y0)**2)
print("mean compressed: ",resG/resC)

data = np.genfromtxt('clustering_results/2023_3_17_16H_31M/clusters.csv', delimiter=',', skip_header=1)
resG = 0
resC = len(data)
for i in range(len(data)):
    resG += np.sqrt((data[i][1] - data[i][3])**2 + (data[i][2] - data[i][4])**2)
print("mean cluster: ",resG/resC)
 """

"""
# calculate the total distance of the original trajectory
res = 0
for i in range(0, len(df)-1):
    res += np.sqrt((df.iloc[i].x0 - df.iloc[i+1].x0)**2 + (df.iloc[i].y0 - df.iloc[i+1].y0)**2)
print("mean orginial: ",res/len(df))

print("nb pc0: ",len(pc_mdl))
print("nb pts: ",len(df)) """