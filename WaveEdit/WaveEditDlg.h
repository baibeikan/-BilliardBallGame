
// WaveEditDlg.h : ͷ�ļ�
//

#pragma once
#include "WaveOut.h"
#include "WaveIn.h"
#include "igfSocket.h"
#include "Fast_Communication.h"
#include "PCIDeviceIO.h"
#include "GdiPlusHead.h"

#define MAX_BUFFER (1024*1024)
#define PCI_MAX (4)

// CWaveEditDlg �Ի���
class CWaveEditDlg : public CDialogEx
{
// ����
public:
	CWaveEditDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WAVEEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
public:
	void PushWaveOutStream(char* _data, unsigned int _dataSize);

	void WaveInStream(char* _data, unsigned int _dataSize);

	void LoadConfig();

private:
	WaveOut m_waveOut;
	WaveIn m_waveIn;
	float m_fPitch;
	ULONG_PTR token;

	float* m_pTempBuffer;
	float* m_pWavBuffer;
	unsigned int m_nBufferSize;
	unsigned int m_nWavSize;
	char* pTemp;

	GpBitmap *m_pGpBitmap;

	int m_nWavOffsetY;
	int m_nWavHeight;
	int m_nPort;
	char m_szIp[512];

	igfSocket* m_pSocket;

	int m_arrPciPort[PCI_MAX];

	float m_arrPitch[PCI_MAX];

	PciDeviceIO m_pciDeviceIO;


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
};
