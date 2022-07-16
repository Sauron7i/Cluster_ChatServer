#include "friendModel.hpp"
#include "dbConnectionPool.h"

extern ConnectionPool* sp;

 // 添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);

    std::shared_ptr<Connection> mysql = sp->getConnection();
    mysql->update(sql);
}

// 返回用户好友列表 两个表的联合查询(friend user)
vector<User> FriendModel::query(int userid)
{
    char sql[1024] = { 0 };
    sprintf(sql, "select a.id, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d", userid);

    std::shared_ptr<Connection> mysql = sp->getConnection();
    vector<User> userNums;

    MYSQL_RES* res = mysql->query(sql);
    if (res != nullptr)
    {
        //
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setState(row[2]);

            userNums.emplace_back(user);
        }
        mysql_free_result(res);
        return userNums;
    }
    return userNums;
}