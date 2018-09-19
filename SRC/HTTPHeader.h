// HTTPHeader.h: interface for the CHTTPHeader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPHEADER_H__B199526B_6316_4DF2_9AB9_4A9E5C05345F__INCLUDED_)
#define AFX_HTTPHEADER_H__B199526B_6316_4DF2_9AB9_4A9E5C05345F__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "linkBuff.h"

class CHTTPHeader  
{
public:
	CLinkBuff * GetHeader();
	char * GetHeaderText();
	int SetParameter(char * type,char * value);
	char * GetParameter(char* type);
	bool IsHeaderComplete();
	int ProcessPacket(char * packet, int len,CLinkBuff **remainder);
	CHTTPHeader();
	virtual ~CHTTPHeader();
	int AddChar(char c);
	bool resetHeader();

private:
	int ParseHeader();
	bool m_bHeaderComplete;
	char m_lastChar;
	bool m_bCrLf;
	char * m_headerText;
	int m_iCharIndx;
	int m_currSize;
	CLinkBuff * m_HeaderList;
};

#endif // !defined(AFX_HTTPHEADER_H__B199526B_6316_4DF2_9AB9_4A9E5C05345F__INCLUDED_)
