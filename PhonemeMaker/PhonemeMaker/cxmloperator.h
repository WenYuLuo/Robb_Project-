#ifndef CXMLOPERATOR_H
#define CXMLOPERATOR_H

#include <QObject>
#include "tinyxml2.h"
using namespace tinyxml2;

class CXMLOperator : public QObject
{
    Q_OBJECT
public:
    explicit CXMLOperator(QObject *parent = nullptr);
    ~CXMLOperator();

    bool InsertXML(QString code, QString loc, QString status);

    bool SaveXML(QString save_path);


private:
    XMLDocument* m_doc = NULL;
signals:
    void debug_info(QString);

public slots:
};

#endif // CXMLOPERATOR_H
