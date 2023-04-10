#!/bin/bash

#Paramètres :
# $1 : csv file
# $2 : traj_succ
# $3 : output_seq_file
# $4 : output_prefixspan_file
# $5 : minsup
minsup=20%
# $6 : mingap
mingap=4
# $7 : maxgap
maxgap=4
# $8 : minlen
minlen=1
# $9 : maxlen
maxlen=10000

file_transac=seq.txt
file_output=output.txt

group_size=60
player=0

#python3 traj2seq_2.py $1 $2 > $3
rm $file_transac
touch $file_transac

rm -rf output
mkdir output

#for i in `ls ../cplusplus/experiments_results/kmed/kmed_200_111/_coord_*`;do
for i in `ls ../cplusplus/experiments_results/kmed/1__/_coord_*`;do
#for i in `ls ../cplusplus/experiments_results/kmed/1__/_coord_*`;do
    echo $i
    python3 traj2seq_2.py $i $group_size $player >> $file_transac
done

echo "seq done"



java -jar spmf.jar run Fournier08-Closed+time $file_transac $file_output $minsup $mingap $maxgap $minlen $maxlen

echo "prefixspan done"

python3 output_to_csv.py ../cplusplus/experiments_results/kmed/1__/

echo "csv done"
