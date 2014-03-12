#! /bin/sh -vx
ID=/usr/bin/id
[ -z "$UID" ] && UID=`$ID -u`
ROOT_UID=0

#Check if run as root
if [ ${UID} -ne ${ROOT_UID} ] ; then
	echo $0 " must be run as root or sudo"
	exit 1
fi

echo "1" > /proc/sys/net/ipv6/conf/all/disable_ipv6
service avahi-daemon stop

modprobe 8021q

ip netns add ONU_A
ip netns add ONU_B
ip netns add ONU_C
ip netns add ONU_D
ip netns add DIN1

ip link add vDIN1_0 type veth peer name vDIN1_1
ifconfig vDIN1_0 -arp hw ether 00:00:10:10:00:01
ifconfig vDIN1_1 -arp hw ether 00:00:10:10:00:02
ifconfig vDIN1_0 10.10.0.1/30 up
ip link set vDIN1_1 netns DIN1
ip netns exec DIN1 ifconfig vDIN1_1 10.10.0.2/30 up
# ip netns exec DIN1 route add default gw 10.10.0.1
ip netns exec DIN1 route add default vDIN1_1


ip link add vonuA_0 type veth peer name vonuA_1
ifconfig vonuA_0 -arp hw ether 00:00:10:09:00:01
ifconfig vonuA_1 -arp hw ether 00:00:10:09:00:02
vconfig add vonuA_1 10
ip link set vonuA_1.10 netns ONU_A
ifconfig vonuA_0 up
ifconfig vonuA_1 up
ip netns exec ONU_A ifconfig vonuA_1.10 10.9.0.2/30 up
# ip netns exec ONU_A route add default gw 10.9.0.1
ip netns exec ONU_A route add default vonuA_1.10

ip link add vonuB_0 type veth peer name vonuB_1
ifconfig vonuB_0 -arp hw ether 00:00:10:09:00:05
ifconfig vonuB_1 -arp hw ether 00:00:10:09:00:06
vconfig add vonuB_1 11
ip link set vonuB_1.11 netns ONU_B
ifconfig vonuB_0 up
ifconfig vonuB_1 up
ip netns exec ONU_B ifconfig vonuB_1.11 10.9.0.6/30 up
# ip netns exec ONU_B route add default gw 10.9.0.5
ip netns exec ONU_B route add default vonuB_1.11

ip link add vonuC_0 type veth peer name vonuC_1
ifconfig vonuC_0 -arp hw ether 00:00:10:09:00:09
ifconfig vonuC_1 -arp hw ether 00:00:10:09:00:10
vconfig add vonuC_1 12
ip link set vonuC_1.12 netns ONU_C
ifconfig vonuC_0 up
ifconfig vonuC_1 up
ip netns exec ONU_C ifconfig vonuC_1.12 10.9.0.10/30 up
# ip netns exec ONU_C route add default gw 10.9.0.9
ip netns exec ONU_C route add default vonuC_1.12

ip link add vonuD_0 type veth peer name vonuD_1
ifconfig vonuD_0 -arp hw ether 00:00:10:09:00:13
ifconfig vonuD_1 -arp hw ether 00:00:10:09:00:14
vconfig add vonuD_1 13
ip link set vonuD_1.13 netns ONU_D
ifconfig vonuD_0 up
ifconfig vonuD_1 up
ip netns exec ONU_D ifconfig vonuD_1.13 10.9.0.14/30 up
# ip netns exec ONU_D route add default gw 10.9.0.13
ip netns exec ONU_D route add default vonuD_1.13

brctl addbr OLT0
brctl addif OLT0 vonuA_0 vonuB_0 vonuC_0 vonuD_0

ifconfig OLT0 up
exit

vconfig add OLT0 10
vconfig add OLT0 11
vconfig add OLT0 12
vconfig add OLT0 13
ifconfig OLT0 -arp
ifconfig OLT0.10 -arp
ifconfig OLT0.11 -arp
ifconfig OLT0.12 -arp
ifconfig OLT0.13 -arp
ifconfig OLT0 up
ifconfig OLT0.10 10.9.0.1/30 up
ifconfig OLT0.11 10.9.0.5/30 up
ifconfig OLT0.12 10.9.0.9/30 up
ifconfig OLT0.13 10.9.0.13/30 up

