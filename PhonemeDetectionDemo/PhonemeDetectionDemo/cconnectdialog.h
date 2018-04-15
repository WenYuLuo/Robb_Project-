#ifndef CCONNECTDIALOG_H
#define CCONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class CConnectDialog;
}

class CConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CConnectDialog(QWidget *parent = 0);
    ~CConnectDialog();
signals:
    void connect_signal(QString, QString, QString, QString);
private slots:
    void on_connect_Button_clicked();

    void on_cancel_button_clicked();

private:
    Ui::CConnectDialog *ui;
};

#endif // CCONNECTDIALOG_H
