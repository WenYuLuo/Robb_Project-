#include "csqlopt.h"


CSQLopt::~CSQLopt()
{
    if(m_database.isOpen())
    {
        m_database.close();
    }
}


bool CSQLopt::InsertData(PreTable1 &pretable)
{
    if(pretable.TimeIsEmpty())
    {
        qDebug() << QString("the TIME of insert data can't be empty!");
        return false;
    }
    QSqlQuery sql_query;
    QString sql_insert = QString("INSERT INTO Pretable1 (LEVEL, TIME, DISTRICT, DURATION, LOC, STATUS, ALARMTYPE, LOCDESCRIBE"
                                ",DEALSUPPOSE, PHASEDATA, WRFLAG, SAMPTIME, TJTIME, TJLOC, AUDIOPATH, MSTZZ, MSTYPE, RATIONW, RATIONH) VALUES "
                                "('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11', '%12', '%13', '%14', "
                                 "'%15', '%16', '%17', '%18', '%19')")
            .arg(pretable.LEVEL)
            .arg(pretable.TIME)
            .arg(pretable.DISTRICT)
            .arg(pretable.DURATION)
            .arg(pretable.LOC)
            .arg(pretable.STATUS)
            .arg(pretable.ALARMTYPE)
            .arg(pretable.LOCDESCRIBE)
            .arg(pretable.DEALSUPPOSE)
            .arg(pretable.PHASEDATA)
            .arg(pretable.WRFLAG)
            .arg(pretable.SAMPTIME)
            .arg(pretable.TJTIME)
            .arg(pretable.TJLOC)
            .arg(pretable.AUDIOPATH)
            .arg(pretable.MSTZZ)
            .arg(pretable.MSTYPE)
            .arg(pretable.RATIONW)
            .arg(pretable.RATIONH);
    qDebug() << sql_insert;
    if(!sql_query.exec(sql_insert))
    {
        qDebug() << QString("Insert error : ") << sql_query.lastError();
        return false;
    }
    return true;
}



bool CSQLopt::SelectData(QString colum_name, QSqlQuery& sql_query)
{
//    QSqlQuery sql_query;
    QString qs_sql = QString("");

    qs_sql = "SELECT " + colum_name + " FROM Pretable1";
    qDebug() << qs_sql;
    sql_query.prepare(qs_sql);
    if(!sql_query.exec())
    {
        qDebug()<< "Search " << sql_query.lastError();
        return false;
    }
    return true;
}

CSQLserver::~CSQLserver()
{
    if(m_database.isOpen()) {
        m_database.close();
    }
}

bool CSQLserver::ConnectSQL(QString server, QString database, QString uid, QString pwd)
{
    m_database = QSqlDatabase::addDatabase("QODBC");
    qDebug()<<"ODBC driver?"<<m_database.isValid();
    m_database.setDatabaseName(QString("DRIVER={SQL Server};"
                                       "SERVER=%1;"
                                       "DATABASE=%2;"
                                       "UID=%3;"
                                       "PWD=%4;")
                               .arg(server)
                               .arg(database)
                               .arg(uid)
                               .arg(pwd));
//    m_database.setDatabaseName("SQLServer");
//    m_database.setUserName("robb");
//    m_database.setPassword("123456");
    if(!m_database.open())
    {
        qDebug()<<QString("can't open database")<<m_database.lastError().databaseText();
        return false;
    } else
    {
        qDebug()<<QString("Connect to DataBase success!");
        return true;
    }
}

bool CSQLserver::InsertData(QString date_time, QString loc, QString alarmtype, QString audio_save_path)
{
    QSqlQuery sql_query;
    QString sql_insert = QString("INSERT INTO Pretable2 (TIME, ALARMTYPE, TJLOC, AUDIOPATH) VALUES "
                                "(CAST(STUFF(STUFF(STUFF('%1',9,0,' ' ),12,0,':'),15,0,':') AS DateTime), '%2', '%3', '%4')")
            .arg(date_time)
            .arg(alarmtype)
            .arg(loc)
            .arg(audio_save_path);
    qDebug() << sql_insert;
    if(!sql_query.exec(sql_insert))
    {
        qDebug() << QString("Insert error : ") << sql_query.lastError();
        return false;
    }
    return true;
}
