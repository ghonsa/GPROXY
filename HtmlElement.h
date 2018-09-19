// HtmlElement.h: interface for the HtmlElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLELEMENT_H__6D051C39_D069_4C90_9295_0E33E99CE18C__INCLUDED_)
#define AFX_HTMLELEMENT_H__6D051C39_D069_4C90_9295_0E33E99CE18C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class HtmlElement  
{
public:
	int GetTagSize();
	bool AddChar(char c);
	bool resetElement();
	HtmlElement();
	HtmlElement(char * Tag);
	HtmlElement(char * Tag,HtmlElement *Parent);

	virtual ~HtmlElement();
	
	void SetTag(char * Tag);

	char * GetParams();
	char * GetTagParam(char * paramName);
	char * GetTagText();
	char * GetTagId();
	int GetCurrIndx()
	{
		return m_iCharIndx;
	};


private:
	unsigned char * m_pSource;
	int m_iCharIndx;
	HtmlElement * m_HeNext;
	HtmlElement * m_HeParent;
	HtmlElement * m_HeContained;
	char  * m_ElementText;
	int m_currSize;
};

#endif // !defined(AFX_HTMLELEMENT_H__6D051C39_D069_4C90_9295_0E33E99CE18C__INCLUDED_)
