#ifndef LinkBuff_Included
#define LinkBuff_Included
#include <stdio.h>
#include <memory.h>
class CLinkBuff
{
public:
	bool m_bIsEnd;
	char * m_Buffer;
	int m_size;
	bool m_allocated;

	CLinkBuff():m_bIsEnd(false), m_Buffer(NULL), m_size(0), m_allocated(false) 
	{
	}
	CLinkBuff(char * buffer,int size,bool allocate)
	{
		
		next=NULL;
		if(allocate)
		{
			m_Buffer= new char[size+1];
			m_size=size;
			if(m_Buffer&&buffer)
			{
				memcpy(m_Buffer,buffer,size);
				m_Buffer[size]=0;
				//m_size=size;
				m_allocated=true;
			}
		}
		else
		{
			m_Buffer=buffer;
			m_size=size;
		}
	}

	
	~CLinkBuff()
	{
		if( m_allocated)
			delete m_Buffer;
	}
	CLinkBuff * next;
	CLinkBuff * prev;

	CLinkBuff *GetNext()
	{
		return next;
	}

	CLinkBuff *GetPrev()
	{
		return prev;
	}
	CLinkBuff * GetTail()
	{
		CLinkBuff * tlnk = this;
		while(tlnk->next!=NULL)
		{
			tlnk=tlnk->next;
		}
		return tlnk;

	}
	bool AddToTail(CLinkBuff * lb)
	{
		CLinkBuff * tlnk=GetTail();
		tlnk->next=lb;
		lb->prev=tlnk;
		return true;
	}
	bool RemoveTail()
	{
		CLinkBuff * tlnk = this;
		if(next==NULL) // we are the tail!
			return false;
		while(tlnk->next->next!=NULL)
			tlnk=tlnk->next;
		delete tlnk->next;
		tlnk->next=NULL;
		return true;
	}
};

#endif