// LThread.h: interface for the LThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LTHREAD_H__C29D0A1B_8604_4EF5_89D5_797C57BB21B4__INCLUDED_)
#define AFX_LTHREAD_H__C29D0A1B_8604_4EF5_89D5_797C57BB21B4__INCLUDED_
#include "windows.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLThread  
{
public:
	void processCmd(char c);
	void Stop();
	DWORD Start();
	CLThread();
	virtual ~CLThread();
	DWORD StartThread();

private:

	char * m_szTextBuff;
	BOOL m_bRunning;
	SOCKET m_sockListn;
	DWORD m_dwThreadID;
	HANDLE m_hThread;
};

#endif // !defined(AFX_LTHREAD_H__C29D0A1B_8604_4EF5_89D5_797C57BB21B4__INCLUDED_)
