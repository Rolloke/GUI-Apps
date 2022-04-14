// CopyToTest.c

#include <windows.h>
#include <ostream.h>

int main(int argc, char *argv[])
{
	char szCL[_MAX_PATH];
	char szCopy[_MAX_PATH+_MAX_PATH+10];

	if (argc!=2)
	{
		cout << "incorrect command line" << endl << flush;
		return 1;
	}
	strcpy(szCL,argv[1]);

	if (strlen(szCL)<3)
	{
		cout << szCL << "incorrect command line" << endl << flush;
		return 1;
	}
	strlwr(szCL);
	strcpy(szCopy,"copy ");
	strcat(szCopy,szCL);
	strcat(szCopy," ");
	if (NULL==strstr(szCL,"garny"))
	{
		// Security
		if (strstr(szCL,"copytotest.exe"))
		{
			strcat(szCopy,"v:\\bin");
		}
		else if ((NULL!=strstr(szCL,"mico")))
		{
			strcat(szCopy,"v:\\bin\\test\\mico");
		}
		else if ((NULL!=strstr(szCL,"coco")))
		{
			strcat(szCopy,"v:\\bin\\test\\coco");
		}
		else
		{
			strcat(szCopy,"v:\\bin\\test");
		}
	}
	else
	{
		// Garny
		if ((NULL!=strstr(szCL,"mico") && strstr(szCL,"garnymico")==NULL))
		{
			strcat(szCopy,"v:\\bin\\testgarny\\mico");
		}
		else if ((NULL!=strstr(szCL,"coco")))
		{
			strcat(szCopy,"v:\\bin\\testgarny\\coco");
		}
		else
		{
			strcat(szCopy,"v:\\bin\\testgarny");
		}
	}
	cout << szCopy << endl << flush;
	//printf("%s\n",szCopy);
	system(szCopy);
	return 0;
}