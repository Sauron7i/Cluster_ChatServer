#include "offlineMessageModel.hpp"

// 存储用户的离线消息
void OfflineMsgModel::insert(int userId, string msg)
{
    char sql[1024] = { 0 };
    sprintf(sql, "insert into offlinemessage values(%d, '%s')", userId, msg.c_str());

    std::shared_ptr<Connection> mysql = sp->getConnection();
    mysql->update(sql);

}

// 删除用户的离线消息
void OfflineMsgModel::remove(int userId)
{
    char sql[1024] = { 0 };
    sprintf(sql, "delete from offlinemessage where userid = %d", userId);

    std::shared_ptr<Connection> mysql = sp->getConnection();
    mysql->update(sql);
}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userId)
{
    char sql[1024] = { 0 };
    sprintf(sql, "select message from offlinemessage where userid = %d", userId);

    vector<string> nums;
    std::shared_ptr<Connection> mysql = sp->getConnection();

    MYSQL_RES* res = mysql->query(sql);
    if (res != nullptr)
    {
        // 把userId的所有离线消息写入nums中
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            nums.emplace_back(row[0]);
        }

        mysql_free_result(res);
        return nums;
    }
    return nums;
}
