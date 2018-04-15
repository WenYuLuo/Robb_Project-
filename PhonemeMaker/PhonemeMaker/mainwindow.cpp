#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    xml_op(NULL)
{
    ui->setupUi(this);
    QStringList isopen_list;
    isopen_list<<"开"<<"关";
    ui->maker_comboBox->addItems(isopen_list);

    QRegExp rx("[0-9A-F]{2}|[0-9A-F]{2}|[0-9A-F]{2}");
    ui->code_lineEdit->setValidator(new QRegExpValidator(rx, ui->code_lineEdit));
    ui->code_lineEdit->setInputMask("HH|HH|HH");
    ui->code_lineEdit->setAlignment(Qt::AlignCenter);
//    ui->code_lineEdit->setMaxLength(6);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_insert_button_clicked()
{
    QString code = ui->code_lineEdit->text();
    QString loc = ui->loc_lineEdit->text();
    QString status = ui->maker_comboBox->currentText();
    if(!(code.isEmpty()||loc.isEmpty()||status.isEmpty()))
    {
        if(xml_op.InsertXML(code, loc, status))
        {
            ui->debug_label->setText("插入成功！");
        }
        else
            ui->debug_label->setText("插入失败！");
    }
    else
        ui->debug_label->setText("无法插入！存在空项！");
}

void MainWindow::on_save_button_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            QString::fromLocal8Bit("文件另存为"),
            "",
            tr("Config Files (*.xml)"));
    if(!fileName.isNull())
    {
        if(xml_op.SaveXML(fileName))
        {
            ui->debug_label->setText("保存成功！");
        }
        else
            ui->debug_label->setText("保存失败！");

    }
}
