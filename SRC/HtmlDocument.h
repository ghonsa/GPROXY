// HtmlDocument.h: interface for the HtmlDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLDOCUMENT_H__FB06FD91_E72D_4C19_A9E5_9840AB12152C__INCLUDED_)
#define AFX_HTMLDOCUMENT_H__FB06FD91_E72D_4C19_A9E5_9840AB12152C__INCLUDED_

#include "HtmlElement.h"	// Added by ClassView
#include "linkBuff.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct Element  
{
public:
	long docOffset;
	long length;
};

class HtmlDocument  
{
public:
	int SetSink(void(*SinkRoutine)(CLinkBuff *,void * ),void *theObj);
	int Parse(CLinkBuff * buff);
	HtmlDocument();
	HtmlDocument(char * url);
	virtual ~HtmlDocument();

private:
	void ProcessElement();

	int m_elementIndx;
	
	bool m_bInElement;
	bool m_bInScript;
	bool m_bInComment;
	long m_docLength;
	long m_docIndx;
	void * m_sinkObj;
	void (*m_Sink)(CLinkBuff * ,void *);
	HtmlElement m_DocTags;
	HtmlElement  m_currElement;
	char * m_Url;
	Element m_eList[5000];
};

#endif // !defined(AFX_HTMLDOCUMENT_H__FB06FD91_E72D_4C19_A9E5_9840AB12152C__INCLUDED_)
