#ifndef CSQLOPT_H
#define CSQLOPT_H

#include <QtSql>
#include <QString>


struct PreTable1
{
    QString LEVEL;
    QString TIME; //不接受空值
    QString DISTRICT;
    QString DURATION;
    QString LOC;
    QString STATUS;
    QString ALARMTYPE;
    QString LOCDESCRIBE;
    QString DEALSUPPOSE;
    QString PHASEDATA;
    QString WRFLAG;
    QString SAMPTIME;
    QString TJTIME;
    QString TJLOC;
    QString AUDIOPATH;
    QString MSTZZ;
    QString MSTYPE;
    QString RATIONW;
    QString RATIONH;
    PreTable1():LEVEL(),TIME(),DISTRICT(),DURATION(),
      LOC(),STATUS(),ALARMTYPE(),LOCDESCRIBE(),DEALSUPPOSE(),
        PHASEDATA(),WRFLAG(),SAMPTIME(),TJTIME(),TJLOC(),
        AUDIOPATH(),MSTZZ(),MSTYPE(),RATIONW(),RATIONH() {}
    bool TimeIsEmpty()
    {
        if (TIME.isNull())
        {
            return true;
        }
        return false;
    }
};


class CSQLopt
{
public:
    CSQLopt() {}
    ~CSQLopt();
//    template<typename T, typename ... Args>
//    virtual bool ConectSQL(T first, Args ... args)=0;
    template<typename T, typename F, typename ... Args>
    bool InsertData(T head, F value, Args ... args)
    {
        PreTable1 pretable;
        ParseArguBag(pretable, head, value, args...);
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


    bool InsertData(PreTable1& pretable);

    //删除ID=value所在的行
    template<typename T>
    bool DeleteData(QString ID, T value)
    {
        QSqlQuery sql_query;
        QString qs_sql;

        qs_sql = QString("DELETE FROM Pretable1 WHERE %1 = %2").arg(ID).arg(value);
        qDebug()<<qs_sql;
        if(!sql_query.exec(qs_sql))
        {
            qDebug()<< "delete_sql_Pretable1 " << sql_query.lastError();
            return false;
        }
        return true;
    }

    //将 src=src_value 所在行的 dest 更新为 dest_value
    template<typename T, typename F>
    bool UpdateData(QString src, T src_value, QString dest, F dest_value)
    {
        QSqlQuery sql_query;
        QString qs_sql = QString("");

        qs_sql += "UPDATE Pretable1 SET " + dest + " = " + dest_value + " WHERE " + src + " = " + src_value;
        qDebug() << qs_sql;

        sql_query.prepare(qs_sql);

        if(!sql_query.exec())
        {
            qDebug()<<sql_query.lastError();
            return false;
        }

        return true;
    }


    //查询 column_name 列的数据
    virtual bool SelectData(QString column_name, QSqlQuery& sql_query);


protected:
    QSqlDatabase m_database;

};


class CSQLserver:public CSQLopt
{
public:
    CSQLserver():CSQLopt() {}
    ~CSQLserver();
    bool ConnectSQL(QString server, QString database, QString UID, QString PWD);
    bool InsertData(QString date_time, QString loc, QString alarmtype, QString audio_save_path);
};

bool CopyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);

template<typename T, typename F, typename ... Args>
void ParseArguBag(PreTable1 &pretable, T head, F value, Args ... args)
{
    if(head==QString("LEVEL"))
    {
        pretable.LEVEL = value;
    } else if (head==QString("TIME"))
    {
        pretable.TIME = value;
    } else if (head==QString("DISTRICT"))
    {
        pretable.DISTRICT = value;
    } else if (head==QString("DURATION"))
    {
        pretable.DURATION = value;
    } else if (head==QString("LOC"))
    {
        pretable.LOC = value;
    } else if (head==QString("STATUS"))
    {
        pretable.STATUS = value;
    } else if (head==QString("ALARMTYPE"))
    {
        pretable.ALARMTYPE = value;
    } else if (head==QString("LOCDESCRIBE"))
    {
        pretable.LOCDESCRIBE = value;
    } else if (head==QString("DEALSUPPOSE"))
    {
        pretable.DEALSUPPOSE = value;
    } else if (head==QString("PHASEDATA"))
    {
        pretable.PHASEDATA = value;
    } else if (head==QString("WRFLAG"))
    {
        pretable.WRFLAG = value;
    } else if (head==QString("SAMPTIME"))
    {
        pretable.SAMPTIME = value;
    } else if (head==QString("TJTIME"))
    {
        pretable.TJTIME = value;
    } else if (head==QString("TJLOC"))
    {
        pretable.TJLOC = value;
    } else if (head==QString("AUDIOPATH"))
    {
        pretable.AUDIOPATH = value;
    } else if (head==QString("MSTZZ"))
    {
        pretable.MSTZZ = value;
    } else if (head==QString("MSTYPE"))
    {
        pretable.MSTYPE = value;
    } else if (head==QString("RATIONW"))
    {
        pretable.RATIONW = value;
    } else if (head==QString("RATIONH"))
    {
        pretable.RATIONH = value;
    }
    ParseArguBag(pretable, args...);
}



//递归终止
template<typename T, typename F>
void ParseArguBag(PreTable1 &pretable, T head, F value)
{
    if(head==QString("LEVEL"))
    {
        pretable.LEVEL = value;
    } else if (head==QString("TIME"))
    {
        pretable.TIME = value;
    } else if (head==QString("DISTRICT"))
    {
        pretable.DISTRICT = value;
    } else if (head==QString("DURATION"))
    {
        pretable.DURATION = value;
    } else if (head==QString("LOC"))
    {
        pretable.LOC = value;
    } else if (head==QString("STATUS"))
    {
        pretable.STATUS = value;
    } else if (head==QString("ALARMTYPE"))
    {
        pretable.ALARMTYPE = value;
    } else if (head==QString("LOCDESCRIBE"))
    {
        pretable.LOCDESCRIBE = value;
    } else if (head==QString("DEALSUPPOSE"))
    {
        pretable.DEALSUPPOSE = value;
    } else if (head==QString("PHASEDATA"))
    {
        pretable.PHASEDATA = value;
    } else if (head==QString("WRFLAG"))
    {
        pretable.WRFLAG = value;
    } else if (head==QString("SAMPTIME"))
    {
        pretable.SAMPTIME = value;
    } else if (head==QString("TJTIME"))
    {
        pretable.TJTIME = value;
    } else if (head==QString("TJLOC"))
    {
        pretable.TJLOC = value;
    } else if (head==QString("AUDIOPATH"))
    {
        pretable.AUDIOPATH = value;
    } else if (head==QString("MSTZZ"))
    {
        pretable.MSTZZ = value;
    } else if (head==QString("MSTYPE"))
    {
        pretable.MSTYPE = value;
    } else if (head==QString("RATIONW"))
    {
        pretable.RATIONW = value;
    } else if (head==QString("RATIONH"))
    {
        pretable.RATIONH = value;
    }
}



#endif // CSQLOPT_H
