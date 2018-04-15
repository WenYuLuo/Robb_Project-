#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cxmloperator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_insert_button_clicked();

    void on_save_button_clicked();

private:
    Ui::MainWindow *ui;
    CXMLOperator xml_op;
};

#endif // MAINWINDOW_H
