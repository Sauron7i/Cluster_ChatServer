# Cluster_ChatServer
基于muduo实现的 可以工作在Nginx TCP负载均衡环境中的集群聊天服务器和客户端源码

编译方式
cd build
rm -rf *
cmake ..
make

学习muduo网络库，作为该项目的网络模块；
添加了MySQL连接池（在db文件中），可以提高数据库的访问效率，省去大量三次握手四次挥手过程；
配置Nginx TCP负载均衡，扩充多个服务器，让每个客户端连接固定端口，再由Nginx负载到具体服务器上；
服务器中间件使用redis（发布-订阅）

