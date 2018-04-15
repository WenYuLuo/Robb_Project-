#ifndef CTHREADDETECTPHONEME_H
#define CTHREADDETECTPHONEME_H

#include <QObject>
#include <QThread>

#include "DetectMusicalScale.h"
#include "ReadWav.h"
#include "csqlopt.h"

struct WellShutter
{
    QString code;
    QString loc;
    QString alarmtype;
};

class CThreadDetectPhoneme: public QThread
{
    Q_OBJECT
public:
    CThreadDetectPhoneme():QThread(),m_sql() {}
    void Init(QString directory, vector<WellShutter> &vec_wellshutter, float ms_energy_ratio, float sub_ms_engergy_ratio, float ms_2_ms_engergy_ratio, long max_timeval_div);
    void QstringToTime(timeval &timetamp, QString name);
    bool ConnectSQL(QString server, QString database, QString UID, QString PWD);
    void run();

private:
    void _RunDetector(int &wav_idx);
    
signals:
    void debug_show(QString);
    void processed_num(int);
    void num_audio_change(int);
    
private:
    CDetectMusicalScale m_detector;
    QString m_directory;
    QStringList m_wavlist;
    CSQLserver m_sql;
    vector<WellShutter> m_vec_wellshutter;
    int m_detector_framelen;
    int m_detector_step;
    int m_detector_nfft;
};

QStringList GetWavList(QString directory);
#endif // CTHREADDETECTPHONEME_H
