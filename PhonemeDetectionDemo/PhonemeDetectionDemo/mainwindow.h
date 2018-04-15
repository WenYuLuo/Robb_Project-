#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "cthreaddetectphoneme.h"
#include "cconnectdialog.h"

//QString g_last_debug;

struct WellShutter;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool ReadXML(QString xml_path);

private slots:
    void on_get_filepath_button_clicked();

    void on_get_filepath_edit_textChanged();
    
    void debug_info_show(QString);

    void available_audio_change(int);

    void processed_num_show(int);

    void on_start_button_clicked();

    void on_stop_button_clicked();

    void on_connect_button_clicked();

    void connect_sql(QString server, QString database, QString uid, QString pwd);

    void on_read_xml_button_clicked();

private:
    Ui::MainWindow *ui;
    QString m_directory;
    CConnectDialog *m_connect_dialog = NULL;
    CThreadDetectPhoneme m_thread_detect_phoneme;
    std::vector<WellShutter> wellshutter_vec;
};

#endif // MAINWINDOW_H
