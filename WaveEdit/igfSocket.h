#pragma once

#ifndef IGF_SOCKET_H
#define IGF_SOCKET_H

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <string>

#ifndef u32
typedef unsigned int u32;
#endif

#ifndef u32
typedef unsigned int u32;
#endif

#ifndef path
typedef std::string path;
#endif

#ifndef stringc
typedef std::string stringc;
#endif
// �ص�����ԭ��(�ڳ�����ʵ�ֺ�������)
// ��������⵽�ͻ������Ӵ���
// �ͻ��˼�⵽���ӷ������ɹ�����
typedef void (CALLBACK *LPCONNECT)( SOCKET sock );
// ��������⵽�ͻ������ӶϿ�����
// �ͻ��˼�⵽�������رմ���
typedef void (CALLBACK *LPDISCONNECT)( SOCKET sock );
// �Ƚ��պ��͹���
// cRecv����ʱΪ���յ�������|�ص���ΪcSend���Ҫ���͵�����
// ����ʵ�ʷ��͵��ֽ���
// ��������ݺ���Ҫ�ٷ�����Ӧ���ݻص����践��ֵΪ0�Ҳ�ΪcSend��������
typedef u32 (CALLBACK *LPPROC)( SOCKET sock, char* cRecv, int nRecvSize, char* cSend, void* userdata);

// ����|ж���׽���ģ��
bool WINAPI igfInitSocket();
void WINAPI igfUnInitSocket();

path WINAPI igfSocket2IP( SOCKET sock );

class igfSocket
{
public:
	igfSocket( path ipaddr, u32 portid, u32 sendbuffersize, u32 recvbuffersize, void* userdata,
		LPPROC pProc, LPCONNECT pConnect = NULL, LPDISCONNECT pDisconnect = NULL );
	~igfSocket();

	LPCONNECT			connectfunc;
	LPDISCONNECT		disconnectfunc;
	LPPROC				procfunc;

	stringc				ip;						// ��������ַ
	u32					port;					// �˿ں�
	u32					sendsize;				// һ�η�������ֽ���
	u32					recvsize;				// һ�ν��յ��ֽ���|�����򴥷��ص�
	bool				isThreadRun;

	fd_set				fdSocket;				// �׽��ּ���

	bool HasServer()	{ return m_isServer; }

	SOCKET IP2Socket( path ipaddr );

	bool Send( SOCKET sock, char* cSend, u32 len );

private:
	HANDLE				m_hThread;
	bool				m_isServer;
public:
	void*				m_userdata;
};

#endif