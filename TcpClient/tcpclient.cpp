#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "protocol.h"
#include "opewidget.h"

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    resize(500,300);
    loadConfig();

    connect(&m_tcpSocket, SIGNAL(connected())
            ,this, SLOT(showConnect()));
    connect(&m_tcpSocket, SIGNAL(readyRead())
            ,this, SLOT(recvMsg()));

    //连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);


}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        qDebug() << strData;
        file.close();

        strData.replace("\r\n"," ");
        qDebug() << strData;

        QStringList strList = strData.split(" ");

        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "ip:" << m_strIP << "port:" << m_usPort;

    }
    else{
        QMessageBox::critical(this, "open config", "open config failed");
    }

}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::loginName()
{
    return  m_strLoginName;
}

QString TcpClient::curPath()
{
    return m_strCurPath;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this, "连接服务器","连接服务器成功");
}

void TcpClient::recvMsg()
{
    qDebug() << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char *)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_LOGIN_RESPOND:
    {
        if (0 == strcmp(pdu->caData, LOGIN_OK))
        {
            QMessageBox::information(this,"登录",LOGIN_OK);
            OpeWidget::getInstance().show();
            hide();
        }
        else if (0 == strcmp(pdu->caData, LOGIN_FAILED))
        {
            QMessageBox::warning(this,"登录",LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_DELETEUSR_RESPOND:
    {
        QMessageBox::information(this,"注销用户",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);

        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
    {
        if(0==strcmp(SEARCH_USR_NO,pdu->caData)){
            QMessageBox::information(this,"搜索",QString("%1: not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }else if (0==strcmp(SEARCH_USR_ONLINE,pdu->caData)){
            QMessageBox::information(this,"搜索",QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }else if (0==strcmp(SEARCH_USR_OFFLINE,pdu->caData)){
            QMessageBox::information(this,"搜索",QString("%1: offine").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        OpeWidget::getInstance().getFriend()->m_strSearchName = "";
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        char caName[32]={'\0'};
        strncpy(caName,pdu->caData+32,32);
        int res = QMessageBox::information(this,"添加好友",QString("%1 want to add you as friend!").arg(caName)
                                           ,QMessageBox::Yes,QMessageBox::No);
        PDU *respdu = mkPDU(0);
        memcpy(respdu->caData,pdu->caData,32);
        if(QMessageBox::Yes==res){
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
        }else{
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE ;
        }
        m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu =NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"添加好友",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_FRIEND_FLUSH_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
    {
        char caName[32] = {'\0'};
        memcpy(caName,pdu->caData,32);
        QMessageBox::information(this,"删除好友",QString("%1 已将你删除").arg(caName));
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"删除好友","删除好友成功");
        break;
    }
    default:
        break;
    }

    free(pdu);
    pdu = NULL;
}


void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录","登录失败：用户名或密码为空");
    }
}


void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册","注册失败：用户名或密码为空");
    }
}


void TcpClient::on_cancel_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty()&& !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETEUSR_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);   // 数组前32位放用户名
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32); // 数组后32位放密码
        qDebug() << pdu->caData;
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::critical(this,"注销用户","注销失败，用户名或用户密码为空！");
    }
}

