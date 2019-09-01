## chat_room项目文档
本项目的内容可以随意进行学习、使用，请勿用于商业目的
## 目录
* 引言
	* 项目综述
	* 术语表
	* 参考资料
	* 开发环境及使用工具
	* 使用说明
* 数据结构说明
* 模块设计
	* 程序函数调用图及模块分化
	* 功能设计说明
* 数据库说明
* 异常，错误处理
* 运行截图
### 引言
*********************
#### 项目综述
随着网络的发展，地球越来越小，人与人之间的距离也越来越小，实施通讯称为必须要解决的一个问题。此项目是基于一些通讯软件的功能，结合所学的进程，线程，网络通信的知识实现的一个通讯软件。
此项目还有很多不足的地方，欢迎邮箱联系，能共同学习，共同进步

#### 表述语
___________________
| 序号 | 术语 | 说明性定义 |
| ---- | ---- | ---- |
| 1 | C/S | Client/Server模型 |
| 2 | epoll | linux下处理并发的API |
| 3 | makefile | linux下进行工程构建的文件 |
#### 参考资料
************************
>a.相关类型的书籍

>| 书籍 | 作者 |
>| ---- | ---- |
>| LinuxC编程实战 | 董永清 |
>| 鸟哥的Linux私房菜(第三版) | VBird |
>| 深入理解计算机系统(原书第三版) | Computer Systems: A Programmer's Perspective (3rd Edition) |

>b.查阅的相关的博客，网上资料

>| 资料名 | 作者 |
>| ---- | ---- |
>| socket文件传输功能的实现 | 编程帮 |
>| web类聊天数据库功能表结构如何设计？ | 白及 |

#### 开发环境及使用工具
开发环境：ubuntu 18.04.2 X86_64
开发工具：Vim及gcc编译器
绘图工具：ProcessOn
#### 使用说明
```
git clone 
cd chat_room
gcc epoll.c -o server -lmysqlclient
./server
gcc client.c -o client -lpthread
./client 9876 [ip]
```
```
使用说明
服务端与客户端要在不同的终端下编译运行，之后按照提示进行使用
```
### 数据结构说明
**********************
本项目中主要是进行线程，进程，网络编程知识的综合运用
所涉及到的数据结构如下：

|数据结构|使用之处|
|----|----|
|数组|使用数组，主要是在TCP发送包以及各种功能函数实现时所使用的。存储字符串、标识符。
|链表|服务器端读取数据库中的内容，存储在线用户。客户端用于存储消息。使用链表是为了删除，插入操作方便。

### 模块设计
**********************
#### Client
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/client.png)
#### server
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/server.png)
#### 功能设计说明
****************************
#### client -- main模块
功能描述：与服务端建立连接，创建接受信息线程
算法与流程图
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/client_main.png)
*********************************************** 
#### client -- server_manage模块
功能描述：client进行收包的模块
算法与流程图
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/client_recv.png)
***********************************************
#### client -- user_face模块
功能描述：为用户显示聊天室的功能
算法与流程图
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/client_user_face.png)
***********************************************
#### Server -- main模块
功能描述：Server总体控制的main模块
算法与流程图
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/server_main.png)
***********************************************
#### Server -- epoll事件处理模块
功能描述：Server进行事件处理的模块
算法与流程图
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/server_chuli.png)
***********************************************
### 数据库说明
| 表名 | 表的功能 |
| ---- | ---- |
| user | 用户信息 |
| friend | 好友关系 |
| user_groups | 用户建立的群 |
| group_relationship | 群关系 |

### 异常，错误处理
*************************************
错误输入引发的错误
解决方案：switch中提示用户错误输入，并重新输入
### 运行截图
*************************************
1.main
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/run_pactures/2019-09-01%2010-19-56屏幕截图.png)
2.user_face
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/run_pactures/2019-09-01%2010-20-47屏幕截图.png)
3.friends
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/run_pactures/2019-09-01%2010-21-33屏幕截图.png)
4.groups
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/run_pactures/2019-09-01%2010-22-17屏幕截图.png)
5.requests
![image](https://github.com/XiyouLinuxGroup-2019-Summer/TeamC/blob/master/Code/huqingwei/chat_room/images/run_pactures/2019-09-01%2010-25-04屏幕截图.png)
