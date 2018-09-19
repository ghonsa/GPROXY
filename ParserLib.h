/**************************************************************************** 
* ParserLib  
*	PARSER_HANDLE CreateParser()
*   int SetSink(PARSER_HANDLE,Sink Routine)
*	int Parse(char * buffer, int length)
*	int DeleteLBuff(LBUFF_HANDLE)
*	char * GetLBuffer(LBUFF_HANDLE)
*   int GetLBuffLen(LBUFF_HANDLE)
*   int DeleteParser(PARSER_HANDLE)
*
******************************************************************************/
#ifdef __cplusplus
#include "HtmlDocument.h"
#include "LinkBuff.h"
#endif

#define PARSER_HANDLE void *
#define LBUFF_HANDLE void *


PARSER_HANDLE CreateParser();
int SetSink(PARSER_HANDLE,void(*SinkRoutine)(LBUFF_HANDLE,void * ),void * theObj);
int Parse(PARSER_HANDLE,char * buffer,int length);
int DeleteLBuff(LBUFF_HANDLE);
char * GetLBuffer(LBUFF_HANDLE);
int GetLBuffLen(LBUFF_HANDLE);
int DeleteParser(PARSER_HANDLE);

