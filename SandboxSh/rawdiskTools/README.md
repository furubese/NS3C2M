このディレクトリはマルウェアをサンドボックス(OpenWrt)にコピーするために使いました．

本研究では，QEMUのdriveオプションでマルウェアをOpenWrtに運びました．  
sshやrsync等でマルウェアをOpenWrtにコピーするならば以下の手順は必要ありません．

raw image diskを作成して，マウントし，マルウェアを詰め込みました．  
1. dd等で作成したmalware_imageをホストPCでマウント(mount-rawdisk.sh)
2. マルウェアを詰め込む
3. malware_imageをumount(umount-rawdisk.sh)
4. QemuとOpenWrtを起動
5. OpenWrtでmalware_imageをマウント
6. OpenWrtでマルウェアをコピーして実行

rawimagediskの作成について  
参考 -> ttps://vorfee.hatenablog.jp/entry/2016/01/16/170654

Qemuのオプションでmalware_imageを設定してください．  
例：-drive file=/home/fse/work/openwrt/malware_image,format=raw

OpenWrtでmalware_imageのドライブをマウントしてください．
OpenWrtでマルウェアをコピー．malware_imageをディスマウントする．
