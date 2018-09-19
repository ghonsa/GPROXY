// LThread.cpp: implementation of the LThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LThread.h"
#include "stdio.h"
#include "Connection.h"


int opens=0;
extern void Gdump(int type,const char *szMsg, ... );
extern char * host;
extern int port;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DWORD WINAPI LThreadProc(VOID *lpParam)
{
	CLThread * pl=(CLThread*)lpParam;
	return pl->StartThread();


}
CLThread::CLThread()
{

}

CLThread::~CLThread()
{

}

DWORD CLThread::Start()
{

	m_hThread=CreateThread( NULL,         // SD
							0x2000000,    // 2 meg initial stack size
							LThreadProc,  // thread function
							this,         // thread argument
							0,            // creation option
							&m_dwThreadID   // thread identifier
							);
return 0;
}

DWORD CLThread::StartThread()
{
	SOCKADDR_IN addr;

	SOCKADDR_IN servAddr;
	SOCKADDR_IN clientAddr;
	int nClientAddrLen;
	IN_ADDR client_in;
	
	
	SOCKET sockClient;
	CConnection* conn;

	
	// lookup the server ip stuff
	{
		struct hostent* pHost;
				
		servAddr.sin_family= AF_INET;
		servAddr.sin_port= htons(port);
		
		if((servAddr.sin_addr.S_un.S_addr= inet_addr(host))==INADDR_NONE)
		{
			if((pHost=gethostbyname(host))==0)
			{
				exit(1);
			}
			memcpy(&servAddr.sin_addr,pHost->h_addr_list,4);
		}

	}
	Gdump(MSG_CONN,"Start Listen Thread\n");

	// create the socket
	m_sockListn =socket(AF_INET,SOCK_STREAM,0);
	if(m_sockListn==INVALID_SOCKET)
	{
		Gdump(MSG_ERROR,"Unable to allocate socket\n");
		return -1;
	}
	addr.sin_family= AF_INET;
	addr.sin_port= htons(PROXY_PORT);
	addr.sin_addr.S_un.S_addr= htonl(INADDR_ANY);
	
	// set the port for the socket
	if(bind(m_sockListn,(SOCKADDR *)&addr,sizeof(addr))==SOCKET_ERROR)
	{
		// bind to socket failed
		Gdump(MSG_ERROR,"Unable to bind socket to port %d\n",PROXY_PORT);
		return -1;
	}
	m_bRunning=true;
	// Wait for connections
	while(m_bRunning)
	{

		if(listen(m_sockListn,3)==SOCKET_ERROR)
		{
			// some error
			Gdump(MSG_ERROR,"Listen error on socket\n");
		}
		else
		{
			nClientAddrLen=sizeof(clientAddr);
			sockClient=accept(m_sockListn,(LPSOCKADDR)&clientAddr,&nClientAddrLen);
			if(sockClient==INVALID_SOCKET)
			{
				// another error
				Gdump(MSG_ERROR,"Accept failed\\n");
			}

			else
			{
				bool val =true;
			//	setsockopt(sockClient, SOL_SOCKET,SO_KEEPALIVE,*val,sizeof(bool));              
    
				memcpy(&client_in,&clientAddr.sin_addr.S_un.S_addr,4);
				Gdump(MSG_CONN,"accept-- Client IP:%s  Port:%d\n",
								inet_ntoa(client_in),
								ntohs(clientAddr.sin_port));
				// we now have someone connected.

				conn= new CConnection(sockClient,servAddr);

			}

		}



	}
	return 0;
}

void CLThread::Stop()
{
	m_bRunning=FALSE;
}

