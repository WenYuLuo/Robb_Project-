#include "cxmloperator.h"
//#include

CXMLOperator::CXMLOperator(QObject *parent) : QObject(parent)
{
    const char* declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    m_doc = new XMLDocument();
    m_doc->Parse(declaration);
    XMLElement* root=m_doc->NewElement("WellShutter");
    m_doc->InsertEndChild(root);
}

CXMLOperator::~CXMLOperator()
{
    if(m_doc!=NULL)
    {
        delete m_doc;
        m_doc = NULL;
    }
}

bool CXMLOperator::InsertXML(QString code, QString loc, QString status)
{
    if (m_doc==NULL) return false;
    XMLElement* root = m_doc->RootElement();
    XMLElement* code_node = m_doc->NewElement("info");
    code_node->SetAttribute("code", code.toStdString().c_str());
    code_node->SetAttribute("loc", loc.toStdString().c_str());
    code_node->SetAttribute("alarmtype", status.toStdString().c_str());
    root->InsertEndChild(code_node);
    return true;
}

bool CXMLOperator::SaveXML(QString save_path)
{
    if(m_doc==NULL) return false;
    if(!m_doc->SaveFile(save_path.toStdString().c_str()))
        return true;
    else
        return false;
}
