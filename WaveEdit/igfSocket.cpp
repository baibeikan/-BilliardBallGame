#include "stdafx.h"
#include "igfSocket.h"

//-----------------------------------------------------------------------------
// 全局函数
//-----------------------------------------------------------------------------
bool WINAPI igfInitSocket()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 1, 1 );
	int err = WSAStartup( wVersionRequested, &wsaData );
	if( err != 0 )
		return false;

	if( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 )
	{
		WSACleanup();
		return false;
	}

	return true;
}

void WINAPI igfUnInitSocket()
{
	WSACleanup();
}

path WINAPI igfSocket2IP( SOCKET sock )
{
	SOCKADDR_IN addr = {0};
	int len = sizeof( addr );
	if( getpeername( sock, (SOCKADDR*)&addr, &len ) != 0 )
		return path("");

	return path(inet_ntoa( addr.sin_addr ));
}

//-----------------------------------------------------------------------------
// 线程函数
//-----------------------------------------------------------------------------
DWORD WINAPI ThreadServerFunc( LPVOID lParam )
{
	igfSocket* ss = (igfSocket*)lParam;
	FD_ZERO( &ss->fdSocket );

	int len = sizeof(SOCKADDR);
	SOCKET sockServer = socket( AF_INET, SOCK_STREAM, 0 );
	SOCKADDR_IN addrServer;
	addrServer.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons( ss->port );

	if( SOCKET_ERROR == bind( sockServer, (SOCKADDR*)&(addrServer), len ) )
		return -1;
	if( SOCKET_ERROR == listen( sockServer, FD_SETSIZE - 1 ) )	
		return -1;

	timeval	tm;
	tm.tv_sec = 1;
	tm.tv_usec = 0;
	FD_ZERO( &ss->fdSocket );
	FD_SET( sockServer, &ss->fdSocket );
	fd_set fdRead;
	while( ss->isThreadRun )
	{
		FD_ZERO( &fdRead );
		fdRead = ss->fdSocket;

		int ret = select( 0, &fdRead, NULL, NULL, &tm );
		if( SOCKET_ERROR == ret )
			break;

		for( u_int i = 0; i < (u_int)ss->fdSocket.fd_count; i++ )
		{
			if(!ss->isThreadRun)
				break;

			if( FD_ISSET( ss->fdSocket.fd_array[i], &fdRead ) )
			{
				// 监听套节字接收到新连接
				if( ss->fdSocket.fd_array[i] == sockServer )
				{
					// 最大连接数
					if( ss->fdSocket.fd_count < FD_SETSIZE )
					{
						// 套接字部分
						SOCKADDR_IN addrClient;
						SOCKET sockClient = accept( sockServer, (SOCKADDR*)&addrClient, &len );
						if( INVALID_SOCKET == sockClient )
							continue;
						FD_SET( sockClient, &ss->fdSocket );

						if( ss->connectfunc != NULL )
						{
							(*(ss->connectfunc))( sockClient );
						}
					}
				}
				else
				{
					if( ss->procfunc != NULL && ss->recvsize > 0 )
					{	
						char* rBuffer = new char[ss->recvsize];
						ZeroMemory( rBuffer, ss->recvsize );
						char* sBuffer = new char[ss->sendsize];
						ZeroMemory( sBuffer, ss->sendsize );
						u32 retSub = 0;
						do 
						{
							int ret = recv( ss->fdSocket.fd_array[i], rBuffer + retSub, ss->recvsize - retSub, 0 );
							if( ret <= 0 )
							{
								if( ss->disconnectfunc != NULL )
								{
									(*(ss->disconnectfunc))( ss->fdSocket.fd_array[i] );
								}
								shutdown( ss->fdSocket.fd_array[i], SD_BOTH );
								closesocket( ss->fdSocket.fd_array[i] );	
								FD_CLR( ss->fdSocket.fd_array[i], &ss->fdSocket );
								retSub = 0;
								break;
							}
							else
							{
								retSub += ret;
							}

							if(!ss->isThreadRun)
								break;
						}
						while( retSub < ss->recvsize && ss->isThreadRun );
						if( retSub == 0 || !ss->isThreadRun )
						{
							delete [] rBuffer;
							delete [] sBuffer;
							continue;
						}

						u32 len = (*(ss->procfunc))( ss->fdSocket.fd_array[i], rBuffer, retSub, sBuffer, ss->m_userdata);
						if( len <= 0 || !ss->isThreadRun )
						{
							delete [] rBuffer;
							delete [] sBuffer;
							continue;
						}
						
						retSub = 0;
						do 
						{
							int ret = send( ss->fdSocket.fd_array[i], sBuffer + retSub, ss->sendsize - retSub, 0 );
							if( ret <= 0 )
							{
								if( ss->disconnectfunc != NULL )
								{
									(*(ss->disconnectfunc))( ss->fdSocket.fd_array[i] );
								}
								shutdown( ss->fdSocket.fd_array[i], SD_BOTH );
								closesocket( ss->fdSocket.fd_array[i] );	
								FD_CLR( ss->fdSocket.fd_array[i], &ss->fdSocket );
								retSub = 0;
								break;
							}
							else
							{
								retSub += ret;
							}
							if(!ss->isThreadRun)
								break;
						}
						while( retSub < ss->sendsize && ss->isThreadRun );	

						delete [] rBuffer;
						delete [] sBuffer;
					}
				}
			}
		}
	}

	shutdown( sockServer, SD_BOTH );
	closesocket( sockServer );

	return 0;
}

DWORD WINAPI ThreadClientFunc( LPVOID lParam )
{
	igfSocket* ss = (igfSocket*)lParam;

	SOCKET sockClient = socket( AF_INET, SOCK_STREAM, 0 );
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr( ss->ip.c_str() );
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons( ss->port );

	FD_ZERO( &ss->fdSocket );
	FD_SET( sockClient, &ss->fdSocket );
	timeval	tm;
	tm.tv_sec = 1;
	tm.tv_usec = 0;
	fd_set set;
	int error = -1;
	int len = sizeof(int);
	unsigned long ul = 1; 
	ioctlsocket( sockClient, FIONBIO, &ul ); // 设置为非阻塞模式
	while( connect( sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR) ) == -1 && ss->isThreadRun )
	{
		FD_ZERO( &set );
		FD_SET( sockClient, &set );
		if( select( (int)sockClient+1, NULL, &set, NULL, &tm ) > 0 )
		{
			getsockopt( sockClient, SOL_SOCKET, SO_ERROR, (char*)(&error), (int*)&len );
			if( error == 0 )
				break;
		}
	}
	ul = 0; 
	ioctlsocket( sockClient, FIONBIO, &ul ); // 设置为阻塞模式

	if( ss->connectfunc != NULL )
	{
		(*(ss->connectfunc))( sockClient );
	}

	while( ss->isThreadRun )
	{
		if( ss->procfunc != NULL && ss->recvsize > 0 )
		{	
			char* rBuffer = new char[ss->recvsize];
			ZeroMemory( rBuffer, ss->recvsize );
			char* sBuffer = new char[ss->sendsize];
			ZeroMemory( sBuffer, ss->sendsize );					
			u32 retSub = 0;
			do 
			{
				int ret = recv( sockClient, rBuffer + retSub, ss->recvsize - retSub, 0 );
				if( ret <= 0 )
				{
					if( ss->disconnectfunc != NULL )
					{
						(*(ss->disconnectfunc))( sockClient );
					}
					shutdown( sockClient, SD_BOTH );
					closesocket( sockClient );
					ul = 1;
					sockClient = socket( AF_INET, SOCK_STREAM, 0 );
					ioctlsocket( sockClient, FIONBIO, &ul ); // 设置为非阻塞模式
					while( connect( sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR) ) == -1 && ss->isThreadRun )
					{
						FD_ZERO( &set );
						FD_SET( sockClient, &set );
						if( select( (int)sockClient+1, NULL, &set, NULL, &tm ) > 0 )
						{
							getsockopt( sockClient, SOL_SOCKET, SO_ERROR, (char*)(&error), (int*)&len );
							if( error == 0 )
								break;
						}
					}
					ul = 0; 
					ioctlsocket( sockClient, FIONBIO, &ul ); // 设置为阻塞模式
					if( ss->connectfunc != NULL && ss->isThreadRun )
					{
						(*(ss->connectfunc))( sockClient );
					}
					retSub = 0;
					break;
				}
				else
				{
					retSub += ret;
				}
			}
			while( retSub < ss->recvsize && ss->isThreadRun );
			if( retSub == 0 || !ss->isThreadRun )
			{
				delete [] rBuffer;
				delete [] sBuffer;
				continue;
			}

			u32 len = (*(ss->procfunc))( sockClient, rBuffer, retSub, sBuffer, ss->m_userdata );
			if( len <= 0 || !ss->isThreadRun )
			{
				delete [] rBuffer;
				delete [] sBuffer;
				continue;
			}

			retSub = 0;
			do 
			{
				int ret = send( sockClient, sBuffer + retSub, ss->sendsize - retSub, 0 );
				if( ret <= 0 )
				{
					if( ss->disconnectfunc != NULL )
					{
						(*(ss->disconnectfunc))( sockClient );
					}
					shutdown( sockClient, SD_BOTH );
					closesocket( sockClient );
					ul = 1;
					sockClient = socket( AF_INET, SOCK_STREAM, 0 );
					ioctlsocket( sockClient, FIONBIO, &ul ); // 设置为非阻塞模式
					while( connect( sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR) ) == -1 && ss->isThreadRun )
					{
						FD_ZERO( &set );
						FD_SET( sockClient, &set );
						if( select( (int)sockClient+1, NULL, &set, NULL, &tm ) > 0 )
						{
							getsockopt( sockClient, SOL_SOCKET, SO_ERROR, (char*)(&error), (int*)&len );
							if( error == 0 )
								break;
						}
					}
					ul = 0; 
					ioctlsocket( sockClient, FIONBIO, &ul ); // 设置为阻塞模式
					if( ss->connectfunc != NULL && ss->isThreadRun )
					{
						(*(ss->connectfunc))( sockClient );
					}
					retSub = 0;
					break;
				}
				else
				{
					retSub += ret;
				}
			}
			while( retSub < ss->sendsize && ss->isThreadRun );	

			delete [] rBuffer;
			delete [] sBuffer;
		}
	}

	if( ss->disconnectfunc != NULL )
	{
		(*(ss->disconnectfunc))( sockClient );
	}

	shutdown( sockClient, SD_BOTH );
	closesocket( sockClient );

	return 0;
}

igfSocket::igfSocket( path ipaddr, u32 portid, u32 sendbuffersize, u32 recvbuffersize, void* userdata,
	LPPROC pProc, LPCONNECT pConnect, LPDISCONNECT pDisconnect ) : 
	ip( ipaddr ), port( portid ), sendsize( sendbuffersize ), recvsize( recvbuffersize ),
		m_userdata(userdata),
	procfunc( pProc ), connectfunc( pConnect ), disconnectfunc( pDisconnect )
{
	isThreadRun = true;
	if( ip.size() == 0 )
	{
		m_hThread = CreateThread( NULL, 0, ThreadServerFunc, (LPVOID)this, 0, NULL );
		m_isServer = true;
	}
	else
	{
		m_hThread = CreateThread( NULL, 0, ThreadClientFunc, (LPVOID)this, 0, NULL );
		m_isServer = false;
	}
}

igfSocket::~igfSocket()
{
	isThreadRun = false;
	WaitForSingleObject( m_hThread, INFINITE );
}

SOCKET igfSocket::IP2Socket( path ipaddr )
{
	for( u_int i = 0; i < fdSocket.fd_count; i++ )
	{
		path ip = igfSocket2IP( fdSocket.fd_array[i] );
		if( ip == ipaddr )
		{
			return fdSocket.fd_array[i];
		}
	}

	return 0;
}

bool igfSocket::Send( SOCKET sock, char* cSend, u32 len )
{
	int ret = 0;
	if( m_isServer )
	{
		for (int i = 1; i < fdSocket.fd_count; ++i)
		{
			ret = send( fdSocket.fd_array[i], cSend, len, 0 );
		}
	}
	else
	{
		ret = send( fdSocket.fd_array[0], cSend, len, 0 );
	}

	return ( ret <= 0 ) ? false : true;
}
