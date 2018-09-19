// Connection.cpp: implementation of the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Connection.h"
#include "stdio.h"
#include <conio.h>
//nclude <windows.h>

extern int opens;
extern void Gdump(int type,const char *szMsg, ... );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void  ParseSink(CLinkBuff *element,VOID *lpParam)
{
	CConnection * pl=(CConnection*)lpParam;
	pl->ProcessBuffer(element);
}


DWORD WINAPI ThreadProc(VOID *lpParam)
{
	DWORD rslt;
	CConnection * pl=(CConnection*)lpParam;
	rslt= pl->Continue();
	delete pl;
	return rslt;

}

CConnection::CConnection(SOCKET sock,SOCKADDR_IN servSA) 
{
	m_bIsTextHtml=false;
	m_bCloseServerConnection=false;
	m_bServerConnected=false;
	m_bRelease=false;
	m_clientSock=sock;
	m_servSock=NULL;
	m_servAddr =servSA;
	m_clientRecvList=NULL;
	m_clientSendList=NULL;
	m_servRecvList=NULL;
	m_servSendList=NULL;
	m_hThread=CreateThread( NULL,         // SD
							0x2000000,    // 2 meg initial stack size
							ThreadProc,  // thread function
							this,         // thread argument
							0,            // creation option
							&m_dwThreadID   // thread identifier
							);
}

CConnection::~CConnection()
{
	opens--;
	closesocket(m_clientSock);
	closesocket(m_servSock);
}

DWORD CConnection::Continue()
{
	char buffer[1025];
	int no_rx=0;
	int sendcnt=0;
	int iRet;
	m_bReadHeader=true;
	m_bReadingHeader=false;
	//m_bGetHeader=true;
	m_docSize=0;
	m_bRunning=TRUE;
	buffer[1024]=0; 


	Gdump(MSG_CONN,"*******************Client Connection made:%d****************************\n",(int)this);
	//Connect to server... 
	
	if(ConnectServer()==FALSE)
	{
		return -1;
	}
	//TheDoc = new HtmlDocument("test");
	//TheDoc->SetSink(ParseSink,this);
	opens++;
	while(m_bRunning)
	{
		timeval timeout;
		timeout.tv_sec=0;
		timeout.tv_usec=100;
		int selectRslt=0;
		fd_set reads;
		fd_set writes;
		CLinkBuff * currBuff;
			

		reads.fd_array[0]=m_clientSock;
		reads.fd_array[1]=m_servSock;
		reads.fd_count=2;
		
		writes.fd_count=0;
		if(m_clientSendList)
		{
			writes.fd_array[writes.fd_count]=m_clientSock;
			writes.fd_count++;
		}
		else if(m_bCloseServerConnection)
			break;

		if(m_servSendList)
		{
			writes.fd_array[writes.fd_count]=m_servSock;
			writes.fd_count++;
		}
		selectRslt=select(0,&reads,&writes,NULL,&timeout);
	    if (selectRslt == SOCKET_ERROR)
		{
			Gdump (MSG_CONN,"Select error:%d  #%d\n",(int) this, WSAGetLastError ());
		}
		
		// ok select came back ok, lets see who needs servicing
		else if(select!=0)
		{
			while(writes.fd_count--)
			{
				if(writes.fd_array[writes.fd_count]==m_servSock)
				{
					CLinkBuff* curr = GetNextServerSend();
					if(curr)
					{
						int sent;
						sent=send(m_servSock,curr->m_Buffer,curr->m_size,0);
						if(sent!=curr->m_size)
						{	
							CLinkBuff* newBuff= new CLinkBuff((curr->m_Buffer+sent),curr->m_size-sent,true);							
							PushBackListHead(&m_clientSendList,newBuff);	
						}
						delete curr;
					}
				}
				else if((writes.fd_array[writes.fd_count]==m_clientSock)&&m_bRelease)
				{
					if(m_bRelease)
					{
						CLinkBuff* curr = GetNextClientSend();
						if(curr)
						{
							int sent;
							//if(!strncmp(curr->m_Buffer,"</HTML",6))
							//	m_bRunning=false;
						
							if((sent=send(m_clientSock,curr->m_Buffer,curr->m_size,0))!=curr->m_size)
							{
								CLinkBuff *newBuff= new CLinkBuff((curr->m_Buffer+sent),curr->m_size-sent,true);							
								PushBackListHead(&m_servSendList,newBuff);	
							}
						
							delete curr;
						}
						else if(m_docSize==0)
							m_bRelease=false;

					} //m_bRelease
				}
			}
			// ok select came back ok, lets see who needs servicing
			while(reads.fd_count--)
			{
				if(reads.fd_array[reads.fd_count]==m_servSock)
				{
					iRet=recv(m_servSock,buffer,1024,0);
					if (iRet == SOCKET_ERROR)
					{
						Gdump (MSG_CONN,"Server- receive failed.Error:%d %d\n",this,WSAGetLastError ());
						m_bRunning=false;
					}
					else if (iRet == 0)
					{
						Gdump (MSG_CONN,"**********Server no data is received%d*******\n",this);
						if((m_docSize==0)&&(m_clientSendList==NULL))
							m_bRunning = false;
					}
					else
					{
						currBuff = new CLinkBuff(buffer,iRet,TRUE);
						AddServerRecieve(currBuff);
					}
				}
				else if(reads.fd_array[reads.fd_count]==m_clientSock)
				{
					iRet=recv(m_clientSock,buffer,1024,0);
					if (iRet == SOCKET_ERROR)
					{
						
						if(WSAGetLastError()==WSAECONNRESET)
							m_bRunning=false;
					}
					else if (iRet == 0)
					{
							m_bRunning=false;
					}
					else
					{// here is where we have a client buffer 
						currBuff = new CLinkBuff(buffer,iRet,TRUE);
						ProcessClientRecieve(currBuff);
					}
				}
			}
		}
		if(currBuff=GetNextClientRecv())
		{
			AddServerSend(currBuff);   //just passthrough
		}
		if(currBuff=GetNextServerRecv())
		{
			ProcessServerRecieve(currBuff);
		}
		Sleep(0);
	}
	Gdump(MSG_CONN,"*******************Connection closed:%d***********************************\n",this);
	return 0;
}

char * CConnection::ApplyFilter(char * tagtext)
{

	char * newbuff = tagtext;
	//Gdump(MSG_PARSER,"TAG:{%s}\n",tagtext);
	return tagtext;
	if((strstr(tagtext,"</BODY"))||(strstr(tagtext,"</body")))
	{
		char *insertText = "<P><FONT color=blue size=1 style=\"BACKGROUND-COLOR: yellow\"><STRONG>This page has been CMGIonized</STRONG></FONT></P>";
		
		newbuff= new char[strlen(tagtext)+strlen(insertText)];
		strcpy(newbuff,insertText);
		strcat(newbuff,tagtext);
		
		//**************** adjust the document size and release it

		m_docSize+=strlen(insertText);


	}

#if 0	
	else if(!strcmp(tagtext,"<!--DoubleClick Ad BEGIN-->"))
	{
		m_bKillBuff=true;
		newbuff=NULL;
	}
	else if(!strcmp(tagtext,"<!--DoubleClick Ad END-->"))
	{
		m_bKillBuff=false;
		newbuff=NULL;
	}
	else if(m_bKillBuff)
		newbuff=NULL;
#endif

	return newbuff;
}
//***********************************************************************
// * this is where the callback from the parser eventually ends up.
//*This is the filter insert point! 
//***********************************************************************
void CConnection::ProcessBuffer(CLinkBuff * currBuff)
{
	char * senddata=NULL;
	CLinkBuff * sendBuff=NULL;

	m_docSize-=currBuff->m_size;
	
	senddata=ApplyFilter(currBuff->m_Buffer);
	if(senddata==currBuff->m_Buffer) // no changes		
	{
		AddClientSend(currBuff);  // just passthrough
	}
	else if(senddata!=NULL) //senddata contains the new tag to send
	{
		sendBuff=new CLinkBuff(senddata,strlen(senddata),TRUE);
		AddClientSend(sendBuff);  // just passthrough
	}
	else
	{
		delete currBuff;		
	}
	
}
//***********************************************************************
//* Recieves from the Client end up here
//*
//*
//***********************************************************************

BOOL CConnection::ProcessClientRecieve(CLinkBuff *buff)
{
	//char * AuthStr ;
	if(m_bGetHeader)
	{
		Gdump(MSG_HEADER,"[Client:%d***]\n",this);
		Gdump(MSG_HEADER,"%s[/Client***]\n\n",buff->m_Buffer);
	}
	AddClientRecieve(buff);
	return true;
}
//***********************************************************************
   

//***********************************************************************

BOOL CConnection::ProcessServerRecieve(CLinkBuff *buff)
{
	CLinkBuff * sendBuff = buff;

	//AddClientSend(buff);
	//return true;

	if(m_bReadHeader)
	{
		char * headerptr = buff->m_Buffer;
		int length = buff->m_size;
		CLinkBuff * remainder;
		if(m_header.ProcessPacket(headerptr,length,&remainder))
		{	
			delete sendBuff;
			if(m_header.IsHeaderComplete())
			{
				char * ct;
				m_bReadHeader=false;
				m_bReadingHeader=false;
				Gdump(MSG_HEADER,"[Server:%d***]\n%s[/Server***]\n\n",this,m_header.GetHeaderText());

				if(ct=m_header.GetParameter("Content-Length:"))
				{
					unsigned long length=0;
					sscanf(ct,"%d/n",&length);
					m_docSize=length;
				}

				if(ct=m_header.GetParameter("Content-Type:"))
				{
					if(!strncmp(ct,"text/html",9))
					{
						TheDoc = new HtmlDocument("test");
						TheDoc->SetSink(ParseSink,this);
						m_bIsTextHtml=true;	
					}
				}
				if(!m_bIsTextHtml||m_docSize==0)
					ReleaseHeader();
			}

			sendBuff=remainder;
		}
	}
	
	if(!m_bReadingHeader)
	{
		if(m_bIsTextHtml&&sendBuff)
		{
			TheDoc->Parse(sendBuff);
		}
		else if(sendBuff)
		{

			AddClientSend(sendBuff);  // just passthrough
			m_docSize-=sendBuff->m_size;
		}
	}
	else
		AddClientSend(sendBuff);  // just passthrough

	if(m_docSize<=0)
	{
		Gdump(MSG_HEADER,"****** server %d: %d bytes ***file complete*******\n",this,m_docSize);
		
		m_bReadHeader=true;
		m_bIsTextHtml=false;
		m_docSize=0;
		if(!m_bRelease) ReleaseHeader();
	}
	else if(m_docSize<2)
	{
		Gdump(MSG_HEADER,"****** server %d: %d bytes left*******\n",this,m_docSize);
	}

	return true;
}



SOCKET CConnection::GetSock()
{
	return m_clientSock;
}

BOOL CConnection::ConnectServer()
{
	if(m_bServerConnected)
		return false;
	if(m_servSock==NULL)
	{
		m_servSock=socket(AF_INET,SOCK_STREAM,0);
	}
	if(m_servSock==INVALID_SOCKET)
	{
		return FALSE;
	}
	if(connect(m_servSock,(struct sockaddr *)&m_servAddr,sizeof(m_servAddr))<0)
	{
		closesocket(m_servSock);
		Gdump(MSG_CONN,"Connect failed: %d\n",(int) this);
		return FALSE;
	}
	m_bServerConnected=true;
	Gdump(MSG_CONN,"*******************************Server Connected:%d********************************\n",(int) this);
	return TRUE;
}

//LinkBuff support 
CLinkBuff * CConnection::GetListHead(CLinkBuff** listp)
{

	CLinkBuff * curr = NULL;
	if(*listp!=NULL)
	{	
		curr=*listp;
		 *listp =curr->next;
	}
	return curr;
}
int CConnection::PushBackListHead(CLinkBuff** listp,CLinkBuff* buff)
{
	if(*listp!=NULL)
	{	
		buff->next=*listp;
		*listp=buff;
		return 0;
	}
	else
		*listp=buff;
	return 0;
}

CLinkBuff * CConnection::GetListTail(CLinkBuff** listp)
{
	CLinkBuff * curr = NULL;
	if(*listp!=NULL)
	{	
		curr=((CLinkBuff *)*listp)->GetTail();
		if(!((CLinkBuff *)*listp)->RemoveTail())
		{
			delete *listp;
			*listp=NULL;
		}
	}
	return curr;
}
BOOL CConnection::AddListTail(CLinkBuff** listp,CLinkBuff* buff)
{
	if(*listp==NULL)
		*listp=buff;
	else
		((CLinkBuff *)*listp)->AddToTail(buff);
	return TRUE;
}

BOOL CConnection::AddClientRecieve(CLinkBuff *buffer)
{
	return 	(AddListTail(&m_clientRecvList,buffer));
}
BOOL CConnection::AddServerRecieve(CLinkBuff *buffer)
{
	return 	(AddListTail(&m_servRecvList,buffer));
}

BOOL CConnection::AddClientSend(CLinkBuff *buffer)
{
	return 	(AddListTail(&m_clientSendList,buffer));
}
BOOL CConnection::AddServerSend(CLinkBuff *buffer)
{
	return 	(AddListTail(&m_servSendList,buffer));
}

CLinkBuff * CConnection::GetNextServerSend()
{
	return(GetListHead(&m_servSendList));
}
CLinkBuff * CConnection::GetNextClientSend()
{
	return(GetListHead(&m_clientSendList));
}
CLinkBuff * CConnection::GetNextServerRecv()
{
	return(GetListHead(&m_servRecvList));
}
CLinkBuff * CConnection::GetNextClientRecv()
{
	return(GetListHead(&m_clientRecvList));
}


int CConnection::ReleaseHeader()
{
	CLinkBuff * headerBuff = m_header.GetHeader();
	PushBackListHead(&m_clientSendList,headerBuff);	
	m_bRelease=true;
	return 0;
}
