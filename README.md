# liveshell
使用野狗开发一个远程调用shell脚本的工具。运维工程师的利器。

####工具说明：

wilddog_watch工具是采用Wilddog云以及WildDog C/嵌入式SDK制作的远程调用shell脚本的工具，监听某个url下数据的变化，当数据发生变化后，触发预先安排好的可执行程序或者shell脚本，同时将变化后的数据作为参数传入。

####linux环境下安装说明：
#####1. 拷贝到某文件夹并进入

	cd wilddog-consolewatch

#####2. 安装（注意权限问题，install需要sudo）

	./configure
	make
	make install

#####3.使用

	wilddog_watch  --url=<url> --cmd=<command> [--ignore-origin-data=yes | no] [--verbose=yes | no] [--ignore-leaf-quote=yes | no]

参数说明：

	<url>：需要关注的节点url

	<command>：节点数据发生变化后，被触发的可执行程序或者shell脚本

	--ignore-origin-data：第一次数据变化（即初始数据）是否触发可执行程序或者shell脚本，默认为no。

	--verbose：触发时是否打印完整命令，默认为no。

	--ignore-leaf-quote：当url为叶子节点，且值为字符串，是否忽略值首尾两端的引号，默认为yes。

#####4.使用说明：

忽略第一次触发：

	wilddog_watch  --url="coap://<your appid>.wilddogio.com/a/b" --cmd="./helloworld.sh" --ignore-origin-data=yes

不忽略第一次触发：

	wilddog_watch  --url="coap://<your appid>.wilddogio.com/a/b" --cmd="./helloworld.sh"

或

	wilddog_watch  --url="coap://<your appid>.wilddogio.com/a/b" --cmd="./helloworld.sh" --ignore-origin-data=no

显示可执行命令和数据：

	wilddog_watch  --url="coap://<your appid>.wilddogio.com/a/b" --cmd="./helloworld.sh"  --verbose=yes

不显示可执行命令和数据：

	wilddog_watch  --url="coap://<your appid>.wilddogio.com/a/b" --cmd="./helloworld.sh"

或：

	wilddog_watch  --url="coap://<your appid>.wilddogio.com/a/b" --cmd="./helloworld.sh"  --verbose=no

如果关注的是叶子节点，并且节点的数据类型是字符串，那么可以使用--ignore-leaf-quote选项去除叶子节点的数据的双引号：

	wilddog_watch  --url="coap://<your appid>.wilddogio.com/a/b" --cmd="./helloworld.sh" --ignore-leaf-quote=yes

#####5. 例子

######显示根目录的文件

1. 在Wilddog云端建立一颗数据树，结构为

		{
		  "path": "/"
		}	

2. 终端运行wilddog_watch，将<your Appid>替换成你自己的appid

		wilddog_watch --url="coaps://<your Appid>.wilddogio.com/path" --cmd="ls"

3. 终端的显示结果等同于在终端运行"ls /"，修改云端数据，把"/"改为"-l /"，在终端立刻显示"ls -l /"的结果；类似的，你也可以将--cmd的参数改为shell脚本，执行更加复杂的操作。
