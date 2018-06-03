
// WaveEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WaveEdit.h"
#include "WaveEditDlg.h"
#include "afxdialogex.h"
#pragma comment(lib, "Fast_Communication.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWaveEditDlg 对话框




CWaveEditDlg::CWaveEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWaveEditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWaveEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CWaveEditDlg::PushWaveOutStream(char* _data, unsigned int _dataSize)
{
	m_waveOut.PushStream(_data, _dataSize);
}

void CWaveEditDlg::WaveInStream(char* _data, unsigned int _dataSize)
{
	WAVEFORMATEX wfx = m_waveIn.GetWaveFormat();
	m_nWavSize = WavToFloat( _data, _dataSize, m_pWavBuffer, MAX_BUFFER,wfx.wBitsPerSample);
	m_nBufferSize = WaveSoundTouch(m_pWavBuffer, m_nWavSize, m_pTempBuffer, MAX_BUFFER, m_fPitch, 1, wfx.nChannels, wfx.nSamplesPerSec, wfx.wBitsPerSample);
	int wavSize = FloatToWav(m_pTempBuffer, m_nBufferSize, pTemp, MAX_BUFFER, wfx.wBitsPerSample);
	if(m_pSocket)
		m_pSocket->Send(0, pTemp, wavSize);

#ifdef _DEBUG
	m_waveOut.PushStream(m_pTempBuffer, m_nBufferSize);
#endif
}

void CWaveEditDlg::LoadConfig()
{
	char szConfigPath[MAX_PATH] = ".\\config.ini";
	char szAppName[MAX_PATH] = "APPLICATION";
	m_nWavOffsetY = GetPrivateProfileIntA(szAppName, "WavOffsetY", 660, szConfigPath);
	m_nWavHeight = GetPrivateProfileIntA(szAppName, "WavHeight", 350, szConfigPath);
	GetPrivateProfileStringA(szAppName, "IP", "127.0.0.1",m_szIp, 512, szConfigPath);
	m_nPort = GetPrivateProfileIntA(szAppName, "PORT", 10000, szConfigPath);

	strcpy_s(szAppName, "PCI_INPUT");
	char szTemp[128] = {};
	for (int i = 0; i < PCI_MAX; ++i)
	{
		memset(szTemp, 0 ,sizeof(szTemp));
		sprintf_s(szTemp, "PciPort%d", i);
		m_arrPciPort[i] = GetPrivateProfileIntA(szAppName, szTemp, i, szConfigPath);
	}

	memset(m_arrPitch, 0, sizeof(m_arrPitch));
	strcpy_s(szAppName, "PITCH");
	char szBuffer[128] = {};
	for (int i = 0; i < PCI_MAX; ++i)
	{
		memset(szTemp, 0 ,sizeof(szTemp));
		memset(szBuffer, 0 ,sizeof(szBuffer));
		sprintf_s(szTemp, "pitch%d", i);
		GetPrivateProfileStringA(szAppName, szTemp, "1.0", szBuffer,128, szConfigPath);
		sscanf_s(szBuffer, "%f", &m_arrPitch[i]);
	}
}

BEGIN_MESSAGE_MAP(CWaveEditDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

u32 CALLBACK CallBackTcpRecv( SOCKET sock, char* cRecv, int nRecvSize, char* cSend, void* pUserdata)
{
	CWaveEditDlg* pWavDlg = (CWaveEditDlg*)pUserdata;
	if(pWavDlg != NULL)
	{
		pWavDlg->PushWaveOutStream(cRecv, nRecvSize);
	}
	return 0;
}


// CWaveEditDlg 消息处理程序

BOOL CWaveEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	LoadConfig();

	igfInitSocket();

	token = NULL;
	Gdiplus::GdiplusStartupInput input = nullptr;
	Gdiplus::GdiplusStartup(&token, &input, nullptr);

	m_pGpBitmap = GpBitmap::FromFile(L"奇趣电话亭bg.jpg");


	m_fPitch = m_arrPitch[0];
	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SCROLLBAR_PITCH);
	pSliderCtrl->SetRange(-150, 150);
	pSliderCtrl->SetPos(int(m_fPitch*10.0f + 0.5f));
	SetDlgItemText(IDC_EDIT_PITCH, _bstr_t(m_fPitch));

	const WAVEFORMATEX& waveformat = m_waveIn.GetWaveFormat();
	Wave* wave = new Wave(waveformat.nSamplesPerSec, waveformat.wBitsPerSample, waveformat.nChannels);
	m_waveOut.Play(wave);

	m_waveIn.m_pOwner = this;
	m_waveIn.StartRecord(0);

	m_pTempBuffer = new float[MAX_BUFFER];
	m_nBufferSize = 0;
	m_pWavBuffer = new float[MAX_BUFFER];
	m_nWavSize = 0;
	pTemp = new char[MAX_BUFFER];
	m_pSocket = new igfSocket(m_szIp, m_nPort,  0, 1000, this, CallBackTcpRecv);

	SetTimer(0, 35, NULL);
	SetWindowText(m_pSocket->HasServer() ? L"服务器" : L"客户端" );
#ifndef _DEBUG
	DWORD dwStyle =GetStyle();//获取旧样式  
	DWORD dwNewStyle =WS_OVERLAPPED |WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	dwNewStyle&=dwStyle;//按位与将旧样式去掉
	SetWindowLong(m_hWnd,GWL_STYLE,dwNewStyle);//设置成新的样式
	DWORD dwExStyle=GetExStyle();//获取旧扩展样式
	DWORD dwNewExStyle=WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR;
	dwNewExStyle&=dwExStyle;//按位与将旧扩展样式去掉
	SetWindowLong(m_hWnd,GWL_EXSTYLE,dwNewExStyle);//设置新的扩展样式
	SetWindowPos(NULL,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);//告诉windows：我的样式改变了，窗口位置和大小保持原来不变!
	ShowWindow(SW_MAXIMIZE);
	ShowCursor(FALSE);

	GetDlgItem(IDC_STATIC0)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SCROLLBAR_PITCH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SCROLLBAR_RATE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PITCH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_RATE)->ShowWindow(SW_HIDE);
#endif

	m_pciDeviceIO.CreateDevice();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWaveEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWaveEditDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWaveEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWaveEditDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0)
	{
		m_pciDeviceIO.UpdateIO();
		for (int i = 0; i < PCI_MAX; ++i)
		{
			if (m_pciDeviceIO.GetInputClicked(m_nPort))
			{
				m_fPitch = m_arrPitch[i];
			}
		}

		m_waveOut.Update();

#ifdef _DEBUG
		CWnd* imageWnd = GetDlgItem(IDC_IMAGE);
#else
		CWnd* imageWnd = this;
#endif
		CRect clientRect;
		imageWnd->GetClientRect(clientRect);

		GpBitmap gpBitmap(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
		GpRect gpRect(0, 0, gpBitmap.GetWidth(), gpBitmap.GetHeight());
		GpGraphics gpGraphics(&gpBitmap);
		GpSolidBrush gpSolidBrush(GpColor(0, 0, 0));
		gpGraphics.FillRectangle(&gpSolidBrush, gpRect);

		int nWidth = m_pGpBitmap->GetWidth();
		gpGraphics.DrawImage(m_pGpBitmap, 0, 0, gpBitmap.GetWidth(), gpBitmap.GetHeight());
		
		GpPen gpPen(GpColor(0xff, 0xff, 0xff), 2);
		GpPointF gpStartPoint(0, 3*clientRect.Height()/4);
		GpPointF gpEndPoint(0, 3*clientRect.Height()/4);
		gpEndPoint.X = clientRect.Width();
		float height = clientRect.Height()/2;

		//// 最原始的绘制在上面
		//if(m_nWavSize > 0)
		//{
		//	float offsetY = clientRect.Height()/4.0f;
		//	gpStartPoint.Y = offsetY;
		//	gpEndPoint.Y = offsetY;
		//	gpGraphics.DrawLine(&gpPen, gpStartPoint, gpEndPoint);
		//	float step = (float)clientRect.Width()/m_nWavSize;
		//	for (int i = 0; i < m_nWavSize; ++i)
		//	{
		//		gpStartPoint.X = i * step;
		//		gpEndPoint.X = gpStartPoint.X;
		//		gpEndPoint.Y = m_pWavBuffer[i] * height + offsetY;
		//		gpGraphics.DrawLine(&gpPen, gpStartPoint, gpEndPoint);
		//	}
		//}

		if(m_nBufferSize > 0)
		{
			height = m_nWavHeight * (clientRect.Height()/1080.0f);
			int offsetY = m_nWavOffsetY / 1080.0f * clientRect.Height();
			gpStartPoint.Y = gpEndPoint.Y = offsetY ;
			gpStartPoint.X = 0;
			gpEndPoint.X = clientRect.Width();
			gpGraphics.DrawLine(&gpPen, gpStartPoint, gpEndPoint);
			float step = (float)clientRect.Width()/m_nBufferSize;
			for (int i = 0; i < m_nBufferSize; ++i)
			{
				gpStartPoint.X = i * step;
				gpEndPoint.X = gpStartPoint.X;
				gpEndPoint.Y = m_pTempBuffer[i]* height  + offsetY;

				gpGraphics.DrawLine(&gpPen, gpStartPoint, gpEndPoint);
			}
		}
		GpBitmap* pCurBitmap = &gpBitmap;
		CDC* cdc = imageWnd->GetDC();
		GpGraphics gpChachedGraphics(cdc->m_hDC);
		GpCachedBitmap gpChachedBitmap(pCurBitmap, &gpChachedGraphics);
		gpChachedGraphics.DrawCachedBitmap(&gpChachedBitmap, 0, 0);
		ReleaseDC(cdc);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CWaveEditDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认* pScrollBar = (pScrollBar)
	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)pScrollBar;
	int scroll = pSliderCtrl->GetPos();
	switch (pSliderCtrl->GetDlgCtrlID())
	{
	case IDC_SCROLLBAR_PITCH:
		{
			m_fPitch = scroll/10.0f;
			SetDlgItemText(IDC_EDIT_PITCH, _bstr_t(m_fPitch));
		}
		break;
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CWaveEditDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	SAFE_DELETE(m_pSocket);

	igfUnInitSocket();

	SAFE_DELETE(m_pGpBitmap);

	Gdiplus::GdiplusShutdown(token);

	m_waveIn.StopRecord();
	m_waveOut.Stop();

	SAFE_DELETE_ARR(m_pTempBuffer);
	m_nBufferSize = 0;

	SAFE_DELETE_ARR(m_pWavBuffer);
	m_nWavSize = 0;

	SAFE_DELETE_ARR(pTemp);

	m_pciDeviceIO.ReleaseDevice();
}
