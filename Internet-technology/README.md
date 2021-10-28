# 互联网技术及其应用实践

- [互联网技术及其应用实践](#互联网技术及其应用实践)
  - [目录结构](#目录结构)
  - [Project1 - 传输层实验](#project1---传输层实验)
    - [实验内容](#实验内容)
    - [Q&A](#qa)

## 目录结构
```
  - Internet-technology   # 根目录
    - project1            # Project1 - 传输层实验
        - example         # 本课的样例代码
    //- project2 ...      # 未完待续
```

## Project1 - 传输层实验
### 实验内容
- [x] 调试 client.c, server.c, ping.c 和 traceroute.c. 
- [ ] 编写基于 UDP 的 client.c 和 server.c.
- [ ] 修改 ping.c 使其支持 ping 攻击。
- [ ] 基于 Raw Sockets, 编写基于 UDP 的 traceroute 程序。
### Q&A
* 为什么编译失败？  
  尝试编译时加入参数 "-lws2_32".

* gethostbyname 函数取不到 ip, 导致 segmentation fault.  
  取消内存字节对齐，即注释掉 `#pragma pack(4)`.

* traceroute 时，中间的路由全部 receive timeout.  
 关闭本机防火墙.
