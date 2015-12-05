
####openwrt环境下安装说明：
#####1. 下载

从git下载到文件夹

	git clone https://github.com/WildDogTeam/liveshell.git
	
更新C/嵌入式SDK

	cd liveshell	
	git submodule update --init --recursive

#####2. 生成liveshell-1.5.tar.gz安装包
注意权限，可能需要root权限

	./configure
	make dist

#####3. 在openwrt下制作ipk并安装

1. 将步骤2中生成的`liveshell-1.5.tar.gz`拷贝到openwrt工程根目录下的`dl`目录中；

2. 向openwrt工程添加liveshell包，在openwrt工程的根目录下；

		cd ./package/utils/
		mkdir liveshell
		cd liveshell/

	将和本文档同一目录下的Makefile拷贝到openwrt工程目录的`package/utils/liveshell`目录下。

	Makefile中有两点值得注意：
		
	- `define Package/liveshell`中的`CATEGORY:=Utilities`表明在`make menuconfig`中的`Utilities`选项中可以找到liveshell。
	
	- `define Build/Configure`中的`$(call Build/Configure/Default,--with-sectype=dtls)`配置了liveshell的安全类型，有三种类型可以选择：`nosec`，`tinydtls`和`dtls`。

3. 在`make menuconfig`中选中liveshell后保存退出，执行`make V=s`进行编译。编译完成后将`liveshell_1.5-xxx.ipk`上传到openwrt中，执行以下命令安装到/usr/bin目录下

		opkg install liveShell_1.5-xxx.ipk


#####4.使用

	liveshell  [option] <your wilddog url> <your callback command>

#####5. 参数说明

	-o 获取到原始数据也会触发一次command回调

	-s 当数据为字符串时，不解析该字符串，完整的将该字符串（以双引号包裹）传递给command回调
	
	-v 触发回调时将命令完整打出
	
	-h 打印帮助信息
	
	--version 打印版本号

	--authvalue=<your auth data> 传入auth数据（如超级密钥、token等）
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
