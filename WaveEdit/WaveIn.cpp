#include "stdafx.h"
#include "WaveIn.h"
#include <Windows.h>
#include <stdio.h>
#include "WaveEditDlg.h"

void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nChannels, DWORD nSamplesPerSec, WORD wBitsPerSample)
{
	m_WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
	m_WaveFormat->nChannels = nChannels;
	m_WaveFormat->nSamplesPerSec = nSamplesPerSec;
	m_WaveFormat->wBitsPerSample = wBitsPerSample;
	m_WaveFormat->nBlockAlign = nChannels * wBitsPerSample / 8;
	m_WaveFormat->nAvgBytesPerSec = nSamplesPerSec * m_WaveFormat->nBlockAlign;
	m_WaveFormat->cbSize = 0;
}

WaveIn::WaveIn()
{
	phwi = NULL;
	m_waveData = NULL;

	memset(&wh1, 0, sizeof(wh1));
	memset(&wh2, 0, sizeof(wh2));
	m_wavFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_wavFormat.nChannels = 2;
	m_wavFormat.nSamplesPerSec = 8000;
	m_wavFormat.wBitsPerSample = 8;
	m_wavFormat.nAvgBytesPerSec = m_wavFormat.nSamplesPerSec * m_wavFormat.nChannels * m_wavFormat.wBitsPerSample / 8;
	m_wavFormat.nBlockAlign = m_wavFormat.nChannels * m_wavFormat.wBitsPerSample / 8;
	m_wavFormat.cbSize = 0;
	WaveInitFormat(&m_wavFormat, 1, 8000, 8);
}

WaveIn::~WaveIn()
{
	StopRecord();
}


void WaveIn::StartRecord(UINT nWaveID)
{
	UINT count = waveInGetNumDevs();//1
	if (count == 0)
	{
		printf("\n音频输入数量：%d\n", count);
		return;
	}

	if (count > nWaveID)
	{
		WAVEINCAPS waveIncaps;
		MMRESULT mmResult = waveInGetDevCaps(nWaveID, &waveIncaps, sizeof(WAVEINCAPS));//2
		m_WaveName = waveIncaps.szPname;

		OutputDebugString(waveIncaps.szPname);
		OutputDebugString(_T("\n"));

		if (MMSYSERR_NOERROR == mmResult)
		{
			printf("\n请求打开音频输入设备");
			printf("\n采样参数：单声道 8kHz 8bit\n");
			mmResult = waveInOpen(&phwi, nWaveID, &m_wavFormat, (DWORD)(MicCallback), (DWORD_PTR)this, CALLBACK_FUNCTION);//3
			UINT waveInID = 0;
			waveInGetID(phwi, &waveInID);

			if (MMSYSERR_NOERROR == mmResult)
			{
				unsigned int recordSize = m_wavFormat.nAvgBytesPerSec/8;
				wh1.dwBufferLength = recordSize;
				wh1.lpData = new char[wh1.dwBufferLength];
				wh1.dwUser = 1;
				wh1.dwFlags = 0;
				printf("\n准备缓冲区1");
				printf("\n将缓冲区1加入音频输入设备");
				mmResult = waveInPrepareHeader(phwi, &wh1, sizeof(WAVEHDR));//4
				mmResult = waveInAddBuffer(phwi, &wh1, sizeof(WAVEHDR));//5

				wh2.dwBufferLength = recordSize;
				wh2.lpData = new char[wh2.dwBufferLength];
				wh2.dwUser = 2;
				wh2.dwFlags = 0;
				printf("\n准备缓冲区2\n");
				mmResult = waveInPrepareHeader(phwi, &wh2, sizeof(WAVEHDR));//4
				printf("\n将缓冲区2加入音频输入设备\n");
				mmResult = waveInAddBuffer(phwi, &wh2, sizeof(WAVEHDR));//5

				if (MMSYSERR_NOERROR == mmResult)
				{
					if (MMSYSERR_NOERROR == mmResult)
					{
						mmResult = waveInStart(phwi);//6
						printf("\n请求开始录音\n");
					}
				}
			}

		}
	}
}

void WaveIn::StopRecord()
{
	waveInStop(phwi);
	OutputDebugStringA("waveInStop\n");
	waveInUnprepareHeader(phwi, &wh1, sizeof(WAVEHDR));//清理数据
	waveInUnprepareHeader(phwi, &wh2, sizeof(WAVEHDR));//清理数据
	waveInClose(phwi);
	OutputDebugStringA("waveInClose\n");

	SAFE_DELETE_ARR(wh1.lpData);
	SAFE_DELETE_ARR(wh2.lpData);
}

DWORD CALLBACK WaveIn::MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	WaveIn* pWaveIn = (WaveIn*)dwInstance;
	if (pWaveIn == nullptr)
		return 0;

	switch (uMsg)
	{
	case WIM_OPEN:
		printf("\n设备已经打开...\n");
		break;
	case WIM_DATA:
		if (1)
		{
			LPWAVEHDR lpWaveHdr = ((LPWAVEHDR)dwParam1);
			printf("\n缓冲区%d存满...\n", lpWaveHdr->dwUser);
			pWaveIn->PushBuffer(lpWaveHdr->lpData, lpWaveHdr->dwBytesRecorded);
			waveInPrepareHeader(hwavein, lpWaveHdr, sizeof(WAVEHDR));//4
			waveInAddBuffer(hwavein, lpWaveHdr, sizeof(WAVEHDR));
		}
		break;
	case WIM_CLOSE:
		printf("\n设备已经关闭...\n");
		break;
	default:
		break;
	}
	return 0;
}

void WaveIn::PushBuffer(char* _dataRecorded, UINT _sizeRecorded)
{
	CWaveEditDlg* pWaveDlg = (CWaveEditDlg*)m_pOwner;
	pWaveDlg->WaveInStream(_dataRecorded, _sizeRecorded);
}
