#pragma once

#ifndef IGF_SOCKET_H
#define IGF_SOCKET_H
#include <string>
#include <vector>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#ifndef path
typedef std::string path;
#endif

#ifndef stringc
typedef std::string stringc;
#endif

#ifndef u32
typedef unsigned int u32;
#endif



// 回调函数原型(在程序中实现函数功能)
// 服务器检测到客户端连接触发
// 客户端检测到连接服务器成功触发
typedef void (CALLBACK *LPCONNECT)( SOCKET sock );
// 服务器检测到客户端连接断开触发
// 客户端检测到服务器关闭触发
typedef void (CALLBACK *LPDISCONNECT)( SOCKET sock );
// 先接收后发送过程
// cRecv传入时为接收到的数据|回调中为cSend填充要发送的数据
// 返回实际发送的字节数
// 如接收数据后不需要再发送响应数据回调中设返回值为0且不为cSend拷贝数据
typedef u32 (CALLBACK *LPPROC)( SOCKET sock, char* cRecv, int nRecvSize, char* cSend, void* pUserdata);

// 加载|卸载套接字模块
bool WINAPI igfInitSocket();
void WINAPI igfUnInitSocket();

path WINAPI igfSocket2IP( SOCKET sock );

class igfSocket
{
public:
	igfSocket( path ipaddr, u32 portid, u32 sendbuffersize, u32 recvbuffersize, void* userdata,
		LPPROC pProc, LPCONNECT pConnect = NULL, LPDISCONNECT pDisconnect = NULL );
	~igfSocket();

	bool HasServer()	{ return m_bIsServer; }

	SOCKET IP2Socket( path ipaddr );

public:
	bool GetIsServer() { return m_bIsServer; }
	bool Send( SOCKET sock, char* cSend, u32 len );
private:
	friend DWORD WINAPI ThreadClientFunc( LPVOID lParam );
	friend DWORD WINAPI ThreadServerFunc( LPVOID lParam );




private:
	HANDLE				m_hThread;
	bool				m_bIsServer;

	LPCONNECT			connectfunc;
	LPDISCONNECT		disconnectfunc;
	LPPROC				procfunc;

	stringc				ip;						// 服务器地址
	u32					port;					// 端口号
	u32					sendsize;				// 一次发送最大字节数
	u32					recvsize;				// 一次接收的字节数|收满则触发回调
	bool				isThreadRun;

	fd_set				fdSocket;				// 套接字集合
	void*				m_pUserData;

	SOCKET				m_socket;

	std::vector<SOCKET> m_vecSocket;
};

#endif