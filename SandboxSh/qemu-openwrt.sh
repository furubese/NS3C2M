sudo brctl addbr br0
echo "option -> ip addr add 10.1.1.254/24 dev br0"
sudo ip link set br0 up
echo -e "\e[31;7m pls. openwrt setting. \e[m -> \e[31mip addr add 10.1.1.1/24 dev br-lan & route add default gw 10.1.1.3 \e[m";
echo -e "\e[31;7m pls. openwrt setting. \e[m -> \e[31mroute add default gw 10.1.1.3 \e[m";
sudo start-qemu-openwrt;

