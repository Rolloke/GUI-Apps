// pass.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

char Transform(char c)
{
	switch (c)
	{
	case '0':
		return 'q';
	case '1':
		return 'w';
	case '2':
		return 'e';
	case '3':
		return 'r';
	case '4':
		return 't';
	case '5':
		return 'z';
	case '6':
		return 'u';
	case '7':
		return 'i';
	case '8':
		return 'o';
	case '9':
		return 'p';
	case 'A':
		return 'a';
	case 'B':
		return 's';
	case 'C':
		return 'd';
	case 'D':
		return 'f';
	case 'E':
		return 'g';
	case 'F':
		return 'h';
	}
	return ' ';
}
void TransPassword(const char* s, char* sP)
{
	char szCopy[9];
	strncpy(szCopy,s,8);
	szCopy[8] = '\0';
	strupr(szCopy);
	for (int i=0;i<8;i++)
	{
		sP[i] = Transform(szCopy[i]);
	}
}

int main(int argc, char* argv[])
{
	// HTTP Header
	printf("Content-Type: text/html\n");
	printf("Pragma: no-cache\n");
	printf("Expires: Tue, 24 Dec 1996 00:00:00 GMT\n");
	printf("\n");

	printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n");
	printf("<html>\n");
	printf("<title>Videte IT Password Generator</title>\n");
	printf("<body>\n");

	time_t t = ::time(NULL);
	char szDWORD[8];
	char szPassword[9];
	sprintf(szDWORD,"%08lx",t);
	TransPassword(szDWORD,szPassword);
	szPassword[8] = '\0';
	printf("The password \"%s\" is valid for half an hour from:\n",
		szPassword);
	char tmpbuf[128]; 
    _strtime( tmpbuf );
    printf( "Time:\t%s\n", tmpbuf );
    _strdate( tmpbuf );
    printf( "Date:\t%s\n", tmpbuf );
	printf("</body>\n");
	printf("</html>\n");
	return 0;
}

