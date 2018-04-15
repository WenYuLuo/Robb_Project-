#include "cconnectdialog.h"
#include "ui_cconnectdialog.h"

CConnectDialog::CConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CConnectDialog)
{
    ui->setupUi(this);
    ui->server_lineEdit->setText("ROBB-LUO\\SQLEXPRESS");
    ui->database_lineEdit->setText("newdb02");
    ui->uid_lineEdit->setText("robb");
    ui->pwd_lineEdit->setText("123456");
    ui->server_lineEdit->setAlignment(Qt::AlignCenter);
    ui->database_lineEdit->setAlignment(Qt::AlignCenter);
    ui->uid_lineEdit->setAlignment(Qt::AlignCenter);
    ui->pwd_lineEdit->setAlignment(Qt::AlignCenter);
    connect(this, SIGNAL(connect_signal(QString,QString,QString,QString)), parent, SLOT(connect_sql(QString, QString, QString, QString)));
}

CConnectDialog::~CConnectDialog()
{
    delete ui;
}

void CConnectDialog::on_connect_Button_clicked()
{
    QString server = ui->server_lineEdit->text();
    QString database = ui->database_lineEdit->text();
    QString uid = ui->uid_lineEdit->text();
    QString pwd = ui->pwd_lineEdit->text();
    if(!(server.isNull()||database.isNull()||uid.isNull()||pwd.isNull()))
    {
        emit connect_signal(server, database, uid, pwd);
    }
    this->close();
}

void CConnectDialog::on_cancel_button_clicked()
{
    this->close();
}
