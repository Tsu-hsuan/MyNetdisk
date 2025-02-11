#include "mytcpsocket.h"
#include <QDebug>
#include<stdio.h>
#include"mytcpserver.h"
#include "protocol.h"
#include <QFileInfoList>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this, SIGNAL(readyRead())
            , this, SLOT(recvMsg()));
    connect(this, SIGNAL(disconnected())
            , this, SLOT(clientOffline()));
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(sendFileToClient()));

}

QString MyTcpSocket::getName()
{
    return m_strName;
}



void MyTcpSocket::recvMsg()
{
    qDebug() << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char *)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGIST_REQUEST:
    {
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
        if (ret)
        {
            strcpy(respdu->caData, REGIST_OK);
            m_strName = caName;
        }
        else
        {
            strcpy(respdu->caData, REGIST_FAILED);
        }
        write((char*)respdu, respdu->uiPDULen);

        free(respdu);
        respdu = NULL;
        break;
    }

    case ENUM_MSG_TYPE_LOGIN_REQUEST:
    {
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret = OpeDB::getInstance().handleLogin(caName, caPwd);
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
        if (ret)
        {
            strcpy(respdu->caData, LOGIN_OK);
            m_strName = caName;
        }
        else
        {
            strcpy(respdu->caData, LOGIN_FAILED);
        }
        write((char*)respdu, respdu->uiPDULen);

        free(respdu);
        respdu = NULL;
        break;
    }

    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
    {
        QStringList res = OpeDB::getInstance().handleAllOnline();
        uint uiMsgLen = res.size()*32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
        for(int i=0;i<res.size();i++){
            memcpy((char*)respdu->caMsg+i*32
                   ,res.at(i).toStdString().c_str()
                   ,res.at(i).size());
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
    {
        int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if(-1 == ret){
            strcpy(respdu->caData,SEARCH_USR_NO);
        }
        else if(1 == ret){
            strcpy(respdu->caData,SEARCH_USR_ONLINE);
        }
        else if(0 == ret){
            strcpy(respdu->caData,SEARCH_USR_OFFLINE);
        }
        write((char*)respdu, respdu->uiPDULen);

        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        int res = OpeDB::getInstance().handAddFriend(caPerName,caName);
        PDU *respdu = NULL;
        if(-1==res){
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            strcpy(respdu->caData,UNKNOWN_ERROR);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }else if(0==res){
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            strcpy(respdu->caData,FRIEND_EXISTS);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }else if(1==res){
            MyTcpServer::getInstance().resend(caPerName,pdu);
        }else if(2==res){
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            strcpy(respdu->caData,USER_OFFINE);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }else if(3==res){
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            strcpy(respdu->caData,USER_NOT_EXISTS);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }else{
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            strcpy(respdu->caData,UNKNOWN_ERROR);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
    {
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        OpeDB::getInstance().handleAgreeAddFriend(caPerName,caName);
        MyTcpServer::getInstance().resend(caName,pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
    {
        char caName[32]= {'\0'};
        strncpy(caName,pdu->caData+32,32);
        MyTcpServer::getInstance().resend(caName,pdu);
        break;
    }
    case ENUM_MSG_TYPE_FRIEND_FLUSH_REQUEST:
    {
        char caName[32] = {'\0'};
        strncpy(caName,pdu->caData,32);
        QStringList res = OpeDB::getInstance().handFlushFriend(caName);
        uint uiMsgLen = res.size() * 32 ;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType = ENUM_MSG_TYPE_FRIEND_FLUSH_RESPOND;
        for(int i=0;i<res.size();i++){
            memcpy((char*)(respdu->caMsg)+i*32,res.at(i).toStdString().c_str(),res.at(i).size());
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
    {
        char selfName[32] = {'\0'};
        char friendName[32] = {'\0'};
        strncpy(selfName,pdu->caData,32);
        strncpy(friendName,pdu->caData+32,32);
        OpeDB::getInstance().handledelFriend(selfName,friendName);

        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
        strcpy(respdu->caData,DEL_FRIEND_OK);
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;

        MyTcpServer::getInstance().resend(friendName,pdu);

        break;
    }

    default:
        break;
    }


    free(pdu);
    pdu = NULL;

    // qDebug()<<caName<< caPwd<< pdu->uiMsgType;
}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}
