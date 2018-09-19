// Connection.cpp: implementation of the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Connection.h"
#include "stdio.h"
#include <conio.h>

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
// This is the proxy thread, each connection gets its own thread

DWORD CConnection::Continue()
{
	char buffer[1025];
	int no_rx=0;
	int sendcnt=0;
	int iRet;
	int timeouts = 0;

	m_bReadHeader=true;
	m_bReadingHeader=false;
	//m_bGetHeader=true;
	m_docSize=0;
	m_bRunning=TRUE;
	buffer[1024]=0; 


	Gdump(MSG_CONN,"**** %d Client Connection made ****\n",(int)this);
	//Connect to server... 
	
	if(ConnectServer()==FALSE)
	{
		return -1;
	}
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
		// If there is anything on the clients send list, add in a check
		// for write on the client socket
		if(m_clientSendList)
		{
			writes.fd_array[writes.fd_count]=m_clientSock;
			writes.fd_count++;
		}
		// If nothing on the client send list, and Server wants the connection closed
		// then shutdown this filter instance
		else if(!m_bKeepAlive)
			m_bRunning=false;
		else if(m_bCloseServerConnection)
			break;

		// If there is data on the server send list, add the check for 
		// writes on the server socket
		if(m_servSendList)
		{
			writes.fd_array[writes.fd_count]=m_servSock;
			writes.fd_count++;
		}
		// Now check the sockets for action
		selectRslt=select(0,&reads,&writes,NULL,&timeout);
	    if(!selectRslt) // nothing available!
		{
			if(((timeouts++)>100)&&	!m_bRequestMade)
			{
				char * ct;
				if(ct=m_header.GetParameter("Content-Type:"))
				{
					if(strncmp(ct,"text/html",9))
					{
						m_bRunning=false;
					}
				}
				//Gdump (MSG_CONN,"Select timeout:%d \n",(int) this);
			}
		}
		if (selectRslt == SOCKET_ERROR)
		{
			Gdump (MSG_CONN,"Select error:%d  #%d\n",(int) this, WSAGetLastError ());
		}
	
		// ok select came back ok, lets see who needs servicing
		else if(selectRslt!=0)
		{
			timeouts=0;
			// Do the writes first
			while(writes.fd_count--)
			{
				if(writes.fd_array[writes.fd_count]==m_servSock)
				{
					//Send next server packet 
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
					CLinkBuff* curr = GetNextClientSend();
					if(curr)
					{
						//Send next client packet
						int sent;
						if((sent=send(m_clientSock,curr->m_Buffer,curr->m_size,0))!=curr->m_size)
						{
							CLinkBuff *newBuff= new CLinkBuff((curr->m_Buffer+sent),curr->m_size-sent,true);							
							PushBackListHead(&m_servSendList,newBuff);	
						}
						delete curr;
					}
				}
			//	else if(m_bKeepAlive==false)
			//			m_bRunning = false;

			}
			// Check the reads
			while(reads.fd_count--)
			{
				if(reads.fd_array[reads.fd_count]==m_servSock)
				{
					//Read the server packet
					iRet=recv(m_servSock,buffer,1024,0);
					if (iRet == SOCKET_ERROR)
					{
						Gdump (MSG_CONN,"Server- receive failed.Error:%d %d\n",this,WSAGetLastError ());
						m_bRunning=false;
					}
					else if (iRet == 0)
					{
						Gdump (MSG_CONN,"**********Server no data is received%d*******\n",this);
						
						if((m_docSize<=0)&&(m_clientSendList==NULL))
							m_bRunning = false;
						else 
							int t=1;
					}
					else
					{

						currBuff = new CLinkBuff(buffer,iRet,TRUE);
						AddServerRecieve(currBuff);
					}
				}
				else if(reads.fd_array[reads.fd_count]==m_clientSock)
				{
					//REad the client packet
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
	Gdump(MSG_CONN,"**** %d Connection closed ****\n",this);
	return 0;
}

char * CConnection::ApplyFilter(char * tagtext)
{

	char * newbuff = tagtext;
	char * ct;

	ReleaseHeader();
	return newbuff;
	
	if((strstr(tagtext,"</HTML"))||(strstr(tagtext,"</html")))
	{
		ReleaseHeader();
	}
	if((strstr(tagtext,"<BODY"))||(strstr(tagtext,"<body")))
	{
		char strsize[10];
		char *insertText = "<P><FONT color=blue size=1 style=\"BACKGROUND-COLOR: yellow\"><STRONG>This page has been CMGIonized</STRONG></FONT></P>";
		newbuff= new char[strlen(tagtext)+strlen(insertText)];
		strcpy(newbuff,tagtext);
		strcat(newbuff,insertText);
		//**************** adjust the document size and release it
		if(ct=m_header.GetParameter("Content-Length:"))
		{
			unsigned long length=0;
			sscanf(ct,"%d/n",&length);
			length+=strlen(insertText);
			sprintf(strsize, "%d\r\n",length);
			m_header.SetParameter("Content-Length:",strsize);
		}
		ReleaseHeader();
	
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

	if(!m_bRelease)
	{
		//Gdump(MSG_PARSER,"[Client:%d filter]\n",this);

		senddata=ApplyFilter(currBuff->m_Buffer);
		if(senddata==currBuff->m_Buffer) // no changes		
		{
			AddClientSend(currBuff);  // just passthrough
		}
		else if(senddata!=NULL) //senddata contains the new tag to send
		{
			sendBuff=new CLinkBuff(senddata,strlen(senddata),TRUE);
			AddClientSend(sendBuff);  
			delete currBuff;
		}
		else
		{
			delete currBuff;		
		}
	}
	else
	{
		//Gdump(MSG_PARSER,"[Client:%d no-filter]\n",this);
		AddClientSend(currBuff);  // just passthrough
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
	m_bRequestMade=true;
	AddClientRecieve(buff);
	return true;
}
//***********************************************************************
   

//***********************************************************************

BOOL CConnection::ProcessServerRecieve(CLinkBuff *buff)
{
	CLinkBuff * sendBuff = buff;

	if(m_bReadHeader) // we expect the header!
	{
		int prslt;
		char * headerptr = buff->m_Buffer;
		int length = buff->m_size;
		CLinkBuff * remainder;

		if(m_docSize!=0)
			int t=1;
		if((prslt=m_header.ProcessPacket(headerptr,length,&remainder))==1)
		{	
			delete sendBuff;
			if(m_header.IsHeaderComplete())
			{
				char * ct;
				m_bRequestMade=false;
				m_bReadHeader=false;
				m_bReadingHeader=false;
				m_bKeepAlive=false;
				Gdump(MSG_HEADER,"[Server:%d***]\n%s[/Server***]\n\n",this,m_header.GetHeaderText());

				if(ct=m_header.GetParameter("Content-Length:"))
				{
					unsigned long length=0;
					sscanf(ct,"%d/n",&length);
					m_docSize=length;
				}
				else m_docSize=0;

				if(ct=m_header.GetParameter("Content-Type:"))
				{
					if(!strncmp(ct,"text/html",9))
					{
						TheDoc = new HtmlDocument("test");
						TheDoc->SetSink(ParseSink,this);
						m_bIsTextHtml=true;	
					}
				}
				if(ct=m_header.GetParameter("Proxy-Connection:"))
				{
					if(!strncmp(ct,"keep-alive",9))
					{
						m_bKeepAlive=true;
					}
				}

				if(!m_bIsTextHtml)
				{	
					ReleaseHeader();
					m_header.resetHeader();
				}
				else if(m_docSize==0)
				{
					if(remainder)
						m_docSize=remainder->m_size;
					else
					{
						ReleaseHeader();
						m_header.resetHeader();
					}
				}
				sendBuff=remainder;
			
			}
		}
	}
	if(!m_bReadingHeader)
	{

		if(m_bIsTextHtml&&sendBuff)
		{
			if(m_docSize<=0)
			{
				int argh=1;
			}

			m_docSize-=sendBuff->m_size;
			TheDoc->Parse(sendBuff);
		}
		else if(sendBuff)
		{
			Gdump(MSG_HEADER,"S%d\n",this);
		
			AddClientSend(sendBuff);  // just passthrough
			m_docSize-=sendBuff->m_size;
		}
		if(m_docSize==0)
		{
			Gdump(MSG_HEADER,"****** server %d: %d bytes ***file complete*******\n",this,m_docSize);
			m_bReadHeader=true;
			m_bIsTextHtml=false;
		}
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
	Gdump(MSG_CONN,"**** %d Server Connected ****\n",(int) this);
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
	Gdump(MSG_PARSER,"[Client:%d release header]\n",this);

	if(!m_bRelease)
	{
		CLinkBuff * headerBuff = m_header.GetHeader();
		PushBackListHead(&m_clientSendList,headerBuff);	
		m_bRelease=true;
		m_header.resetHeader();
	}

	return 0;
}
