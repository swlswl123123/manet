# Qsize greper
grep '@[0-9.]*\[[0-9]*\]REACHABLE = [0-9]*' $1 > ${1%.*}.nb.csv

sed -i 's/@\([0-9.]*\)\[\([0-9]*\)\]REACHABLE = \([0-9]*\)/\1,\2,\3/g' ${1%.*}.nb.csv