
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif
#include "WaveHead.h"

#include "targetver.h"
#include "resource.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持








#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


typedef unsigned __int32 uint32_t;
typedef unsigned __int16 uint16_t;
typedef  __int16 int16_t;
typedef  __int32 int32_t;

typedef struct
{
	char *szName;     // wav文件名
	short wFormatTag;				//编码类型标示PCM(0001)或 ADPCM(0002)	2字节
	short nChannels;			//声道数	2字节
	unsigned long nAvgBytesPerSec;	//每秒记录数据量字节数	4字节
	short	nBlockAlign;			//采样点占字节大小	2字节 wBitsPerSample*nChannels/8
	short  wBitsPerSample;			//采样位数	2字节 8表示单通道 16双通道
	long nSamplesPerSec;    // 总共的样本点数 每秒采样率
	uint32_t dataSize;	//"DATA"块大小	4字节
	char *data;		//存放采样数据
}WAV_INFO;
