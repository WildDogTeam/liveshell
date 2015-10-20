
####Openwrt环境下安装说明：
#####1. 下载

从git下载到文件夹

	git clone https://github.com/WildDogTeam/liveshell.git
	
更新C/嵌入式SDK

	cd liveshell	
	git submodule update --init --recursive

#####2. 生成LiveShell-1.2.tar.gz安装包
注意权限，可能需要root权限

	./configure
	make dist

#####3. 在Openwrt下制作ipk并安装

1. 将步骤2中生成的`LiveShell-1.2.tar.gz`拷贝到Openwrt根目录下的dl目录中；

2. 在Openwrt添加LiveShell包，在Openwrt的根目录下；

		cd ./package/utils/
		mkdir LiveShell
		cd LiveShell/
		touch Makefile

	Makefile内容如下：

		include $(TOPDIR)/rules.mk
		PKG_NAME:=LiveShell
		PKG_VERSION:=1.2
		PKG_RELEASE:=1
	
	
		PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
	
	
		PKG_FIXUP:=autoreconf
		PKG_INSTALL:=1
	
		include $(INCLUDE_DIR)/package.mk
	
	
		define Package/LiveShell
			SECTION:=utils
			CATEGORY:=Utilities
			TITLE:=LiveShell — live shell
		endef
	
		PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
		PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install
	
	
		define Package/LiveShell/description
			If you can’t figure out what this program does, you’re probably
			brain-dead and need immediate medical attention.
		endef
	
	
		define Build/Configure
	  		$(call Build/Configure/Default,--with-sectype=dtls)
		endef
	
		define Package/LiveShell/install
			$(INSTALL_DIR) $(1)/usr/bin
			$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(1)/usr/bin/
		endef
	
		$(eval $(call BuildPackage,LiveShell))

	Makefile中有两点值得注意：
		
	- `define Package/LiveShell`中的`CATEGORY:=Utilities`表明在`make menuconfig`中的`Utilities`选项中可以找到LiveShell。
	
	- `define Build/Configure`中的`$(call Build/Configure/Default,--with-sectype=dtls)`配置了LiveShell的安全类型，有三种类型可以选择：`nosec`，`tinydtls`和`dtls`。

3. 在`make menuconfig`中选中LiveShell后保存退出，执行`make V=s`进行编译。编译完成后将`LiveShell_1.2-1_xxx.ipk`上传到Openwrt中，执行以下命令安装到/usr/bin目录下

		opkg install LiveShell_1.2-1_xxx.ipk


#####4.使用

	liveshell  [option] <your wilddog url> <your callback command>

#####5. 参数说明

	-o 获取到原始数据也会触发一次command回调

	-s 当数据为字符串时，不解析该字符串，完整的将该字符串（以双引号包裹）传递给command回调
	
	-v 触发回调时将命令完整打出
	
	-h 打印帮助信息
	
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
