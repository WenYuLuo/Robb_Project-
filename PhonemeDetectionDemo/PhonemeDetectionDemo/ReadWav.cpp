#include "StdAfx.h"
#include "ReadWav.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include<math.h>
#include<cmath>
#include<stdlib.h>
#include <bitset>
#include <iomanip>

using namespace std;

CReadWav::CReadWav(void)
{
	_data = NULL;
	_data_size = 0;
	_sample = NULL;
	_sample_size = 0;
}

CReadWav::~CReadWav(void)
{
	if(_data != NULL) delete []_data; _data = NULL;
	if(_sample != NULL) delete []_sample; _sample = NULL;
}

bool CReadWav::Read(const string & file_path)
{
    std::fstream fs;
    fs.open(file_path, ios::binary | ios::in);
	if(!fs.is_open())
	{
		std::cout<<"Open file fail !";  
		return false;
	}

	// Read RIFF chunk
	FOURCC fourcc;
	fs.read((char*)&fourcc, sizeof(FOURCC));

	if (fourcc != MAKE_FOURCC('R', 'I', 'F', 'F')) // 判断是不是RIFF
		return false;

	Base_chunk riff_chunk(fourcc);
	fs.read((char*)&riff_chunk.cb_size, sizeof(uint32_t));

	// Read WAVE FOURCC
	fs.read((char*)&fourcc, sizeof(FOURCC));
	if (fourcc != MAKE_FOURCC('W', 'A', 'V', 'E'))
		return false;

	// Read fmt FOURCC
	fs.read((char*)&fourcc, sizeof(FOURCC));
	if (fourcc == MAKE_FOURCC('f', 'm', 't', ' '))
	{
		Base_chunk fmt_chunk(fourcc);
		fs.read((char*)&fmt_chunk.cb_size, sizeof(uint32_t));		
		fs.read((char*)&_wave_format, fmt_chunk.cb_size);
	}

	// Read fact FOURCC
	fs.read((char*)&fourcc, sizeof(FOURCC));
	if (fourcc == MAKE_FOURCC('f', 'a', 'c', 't'))
	{
		Base_chunk fact_chunk(fourcc);
		fs.read((char*)&fact_chunk.cb_size, sizeof(uint32_t));

		fs.seekg(fact_chunk.cb_size, ios::cur);
	}

	// Read data FOURCC
	// 数据段
	fs.read((char*)&fourcc, sizeof(FOURCC));
	if (fourcc == MAKE_FOURCC('d', 'a', 't', 'a'))
	{
		Base_chunk data_chunk(fourcc);
		fs.read((char*)&data_chunk.cb_size, sizeof(uint32_t));

		_data_size = data_chunk.cb_size;

		if(_data != NULL) delete []_data;
		_data = new uint8_t[_data_size];

		_sample_size = _data_size / _wave_format.block_align;
		_channels = _wave_format.channels;
		if(_sample != NULL) delete []_sample; 
		_sample = new float[_sample_size*_channels];
		
		fs.read((char*)_data, _data_size);
		unsigned int count = fs.gcount();

		setprecision(4);
		for (unsigned long i =0; i<_data_size; i = i + _wave_format.block_align)
		{
			for (unsigned int j = 0; j < _channels; j++)
			{
				//右边为大端
				unsigned int data_low = _data[i + j*2];
				unsigned int data_high = _data[i + 1 + j*2];
				int data_value = (data_high << 8) + data_low;

				int data_complement = 0; // 补码			
				if ((data_high & 0x80) == 0x80) //取大端的最高位（符号位）
				{
					data_complement = data_value - 65536;
				}
				else
				{
					data_complement = data_value;
				}
				float float_data = (float) (data_complement / (float) 32768);

				_sample[j * _sample_size + i / _wave_format.block_align] = float_data;
			}

		}
	}

	fs.close();

	return true;
}
