#include "dbConnectionPool.h"
#include "public.hpp"

// 线程安全的懒汉单例模式函数接口
ConnectionPool* ConnectionPool::getConnectionPool()
{
	// 静态局部变量，编译器自动lock和unlock
	static ConnectionPool pool;
	return& pool;
}

// 初始化连接池
ConnectionPool* sp = ConnectionPool::getConnectionPool();

// 从配置文件中加载配置项
bool ConnectionPool::loadConfigFile()
{
	FILE* pf = fopen("/home/ubuntu/projects/ChatServer/src/server/db/mysql.conf", "r");
	if (pf == nullptr)
	{
		LOG("mysql.conf file is not exist!");
		return false;
	}

	while (!feof(pf))
	{
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find('=', 0);
		if (-1 == idx) // 无效的配置
		{
			continue;
		}

		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut")
		{
			_connectionTimeOut = atoi(value.c_str());
		}
	}
	return true;
}

// 连接池的构造函数
ConnectionPool::ConnectionPool()
{
	// 1.加载配置项
	if (!loadConfigFile())
	{
		return;
	}

	// 2.创建初始数量连接
	for (int i = 0; i < _initSize; ++i)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);

		p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
		_connectionQue.push(p);
		_connectionCnt++;
	}

	// 3.启动新的线程作为生产者线程
	thread produce(std::bind(& ConnectionPool::produceConnectionTask, this));
	produce.detach(); // 守护线程（分离线程）

	// 4. 启动新的线程，扫描超过maxIdleTime的空闲连接，进行多余的连接回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

// 生产者：运行在独立的线程，专门负责生产连接
void ConnectionPool::produceConnectionTask()
{
	// 循环（一直再监听）
	for (; ;)
	{
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock); // 队列不空，生产线程进入条件变量的等待队列
		}

		// 连接数量没有到达上限，继续创建新的连接
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);

			p->refreshAliveTime();	// 刷新一下开始空闲的起始时间
			_connectionQue.push(p);
			_connectionCnt++;
		}

		// 唤醒等待队列里的所有线程
		cv.notify_all();	// 通知消费者线程， 可以消费连接了
	}
}

// 给外部提供接口，提供一个空闲的连接
shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty())
	{
		// 不是直接sleep
		// 超时醒来发现还是空的，就返回nullptr
		if (cv_status::timeout == cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeOut)))
		{
			if (_connectionQue.empty())
			{
				LOG("Getting connection timed out...get connection failed !");
				return nullptr;
			}
		}
	}

	/*
	由于shared_ptr析构的时候，会把Connection的资源直接delete掉，
	相当于调用了Connection的析构函数，Connection就被close掉了，
	所以这里需要自定义智能指针释放资源的方式，改用将资源还到队列里
	*/
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection* pcon) {
			// 这里是在服务器应用线程中调用的，需要考虑线程安全
			unique_lock<mutex> lock(_queueMutex);

			pcon->refreshAliveTime();	// 刷新一下开始空闲的起始时间
			_connectionQue.push(pcon);
		});
	_connectionQue.pop();

	// 消费完队列里最后一个Connection，就通知生产者线程生产连接
	cv.notify_all(); 
	return sp;
}

// 扫描超过maxIdleTime的空闲连接，进行多余的连接回收
void ConnectionPool::scannerConnectionTask()
{
	for (; ;)
	{
		// 通过sleep模拟定时效果
		this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));

		// 扫描整个队列，释放多于连接
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveTime() >= (_maxIdleTime * 1000))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p;	// 调用 ~Connection();
			}
			else
			{
				break; // 队头的连接没有超过_maxIdleTime，后面的肯定没有
			}
		}
	}
}