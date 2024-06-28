#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("C:\\MyProject\\MyNetdisk-main\\TcpServer\\cloud.db");
    m_db.setDatabaseName("C:\\Users\\19182\\Desktop\\NetDisk\\MyNetDisk\\TcpServer\\cloud.db");
    if (m_db.open())
    {
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while(query.next())
        {
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(NULL, "打开数据库", "打开数据库失败");
    }
}

OpeDB::~OpeDB()
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    if (NULL == name || NULL == pwd)
    {
        qDebug() << "name | pwd is NULL";
        return false;
    }

    QString data = QString("insert into usrInfo(name, pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    qDebug() << data;
    QSqlQuery query;
    return query.exec(data);
}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if (NULL == name || NULL == pwd)
    {
        qDebug() << "name | pwd is NULL";
        return false;
    }
    QString data = QString("select * from usrInfo where name = \'%1\' and pwd = \'%2\' and online=0 ").arg(name).arg(pwd);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    if (query.next())
    {
        data = QString("update usrInfo set online=1 where name = \'%1\' and pwd = \'%2\'").arg(name).arg(pwd);
        qDebug() << data;
        QSqlQuery query;
        query.exec(data);

        return true;
    }
    else
    {
        return false;
    }
}

void OpeDB::handleOffline(const char *name)
{
    if (NULL == name)
    {
        qDebug() << "name is NULL";
        return;
    }
    QString data = QString("update usrInfo set online=0 where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
}

bool OpeDB::handleDeleteUsr(const char *name,const char *pwd)
{
    if (NULL == name || NULL ==pwd){
        return false;
    }
    QString data = QString("select * from usrInfo where name = \'%1\' and pwd = \'%2\' and online = 0").arg(name).arg(pwd);
    qDebug() << data ;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        data = QString("delete from friend where id = (select id from usrInfo where name = \'%1\')"
                       "or friendId = (select id from usrInfo where name = \'%2\')").arg(name).arg(name);
        qDebug() << data ;
        query.exec(data);

        data = QString("delete from usrInfo where name = \'%1\' ").arg(name);
        qDebug() << data ;
        query.exec(data);

    }else{
        return false;
    }
    return true;
}

QStringList OpeDB::handleAllOnline()
{
    QString data = QString("select name from usrInfo  where online = 1");
    qDebug() << data ;
    QSqlQuery query;
    query.exec(data);
    QStringList result;
    result.clear();
    while(query.next()){
        result.append(query.value(0).toString());
    }
    return result;
}

void OpeDB::handleFirstBootOffine()
{
    QString data = QString("update usrInfo set online = 0 where 1=1");
    QSqlQuery query;
    query.exec(data);
}

int OpeDB::handleSearchUsr(const char *name)
{
    if(NULL==name){
        return -1;
    }
    QString data = QString("select online from usrInfo where name =\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        int res = query.value(0).toInt();
        if(1==res){
            return 1;
        }else if (0==res){
            return 0;
        }else{
            return -1;
        }
    }else{
        return -1;
    }

}
int OpeDB::handAddFriend(const char *perName, const char *name)
{
    if(NULL==perName || name==NULL){
        return -1;
    }
    QString data = QString("select * from friend where (id = select id from userInfo where name = \'%1\' and friendId = (select id from userInfo where name = \'%2\'))'))"
                           " or (id = select id from userInfo where name = \'%3\' and friendId = (select id from userInfo where name = \'%4\'))')").arg(perName).arg(name).arg(name).arg(perName);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        return 0;  // 双方已是好友
    }
    else{
        QString data = QString("select online from userInfo  where name =\'%1\'").arg(name);
        QSqlQuery query;
        query.exec(data);
        if(query.next()){
            int res = query.value(0).toInt();
            if(1==res){
                return 1;   // 在线
            }else if (0==res){
                return 2;   // 不在线
            }else{
                return 3;   // 不存在
            }
        }else{
            return 3;
        }
    }


}

void OpeDB::handleAgreeAddFriend(const char *perName, const char *name)
{
    if(NULL==perName || NULL == name){
        return ;
    }
    QString data = QString("insert into friend(id,friendId) values((select id from userInfo where name = \'%1\'),"
                           "(select id from userInfo where name = \'%2\'));").arg(perName).arg(name);
    QSqlQuery query;
    query.exec(data);
}

bool OpeDB::handledelFriend(const char *name, const char *friendName)
{
    if(NULL==name || friendName == NULL )
    {
        return false;
    }
    QString data = QString("delete from friend where id = ((select id from userInfo where name = \'%1\') and "
                           "friendId = (select id from userInfo where name = \'%2\'));").arg(name).arg(friendName);
    QSqlQuery query;
    query.exec(data);

    data = QString("delete from friend where id = ((select id from userInfo where name = \'%1\') and "
                   "friendId = (select id from userInfo where name = \'%2\'));").arg(friendName).arg(name);
    query.exec(data);

    return true;
}

QStringList OpeDB::handFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if(NULL==name){
        return strFriendList;
    }
    QString data1= QString("select name from userInfo where online = 1 and id in"
                            "(select id from friend where friendId = (select id from userInfo "
                            "where name = \'%1\'))").arg(name);
    qDebug() << data1;
    QSqlQuery query1;
    query1.exec(data1);
    while(query1.next()){
        strFriendList.append(query1.value(0).toString());
        qDebug() << query1.value(0).toString();
    }

    QString data2 = QString("select name from userInfo where online = 1 and id in"
                            "(select friendId from friend where id = (select id from userInfo "
                            "where name = \'%1\'))").arg(name);
    QSqlQuery query2;
    qDebug() << data2;
    query2.exec(data2);
    while(query2.next()){
        strFriendList.append(query2.value(0).toString());
        qDebug() << query2.value(0).toString();
    }
    return strFriendList;
}
