#pragma once
#include <string>
using namespace std;

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#define FOURCC uint32_t 

#define MAKE_FOURCC(a,b,c,d) \
	( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) | ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) )

//#define MAKE_FOURCC(a,b,c,d) \
//	( ((uint32_t)d) | ( ((uint32_t)c) << 8 ) | ( ((uint32_t)b) << 16 ) | ( ((uint32_t)a) << 24 ) )

class CReadWav
{
	struct Base_chunk
	{
		FOURCC fcc;    // FourCC id
		uint32_t cb_size; // 数据域的大小
		Base_chunk(FOURCC fourcc) : fcc(fourcc)
		{
			cb_size = 0;
		}
	};

	struct Wave_format{

		uint16_t format_tag;      // WAVE的数据格式，PCM数据该值为1
		uint16_t channels;        // 声道数
		uint32_t sample_per_sec;  // 采样率
		uint32_t bytes_per_sec;   // 码率，channels * sample_per_sec * bits_per_sample / 8
		uint16_t block_align;     // 音频数据块，每次采样处理的数据大小，channels * bits_per_sample / 8
		uint16_t bits_per_sample; // 量化位数，8、16、32等
		uint16_t ex_size;         // 扩展块的大小，附加块的大小

		Wave_format()
		{
			format_tag = 1; // PCM format data
			ex_size = 0; // don't use extesion field

			channels = 0;
			sample_per_sec = 0;
			bytes_per_sec = 0;
			block_align = 0;
			bits_per_sample = 0;
		}
	};

public:
	CReadWav(void);
	~CReadWav(void);

    bool Read(const string & file_path);
    const unsigned int GetSize() { return _sample_size; }
    const unsigned int GetChannels() { return _channels; }
    float* GetData() { return _sample; }
    const unsigned int GetSampleRate() { return _wave_format.sample_per_sec; }

protected:

	float *_sample;
	unsigned int _sample_size;
	unsigned int _channels;

	uint8_t *_data;
	uint32_t _data_size;
	Wave_format _wave_format;
};
