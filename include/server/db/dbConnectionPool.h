#ifndef DBCONNECTIONPOOL_H
#define DBCONNECTIONPOOL_H

// 实现连接池功能模块
#include<iostream>
#include<string>
#include<queue>
#include<mutex>
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<condition_variable>
#include "db.h"

using namespace std;

class ConnectionPool
{
public:
	// 获取连接池对象实例 静态的
	static ConnectionPool* getConnectionPool();

	// 给外部提供接口，提供一个空闲的连接
	shared_ptr<Connection> getConnection();

private:
	ConnectionPool(); // 单例模式 构造函数私有化

	bool loadConfigFile(); // 加载配置文件

	// 生产者线程函数, 负责生产新连接
	void produceConnectionTask();

	// 回收线程函数，负责回收多余空闲连接
	void scannerConnectionTask();

	string _ip;				// MySQL的IP地址
	unsigned short _port;	// MySQL的端口号
	string _username;		// MySQL的登陆用户名
	string _password;		// MySQL的登录密码
	string _dbname;			// 数据库名字
	int _initSize;			// 连接池的初始连接量
	int _maxSize;			// 连接池的最大连接量
	int _maxIdleTime;		// 连接池的最大等待时间
	int _connectionTimeOut;	// 连接池获取连接的超时时间

	queue<Connection*> _connectionQue; // 存储MySQL连接的队列

	mutex _queueMutex;			// 维护连接队列的线程安全互斥锁
	atomic_int _connectionCnt;	// 记录连接所创建的connection的总连接数，不能超过_maxSize
	condition_variable cv;		// 连接生产线程和消费线程之间的通信
};

#endif