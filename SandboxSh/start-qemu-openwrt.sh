qemu-system-arm -M virt-2.9 \
 -kernel /home/fse/work/openwrt/zImage \
 -no-reboot -nographic \
 -net nic -net bridge,br=br0 \
 -drive file=/home/fse/work/openwrt/root.squashfs,if=virtio,format=raw \
 -drive file=/home/fse/work/openwrt/malware_image,format=raw \
 -append "root=/dev/vda"
