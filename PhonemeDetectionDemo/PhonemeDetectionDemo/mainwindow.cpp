#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tinyxml2.h"

using namespace tinyxml2;
QString g_last_debug;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ms_energy_ratio_box->setValue(0.35);
    ui->sub_ms_energy_ratio_box->setValue(5.0);
    ui->ms_2_energy_ratio_box->setValue(0.5);
    ui->max_timeval_div_box->setRange(30000,60000);
    ui->max_timeval_div_box->setValue(40000);
    ui->max_timeval_div_box->setSuffix(" /Fs");
    ui->max_timeval_div_box->setWrapping(true);
    connect(&m_thread_detect_phoneme, SIGNAL(processed_num(int)), this, SLOT(processed_num_show(int)));
    connect(&m_thread_detect_phoneme, SIGNAL(debug_show(QString)),this, SLOT(debug_info_show(QString)));
    connect(&m_thread_detect_phoneme, SIGNAL(num_audio_change(int)),this, SLOT(available_audio_change(int)));
    m_connect_dialog = new CConnectDialog(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    if(m_connect_dialog!=NULL)
    {
        delete m_connect_dialog;
        m_connect_dialog = NULL;
    }
}

bool MainWindow::ReadXML(QString xml_path)
{
    tinyxml2::XMLDocument doc;
    int errorID = doc.LoadFile(xml_path.toStdString().c_str());
//    const char* error_str = doc.ErrorStr();
    if(errorID!=0)
    {
        g_last_debug = QString("无法打开井盖标识XML文件！");
        debug_info_show(g_last_debug);
        return false;
    }
    XMLElement* root = doc.RootElement();
    XMLElement* userNode = root->FirstChildElement();
    int count = 0;
    while(userNode!=NULL)
    {
        count++;
        QString code = QString(userNode->Attribute("code"));
        QString loc = QString(userNode->Attribute("loc"));
        QString alarmtype = QString(userNode->Attribute("alarmtype"));
        WellShutter wellshutter{code, loc, alarmtype};
        wellshutter_vec.push_back(wellshutter);
        userNode = userNode->NextSiblingElement();
    }
    g_last_debug = QString("读取到%1条井盖标识数据").arg(count);
    debug_info_show(g_last_debug);
    return true;
}

void MainWindow::on_get_filepath_button_clicked()
{
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    m_directory = QFileDialog::getExistingDirectory(this,
                                tr("select directory"),
                                ui->get_filepath_edit->toPlainText(),
                                options);
    if (!m_directory.isEmpty())
    {
        ui->get_filepath_edit->setText(m_directory);
        ui->ms_2_energy_ratio_box->setEnabled(true);
        ui->sub_ms_energy_ratio_box->setEnabled(true);
        ui->ms_2_energy_ratio_box->setEnabled(true);
    }

}

void MainWindow::on_get_filepath_edit_textChanged()
{
    m_directory = ui->get_filepath_edit->toPlainText();
    QDir dir(m_directory);
    if (dir.exists())
    {
        QStringList filters;
        filters<<QString("*.wav");
        dir.setFilter(QDir::Files|QDir::NoSymLinks);
        dir.setNameFilters(filters);
        //统计wav文件个数
        int dir_count = dir.count();
        if (dir_count<=0)
        {
            g_last_debug = QString("路径中不存在.wav文件！");
            ui->debug_info_edit->append(g_last_debug);
            ui->ms_energy_ratio_box->setEnabled(false);
            ui->sub_ms_energy_ratio_box->setEnabled(false);
            ui->ms_2_energy_ratio_box->setEnabled(false);
            ui->audio_lcdnum->display(0);
            ui->audio_lcdnum->setEnabled(false);
            return;
        }

        else
        {
            ui->ms_energy_ratio_box->setEnabled(true);
            ui->sub_ms_energy_ratio_box->setEnabled(true);
            ui->ms_2_energy_ratio_box->setEnabled(true);
            ui->max_timeval_div_box->setEnabled(true);
            ui->audio_lcdnum->setEnabled(true);
            ui->audio_lcdnum->display(dir_count);
            ui->start_button->setEnabled(true);
        }
    }
    else
    {
        g_last_debug = QString("路径不存在！");
        ui->debug_info_edit->append(g_last_debug);
    }
}

void MainWindow::debug_info_show(QString debug)
{
    ui->debug_info_edit->append(debug);
}

void MainWindow::available_audio_change(int length)
{
    ui->audio_lcdnum->display(length);
}

void MainWindow::processed_num_show(int number)
{
    ui->processed_lcdnum->display(number);
}

void MainWindow::on_start_button_clicked()
{
    float ms_energy_ratio = ui->ms_energy_ratio_box->value();
    float sub_ms_engergy_ratio = ui->sub_ms_energy_ratio_box->value();
    float ms_2_ms_engergy_ratio = ui->ms_2_energy_ratio_box->value();
    long max_timeval_div = ui->max_timeval_div_box->value();
    m_thread_detect_phoneme.Init(m_directory, wellshutter_vec, ms_energy_ratio, sub_ms_engergy_ratio, ms_2_ms_engergy_ratio, max_timeval_div);
    ui->processed_lcdnum->setEnabled(true);
    ui->stop_button->setEnabled(true);
    m_thread_detect_phoneme.start();

}

void MainWindow::on_stop_button_clicked()
{
    m_thread_detect_phoneme.requestInterruption();
}


void MainWindow::on_connect_button_clicked()
{
    if(m_connect_dialog!=NULL) {
        m_connect_dialog->exec();
    }
//    m_connect_dialog->close();

}

void MainWindow::connect_sql(QString server, QString database, QString uid, QString pwd)
{
    if(m_thread_detect_phoneme.ConnectSQL(server, database, uid, pwd))
    {
        g_last_debug = QString("连接服务器成功！");
        debug_info_show(g_last_debug);
    }
    else
    {
        g_last_debug = QString("连接服务器失败！");
        debug_info_show(g_last_debug);
    }
}

void MainWindow::on_read_xml_button_clicked()
{
    QString xml_file = QFileDialog::getOpenFileName(this,
                                tr("Open XML"),
                                QDir::currentPath(),
                                tr("XML(*.xml)"));
    if (!xml_file.isEmpty())
    {
        if(ReadXML(xml_file))
        {
            ui->connect_button->setEnabled(true);
            ui->get_filepath_button->setEnabled(true);
            ui->get_filepath_edit->setEnabled(true);
        }
    }
}
