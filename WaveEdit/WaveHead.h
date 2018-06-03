#ifndef __WAVE_HEAD_H__
#define __WAVE_HEAD_H__

#include <assert.h>

#include <stdio.h>

#include "SoundTouch.h"
#ifdef _DEBUG
#pragma comment(lib, "SoundTouchDLL.lib")
#pragma comment(lib, "SoundTouchD.lib")
#else
#pragma comment(lib, "SoundTouch.lib")
#endif


#ifndef uint
typedef unsigned int uint;
#endif           
#define BUFF_SIZE (6720)

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef SAFE_DELETE_ARR
#define SAFE_DELETE_ARR(ptr) { if(ptr) {delete[] ptr; ptr = NULL;}}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) { if(ptr) {delete ptr; ptr = NULL;}}
#endif

using namespace soundtouch;

//WAV audio file 'riff' section header
typedef struct 
{
	char riff_char[4];
	int  package_len;
	char wave[4];
} WavRiff;

/// WAV audio file 'format' section header
typedef struct 
{
	char  fmt[4];
	int   format_len;
	short fixed;
	short channel_number;
	int   sample_rate;
	int   byte_rate;
	short byte_per_sample;
	short bits_per_sample;
} WavFormat;

/// WAV audio file 'fact' section header
typedef struct 
{
	char  fact_field[4];
	int   fact_len;
	uint  fact_sample_len;
} WavFact;

/// WAV audio file 'data' section header
typedef struct 
{
	char  data_field[4];
	uint  data_len;
} WavData;


/// WAV audio file header
typedef struct 
{
	WavRiff   riff;
	WavFormat format;
	WavFact   fact;
	WavData   data;
} WavHeader;

#ifdef BYTE_ORDER
// In gcc compiler detect the byte order automatically
#if BYTE_ORDER == BIG_ENDIAN
// big-endian platform.
#define _BIG_ENDIAN_
#endif
#endif

#ifdef _BIG_ENDIAN_
// big-endian CPU, swap bytes in 16 & 32 bit words

// helper-function to swap byte-order of 32bit integer
static inline int _swap32(int &dwData)
{
	dwData = ((dwData >> 24) & 0x000000FF) | 
		((dwData >> 8)  & 0x0000FF00) | 
		((dwData << 8)  & 0x00FF0000) | 
		((dwData << 24) & 0xFF000000);
	return dwData;
}   

// helper-function to swap byte-order of 16bit integer
static inline short _swap16(short &wData)
{
	wData = ((wData >> 8) & 0x00FF) | 
		((wData << 8) & 0xFF00);
	return wData;
}

// helper-function to swap byte-order of buffer of 16bit integers
static inline void _swap16Buffer(short *pData, int numWords)
{
	int i;

	for (i = 0; i < numWords; i ++)
	{
		pData[i] = _swap16(pData[i]);
	}
}

#else   // BIG_ENDIAN
// little-endian CPU, WAV file is ok as such

// dummy helper-function
static inline int _swap32(int &dwData)
{
	// do nothing
	return dwData;
}   

// dummy helper-function
static inline short _swap16(short &wData)
{
	// do nothing
	return wData;
}

// dummy helper-function
static inline void _swap16Buffer(short *pData, int numBytes)
{
	// do nothing
}

#endif  // BIG_ENDIAN


inline int saturate(float fvalue, float minval, float maxval)
{
	if (fvalue > maxval) 
	{
		fvalue = maxval;
	} 
	else if (fvalue < minval)
	{
		fvalue = minval;
	}
	return (int)fvalue;
}


inline int FloatToWav(const float *dataElems, int numElems, char* outBuffer, unsigned int outBufferSize, int bits_per_sample = 16)
{
	// 每个样本的字节数
	int bytesPerSample;
	assert(numElems > 0);

	bytesPerSample = bits_per_sample / 8;
	assert(numElems*bytesPerSample <= outBufferSize);

	switch (bytesPerSample)
	{
	case 1:
		{
			unsigned char *temp2 = (unsigned char *)outBuffer;
			for (int i = 0; i < numElems; i ++)
			{
				temp2[i] = (unsigned char)saturate(dataElems[i] * 128.0f + 128.0f, 0.0f, 255.0f);
			}
			break;
		}
	case 2:
		{
			short *temp2 = (short *)outBuffer;
			for (int i = 0; i < numElems; i ++)
			{
				short value = (short)saturate(dataElems[i] * 32768.0f, -32768.0f, 32767.0f);
				temp2[i] = _swap16(value);
			}
			break;
		}
	case 3:
		{
			char *temp2 = (char *)outBuffer;
			for (int i = 0; i < numElems; i ++)
			{
				int value = saturate(dataElems[i] * 8388608.0f, -8388608.0f, 8388607.0f);
				*((int*)temp2) = _swap32(value);
				temp2 += 3;
			}
			break;
		}
	case 4:
		{
			int *temp2 = (int *)outBuffer;
			for (int i = 0; i < numElems; i ++)
			{
				int value = saturate(dataElems[i] * 2147483648.0f, -2147483648.0f, 2147483647.0f);
				temp2[i] = _swap32(value);
			}
			break;
		}

	default:
		assert(false);
	}

	return numElems * bytesPerSample;
}

inline int WavToFloat(const char* data, unsigned int numBytes, float* dataElems, unsigned int maxElems, int nBitPerSample)
{
	int bytesPerSample = nBitPerSample / 8;
	int numElems = numBytes/bytesPerSample;
	assert(numElems <= maxElems);

	unsigned char* temp = (unsigned char*)data;
	float* buffer = dataElems;
	switch (bytesPerSample)
	{
	case 1:
		{
			unsigned char *temp2 = (unsigned char*)temp;
			double conv = 1.0 / 128.0;
			for (int i = 0; i < numElems; i ++)
			{
				buffer[i] = (float)(temp2[i] * conv - 1.0);
			}
			break;
		}
	case 2:
		{
			short *temp2 = (short*)temp;
			double conv = 1.0 / 32768.0;
			for (int i = 0; i < numElems; i ++)
			{
				short value = temp2[i];
				buffer[i] = (float)(_swap16(value) * conv);
			}
			break;
		}
	case 3:
		{
			char *temp2 = (char *)temp;
			double conv = 1.0 / 8388608.0;
			for (int i = 0; i < numElems; i ++)
			{
				int value = *((int*)temp2);
				value = _swap32(value) & 0x00ffffff;             // take 24 bits
				value |= (value & 0x00800000) ? 0xff000000 : 0;  // extend minus sign bits
				buffer[i] = (float)(value * conv);
				temp2 += 3;
			}
			break;
		}
	case 4:
		{
			int *temp2 = (int *)temp;
			double conv = 1.0 / 2147483648.0;
			assert(sizeof(int) == 4);
			for (int i = 0; i < numElems; i ++)
			{
				int value = temp2[i];
				buffer[i] = (float)(_swap32(value) * conv);
			}
			break;
		}
	}
	return numElems;
}

inline int WaveSoundTouch(char* data, unsigned int dataSize, float* outDataElems, unsigned int maxElems, float pitch, float rate, int nChannels, int nSmapleRate, int nBitPerSample)
{
	SoundTouch touch;
	touch.setChannels(nChannels);
	touch.setSampleRate(nSmapleRate);
	touch.setPitchSemiTones(pitch);
	//touch.setRate(rate);

	unsigned int numTotalBytes = dataSize;
	int nBytesPerSample = nBitPerSample / 8;

	float *pElems = new float[BUFF_SIZE];
	memset(pElems, 0, sizeof(float)*BUFF_SIZE);

	int numElems = 0, outElemsOffset = 0;
	int  numBytes = 0, readBytesOffset = 0;
	int nSamples = 0;
	while (readBytesOffset < numTotalBytes)
	{
		numElems = min((numTotalBytes-readBytesOffset)/nBytesPerSample, BUFF_SIZE);
		numElems = WavToFloat(data+readBytesOffset, numTotalBytes-readBytesOffset, pElems, numElems, nBitPerSample);
		readBytesOffset += numElems*nBytesPerSample;

		touch.putSamples(pElems, numElems/nChannels);
		do 
		{
			nSamples = touch.receiveSamples(pElems, BUFF_SIZE/nChannels);
			numElems = nSamples*nChannels;
			if(numElems > 0)
			{
				assert(outElemsOffset+numElems < maxElems);
				memcpy(outDataElems+outElemsOffset, pElems, numElems*sizeof(float));
				outElemsOffset += numElems;
			}
		} while (nSamples != 0);
	}

	touch.flush();
	do 
	{
		nSamples = touch.receiveSamples(pElems, BUFF_SIZE/nChannels);
		if(numElems > 0)
		{
			assert(outElemsOffset+numElems < maxElems);
			memcpy(outDataElems+outElemsOffset, pElems, numElems*sizeof(float));
			outElemsOffset += numElems;
		}
	} while (nSamples != 0);

	SAFE_DELETE_ARR(pElems);
	return outElemsOffset;
}

inline int WaveSoundTouch(char* data, unsigned int dataSize, char* outdata, unsigned int outsize, float pitch, float rate, int nChannels, int nSmapleRate, int nNumBits)
{
	SoundTouch touch;
	touch.setChannels(nChannels);
	touch.setSampleRate(nSmapleRate);
	touch.setPitchSemiTones(pitch);
	//touch.setRate(rate);

	unsigned int numTotalBytes = dataSize;

	int nBitPerSample = nNumBits;
	int nBytesPerSample = nBitPerSample / 8;
	int maxElems = numTotalBytes/nBytesPerSample;

	float *pTemp = new float[BUFF_SIZE];
	memset(pTemp, 0, sizeof(float)*BUFF_SIZE);

	int numElems = 0, numBytes = 0;
	int nSamples = 0, readOffset = 0;
	int writeOffset = 0;
	while (readOffset < numTotalBytes)
	{
		numElems = min((numTotalBytes-readOffset)/nBytesPerSample, BUFF_SIZE);
		numElems = WavToFloat( data+readOffset, numTotalBytes-readOffset,pTemp, numElems, nBitPerSample);
		readOffset += numElems*nBytesPerSample;

		touch.putSamples(pTemp, numElems/nChannels);
		int a = 0;

		do 
		{
			nSamples = touch.receiveSamples(pTemp, BUFF_SIZE/nChannels);
			do 
			{
				numElems = nSamples*nChannels;
				if(numElems <= 0)
					break;

				numBytes = FloatToWav(pTemp, numElems, outdata+writeOffset, outsize-writeOffset, nBitPerSample);
				writeOffset = writeOffset + numBytes;
			} while (false);
		} while (nSamples != 0);
	}

	touch.flush();
	do 
	{
		nSamples = touch.receiveSamples(pTemp, BUFF_SIZE/nChannels);
		do 
		{
			numElems = nSamples*nChannels;
			if(numElems <= 0)
				break;

			numBytes = FloatToWav(pTemp, numElems, outdata+writeOffset, outsize-writeOffset, nBitPerSample);
			writeOffset = writeOffset + numBytes;
		} while (false);
	} while (nSamples != 0);
	SAFE_DELETE_ARR(pTemp);

	return writeOffset;
}


inline int WaveSoundTouch(float* dataElems, unsigned int dataElemsSize, float* outDataElems, unsigned int maxElems, float pitch, float rate, int nChannels, int nSmapleRate, int nBitPerSample)
{
	SoundTouch touch;
	touch.setChannels(nChannels);
	touch.setSampleRate(nSmapleRate);
	touch.setPitchSemiTones(pitch);
	//touch.setRate(rate);

	float *pElems = new float[BUFF_SIZE];
	memset(pElems, 0, sizeof(float)*BUFF_SIZE);

	int numElems = 0, outElemsOffset = 0;
	int  numBytes = 0, readElemsOffset = 0;
	int nSamples = 0;
	while (readElemsOffset < dataElemsSize)
	{
		numElems = min(dataElemsSize - readElemsOffset, BUFF_SIZE);
		readElemsOffset += numElems;

		touch.putSamples(dataElems, numElems/nChannels);
		do 
		{
			nSamples = touch.receiveSamples(pElems, BUFF_SIZE/nChannels);
			numElems = nSamples*nChannels;
			if(numElems > 0)
			{
				assert(outElemsOffset+numElems < maxElems);
				memcpy(outDataElems+outElemsOffset, pElems, numElems*sizeof(float));
				outElemsOffset += numElems;
			}
		} while (nSamples != 0);
	}

	touch.flush();
	do 
	{
		nSamples = touch.receiveSamples(pElems, BUFF_SIZE/nChannels);
		numElems = nSamples*nChannels;
		if(numElems > 0)
		{
			assert(outElemsOffset+numElems < maxElems);
			memcpy(outDataElems+outElemsOffset, pElems, numElems*sizeof(float));
			outElemsOffset += numElems;
		}
	} while (nSamples != 0);

	SAFE_DELETE_ARR(pElems);
	return outElemsOffset;
}


inline int WaveSoundTouch(float* dataElems, unsigned int dataElemsSize, char* outbuffer, unsigned int maxBytes, float pitch, float rate, int nChannels, int nSmapleRate, int nBitPerSample)
{
	SoundTouch touch;
	touch.setChannels(nChannels);
	touch.setSampleRate(nSmapleRate);
	touch.setPitchSemiTones(pitch);
	//touch.setRate(rate);

	float *pElems = new float[BUFF_SIZE];
	memset(pElems, 0, sizeof(float)*BUFF_SIZE);

	char *pBuffer = new char[BUFF_SIZE];
	memset(pBuffer, 0, sizeof(char)*BUFF_SIZE);

	int numElems = 0, outBytesOffset = 0;
	int  numBytes = 0, readElemsOffset = 0;
	int nSamples = 0;
	while (readElemsOffset < dataElemsSize)
	{
		numElems = min(dataElemsSize - readElemsOffset, BUFF_SIZE);
		readElemsOffset += numElems;

		touch.putSamples(dataElems, numElems/nChannels);
		do 
		{
			nSamples = touch.receiveSamples(pElems, BUFF_SIZE/nChannels);
			numElems = nSamples*nChannels;
			if(numElems > 0)
			{
				numBytes = FloatToWav(pElems, numElems, pBuffer, BUFF_SIZE, nBitPerSample);
				assert(outBytesOffset+numElems < maxBytes);
				memcpy(outbuffer+outBytesOffset, pBuffer, numElems);
				outBytesOffset += numElems;
			}
		} while (nSamples != 0);
	}

	touch.flush();
	do 
	{
		nSamples = touch.receiveSamples(pElems, BUFF_SIZE/nChannels);
		numElems = nSamples*nChannels;
		if(numElems > 0)
		{
			numBytes = FloatToWav(pElems, numElems, pBuffer, BUFF_SIZE, nBitPerSample);
			assert(outBytesOffset+numElems < maxBytes);
			memcpy(outbuffer+outBytesOffset, pBuffer, numElems);
			outBytesOffset += numElems;
		}
	} while (nSamples != 0);

	SAFE_DELETE_ARR(pElems);
	SAFE_DELETE_ARR(pBuffer);
	return outBytesOffset;
}


#endif
