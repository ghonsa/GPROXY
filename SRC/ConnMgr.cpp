// ConnMgr.cpp: implementation of the CConnMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConnMgr.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD WINAPI CThreadProc(VOID *lpParam)
{
	CConnMgr * pm=(CConnMgr*)lpParam;
	return pm->StartThread();


}

CConnMgr::CConnMgr()
{

}

CConnMgr::~CConnMgr()
{

}
DWORD CConnMgr::Start()
{

	m_hThread=CreateThread( NULL,         // SD
							0x2000000,    // 2 meg initial stack size
							CThreadProc,  // thread function
							this,         // thread argument
							0,            // creation option
							&m_dwThreadID   // thread identifier
							);
return 0;
}

DWORD CConnMgr::StartThread()
{
	m_bRunning=TRUE;
	m_ListSemaphore = CreateMutex(NULL,FALSE ,"GSERVLISTACCESS" );

	while(m_bRunning)
	{
		DWORD  rslt;
		int numselect;
		// wait value for select...
		timeval tv;
		tv.tv_sec=10000;
		tv.tv_usec=0;

		rslt=WaitForSingleObject(m_ListSemaphore,10000);
		if(rslt==WAIT_OBJECT_0)
		{
			int numsock;
			int indx=0;
			fd_set * pfdReads;
				CConnection * pconn;
			if(m_bResetList)
			{
				m_bResetList=FALSE;
				// we have the list, get the sockets
				numsock=m_ConnList.GetCount();
				//allocate a new fd structure
				pfdReads= (fd_set *)new byte((numsock*sizeof(SOCKET))+sizeof(u_int));
				pfdReads->fd_count=numsock;

				m_ConnList.ResetPtr();
				pconn=m_ConnList.GetHead();
				
				while(pconn)
				{
					pfdReads->fd_array[indx++]=pconn->GetSock();
				}
				delete m_fdReads;
				m_fdReads=pfdReads;
			}		
			ReleaseMutex(m_ListSemaphore);
		}

		while(numselect = select(0,m_fdReads,NULL,NULL,&tv))
		{
			if(numselect==SOCKET_ERROR)
			{
				int errnum = WSAGetLastError();
			}
			else
			{
				unsigned count;
				for(count=0; count<m_fdReads->fd_count;count++)
				{
					SOCKET sock=m_fdReads->fd_array[count];
					// find the connection who ownes the socket

				}

			}
			if(m_bResetList||m_bRunning)
				break;
		}
	


	}
	return 0;
}

void CConnMgr::stop()
{
	m_bRunning=FALSE;

}

HRESULT CConnMgr::AddConnection(CConnection *pConn)
{
	DWORD  rslt;
	rslt=WaitForSingleObject(m_ListSemaphore,10000);
	if(rslt==WAIT_OBJECT_0)
	{
	// we have the list, add in an item

		//pconnElement->SetItem(pConn);
		m_ConnList.AddToTail(pConn);
		ReleaseMutex(m_ListSemaphore);
  

	}

	return 0;
}
