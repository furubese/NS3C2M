# NS3C2M
Used ns-3.36.
Doesn't work below ns3.35
# 手順
## 1. サンドボックスの起動
SandboxSh/qemu-openwrt.shを参考  
※qemu-bridge-helperを使用してください．  
参照 -> ttps://wiki.archlinux.jp/index.php/QEMU  
## 2. thetapの作成

## 3. ns-3起動

## 4. サンドボックス内での操作
br-lanにIPv4追加と，デフォルトゲートの設定  
10.1.1.1/24がサンドボックス，10.1.1.3がns-3のおとりのサーバ
 `ip addr add 10.1.1.1/24 dev br-lan`
 `route add default gw 10.1.1.3`
