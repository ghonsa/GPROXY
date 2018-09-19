// gproxy.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "winsock2.h"
#include "lthread.h"
#include <stdio.h>
#include <conio.h>

extern int opens;
bool bToScreen;
int MsgMask;
void Gdump(int type,const char *szMsg, ... );

int main(int argc, char* argv[])
{
	WSADATA WSAData;
	CLThread * pListn;
	char c;
	MsgMask=MSG_ERROR;

	//1) check status of network connections
	if (WSAStartup (MAKEWORD(1,1), &WSAData) != 0) 
	{
		MessageBox (NULL, TEXT("WSAStartup failed!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	//2) open usage database connection

	//3) create listen socket thread
	pListn=new CLThread;
	pListn->Start();

	while((c=_getch())!='q')
	{
		switch (c)
		{
		case 'o':
		case 'O':
			printf("Opens:%d\n",opens);
			break;
		case 's':
		case 'S':
			bToScreen=(bToScreen?false:true);
			if(bToScreen)
				printf("Screen dump on\n");
			else
				printf("Screen dump off\n");
			break;
		case 'h':
		case 'H':
			MsgMask=(MsgMask & MSG_HEADER)?(MsgMask&!MSG_HEADER):(MsgMask|MSG_HEADER);
			Gdump(0,"Header dump %s\n",((MsgMask & MSG_HEADER)?"on":"off"));
			break;
		case 'c':
		case 'C':
			MsgMask=(MsgMask & MSG_CONN)?(MsgMask&!MSG_CONN):(MsgMask|MSG_CONN);
			Gdump(0,"Connection message dump %s\n",((MsgMask & MSG_CONN)?"on":"off"));
			break;
		case 'p':
		case 'P':
			MsgMask=(MsgMask & MSG_PARSER)?(MsgMask&!MSG_PARSER):(MsgMask|MSG_PARSER);
			Gdump(0,"Proxy message dump %s\n",((MsgMask & MSG_PARSER)?"on":"off"));
			break;

		}

	}
		;
	//4) 
	return 0;
}
void Gdump(int type,const char *szMsg, ... )
{
	if(bToScreen)
	{
		va_list vl;
		va_start(vl,szMsg);

		if((type&MsgMask)==type)
			vprintf(szMsg,vl);
		va_end(vl);
	}

}


