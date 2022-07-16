#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include "dbConnectionPool.h"
#include <string>
#include <vector>
using namespace std;

extern ConnectionPool* sp;

// 提供离线消息表的操作接口方法
class OfflineMsgModel
{
public:
    // 存储用户的离线消息
    void insert(int userId, string msg);
    // 删除用户的离线消息
    void remove(int userId);

    // 查询用户的离线消息
    vector<string> query(int userId);
};

#endif