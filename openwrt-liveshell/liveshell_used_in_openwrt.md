
####openwrt环境下安装说明：
#####1. 下载

从git下载到文件夹

	git clone https://github.com/WildDogTeam/liveshell.git
	
更新C/嵌入式SDK

	cd liveshell	
	git submodule update --init --recursive

#####2. 部署到openwrt项目中

将本目录下的liveshell文件夹（以及其中的Makefile文件）拷贝到openwrt项目中的package/utils目录下（如果没有utils目录直接放在package目录）。

	cp -rf openwrt-liveshell/liveshell <your openwrt path>/package/utils/

#####3. 在openwrt项目下制作ipk并安装

1. 在openwrt项目的根目录下运行`make menuconfig`；

2. 在`Utilities`目录下，选中`liveshell`，并设置为module（如果设置为built-in，则可忽略4、5两步，但需要将整个openwrt固件刷到设备中）；

3. 运行make编译openwrt；

4. 编译成功后，在bin目录下能找到`liveshell_xxxxxx.ipk`；

5. 将这个ipk上传到openwrt中，执行opkg安装

		opkg install liveshell_xxxxxx.ipk


#####4.使用

	liveshell  [option] <your wilddog url> <your callback command>

liveshell命令监听`your wilddog url`下数据的变化，当数据发生变化时，调用`your callback command`，并且将最新的数据（json字符串）作为第一个参数传递给`your callback command`，注意，`your callback command`需要本身能够在console中执行，如果`your callback command`还需要root权限，请用root权限调用liveshell。

#####5. 参数说明

######liveshell参数
	-o 获取到原始数据也会触发一次command回调

	-s 当数据为字符串时，不解析该字符串，完整的将该字符串（以双引号包裹）传递给command回调
	
	-v 触发回调时将命令完整打出
	
	-h 打印帮助信息
	
	--version 打印版本号

	--authvalue=<your auth data> 传入auth数据（如超级密钥、token等）

######configure 配置参数说明

你也可以修改openwrt项目下的package/utils/liveshell目录下的Makefile，修改configure配置(34行Build/Configure宏)：

	--with-endian=ARG       大小端, ARG可设为big|little,  默认little
	--with-bits=ARG         机器位数, ARG可设为8|16|32|64, 默认32
	--with-maxsize=ARG      应用层协议长度, ARG可设为0~1300, 默认1280
	--with-queuenum=ARG     消息队列个数, 默认32
	--with-retranstime=ARG  重传超时时间（ms）, 默认10000ms
	--with-recvtimeout=ARG  单次最大接收时间（ms）, 默认100ms
	--with-sectype=ARG      加密方式, ARG可设为nosec|tinydtls, 默认tinydtls

#####6. 例子

######利用wget下载文件

1. 在Wilddog云端建立一颗数据树，结构为

		{
		  "url": "http://www.libssh2.org/download/libssh2-0.11.tar.gz"
		}	

2. 新建一个名为dl.sh的shell脚本，脚本内容如下：

		#!/bin/sh

		wget $1 -P /mnt/disk


3. 终端运行liveshell，将`<your Appid>`替换成你自己的appid

		liveshell coaps://<your Appid>.wilddogio.com/url ./dl.sh

4. 终端将会执行wget http://www.libssh2.org/download/libssh2-0.11.tar.gz -P /mnt/disk命令而将文件下载到/mnt/disk目录下。
