# Qsize greper

grep '@[0-9.]*\[[0-9]*\]TRAFFIC:SEND' $1 > ${1%.*}.send.csv
sed -i 's/@\([0-9.]*\)\[\([0-9]*\)\]TRAFFIC:SEND\[\([0-9]*\)\] to \[[0-9]*\] URG \([0-1]*\)/\1,\3,\4/g' ${1%.*}.send.csv