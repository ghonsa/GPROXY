// HtmlDocument.cpp: implementation of the HtmlDocument class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include <string.h>
#include "HtmlDocument.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HtmlDocument::HtmlDocument():m_Url(NULL),m_currElement(NULL),m_docLength(0) 
{
	m_docLength=0;
	m_bInComment=false;
	m_bInScript=false;
	m_bInElement=false;
	m_elementIndx=0;
	m_docIndx=0;

}
HtmlDocument::HtmlDocument(char * Url):m_Url(NULL) 
{
	m_docLength=0;
	m_bInComment=false;
	m_bInScript=false;
	m_bInElement=false;
	m_elementIndx=0;
	m_docIndx=0;
	
}

HtmlDocument::~HtmlDocument()
{
}

int HtmlDocument::Parse(CLinkBuff * currBuff)
{
	// this the main function for parsing the html document.
	int tmpctr = currBuff->m_size;	
	char * basePtr=currBuff->m_Buffer;
	char * workingElement = NULL;
	int currIndx=0;
	int workingLength;
	int BuffLen=currBuff->m_size;
	char * buffer=currBuff->m_Buffer;
	Element tmpElement; 


	//printf("Parse()\n");
	while(currIndx<BuffLen)
	{
		m_docLength++;
		char curr = buffer[currIndx];
		switch(curr)
		{
		case '<':
			// start of a tag ?
			if(!m_bInScript)
			{
				// This is the start of a new tag. 
				// 
				if(m_bInElement)
				{
					// close out previous element if not a comment

					if(m_bInComment)
					{
						// in a comment
						m_currElement.AddChar(curr);
						break;
					}
				//****start of a new element
				if(m_currElement.GetCurrIndx()!=0)
					ProcessElement();
					
				}
				tmpElement.length=1;
				tmpElement.docOffset=m_docIndx;
				m_bInElement=true;
				m_currElement.resetElement();
				m_currElement.AddChar(curr);
				
			}
			else
				m_currElement.AddChar(curr);


			break;
		case '>':
			if	((!strncmp(m_currElement.GetTagText(),"<SCRIPT",7))||
				(!strncmp(m_currElement.GetTagText(),"<script",7)))
					m_bInScript=true;


			if	((strstr(m_currElement.GetTagText(),"</SCRIPT"))||
				(strstr(m_currElement.GetTagText(),"</script")))
					m_bInScript=false;
			
			
			if(!m_bInScript)
			{
				tmpElement.length=workingLength;
				m_eList[m_elementIndx++]=tmpElement;
				m_bInElement=false;
				m_currElement.AddChar(curr);
				ProcessElement();
			}
			else
				m_currElement.AddChar(curr);

			m_bInComment=false;
			break;
		case '!':

			if(m_currElement.GetCurrIndx()==1)
			{
				if(!m_bInScript)
					m_bInComment=true;
			}			

		default:
			if(!m_bInElement) // that would make it a text element
			{
				tmpElement.length=1;
				tmpElement.docOffset=m_docIndx;
				m_bInElement=true;	
				m_currElement.resetElement();
			}
			//if(curr>13)
				m_currElement.AddChar(curr);
			break;

		}
		//printf(".");
		m_docIndx++;
		currIndx++;
	}

	delete currBuff;
	return 0;
}

int HtmlDocument::SetSink(void(*SinkRoutine)(CLinkBuff *,void * ),void * theObj)
{
	m_Sink = SinkRoutine;
	m_sinkObj=theObj;
	return 0;
}

void HtmlDocument::ProcessElement()
{
	m_bInElement=false;

	m_currElement.AddChar(0);
	
	//if	((!strncmp(m_currElement.GetTagText(),"<SCRIPT",7))||
	//	(!strncmp(m_currElement.GetTagText(),"<script",7)))
	//	m_bInScript=true;
	//else if	((!strncmp(m_currElement.GetTagText(),"</SCRIPT",8))||
	//	(!strncmp(m_currElement.GetTagText(),"</script",7)))
	//	m_bInScript=false;


	if( m_Sink!=NULL)
	{
		CLinkBuff * newBuff = new CLinkBuff(m_currElement.GetTagText(),m_currElement.GetTagSize(),true);
		if(!strcmp(m_currElement.GetTagText(),"</HTML>"))
			newBuff->m_bIsEnd=true;
		m_Sink(newBuff,m_sinkObj);
	}
	m_currElement.resetElement();
}
