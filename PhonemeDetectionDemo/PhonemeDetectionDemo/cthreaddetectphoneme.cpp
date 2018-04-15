#include "cthreaddetectphoneme.h"
#include <QDir>
#include <string>
#include <algorithm>

using namespace std;
extern QString g_last_debug;

void CThreadDetectPhoneme::Init(QString directory, vector<WellShutter> &vec_wellshutter, float ms_energy_ratio, float sub_ms_engergy_ratio, float ms_2_ms_engergy_ratio, long max_timeval_div)
{
    m_vec_wellshutter = vec_wellshutter;
    m_detector_framelen = 1024;
    m_detector_step = 512;
    m_detector_nfft = 2048;
    m_directory = directory;
    m_wavlist = GetWavList(m_directory);
    //获取fs
    CReadWav tmp_wav;
    QString first_audio = m_directory + '\\' + m_wavlist[0];
//    first_audio = QString::fromLocal8Bit(first_audio);
//    const unsigned int fs = 0;
    if(tmp_wav.Read(first_audio.toStdString()))
    {
//        fs = tmp_wav.GetSampleRate();
        float ab_max_timeval_div = max_timeval_div/float(tmp_wav.GetSampleRate());
        m_detector.Init(tmp_wav.GetSampleRate(), m_detector_framelen, m_detector_step, m_detector_nfft);
        m_detector.SetThreshold(ms_energy_ratio, sub_ms_engergy_ratio, ms_2_ms_engergy_ratio, ab_max_timeval_div);
    }
    else
    {
        g_last_debug = QString("init ThreadDetectPhoneme: 无法打开音频文件!");
        emit debug_show(g_last_debug);
    }
}

void CThreadDetectPhoneme::QstringToTime(timeval &timetamp, QString name)
{
    QString str_match = name.split(QString("."))[0];
    QString pattern("(\\d{4})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})");
    QRegExp catch_reg;
    catch_reg.setPattern(pattern);
    catch_reg.setMinimal(true);
    if(catch_reg.indexIn(str_match)!=-1)
    {
        tm struct_tm;
        struct_tm.tm_year = catch_reg.cap(1).toInt() - 1900;
        struct_tm.tm_mon = catch_reg.cap(2).toInt();
        struct_tm.tm_mday = catch_reg.cap(3).toInt();
        struct_tm.tm_hour = catch_reg.cap(4).toInt();
        struct_tm.tm_min = catch_reg.cap(5).toInt();
        struct_tm.tm_sec = catch_reg.cap(6).toInt();
        struct_tm.tm_isdst = 0;
        time_t time_sec = mktime(&struct_tm);
        timetamp.tv_sec = time_sec;
        timetamp.tv_usec = 0;
    }
    else
    {
        g_last_debug = QString("QStringToTime：无法匹配文件名时间！文件名%1").arg(name);
        emit debug_show(g_last_debug);
    }
}

bool CThreadDetectPhoneme::ConnectSQL(QString server, QString database, QString UID, QString PWD)
{
    return m_sql.ConnectSQL(server, database, UID, PWD);
}

void CThreadDetectPhoneme::run()
{
    int wav_idx = 0;
    while(wav_idx<m_wavlist.size())
    {
        emit num_audio_change(m_wavlist.size());
        _RunDetector(wav_idx);
        if(!isInterruptionRequested())
            m_wavlist = GetWavList(m_directory);
        else
            break;
        //TODO: 添加暂停
    }
}

void CThreadDetectPhoneme::_RunDetector(int& wav_idx)
{
    while(wav_idx < m_wavlist.size())
    {
        QString wav_name = m_wavlist[wav_idx];
        wav_idx++;
        timeval timetamp;
        CReadWav read_wav;
        std::vector<string> code;
        QstringToTime(timetamp, wav_name);
//        wav_name = QString::fromLocal8Bit(wav_name);
        QString whole_name = m_directory + '/' + wav_name;
        if(!read_wav.Read(whole_name.toStdString()))
        {
            g_last_debug = QString("无法读取%1，跳过").arg(wav_name);
            continue;
        }
        m_detector.Detect(read_wav.GetData(), read_wav.GetSize(), timetamp, code);
        if(!code.empty())
        {
            sort(code.begin(),code.end());
            code.erase(std::unique(code.begin(),code.end()), code.end());

            QString copy_path = QDir::currentPath() + "\\" + QString("detected_wav") + "\\" + wav_name;
            //TODO: 插入数据库操作
            // tmp for debug //
            emit debug_show(QString("%1检测到音阶编码").arg(wav_name));
            for(auto it=code.begin();it!=code.end();it++)
            {
                g_last_debug = QString::fromStdString(*it);
                QString current_code = QString::fromStdString(*it);
                for(auto iter=m_vec_wellshutter.begin();iter!=m_vec_wellshutter.end();iter++)
                {
                    if(iter->code == current_code)
                    {
                        if(!CopyFileToPath(whole_name, copy_path, true))
                        {
                            emit debug_show(QString("%1复制失败").arg(wav_name));
                        }
                        m_sql.InsertData(wav_name.split(".")[0],iter->loc, iter->alarmtype, copy_path);
                    }
                }
                emit debug_show(g_last_debug);
            }
        }
        emit processed_num(wav_idx);
    }
    
}

QStringList GetWavList(QString directory)
{
    QDir dir(directory);
    QStringList filters;
    filters<<QString("*.wav");
    dir.setFilter(QDir::Files|QDir::NoSymLinks);
    dir.setNameFilters(filters);
    QStringList wav_list = dir.entryList();
    return wav_list;
}


//拷贝文件：
bool CopyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}
