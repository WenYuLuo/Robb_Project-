//#include "StdAfx.h"
#include "DetectMusicalScale.h"

#include <cv.h>
#include <iostream>
#include <complex>

using namespace std;
using namespace cv;

#include <windows.h> 
#include <time.h>

typedef unsigned long long uint64_t;

int get_time_now(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970

	static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime( &system_time );
	SystemTimeToFileTime( &system_time, &file_time );
	time =  ((uint64_t)file_time.dwLowDateTime )      ;
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
	tp->tv_usec = (long) (system_time.wMilliseconds * 1000); // ΢��
	return 0;
}

timeval timeval_add_dt(timeval timestamp, double dt)
{
	timeval nts;
	nts.tv_sec = (long)(timestamp.tv_sec + dt + (double)timestamp.tv_usec/1000000);
	nts.tv_usec = (uint64_t)(dt * 1000000 + timestamp.tv_usec) % 1000000;
	return nts;
}

double timeval_div(timeval ts1, timeval ts2)
{
	double ft1 = ts1.tv_sec + (double)ts1.tv_usec/1000000;
	double ft2 = ts2.tv_sec + (double)ts2.tv_usec/1000000;
	return fabs(ft1 - ft2);
}

CDetectMusicalScale::CDetectMusicalScale(void)
{
	_hann_win = NULL;
	_buffer = NULL;
	_buffer_size = 0;
	_data_len = 0; 

	_nfft = 2048; // ����Ҷ�任���ĳ��� //
	_step = 256; // �������� // 
	_frame_len = 512; // ÿ�δ��������ݳ��� // 

	REFFREQUENCY[0] =1335;
	REFFREQUENCY[1] =1485;
	REFFREQUENCY[2] =1658;
	REFFREQUENCY[3] =1744;
	REFFREQUENCY[4] =1959;
	REFFREQUENCY[5] =2196;
	REFFREQUENCY[6] =2433;

	_fs = 22050;

	Init(22050, 512, 256, 2048);

	_count = 0;
	FRAME_NUM = 100;
	memset(_back_ground_energy, 0, sizeof(_back_ground_energy));

	_ms_energy_ratio = 0.35f; // ��Ƶ��������(_f_low,  _f_high) Ƶ����������ֵ
	_sub_ms_engergy_ratio = 5; // �������������뱳����������ֵ
	_ms_2_ms_engergy_ratio = 0.5f; // ����֮����������ֵ
	_max_timeval_div = (float)40000/_fs;

	MAX_BIT_NUM = 2;
	BEGIN_CODE = (0x01 << 1) | (0x01 << 4); 
}

CDetectMusicalScale::~CDetectMusicalScale(void)
{
	if(_buffer != NULL) delete []_buffer; _buffer = NULL;
	if(_hann_win != NULL) delete []_hann_win; _hann_win = NULL;	
}

bool CDetectMusicalScale::Init(uint32_t fs, uint32_t frame_len, uint32_t step, uint32_t nfft)
{
	_fs = fs;
	_frame_len = frame_len;
	_step = step;
	_nfft = nfft;

	if(_hann_win != NULL) delete []_hann_win; _hann_win = NULL;	

	_hann_win = new float[_frame_len];
	for(int i = 0; i < _frame_len; i++)
		_hann_win[i] = 0.5f * (1 - cos((2 * CV_PI * i)/(_frame_len - 1)));


	float dfre = 0;
	for(int i = 1; i < MS_NUM; i++)
	{
		dfre += REFFREQUENCY[i] - REFFREQUENCY[i-1];
	}
	dfre /= (MS_NUM-1);

	_win_size = int(dfre / _fs * _nfft / 2 / 2 / 2) ;

	for(int i = 0; i < MS_NUM; i++)
	{
		REF_FREQ_INDEX[i] = int(REFFREQUENCY[i] / _fs * _nfft + 0.5f);
	}

	_f_low = 1000;
	_f_low_idx = int(_f_low / (_fs / _nfft) + 0.5f);

	_f_high = 6000;
	_f_high_idx = int(_f_high / (_fs / _nfft) + 0.5f);

	return true;
}

void CDetectMusicalScale::Detect(float *data, uint32_t data_size, timeval timestamp, std::vector<string>& szcodes)
{
	if (_buffer == NULL || _buffer_size < (_data_len + data_size))
	{
		uint32_t _buffer_size = _data_len + data_size + 2048;
		float *temp = new float[_buffer_size];

		if(_data_len > 0) memcpy(temp, _buffer, _data_len * sizeof(float)); // ����֮ǰ������ //
		memcpy(temp+_data_len, data, data_size * sizeof(float)); //

		if(_buffer != NULL) delete []_buffer; 
		_buffer = temp;
		_data_len = _data_len + data_size;
	}
	else
	{
		memcpy(_buffer+_data_len, data, data_size * sizeof(float)); //
		_data_len = _data_len + data_size;
	}

	float *frame = new float[_frame_len];

	int beg_n = 0;
	for(beg_n = 0; beg_n + _frame_len <= _data_len; beg_n += _step)
	{
		cout<<beg_n<< "  :  "; 
		memcpy(frame, _buffer+beg_n, _frame_len * sizeof(float)); //

		HannFun(frame, _frame_len);
		// ����DFT�任���������� //
		MSCODE mscode;
		mscode._code = 0;
		mscode._timestamp = timeval_add_dt(timestamp, (double)beg_n/_fs);

		SubbandEnergy(frame, _frame_len, mscode._code);
		_codes.push_back(mscode);
		if(
			((float)beg_n/_fs < 4.6f && (float)(beg_n+_frame_len)/_fs > 4.6f) 
			|| ((float)beg_n/_fs < 3.1f && (float)(beg_n+_frame_len)/_fs > 3.1f) 
			|| ((float)beg_n/_fs < 6.18f && (float)(beg_n+_frame_len)/_fs > 6.18f)
			) 
		{
			int debug = 0;
		}

		if(
			(beg_n < 1497697 && (beg_n+_frame_len) > 1497697)
			|| (beg_n < 1555109 && (beg_n+_frame_len) > 1555109)
			|| (beg_n < 1593896 && (beg_n+_frame_len) > 1593896)
			|| (beg_n < 1630331 && (beg_n+_frame_len) > 1630331)
			) 
		{
			int debug = 0;
		}
	}
	//
	if(beg_n < _data_len)
	{
		// ��ʣ�������ݿ����� _buffer ��ͷ����Ϊ��һ�ε���������׼�� // 
		memcpy(frame, _buffer+beg_n, (_data_len - beg_n) * sizeof(float)); //
		memcpy(_buffer, frame, (_data_len - beg_n) * sizeof(float)); //
		_data_len = _data_len - beg_n;
	}

//	std::vector<string> szcodes;
	ProcessCodes(_codes, _pro_codes, szcodes);

    delete []frame;
}

void CDetectMusicalScale::SetThreshold(float ms_energy_ratio, float sub_ms_engergy_ratio, float ms_2_ms_engergy_ratio, float max_timeval_div)
{
    _ms_energy_ratio = ms_energy_ratio;
    _sub_ms_engergy_ratio = sub_ms_engergy_ratio;
    _ms_2_ms_engergy_ratio = ms_2_ms_engergy_ratio;
    _max_timeval_div = max_timeval_div;
}


void CDetectMusicalScale::SubbandEnergy(float *frame, uint32_t frame_len, int &code)
{
	code = 0; // ����
	Mat padded = Mat::zeros(1, _nfft, CV_32FC1);
	memcpy(padded.ptr<float>(0), frame, frame_len * sizeof(float)); //	

	Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
	Mat complexI;
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

	dft(complexI, complexI, DFT_ROWS);            // this way the result may fit in the source matrix

	// compute the magnitude and switch to logarithmic scale
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude	

	Mat mag = planes[0];

	float energys[MS_NUM] = {0};

	//_win_size = 1;
	for(int i = 0; i < MS_NUM; i++)
	{
		for(int k = -_win_size; k <= _win_size; k++)
		{
			energys[i] = MAX(mag.at<float>(0, REF_FREQ_INDEX[i]+k), energys[i]);
		}
	}

	cv::Rect rt(_f_low_idx, 0, _f_high_idx - _f_low_idx + 1, 1);
	Mat sub = mag(rt);

	Mat mean, stddev;
	cv::meanStdDev(sub, mean, stddev);

	if(_count < FRAME_NUM)
	{
		_count++;
		_back_ground_energy[0] = mean.at<double>(0,0)/_count + _back_ground_energy[0] * (_count - 1)/_count;
		_back_ground_energy[1] = stddev.at<double>(0,0)/_count + _back_ground_energy[1] * (_count - 1)/_count;
		return;
	}
	//
	float band_energy_0 = 0, band_energy_1 = 0;
	for(int k = _f_low_idx; k <= _f_high_idx; k++)
	{
		band_energy_0 += mag.at<float>(0, k);
	}
	for(int k = REF_FREQ_INDEX[0] - _win_size; k <= REF_FREQ_INDEX[MS_NUM-1] + _win_size; k++)
	{
		band_energy_1 += mag.at<float>(0, k);
	}

#ifdef _DEBUG
	cout<<endl;
	cout<< band_energy_1/band_energy_0;
	cout<<endl;

	for(int i = 0; i < MS_NUM; i++)
	{
		cout<< energys[i]/(_back_ground_energy[0] + 3 * _back_ground_energy[1]);
		cout<<"   ";
	}
	cout<<endl;
#endif
	
	code = 0; // �м�����
	do 
	{
		if(band_energy_1/band_energy_0 < _ms_energy_ratio) // ��Ƶ��������(_f_low,  _f_high) Ƶ����������ֵ
			break;
		bool has_code = false;
		for(int i = 0; i < MS_NUM; i++)
		{
			if((energys[i]/(_back_ground_energy[0] + 3 * _back_ground_energy[1])) > _sub_ms_engergy_ratio) // �������������뱳����������ֵ
			{
				has_code = true;
				code = code | (0x01 << i);
			}
		}
		if(!has_code) break;
		// ����֮����������ֵ _ms_2_ms_engergy_ratio

	} while (false);


	if(band_energy_0/(_f_high_idx - _f_low_idx + 1) < _back_ground_energy[0] + 5 * _back_ground_energy[1])
	{
		_back_ground_energy[0] = mean.at<double>(0,0)/FRAME_NUM + _back_ground_energy[0] * (FRAME_NUM - 1)/FRAME_NUM;
		_back_ground_energy[1] = stddev.at<double>(0,0)/FRAME_NUM + _back_ground_energy[1] * (FRAME_NUM - 1)/FRAME_NUM;
	}
}

void CDetectMusicalScale::HannFun(float *frame, uint32_t frame_len)
{
	for(int i = 0; i < _frame_len; i++)
		frame[i] *= _hann_win[i];
}

void CDetectMusicalScale::ProcessCodes(std::vector<MSCODE> &codes, std::vector<MSCODE> &pro_codes, std::vector<std::string> &sz_codes)
{
	int beg_idx = -1, end_idx = -1;
	for(size_t i = 0; i < codes.size(); i++)
	{
		if(codes[i]._code > 0 && beg_idx == -1) beg_idx = i;
		if(codes[i]._code != 0 || beg_idx == -1) continue;

		end_idx = i;
		int hist[MS_NUM] = {0};
		for(size_t k = beg_idx; k < end_idx; k++)
		{
			int code = codes[k]._code;
			for(int b = 0; b < MS_NUM; b++)
			{
				if((code & (0x01 << b)) > 0)
				{
					hist[b]++;
				}
			}
		} 
		//
		int thd = 0;
		do 
		{
			for(int b = 0; b < MS_NUM; b++)
				if(hist[b] <= thd)
					hist[b] = 0;
			thd++;
		} while (CountBits(hist) > MAX_BIT_NUM);
		// ���ձ���
		MSCODE mscode;
		int &code = mscode._code;
		code = 0;
		for(int b = 0; b < MS_NUM; b++)
			if(hist[b] > 0)
				code = code | (0x01 << b);

		mscode._timestamp = codes[beg_idx]._timestamp;
		pro_codes.push_back(mscode);

		beg_idx = -1; 
		end_idx = -1;
	}

	if(beg_idx >= 0) // ����û�д��������м�����
	{
		std::vector<MSCODE> tmp_vec;
		for(size_t i = beg_idx; i < codes.size(); i++)
		{
			tmp_vec.push_back(codes[i]);
		}
		codes = tmp_vec;
	}
	else
	{
		codes.clear();
	}

	std::vector<MSCODE> left_code;
	for(size_t i = 0; i < pro_codes.size(); i++)
	{
		if(BEGIN_CODE != pro_codes[i]._code) continue;
		if(i + 3 < pro_codes.size())
		{
			if(timeval_div(pro_codes[i]._timestamp, pro_codes[i+1]._timestamp) > _max_timeval_div)
				continue;
			if(timeval_div(pro_codes[i+1]._timestamp, pro_codes[i+2]._timestamp) > _max_timeval_div)
				continue;
			if(timeval_div(pro_codes[i+2]._timestamp, pro_codes[i+3]._timestamp) > _max_timeval_div)
				continue;
		}

		if(i + 3 < pro_codes.size()) // ��ʼ������������ 3 λ�ı���
		{
			string szcode;
			szcode = szcode + Code(pro_codes[i+1]._code);
			szcode = szcode + "|";
			szcode = szcode + Code(pro_codes[i+2]._code);
			szcode = szcode + "|";
			szcode = szcode + Code(pro_codes[i+3]._code);

			sz_codes.push_back(string(szcode));
		}
		else
		{
			for(size_t k = i; k < pro_codes.size(); k++)
			{
				left_code.push_back(pro_codes[i]);
			}			
		}
	}
	pro_codes = left_code;
}

int CDetectMusicalScale::CountBits(int hist[MS_NUM])
{
	int count = 0;
	for(int b = 0; b < MS_NUM; b++)
	{
		if(hist[b] > 0)
			count++;
	}
	return count;
}

string CDetectMusicalScale::Code(int code)
{
	int data_low = code & 0x0F;
	int data_high = (code & 0xF0) >> 4;

	string sz;

	if(data_high <= 9)
		sz.push_back(char(48+data_high));
	else
		sz.push_back(char(65+data_high-10));

	if(data_low <= 9)
		sz.push_back(char(48+data_low));
	else
		sz.push_back(char(65+data_low-10));

	return sz;
}
