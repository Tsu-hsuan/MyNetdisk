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

#define DELETEUSER_SUCCESS "Deleteuser success"
#define DELETEUSER_FAILED "Deleteuser failed : name or password error"

#define SEARCH_USR_NO "no such people!"
#define SEARCH_USR_ONLINE "online!"
#define SEARCH_USR_OFFLINE "offine!"

#define UNKNOWN_ERROR "unknown error!"
#define FRIEND_EXISTS "friend exists!"
#define USER_NOT_EXISTS "user not exists!"
#define USER_OFFINE "user offine!"

#define DEL_FRIEND_OK "delete friend ok!"

enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,

    ENUM_MSG_TYPE_REGIST_REQUEST,       // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,       // 注册回复

    ENUM_MSG_TYPE_LOGIN_REQUEST,        // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,        // 登录回复

    ENUM_MSG_TYPE_DELETEUSER_REQUEST,   // 注销回复
    ENUM_MSG_TYPE_DELETEUSER_RESPOND,   // 注销回复

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,   // 请求所有在线用户
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,   // 请求所有在线用户的回复

    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,   // 搜索请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,   // 搜索回复

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   // 添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   // 添加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,   // 同意好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,   // 拒绝好友请求

    ENUM_MSG_TYPE_FRIEND_FLUSH_REQUEST,   // 刷新好友请求
    ENUM_MSG_TYPE_FRIEND_FLUSH_RESPOND,   // 刷新好友回复

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,   // 删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,   // 删除好友复制

    ENUM_MSG_TYPE_MAX = 0x00ffffff, // uint最大值 0xffffffff
};

struct PDU
{
    uint uiPDULen;       // 总的协议数据单元大小
    uint uiMsgType;      // 消息类型
    char caData[64];     // 其他数据
    uint uiMsgLen;       // 实际消息长度
    int caMsg[];         // 实际消息，主要通过caMsg访问消息数据
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
