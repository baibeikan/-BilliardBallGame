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
typedef u32 (CALLBACK *LPPROC)( SOCKET sock, char* cRecv, int nRecvSize, char* cSend, void* pUserdata);

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

	stringc				ip;						// ��������ַ
	u32					port;					// �˿ں�
	u32					sendsize;				// һ�η�������ֽ���
	u32					recvsize;				// һ�ν��յ��ֽ���|�����򴥷��ص�
	bool				isThreadRun;

	fd_set				fdSocket;				// �׽��ּ���
	void*				m_pUserData;

	SOCKET				m_socket;

	std::vector<SOCKET> m_vecSocket;
};

#endif