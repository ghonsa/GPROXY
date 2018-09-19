// ConnMgr.h: interface for the CConnMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNMGR_H__E4D25223_B37E_4985_AF90_C9D1C426D1B9__INCLUDED_)
#define AFX_CONNMGR_H__E4D25223_B37E_4985_AF90_C9D1C426D1B9__INCLUDED_
#include "windows.h"
#include "connection.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CConnMgr  
{
public:
	HRESULT AddConnection(CConnection * pConn);
	void stop();
	CConnMgr();
	virtual ~CConnMgr();

	DWORD Start();
	DWORD StartThread();
private:
	fd_set * m_fdReads;
	HANDLE m_ListSemaphore;
	DWORD m_dwThreadID;
	HANDLE m_hThread;
	BOOL m_bRunning;
	BOOL m_bResetList;
	CConnectionList m_ConnList;


};

#endif // !defined(AFX_CONNMGR_H__E4D25223_B37E_4985_AF90_C9D1C426D1B9__INCLUDED_)
