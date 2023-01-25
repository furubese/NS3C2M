# python-udp-sender.py
UDPのテストに利用しました．
# qemu-debian.sh
QemuでDebianを起動させるときに使いました．
# qemu-openwrt.sh
Qemuでopenwrtを起動させるために使いました．  
ホストにbr0という名前のブリッジを作成し，br0を準備OKの状態にします．  
その後，start-qemu-openwrt.shを起動します．
# set_route_br0.sh
主に1.2.3.4宛てのパケットでテストするために用いました．  
ホストのbr0に「1.2.3.0/24宛ての通信は10.1.1.3にもっていってね」という静的ルートを設定します．  
# set_route_thetap.sh
主に1.2.3.4宛てのパケットでテストするために用いました．  
ns-3のConfigureLocalモードとUseLocalでテストするために用いました．  
ホストのthetapに「1.2.3.0/24宛ての通信は10.1.1.3にもっていってね」という静的ルートを設定します．  
# start-qemu-openwrt.sh
本研究のサンドボックスを起動するためのメインのshです．  
qemu-system-armにより，arm32bitアーキテクチャの仮想環境を作成します．  
また，kvmは必要ないです．  
また，-M virt-2.9は必要ないとは思います．こちらを参照してください．  
　-> ttps://gist.github.com/extremecoders-re/f2c4433d66c1d0864a157242b6d83f67  

-net nic -net bridge, br=br0
については，こちらの「6.3	QEMU の Tap ネットワーク」を参照ください．
 -> ttps://wiki.archlinux.jp/index.php/QEMU
-drive file
等は，ドライブの設定です．ArchWiki等を確認してください．
