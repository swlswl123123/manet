cd ../sim/
for ((i=1;i<=800;i++));
do
    num=`cat CHAIN_4_B266.log | grep "\[$i\] DELAY" | wc -l`
    if [[ $num == 0 ]];
    then
        echo $i "PACKET LOSS"
    fi
done
