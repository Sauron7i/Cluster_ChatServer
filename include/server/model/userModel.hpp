#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"
#include "dbConnectionPool.h"

extern ConnectionPool* sp;

// User表的数据操作类
class UserModel {
public:
    // User表的增加操作
    bool insert(User& user);

    // 根据用户号码返回用户信息
    User query(int id);

    // 更新用户的状态信息
    bool updateState(User user);

    // 重置用户得状态信息
    void resetState();
};


#endif