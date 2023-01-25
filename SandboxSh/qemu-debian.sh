sudo brctl addbr br0
echo "option -> ip addr add 10.1.1.254/24 dev br0"
sudo ip link set br0 up
echo -e "\e[31;7m pls. debid setting. \e[m -> \e[31mip addr add 10.1.1.1/24 dev eth0 \e[m";
echo -e "\e[31;7m pls. debid setting. \e[m -> \e[31mroute add default gw 10.1.1.3 \e[m";
echo -e "\e[31;7m pls. debid setting. \e[m -> \e[31mmount /dev/vda3 /mnt \e[m";
sudo qemu-system-arm -M virt -m 1024 \
  -kernel /home/fse/Pubwork/debid/vmlinuz-4.19.0-20-armmp-lpae \
  -initrd /home/fse/Pubwork/debid/initrd.img-4.19.0-20-armmp-lpae \
  -drive if=none,file=/home/fse/Pubwork/debid/hda.qcow2,format=qcow2,id=hd \
  -device virtio-blk-device,drive=hd \
\
  -netdev bridge,id=debian_tapbridge \
  -device virtio-net-device,netdev=debian_tapbridge \
  -append "root=/dev/vda2" \
  -nographic -no-reboot \
\
  -drive file=/home/fse/work/openwrt/malware_image,format=raw
