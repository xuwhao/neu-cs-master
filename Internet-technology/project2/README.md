# 实验二： 简易多线程静态 HTTP 服务器

## 功能

* 基于线程池的多线程
* 支持 HTTP GET 请求 (基于 RFC 2616)
* 支持静态资源内容请求
* MIME 类型支持
* 支持 200, 206 和 404 HTTP Response Status Code

## 功能展示

在 `./static_files/` 文件夹下放入由 Hexo 生成的纯静态网页后的效果。P.S. 你可以放入任何纯静态的网页内容.

[![IgiagU.md.png](https://z3.ax1x.com/2021/11/15/IgiagU.md.png)](https://imgtu.com/i/IgiagU)

[![IgiBDJ.md.png](https://z3.ax1x.com/2021/11/15/IgiBDJ.md.png)](https://imgtu.com/i/IgiBDJ)

其效果等效与本人用 Ngnix 部署的个人网站，浏览请 [点击此处](https://www.xuwhao.top)。

## 安装

1. clone 本仓库

```shell
git clone https://github.com/xuwhao/neu-cs-master.git
```

2. 安装 python2.7
自行安装. 如没有集成开发环境等也可自行安装，如 PyCharm.

3. 进入本实验目录.

```shell
cd ./Internet-technology/project2/
```

4. 安装相关依赖

```shell
pip install -r requirements.txt
```

5. （可选）执行测试，检测环境是否安装

```
nosetests .
```

## 运行

1. 启动服务器

 ```python
 python run.py
 ```

2. 在浏览器中打开对应网页
    * open url: <http://localhost:5555/test_1.txt>
    * open url: <http://localhost:5555/test_2.html>
    * open url: <http://localhost:5555/test_3.html>
    * open url: <http://localhost:5555/test_4.gif>
    * open url: <http://localhost:5555/test_5.mp4>

3. everything works?
