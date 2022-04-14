// RSA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main(int argc, char* argv[])
{
	if (argc<3)
	{
		printf("too less parameters\n");
		printf("usage: rsa {[-d],[-e]} sourcefile destinationfile\n");
		printf("-e to encode/encrypt a file\n");
		printf("-d to decode/decrypt a file\n");
	}
	else
	{
		CRSA rsa;

		if (!rsa.IsValid())
		{
			rsa.Init(0x1000,0x2000);
		}

		if (rsa.IsValid())
		{
			if (   0==stricmp(argv[1],"-e")
				|| 0==stricmp(argv[1],"/e")
				|| 0==stricmp(argv[1],"e")
				)
			{
				if (rsa.Encode(argv[2],argv[3]))
				{
					printf("%s to %s encoded.\n",argv[2],argv[3]);
				}
				else
				{
					printf("%s to %s not encoded.\n",argv[2],argv[3]);
				}
			}
			else if (   0==stricmp(argv[1],"-d")
					 || 0==stricmp(argv[1],"/d")
					 || 0==stricmp(argv[1],"d")
					)
			{
				if (rsa.Decode(argv[2],argv[3]))
				{
					printf("%s to %s decoded.\n",argv[2],argv[3]);
				}
				else
				{
					printf("%s to %s not decoded.\n",argv[2],argv[3]);
				}
			}
			else
			{
				printf("wrong switch -e or -d and not %s\n",argv[0]);
			}
		}
		else
		{
			printf("cannot initialize RSA\n");
		}
	}
	return 0;
}
