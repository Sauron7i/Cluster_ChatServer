#include "groupModel.hpp"
#include "dbConnectionPool.h"

extern ConnectionPool* sp;

// 创建群组
bool GroupModel::createGroup(Group& group)
{
    // 1.组装sql语句
    char sql[1024] = { 0 };
    sprintf(sql, "insert into allproup(groupname, groupdesc) values('%s', '%s')",
        group.getName().c_str(), group.getDesc().c_str());

    // 连接数据库
    std::shared_ptr<Connection> mysql = sp->getConnection();
    if (mysql->update(sql))
    {
        group.setId(mysql_insert_id(mysql->getConn()));
        return true;
    }

    return false;
}

// 加入群组
void GroupModel::addGroup(int userid, int groupid, string role)
{
    char sql[1024] = { 0 };
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')", groupid, userid, role.c_str());

    std::shared_ptr<Connection> mysql = sp->getConnection();
    mysql->update(sql);
}

// 查询用户所在群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    /*
    1. 先根据userid在groupuser表中查询出该用户所属的群组信息
    2. 再根据群组信息，查询属于改群组的所有用户的userid，
    3. 并且和user表进行多表联合查询，查出用户的详细信息
    */
   char sql[1024] = { 0 };
   sprintf(sql, "select a.id, a.groupname, a.groupdesc from allgroup a inner join \
        groupuser b on a.id = b.groupid where b.userid = %d", userid);

    vector<Group> groupVec;

    std::shared_ptr<Connection> mysql = sp->getConnection();
    MYSQL_RES* res = mysql->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        // 查出userid所有的群组信息
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            Group group;
            group.setId(atoi(row[0]));
            group.setName(row[1]);
            group.setDesc(row[2]);
            groupVec.emplace_back(group);
        }
        mysql_free_result(res);
    }

    // 查询群组的用户信息
    for (Group& group : groupVec)
    {
        sprintf(sql, "select a.id, a.name, a.state, b.grouprole from user a inner \
            join groupuser b on b.userid = a.id where b.groupid = %d", group.getId());

        MYSQL_RES* res = mysql->query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);

                group.getUsers().emplace_back(user);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}

// 根据指定的groupid查询群组用户id列表，除了userid自己，
// 主要用户群聊业务给群组其他成员群发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = { 0 };
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);

    vector<int> idVec;
    std::shared_ptr<Connection> mysql = sp->getConnection();

    MYSQL_RES* res = mysql->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            idVec.emplace_back(atoi(row[0]));
        }
        mysql_free_result(res);
    }

    return idVec;
}