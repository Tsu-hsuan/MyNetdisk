#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;
#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed: name exited"
#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed: name error or pwd error or relogin"

#define SEARCH_USR_NO "no such people!"
#define SEARCH_USR_ONLINE "online!"
#define SEARCH_USR_OFFLINE "offine!"

enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST,    //
    ENUM_MSG_TYPE_REGIST_RESPOND,    //

    ENUM_MSG_TYPE_LOGIN_REQUEST,    //
    ENUM_MSG_TYPE_LOGIN_RESPOND,    //

    ENUM_MSG_TYPE_DELETEUSR_REQUEST,   // 注销回复
    ENUM_MSG_TYPE_DELETEUSR_RESPOND,   // 注销回复

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,   // 请求所有在线用户
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,   // 请求所有在线用户的回复

    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,   // 搜索请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,   // 搜索回复

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   // 添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   // 添加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,   // 同意好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,   // 拒绝好友请求
    ENUM_MSG_TYPE_MAX = 0X00ffffff,
};

struct PDU
{
    uint uiPDULen;   //总的协议数据单元大小
    uint uiMsgType;  //消息类型
    char caData[64];
    uint uiMsgLen;   //
    int caMsg[];     //
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
