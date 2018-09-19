// HtmlElement.cpp: implementation of the HtmlElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "HtmlElement.h"
#include <string.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HtmlElement::HtmlElement():m_ElementText(NULL),m_HeContained(NULL),
                          m_HeNext(NULL),m_HeParent(NULL)
{
	m_ElementText= new char[128];
	m_currSize=128;
	m_iCharIndx=0;
}
HtmlElement::HtmlElement(char * tag):m_ElementText(NULL),m_HeContained(NULL),
                          m_HeNext(NULL),m_HeParent(NULL)
{
	SetTag(tag);
}
HtmlElement::HtmlElement(char * tag,HtmlElement *Parent):m_ElementText(NULL),m_HeContained(NULL),
                          m_HeNext(NULL),m_HeParent(NULL)
{
	m_HeParent=Parent;
	SetTag(tag);
	
}

HtmlElement::~HtmlElement()
{
	delete m_ElementText;

}

char * HtmlElement::GetTagId()
{
	return(NULL);	
}

char * HtmlElement::GetTagText()
{
	return(m_ElementText);	

}

char * HtmlElement::GetTagParam(char *paramName)
{
	return(NULL);	

}

char * HtmlElement::GetParams()
{
	return(NULL);	

}

void HtmlElement::SetTag(char *tag)
{
	m_ElementText=new char(strlen(tag));
	strcpy(m_ElementText,tag);

}

bool HtmlElement::AddChar(char c)
{
	m_ElementText[m_iCharIndx++]=c;
	if(m_iCharIndx>=m_currSize)
	{
	//	printf("{resize}");
		char * tmp= new char[m_currSize+128];
		strncpy(tmp,m_ElementText,m_currSize);
		delete  m_ElementText;
		m_currSize+=128;
		m_ElementText=tmp;

	}
	return true;
}
bool HtmlElement::resetElement()
{
	m_iCharIndx=0;
	//memset(m_ElementText,0,m_currSize);
	return true;
}

int HtmlElement::GetTagSize()
{
	return strlen(m_ElementText);
}
