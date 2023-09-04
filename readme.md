### SDK自己编译环境
#### centos7  
#### gcc8.5.0
### 下载第三方依赖库
1. 安装 CMake 工具（本sdk使用版本为:3.27.0），点击 [这里](http://www.cmake.org/download/) 下载，安装方式如下：
```bash
./configure
make
make install
```
2. 安装 libcurl（本sdk使用版本为:8.1.2），点击 [这里](https://github.com/curl/curl/releases/download/curl-8_1_2/curl-8.1.2.tar.gz) 下载，安装方式如下：
```bash
./configure 
make
make install
```
3. 安装 apr（本sdk使用版本为:1.7.4），点击 [这里](https://apr.apache.org/download.cgi?spm=5176.doc32132.2.9.23MmBq&file=download.cgi) 下载，安装方式如下：
```bash
./configure
make
make install
```
4. 安装 apr-util（本sdk使用版本为:1.6.3），点击 [这里](https://apr.apache.org/download.cgi?spm=5176.doc32132.2.10.23MmBq&file=download.cgi) 下载，安装时需要指定— with-apr 选项，安装方式如下：
```bash
./configure --with-apr=/your/apr/install/path
make
make install
```
5. 安装 minixml（本sdk使用版本为:3.0），点击 [这里](https://www.msweet.org/mxml/) 下载，安装方式如下：
```bash
./configure
make
make install
```
6.  安装COS SDK
源码安装。从 [GitHub](https://github.com/tencentyun/cos-c-sdk-v5) 下载源码，编译命令如下：
```bash
cmake .
make
make install
```
7. 安装 gRPC（本sdk使用版本为:1.57.0），安装方式如下：
```bash
git clone -b v1.57.0 https://github.com/grpc/grpc.git
cd grpc
git submodule update --init
mkdir -p cmake/build
cd cmake/build
cmake ../..
make
make install
cd ../../third_party/libuv
./autogen.sh
./configure
make
make install

```
8. 安装 rtmpdump（本sdk使用版本为:2.2e），点击 [这里](http://rtmpdump.mplayerhq.hu/download/rtmpdump-2.2e.tar.gz) 下载，安装方式如下：
```bash
make
make install
cp librtmp/rtmp_sys.h /usr/local/include/librtmp/
```

10. 安装 glog（本sdk使用版本为:0.6.0），点击 [这里](https://github.com/google/glog/archive/refs/tags/v0.6.0.tar.gz) 下载，安装方式如下：
```bash
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
cmake --build build --target test
cmake --build build --target install
```
11. 安装 libuuid（本sdk使用版本为:1.0.3），点击 [这里](http://sourceforge.net/projects/libuuid/files/libuuid-1.0.3.tar.gz/download) 下载，安装方式如下：
```bash
./configure
make
make install
```
11. 安装 jsoncpp（本sdk使用版本为:00.11.z），点击 [这里](https://github.com/open-source-parsers/jsoncpp/archive/refs/heads/00.11.z.zip) 下载，安装方式如下：
```bash
cmake .
make
make install
```
### 编译本代码：
编译方法如下：  
#### 测试用时
 ```bash
mkdir build 
cd build 
cmake .. -DTEST_OPTION=ON -DCMAKE_BUILD_TYPE=Debug
make
make install
```
#### 正式用时
 ```bash
mkdir build 
cd build 
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make install
```

## DEMO
demo在demo文件夹下
使用方法
 ```bash
cd demo
mkdir build 
cd build 
cmake .. 
make
cd ../bin
./ivcpClient
```