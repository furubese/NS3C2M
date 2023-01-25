# 本研究
Used ns-3.36.1
Doesn't work below ns3.36.1  
```mermaid
graph LR
    ID10(OpenWrtのデバイス) -- 転送 --> ID11
    ID11[ホストqemuのtapデバイス] -- 転送 --> ID12
    ID12[ホストthetap] -- 転送 --> ID1
    ID10 -.-> ID2
    ID2 -.-> ID10
    ID3 -.-> ID10
    ID1((GhostNode)) -- マルウェアの通信 --> ID2((普通のサーバ))
    ID2 -- C2への接続なら転送 --> ID3((C2サーバ))
    ID2 -- 返信 --> ID1
    ID3 -- 返信 --> ID1
```
# ns-3のインストール
endeavourosのaurを使ってインストールしました．  
エラーが出たらエラーの内容に従って各自修正してください．  
`yay -S ns3`  
参照 -> ttps://aur.archlinux.org/packages/ns3
# 本研究の実験手順
## 1. サンドボックスの起動
SandboxSh/qemu-openwrt.shを参考  
※qemu-bridge-helperを使用してください．  
参照 -> ttps://wiki.archlinux.jp/index.php/QEMU  
## 2. thetapの作成と設定
SandboxSh/ns3-tap-bridge.shを参考  
## 3. ns-3起動

## 4. サンドボックス内での操作
br-lanにIPv4追加と，デフォルトゲートの設定  
10.1.1.1/24がサンドボックス，10.1.1.3がns-3のおとりのサーバ  
 `ip addr add 10.1.1.1/24 dev br-lan`  
 `route add default gw 10.1.1.3`  

## 5. マルウェアの実行
## 6. pcapがいっぱい生成されてる
## 7. 見る
