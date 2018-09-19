// HTTPHeader.cpp: implementation of the CHTTPHeader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HTTPHeader.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTTPHeader::CHTTPHeader(): m_bCrLf(false), m_bHeaderComplete(false), m_HeaderList(0)
{
	m_headerText= new char[128];
	m_currSize=128;
	m_iCharIndx=0;

}

CHTTPHeader::~CHTTPHeader()
{

}
int CHTTPHeader::AddChar(char c)
{
	if(m_iCharIndx==5)
	{
		if(strncmp(m_headerText,"HTTP",4))
		{
			return -1;
		}
	}
	m_headerText[m_iCharIndx++]=c;
	switch(c)
	{
	case '\r':
		break;
	case '\n':
		if(m_lastChar=='\r')
		{
			if(m_bCrLf)
			{
				m_headerText[m_iCharIndx++]=0;
				m_bHeaderComplete=true;
				ParseHeader();
				return 1;
			}
			m_bCrLf=true;
		}
		break;
	default:
		m_bCrLf=false;
		break;
	}
	m_lastChar=c;
	if(m_iCharIndx>=(m_currSize-1))
	{
	//	printf("{resize}");
		char * tmp= new char[m_currSize+128];
		strncpy(tmp,m_headerText,m_currSize);
		delete  m_headerText;
		m_currSize+=128;
		m_headerText=tmp;
	}
	return 0;
}
bool CHTTPHeader::resetHeader()
{
	m_iCharIndx=0;
	m_bHeaderComplete= false;
	if(m_HeaderList)
	{
		while(m_HeaderList->RemoveTail());
		delete m_HeaderList;
		m_HeaderList=NULL;
	}
	//memset(m_ElementText,0,m_currSize);
	return true;
}

int CHTTPHeader::ProcessPacket(char *packet, int len, CLinkBuff** remainder)
{
	int rslt=0;
	int tmplen=len;
	char * tmppacket=packet;
	*remainder=NULL;
	while(tmplen--)
	{
		rslt=AddChar(*tmppacket++);
		if(rslt)
			break;
	}
	if(rslt==-1)
		return -1;
	if(strncmp(m_headerText,"HTTP/",5))
		return -2;
	if(tmplen>0)
		*remainder= new CLinkBuff(tmppacket,tmplen,true);
	else
		int t=1;
	return 1;
}

bool CHTTPHeader::IsHeaderComplete()
{
	return m_bHeaderComplete;
}

int CHTTPHeader::ParseHeader()
{
	// we need a list of strings that make up the header...
	// each string seperated by CRLF
	// parameters usally name:value
	char * tag;
	char * headerElement = m_headerText;
	while((tag=strstr(headerElement,"\r\n"))!=NULL)
	{
		int length;
		CLinkBuff * tmpElement ; 
		length = tag-headerElement+2;  // pointer to the "/r/n" minus the start plus 2 for the CRLF
		tmpElement = new CLinkBuff(headerElement,length,true);
		headerElement=tag+2;
		if(m_HeaderList==0)
			m_HeaderList=tmpElement;
		else
			m_HeaderList->AddToTail(tmpElement);
		
	}

	
	return 0;
}

char * CHTTPHeader::GetParameter(char *type)
{
	CLinkBuff * curr;
	if((!m_bHeaderComplete)||m_HeaderList==NULL)
		return NULL;
	curr=m_HeaderList;
	while(strncmp(curr->m_Buffer,type,strlen(type))!=0)
	{
		curr=curr->GetNext();
		if(curr==NULL)
			return NULL;
	}
	if(curr!=NULL)
	{
		char * tptr= curr->m_Buffer+strlen(type);
		if (*tptr==' ') tptr++;
		return tptr;
		int t=1;
	}
	return NULL;
}

int CHTTPHeader::SetParameter(char *type, char *value)
{
	CLinkBuff * curr;
	CLinkBuff * newLink;
	int newsize = strlen(type)+ strlen(value);

	if((!m_bHeaderComplete)||m_HeaderList==NULL)
		return NULL;

	newLink = new CLinkBuff(NULL,newsize,true);
	if(!newLink)  return -1;
	
	strcpy(newLink->m_Buffer,type);
	strcat(newLink->m_Buffer,value);
	
	curr=m_HeaderList;
	while(strncmp(curr->m_Buffer,type,strlen(type))!=0)
	{
		curr=curr->GetNext();
		if(curr==NULL)
			break;
	}
	if(curr!=NULL)
	{
		CLinkBuff * prev;
		prev = curr->GetPrev();
		newLink->next=curr->next;
		if(prev)
		{
			prev->next=newLink;
			newLink->prev=curr->prev;
		}
		delete curr;
	}
	else
	{
		curr=m_HeaderList;
		while(strcmp(curr->m_Buffer,"/r/n")!=0)
		{
			curr=curr->GetNext();
			if(curr==NULL)
				break;
		}
		int t=1;
	}
	return 0;
}

char * CHTTPHeader::GetHeaderText()
{
	return m_headerText;
}

CLinkBuff * CHTTPHeader::GetHeader()
{
	int headerSize=0;
	CLinkBuff *HeaderBuff;
	// calc the header size
	CLinkBuff * curr = m_HeaderList;
	while(curr!=NULL)
	{
		headerSize+=curr->m_size;
		curr=curr->GetNext();
	}
	HeaderBuff = new CLinkBuff(NULL,headerSize,true);
	HeaderBuff->m_Buffer[0]=0;
	curr = m_HeaderList;
	while(curr!=NULL)
	{
		strcat(HeaderBuff->m_Buffer,curr->m_Buffer);
		curr=curr->GetNext();
	}
	return HeaderBuff;

}
