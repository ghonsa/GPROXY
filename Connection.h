// Connection.h: interface for the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTION_H__B576EFC3_1E48_4557_8758_BCC5E4DCB416__INCLUDED_)
#define AFX_CONNECTION_H__B576EFC3_1E48_4557_8758_BCC5E4DCB416__INCLUDED_
#include "windows.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ParserLib.h"
#include "HTTPHeader.h"
	


class CConnection  
{
	enum ConnectState {
		FREE,				//  00=Connection is free
		CONNECT,			//  01= connect process
		REQUEST,			//  02= request process
		FILTER,				//  03= filter process
		FORWARD,			//  04= forward process
		RESPONCE,			//  04= responce process
		WAIT,				//  05= waiting- keepalive?
	};
	enum ProxyState {
		INITIAL,
		PROTOCOL,
		SECURITY,
		SIGNON,
		UPANDRUNNING
	};

public:

	CConnection(SOCKET sock,SOCKADDR_IN servSA);
	virtual ~CConnection();

	SOCKET GetSock();
	DWORD Continue();
	void ProcessBuffer(CLinkBuff* buff);

private:
	bool m_bKeepAlive;
	int ReleaseHeader();
	bool m_bRelease;

	BOOL ConnectServer();			// Connect to the cache array...   

	BOOL ProcessClientRecieve(CLinkBuff* buff); // where we look at requests from client
	BOOL ProcessServerRecieve(CLinkBuff* buff); // where we look at responces from cache array

	char * ApplyFilter(char *); // filter functions...
	
	// Link buffers functions for server and client sends and recieves

	CLinkBuff * GetNextClientSend();
	CLinkBuff * GetNextServerSend();
	CLinkBuff * GetNextClientRecv();
	CLinkBuff * GetNextServerRecv();
	BOOL AddClientRecieve(CLinkBuff* buffer);
	BOOL AddServerRecieve(CLinkBuff* buffer);
	BOOL AddClientSend(CLinkBuff* buffer);
	BOOL AddServerSend(CLinkBuff* buffer);
	
	int PushBackListHead(CLinkBuff** listp,CLinkBuff* buff)	;
	CLinkBuff * GetListHead(CLinkBuff** listp);
	CLinkBuff * GetListTail(CLinkBuff** listp);
	
	BOOL AddListTail(CLinkBuff** listp,CLinkBuff* buff);

	char * m_currentHeader;
	bool m_bIsTextHtml;
	long m_docSize;
	char * m_currBuffer;
		
	SOCKET m_clientSock;
	CLinkBuff * m_clientSendList;
	CLinkBuff * m_clientRecvList;
	
	SOCKET m_servSock;
	SOCKADDR_IN m_servAddr;
	CLinkBuff * m_servSendList;
	CLinkBuff * m_servRecvList;
	CHTTPHeader m_header;
	
	BOOL m_bRequestMade;
	BOOL m_bCloseServerConnection;
	BOOL m_bGetHeader;
	BOOL m_bRunning;
	BOOL m_bReadHeader;
	BOOL m_bReadingHeader;
	BOOL m_bServerConnected;
	BOOL m_bKillBuff;
	
	DWORD m_dwThreadID;
	HANDLE m_hThread;

	HtmlDocument * TheDoc;
};

#endif // !defined(AFX_CONNECTION_H__B576EFC3_1E48_4557_8758_BCC5E4DCB416__INCLUDED_)
