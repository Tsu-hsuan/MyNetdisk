#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();
    void init();
    ~OpeDB();

    bool handleRegist(const char *name, const char *pwd);
    bool handleLogin(const char *name, const char *pwd);
    void handleOffline(const char *name);

    bool handleDeleteUsr(const char *name,const char *pwd);

    QStringList handleAllOnline();

    void handleFirstBootOffine();

    int handleSearchUsr(const char *name);


    int handAddFriend(const char * perName,const char * name);

    void handleAgreeAddFriend(const char * perName,const char * name);

    bool handledelFriend(const char * name,const char * friendName);

    QStringList handFlushFriend(const char * name);

signals:

private slots:

private:
    QSqlDatabase m_db;   //

};

#endif // OPEDB_H
