#!/bin/bash

time ./bin/main compression 14 1.0

for i in `ls -d coordinates/*/ | sed 's/coordinates\///' | sed 's/\///'`; do
    paste -d "," coordinates/$i.csv coordinates/$i/*  | sed 's/\r//' > mdl_trajectories/mdl_$i.csv
done

echo 'done'
