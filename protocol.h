#ifndef PROTOCOL_H
#define PROTOCOL_H
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
typedef unsigned int uint;

#define REGIST_SUCCESS "regist_success"
#define REGIST_FAILED "regist_failed : name existed"
#define LOGIN_SUCCESS "login success"
#define LOGIN_FAILED "login failed : name or password error or relogin"

#define SEARCH_USR_NO "no such people!"
#define SEARCH_USR_ONLINE "online!"
#define SEARCH_USR_OFFLINE "offine!"

#define UNKNOWN_ERROR "unknown error!"
#define FRIEND_EXISTS "friend exists!"
#define USER_NOT_EXISTS "user not exists!"
#define USER_OFFINE "user offine!"

#define DEL_FRIEND_OK "delete friend ok!"

#define DIR_NO_EXISTS "dir not exist"
#define FILE_NAME_EXISTS "file name exist"
#define CREATE_DIR_SUCCESS "create dir success"

#define DEL_DIR_SUCCESS "delete dir success"
#define DEL_DIR_FAILED "delete dir failed"

#define RENAME_SUCCESS "rename success"
#define RENAME_FAILED "rename failed"

#define ENTER_DIR_FAILED "enter dir failed"
#define ENTER_DIR_SUCCESS "enter dir success"

#define DEL_FILE_SUCCESS "delete file success"
#define DEL_FILE_FAILED "delete file failed"

#define UPLOAD_FILE_SUCCESS "upload file success"
#define UPLOAD_FILE_FAILED "upload file failed"

// 消息类型写成枚举形式

enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGIST_REQUEST,   // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,   // 注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST,   // 登录回复
    ENUM_MSG_TYPE_LOGIN_RESPOND,   // 登录回复

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,   // 请求所有在线用户
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,   // 请求所有在线用户的回复


    ENUM_MSG_TYPE_SEARCH_USER_REQUEST,   // 搜索请求
    ENUM_MSG_TYPE_SEARCH_USER_RESPOND,   // 搜索回复

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   // 添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   // 添加好友回复

    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,   // 同意好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,   // 拒绝好友请求


    ENUM_MSG_TYPE_FRIEND_FLUSH_REQUEST,   // 刷新好友请求
    ENUM_MSG_TYPE_FRIEND_FLUSH_RESPOND,   // 刷新好友回复

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,   // 删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,   // 删除好友复制


    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,   // 私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,   // 私聊回复

    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,   // 群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,   // 群聊回复

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,   // 创建目录请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,   // 创建目录回复

    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,   // 刷新文件请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,   // 刷新文件回复

    ENUM_MSG_TYPE_DEL_DIR_REQUEST,   // 删除文件夹请求
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,   // 删除文件夹回复

    ENUM_MSG_TYPE_RENAME_DIR_REQUEST,   // 重命名请求
    ENUM_MSG_TYPE_RENAME_DIR_RESPOND,   // 重命名回复

    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,   // 进入文件夹请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,   // 进入文件夹回复

    ENUM_MSG_TYPE_DEL_FILE_REQUEST,   // 删除文件请求
    ENUM_MSG_TYPE_DEL_FILE_RESPOND,   // 删除文件回复

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,   // 上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,   // 上传文件回复

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,   // 下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,   // 下载文件回复

    ENUM_MSG_TYPE_MAX=0x00ffffff,
};

struct FileInfo {
    char caName[32];  // 文件名称
    int iFileType;   // 文件类型
};

struct PDU {
    uint uiPDULen;          // 总的协议数据单元大小
    uint uiMsgType;         // 消息类型
    char caData[64];
    uint uiMsgLen;          //实际消息长度
    int caMsg[];            // 实际消息
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
