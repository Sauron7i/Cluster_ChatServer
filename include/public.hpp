#ifndef PUBLIC_H
#define PUBLIC_H

#include<iostream>
/*
 server和client的公共文件
*/

#define LOG(str) \
	std::cout << __FILE__ << " : " << __LINE__ << " " << \
	__TIMESTAMP__ << " : " << str << std::endl;

enum EnMsgType
{
    LOGIN_MSG = 1,  // 登录消息
    LOGIN_MSG_ACK,  // 登录响应
    REG_MSG,        // 注册消息
    REG_MSG_ACK,    // 注册响应
    ONE_CHAT_MSG,   // 聊天消息
    ADD_FRIEND_MSG, // 添加好友

    CREATE_GROUP_MSG,   // 创建群组
    ADD_GROUP_MSG,      // 加入群组
    GROUP_CHAT_MSG,      // 群聊天

    LOGINOUT_MSG     // 注销
};

#endif