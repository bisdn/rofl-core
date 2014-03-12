#!/bin/sh

ID=/usr/bin/id
[ -z "$UID" ] && UID=`$ID -u`
ROOT_UID=0

#Check if run as root
if [ ${UID} -ne ${ROOT_UID} ] ; then
        echo $0 " must be run as root or sudo"
        exit 1
fi


for i in ONU_A ONU_B ONU_C ONU_D DIN1 DIN1
do
xterm -T "$i" -e "ip netns exec $i bash --posix -i " &
done

xterm -T OLT0 &
xterm -T OLT0 -e "tcpdump -e -v -x -l -i OLT0 " &

