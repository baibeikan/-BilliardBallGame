#ifndef __AudioRecord_H__
#define __AudioRecord_H__

#define WAVE_BUFF_SIZE (1024 * 16)
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <string>
#ifdef UNICODE
typedef std::wstring string;
#else
typedef std::string string;
#endif
#include "WaveOut.h"

class WaveIn
{
public:
	WaveIn();
	~WaveIn();

public:
	void StartRecord(UINT nWaveID);

	void StopRecord();

	bool GetIsRecord() {
		return phwi != NULL;
	 }

	static DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

	const WAVEFORMATEX& GetWaveFormat()const { return m_wavFormat;}

private:
	void PushBuffer(char* _data, UINT _size);

private:
	WAVEFORMATEX m_wavFormat;
	WAVEHDR wh1;
	WAVEHDR wh2;
	HWAVEIN phwi;
	string m_WaveName;

	char* m_waveData;
	uint32_t m_waveDataSize;
	uint32_t m_waveDataLen;

public:
	CDialogEx* m_pOwner;
	//WaveOut m_waveOut;
};

#endif
