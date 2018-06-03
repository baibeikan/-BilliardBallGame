#ifndef __WAVEOUT_H__
#define __WAVEOUT_H__

#define WAVE_OUT_SIZE (256 * 1024)
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include "WavFile.h"

#include <string>
#include <queue>
#ifdef UNICODE
typedef std::wstring string;
#else
typedef std::string string;
#endif

using namespace soundtouch;

void GetWavInfo(char * filename, WAV_INFO * wav_info);

struct WaveBuffer
{
	char* pBuffer;
	uint32_t nBufferSize;
	uint32_t nDataSize;
};

class WaveOut
{
public:
	WaveOut();
	~WaveOut();

public:
	void Play(char* szName);

	void Play(Wave* wave);

	void Stop();

	void SetPitch(float pitch);

	void SetPlaybackRate(float playbackRate);

	static DWORD CALLBACK WaveCallback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

	void Update();

private:
	friend DWORD WINAPI ThreadMain(VOID* lpParam);

	bool FillDataToWaveHdr(WAVEHDR& wavehdr);

public:
	void PushStream(float* data, unsigned int dataSize);

	void PushStream(char* data, unsigned int dataSize);

private:
	// wave 格式
	WAVEFORMATEX	wfx;
	HWAVEOUT		hwo;
	// 缓冲区
	WAVEHDR			m_arrWaveHdr[2];

	std::queue<WAVEHDR*> m_queWaveHdruffer;
	// 是不是向缓冲区里面写数据
	bool			m_bIsWaveOutWrite;

	Wave *m_pWavInFile;
	float m_fPitch;

	HANDLE m_hThread;
	HANDLE m_hEvent;
};

#endif
