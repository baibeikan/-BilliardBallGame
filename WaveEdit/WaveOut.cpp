#include "stdafx.h"
#include <Windows.h>
#include "WaveOut.h"
#include <stdio.h>
#include "WavFile.h"
using namespace soundtouch;
#define BUFF_SIZE (6720)

int FloatToWav(const float *buffer, unsigned int numElems, short* outBuffer, unsigned int outBufferSize)
{
	if(numElems <= 0) return 0;
	if(numElems > outBufferSize)
	{
		int a = 0;
	}

	assert(numElems <= outBufferSize);
	short *temp2 = (short *)outBuffer;
	for (unsigned int i = 0; i < numElems; i ++)
	{
		short value = (short)saturate(buffer[i] * 32768.0f, -32768.0f, 32767.0f);
		temp2[i] = _swap16(value);
	}
	return numElems;
}
//
//void write(const float *buffer, int numElems, char* outBuffer, unsigned int outBufferSize, int bits_per_sample = 16)
//{
//	int numBytes;
//	int bytesPerSample;
//
//	if (numElems == 0) return;
//
//	// ÿ���������ֽ���
//	bytesPerSample = bits_per_sample / 8;
//	// �ܵ��ֽ���
//	numBytes = numElems * bytesPerSample;
//	short *temp = (short *)outBuffer;
//	switch (bytesPerSample)
//	{
//	case 1:
//		{
//			unsigned char *temp2 = (unsigned char *)temp;
//			for (int i = 0; i < numElems; i ++)
//			{
//				temp2[i] = (unsigned char)saturate(buffer[i] * 128.0f + 128.0f, 0.0f, 255.0f);
//			}
//			break;
//		}
//	case 2:
//		{
//			short *temp2 = (short *)temp;
//			for (int i = 0; i < numElems; i ++)
//			{
//				short value = (short)saturate(buffer[i] * 32768.0f, -32768.0f, 32767.0f);
//				temp2[i] = _swap16(value);
//			}
//			break;
//		}
//
//	case 3:
//		{
//			char *temp2 = (char *)temp;
//			for (int i = 0; i < numElems; i ++)
//			{
//				int value = saturate(buffer[i] * 8388608.0f, -8388608.0f, 8388607.0f);
//				*((int*)temp2) = _swap32(value);
//				temp2 += 3;
//			}
//			break;
//		}
//
//	case 4:
//		{
//			int *temp2 = (int *)temp;
//			for (int i = 0; i < numElems; i ++)
//			{
//				int value = saturate(buffer[i] * 2147483648.0f, -2147483648.0f, 2147483647.0f);
//				temp2[i] = _swap32(value);
//			}
//			break;
//		}
//
//	default:
//		assert(false);
//	}
//}
//
// Processes the sound
static void process(SoundTouch *pSoundTouch, WavInFile *inFile, WavOutFile *wavOut)
{
	int nSamples;
	int nChannels;
	int buffSizeSamples;
	SAMPLETYPE sampleBuffer[BUFF_SIZE];

	if ((inFile == NULL) || (wavOut == NULL)) return;  // nothing to do.

	unsigned int dataSizeInBytes = inFile->getDataSizeInBytes();
	int bytesPerSample = inFile->getNumBits()/8;
	unsigned int bufferSize = dataSizeInBytes / bytesPerSample;
	//short* buffer = new short[bufferSize];
	//unsigned int bufferIndex = 0;

	nChannels = (int)inFile->getNumChannels();
	assert(nChannels > 0);
	buffSizeSamples = BUFF_SIZE / nChannels;

	int totalNum = 0;

	// Process samples read from the input file
	while (inFile->eof() == 0)
	{
		int num;

		// Read a chunk of samples from the input file
		num = inFile->read(sampleBuffer, BUFF_SIZE);
		nSamples = num / (int)inFile->getNumChannels();
		totalNum += num;

		// Feed the samples into SoundTouch processor
		pSoundTouch->putSamples(sampleBuffer, nSamples);

		// Read ready samples from SoundTouch processor & write them output file.
		// NOTES:
		// - 'receiveSamples' doesn't necessarily return any samples at all
		//   during some rounds!
		// - On the other hand, during some round 'receiveSamples' may have more
		//   ready samples than would fit into 'sampleBuffer', and for this reason 
		//   the 'receiveSamples' call is iterated for as many times as it
		//   outputs samples.
		do 
		{
			nSamples = pSoundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
			int numElems = nSamples*nChannels;
			//write(sampleBuffer, numElems, buffer+bufferIndex, bufferSize-bufferIndex);
			//bufferIndex+= numElems;
			wavOut->write(sampleBuffer, nSamples * nChannels);
		} while (nSamples != 0);
	}

	// Now the input file is processed, yet 'flush' few last samples that are
	// hiding in the SoundTouch's internal processing pipeline.
	pSoundTouch->flush();
	do 
	{
		nSamples = pSoundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
		wavOut->write(sampleBuffer, nSamples * nChannels);
		//int numElems = nSamples*nChannels;
		//write(sampleBuffer, numElems, buffer+bufferIndex, bufferSize-bufferIndex);
		//bufferIndex+= numElems;
	} while (nSamples != 0);

	//wavOut->PushStream((char*)buffer, bufferIndex*sizeof(short)/sizeof(char));
	//SAFE_DELETE_ARR(buffer);
}

DWORD WINAPI ThreadMain(VOID* lpParam)
{
	WaveOut* pWavOut = (WaveOut*)lpParam;
	if(pWavOut == NULL)
		return -1;

	LARGE_INTEGER frequency, currentCounter, lastCounter;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&currentCounter);

	float passTime = 0.0f, deltaTime = 0.0f, deltaPassTime = 0.0f;
	float fps = 240;
	int nWaitTime = (int)(1000 / fps);
	while (WaitForSingleObject(pWavOut->m_hEvent, nWaitTime) == WAIT_TIMEOUT)
	{
		lastCounter = currentCounter;
		QueryPerformanceCounter(&currentCounter);
		deltaTime = (float)((double)(currentCounter.QuadPart - lastCounter.QuadPart) / frequency.QuadPart);
		passTime += deltaTime;
		//pWavOut->Update();
	}

	return 0;
}

#define WAV_OUT_MAX (44100)

WaveOut::WaveOut()
{
	hwo = NULL;
	memset(&wfx, 0, sizeof(wfx));
	m_bIsWaveOutWrite = false;

	memset(m_arrWaveHdr, 0, sizeof(m_arrWaveHdr));
	m_queWaveHdruffer.push(&m_arrWaveHdr[0]);
	m_queWaveHdruffer.push(&m_arrWaveHdr[1]);

	m_arrWaveHdr[0].lpData = new char[WAV_OUT_MAX];
	m_arrWaveHdr[1].lpData = new char[WAV_OUT_MAX];

	m_pWavInFile = NULL;

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD dwThread = 0;
	m_hThread = CreateThread(NULL, 0, ThreadMain, this, 0, &dwThread);
}

WaveOut::~WaveOut()
{
	SetEvent(m_hEvent);
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	m_hThread = INVALID_HANDLE_VALUE;
	CloseHandle(m_hEvent);
	m_hEvent = INVALID_HANDLE_VALUE;

	Stop();
	
	SAFE_DELETE_ARR(m_arrWaveHdr[0].lpData);
	SAFE_DELETE_ARR(m_arrWaveHdr[1].lpData);
	SAFE_DELETE(m_pWavInFile);
}

void WaveOut::Play(char* szName)
{
	SAFE_DELETE(m_pWavInFile);
	m_pWavInFile = new Wave(szName);
	m_pWavInFile->setPitch(0.5f);
}

void WaveOut::Play(Wave* wave)
{
	SAFE_DELETE(m_pWavInFile);
	m_pWavInFile = wave;

	WAVEFORMATEX waveformat;
	memset(&waveformat, 0, sizeof(waveformat));
	waveformat.wFormatTag = WAVE_FORMAT_PCM; // WAVE_FORMAT_PCM;//���ò��������ĸ�ʽ
	waveformat.nChannels = m_pWavInFile->getNumChannels();//������Ƶ�ļ���ͨ������
	waveformat.nSamplesPerSec = m_pWavInFile->getSampleRate();//����ÿһ���������źͼ�¼ʱ������Ƶ��
	waveformat.nBlockAlign = m_pWavInFile->getBytesPerSample();//���ֽ�Ϊ��λ���ÿ����
	waveformat.wBitsPerSample = m_pWavInFile->getNumBits(); // ����ÿ��������λ��,��Ϊ8��16
	waveformat.nAvgBytesPerSec = waveformat.nSamplesPerSec*waveformat.nBlockAlign;//���������ƽ������������,��λbyte/s��
	waveformat.cbSize = 0;//������Ϣ�Ĵ�С

	bool bCreate = true;
	if (hwo != NULL)
	{
		waveOutPause(hwo);
		for (int i = 0; i < 2; ++i)
		{
			WAVEHDR& hdr = m_arrWaveHdr[i];
			waveOutUnprepareHeader(hwo, &hdr, sizeof(WAVEHDR));//��������
			m_queWaveHdruffer.push(&hdr);
		}

		if (memcmp(&waveformat, &wfx, sizeof(WAVEFORMATEX)) == 0)
		{
			//���Ÿ�ʽ����ǰһ�� ����Ҫ���´���
			OutputDebugString(L"�������´���\n");
			bCreate = false;
		}
		else
		{
			// ���´���
			OutputDebugString(L"���´���\n");
			waveOutPause(hwo);
			waveOutClose(hwo);
			hwo = NULL;
		}
	}

	if(bCreate)
	{
		memcpy(&wfx, &waveformat, sizeof(WAVEFORMATEX));
		//��һ�������Ĳ�����Ƶ���װ���������������ţ���ʽΪ�ص�������ʽ�������ǶԻ�������ܹ��������������Ϊ(DWORD)this����������Demo��������
		MMRESULT mmresult = waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD)WaveCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);
		switch (mmresult)
		{
		case MMSYSERR_ALLOCATED:
			OutputDebugString(L"MMSYSERR_ALLOCATED");
			break;
		case MMSYSERR_BADDEVICEID:
			OutputDebugString(L"MMSYSERR_BADDEVICEID");
			break;
		case MMSYSERR_NODRIVER:
			OutputDebugString(L"MMSYSERR_NODRIVER");
			break;
		case MMSYSERR_NOMEM:
			OutputDebugString(L"MMSYSERR_NOMEM");
			break;
		case WAVERR_BADFORMAT:
			OutputDebugString(L"WAVERR_BADFORMAT");
			break;
		case  WAVERR_SYNC:
			OutputDebugString(L"WAVERR_SYNC");
			break;
		default:
			break;
		}
	}

	while(m_queWaveHdruffer.size() > 0)
	{
		LPWAVEHDR lpWaveHdr = m_queWaveHdruffer.front(); 
		if (FillDataToWaveHdr(*lpWaveHdr))
		{
			waveOutPrepareHeader(hwo, lpWaveHdr, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
			waveOutWrite(hwo, lpWaveHdr, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������ָ�������ݣ�Ҳ�൱�ڿ���һ������������˼
			m_queWaveHdruffer.pop();
		}
		else
		{
			break;
		}
	}
	waveOutRestart(hwo);
}

void WaveOut::Stop()
{
	if (hwo != NULL)
	{
		do 
		{
			waveOutPause(hwo);
			Sleep(1);
		} while (m_bIsWaveOutWrite);
		
		while(m_queWaveHdruffer.size() > 0)
		{
			m_queWaveHdruffer.pop();
		}
		for (int i = 0; i < 2; ++i)
		{
			WAVEHDR& hdr = m_arrWaveHdr[i];
			waveOutUnprepareHeader(hwo, &hdr, sizeof(WAVEHDR));//��������
			m_queWaveHdruffer.push(&hdr);
		}
		waveOutClose(hwo);
		hwo = NULL;
	}
}

void WaveOut::SetPitch(float pitch)
{
	m_fPitch = pitch;
}

void WaveOut::SetPlaybackRate(float playbackRate)
{
	if (hwo != NULL)
	{
		DWORD dwPlaybackRate = DWORD(playbackRate * 0x10000);
		waveOutSetPlaybackRate(hwo, dwPlaybackRate);
	}
}

bool WaveOut::FillDataToWaveHdr(WAVEHDR& wavehdr)
{
	unsigned int readSize = m_pWavInFile->readBuffer(wavehdr.lpData, WAV_OUT_MAX, 1.2f);
	wavehdr.dwBufferLength = readSize;
	return readSize > 0;
}

void WaveOut::PushStream(float* data, unsigned int dataSize)
{
	m_pWavInFile->write(data, dataSize);
}

void WaveOut::PushStream(char* data, unsigned int dataSize)
{
	m_pWavInFile->write(data, dataSize);
}

DWORD CALLBACK WaveOut::WaveCallback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	WaveOut* pWaveOut = (WaveOut*)dwInstance;
	if (pWaveOut == nullptr)
		return 0;
	switch (uMsg)
	{
	case WIM_OPEN:
		break;
	case WOM_DONE:
		if(1)
		{
			pWaveOut->m_bIsWaveOutWrite = true;
			LPWAVEHDR pWaveHeader = (LPWAVEHDR)dwParam1;
			if (pWaveOut->FillDataToWaveHdr(*pWaveHeader))
			{
				waveOutPrepareHeader(pWaveOut->hwo, pWaveHeader, sizeof(WAVEHDR));
				waveOutWrite(pWaveOut->hwo, pWaveHeader, sizeof(WAVEHDR));
			}
			else
			{
				pWaveOut->m_queWaveHdruffer.push(pWaveHeader);
			}
			pWaveOut->m_bIsWaveOutWrite = false;
		}
		break;
	case WOM_CLOSE:
		if (1)
		{
			OutputDebugString(TEXT("�豸�Ѿ��ر�...\n"));
			int a = 0;
		}
		break;
	default:
		break;
	}
	return 0;
}

void WaveOut::Update()
{
	do
	{
		if (hwo == NULL)
			break;

		while(m_queWaveHdruffer.size() > 0)
		{
			LPWAVEHDR lpWaveHdr = m_queWaveHdruffer.front(); 
			if (FillDataToWaveHdr(*lpWaveHdr))
			{
				waveOutPrepareHeader(hwo, lpWaveHdr, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
				waveOutWrite(hwo, lpWaveHdr, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������ָ�������ݣ�Ҳ�൱�ڿ���һ������������˼
				m_queWaveHdruffer.pop();
			}
			else
			{
				break;
			}
		}
	}
	while(false);
}

void GetWavInfo(char * filename, WAV_INFO * wav_info)
{
	FILE* file;
	int16_t blockAlign = 0, bitsPerSample = 0;
	int32_t bytesPerSec = 0, len = 0, data32 = 0;
	char tag[5];
	(*wav_info).szName = filename;

	/*******************���ļ���ȡ*************************/
	fopen_s(&file, filename, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "����Ҫ��ȡ�Ĵ��ļ�\n");
	}

	fseek(file, 0, SEEK_END);
	uint32_t filelen = ftell(file);
	if (filelen <= 44)
	{
		fclose(file);
		file = nullptr;
		return;
	}

	char *bpData = new char[44];
	memset(bpData, 0, sizeof(char)*44);
	rewind(file);
	fread(bpData, 1, 44, file);
	char *bpWaveData = new char[filelen - 44];
	fread(bpWaveData, 1, filelen - 44, file);
	fclose(file);
	file = nullptr;

	/***************�ж��Ƿ�Ϊ��Ч��wav�ļ�*******************
	*********************************************************/
	bool bIsError = false;
	bool bIsLoop = false;// �Ƿ�ѭ��
	do
	{
		// ƫ�� 
		int nOffset = 0;
		// �ֽ�
		int nSizeBytes = 0;
		// Parse "RIFF" string (chars 0-3)
		nOffset = 0;
		memset(tag, 0, sizeof(tag));
		memcpy(tag, bpData, 4);
		if (strcmp(tag, "RIFF") != 0)
		{
			bIsError = true;
			fprintf(stderr, "����һ��RIFF�ļ�\n");
			break;
		}

		// ����һ����ַ���ļ�β�����ֽ���
		nOffset = 4;
		len = *((int32_t*)(bpData + nOffset));

		//Parse "WAVE" string len (chars 8-11)
		nOffset = 8;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);
		if (strcmp(tag, "WAVE") != 0)
		{
			bIsError = true;
			fprintf(stderr, "����һ����ȷ��wave�ļ�\n");
			break;
		}

		//Parse "fmt" string  (chars 12-15)
		nOffset = 0x0c;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);
		if (strcmp(tag, "fmt ") != 0)
		{
			bIsError = true;
			fprintf(stderr, "������wave�ļ����ҵ���ʽ\n");
			break;
		}
		// �����ֽڣ�һ��Ϊ00000010H��
		nOffset = 0x10;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);

		// ��ʽ���ֵࣨΪ1ʱ����ʾ����Ϊ����PCM���룩
		nOffset = 0x14;
		unsigned short nPCM = *(unsigned short*)(bpData + nOffset);
		(*wav_info).wFormatTag = (short)nPCM;

		// ͨ������������Ϊ1��˫����Ϊ2
		nOffset = 0x16;
		unsigned short nChan = *(unsigned short*)(bpData + nOffset);
		(*wav_info).nChannels = nChan;

		// ����Ƶ�� 4�ֽ� ������
		//Parse frequency (chars 24-27)
		nOffset = 0x18;
		uint32_t nSamplesPerSec = *(uint32_t*)(bpData + nOffset);
		wav_info->nSamplesPerSec = nSamplesPerSec;

		// �������ݴ������ʣ�ÿ��ƽ���ֽ�����4�ֽ� ������
		//Parse bytesPerSec (chars 28-31)
		nOffset = 0x1c;
		uint32_t bytesPerSec = *(uint32_t*)(bpData + nOffset);
		(*wav_info).nAvgBytesPerSec = bytesPerSec;

		// DATA���ݿ鳤�ȣ��ֽڡ� 2�ֽ�����
		//(chars 32--33)
		nOffset = 0x20;
		uint16_t blockAlign = *(uint16_t*)(bpData + nOffset);
		(*wav_info).nBlockAlign = blockAlign;

		// PCMλ�� 2�ֽ� ����
		//(chars 34--35)
		nOffset = 0x22;
		uint16_t BitsPerSample = *(uint16_t*)(bpData + nOffset);
		wav_info->wBitsPerSample = BitsPerSample;
		// ��fact��,�ò���һ���ǿ�ѡ���֣��������У�����û��,һ�㵽WAV�ļ���ĳЩ���ת������ʱ�������ⲿ�֡� 4�ֽ� 4�ַ�
		// size, ��ֵΪ4 4�ֽ� ������
		nOffset = 0x24;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);
		uint32_t size = *(uint32_t*)(bpData + nOffset);

		nOffset = 0x28;
		uint32_t dataSize = *(uint32_t*)(bpData + nOffset);
		if (dataSize > filelen - 44)
		{
			bIsError = true;
			fprintf(stderr, "������wave�ļ����ҵ���ʽ\n");
			break;
		}
		wav_info->dataSize = dataSize;
		wav_info->data = bpWaveData;

		nOffset = 0x2c;
		int a = 0;
	} while (bIsLoop);

	if (bIsError)
	{
		delete[] bpWaveData;
	}
	delete[] bpData;
	bpData = NULL;
}
