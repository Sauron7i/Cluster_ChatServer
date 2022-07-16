# Cluster_ChatServer
基于muduo实现的 可以工作在Nginx TCP负载均衡环境中的集群聊天服务器和客户端源码

编译方式
cd build
rm -rf *
cmake ..
make

需要配置Nginx TCP负载均衡
