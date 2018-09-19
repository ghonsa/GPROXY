// GServ.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "winsock2.h"
#include "lthread.h"
#include <stdio.h>
#include <conio.h>
int main(int argc, char* argv[])
{
	WSADATA WSAData;
	CLThread * pListn;
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

	while(_getch()!='q');
	//4) 
	
	return 0;
}
