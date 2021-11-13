# Project1 - 传输层实验

- [Project1 - 传输层实验](#project1---传输层实验)
  - [目录结构](#目录结构)
  - [(非必须) 项目环境](#非必须-项目环境)
    - [(非必须) 系统环境](#非必须-系统环境)
    - [(非必须) 安装 MinGW-w64](#非必须-安装-mingw-w64)
    - [(非必须) 开始 Coding](#非必须-开始-coding)
  - [[非必须] 快速开始](#非必须-快速开始)
  - [实验内容](#实验内容)
  - [Q&A](#qa)
  - [Reference](#reference)

## 目录结构

```shell
  - Internet-technology    # 根目录
    - project1             # Project1 - 传输层实验
        - example          # 本课的样例代码
        - udp-client.c     # Task2
        - udp-server.c     # Task2
        - ping.c           # Task3
        - traceroute-udp.c # Task4
    //- project2 ...       # 未完待续
```

## (非必须) 项目环境

如果电脑上没有写 c 语言的环境，则执行下列步骤。

### (非必须) 系统环境

windows10.

### (非必须) 安装 MinGW-w64

打开 [官网](https://www.mingw-w64.org/downloads/)，选 MingW-W64-builds 下载。

[![5LCEzq.md.png](https://z3.ax1x.com/2021/10/28/5LCEzq.md.png)](https://imgtu.com/i/5LCEzq)

安装时 Architecture 选 x86_64. (如果你是 32 位操作系统那还是选 i686)。别的默认一路下一步。

[![5LCDfA.md.png](https://z3.ax1x.com/2021/10/28/5LCDfA.md.png)](https://imgtu.com/i/5LCDfA)

### (非必须) 开始 Coding

> 选择任意一个你喜欢的编辑器或者任何 IDE，此处以 Visual Studio Code 为例。  

进入官网下载 vscode 并安装。  
`File -> Preference -> Extensions` 搜索 `c/c++` 插件安装。  

Clone 本仓库并在 vscode 中打开。  
`Terminal -> Run Build Task` 选 C/C++: gcc.exe.  
在生成的 `.vsocde/tasks.json` 中，`args` 处追加一行 `-lws2_32`.

[![5LFT1A.md.png](https://z3.ax1x.com/2021/10/28/5LFT1A.md.png)](https://imgtu.com/i/5LFT1A)

打开示例代码，ctrl + F5 运行。

## [非必须] 快速开始

Clone 本项目，确保安装 MinGw 后，执行以下命令编译。

```shell
cd /Internet-technology/project1

mingw32-make
```

之后会生成 `udp-server.exe`，`udp-client.exe`， `ping.exe` 和 `traceroute-udp.exe`，即可执行相应程序。 其中 `ping.exe` 和 `traceroute-udp.exe` 需要管理员权限。

## 实验内容

- [x] Task1: 调试 client.c, server.c, ping.c 和 traceroute.c.
- [x] Task2: 编写基于 UDP 的 client.c 和 server.c.
- [x] Task3: 修改 ping.c 使其支持 ping 攻击。
- [x] Task4: 基于 Raw Sockets, 编写基于 UDP 的 traceroute 程序。

## Q&A

- 为什么编译失败？  
  尝试编译时加入参数 "-lws2_32".

- gethostbyname 函数取不到 ip, 导致 segmentation fault.  
  取消内存字节对齐，即注释掉 `#pragma pack(4)`.

- traceroute 时，中间的路由全部 receive timeout.  
 关闭本机防火墙.

## Reference

| Description      | Link |
| ----------- | ----------- |
| socket function      | <https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket>       |
| bind function   | <https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-bind>        |
| recvfrom function | <https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-recvfrom> |
| sendto function | <https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-sendto>  |
| ping in c| <https://www.geeksforgeeks.org/ping-in-c/> |
