sudo ip tuntap add dev thetap mode tap 
sudo ip link set dev thetap address 00:00:00:00:00:01
sudo ip link set dev thetap up
sudo brctl addif br0 thetap
