# Qsize greper

grep '@[0-9.]*\[[0-9]*\]TRAFFIC:\[[0-9]*\] DELAY = [0-9.]*' $1 > ${1%.*}.delay.csv
sed -i 's/@\([0-9.]*\)\[[0-9]*\]TRAFFIC:\[\([0-9]*\)\] DELAY = \([0-9.]*\)/\1,\2,\3/g' ${1%.*}.delay.csv