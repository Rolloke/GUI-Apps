#ifndef _CCARAUNZIP_
#define _CCARAUNZIP_

/**************************************+****************************************

                                   CCaraUnzip.h
                                   ------------

            Declaration der Schnittstellenklasse für die CARAUNZP.DLL

                     (C) 2000 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 3008.2000


                                                   programmed by Oliver Wesnigk
*******************************************************************************/

class CCaraUnzip
{
public:
   ~CCaraUnzip();

   static void Destructor();
   static int  Unzip(const char * zipfilename,const char * extractpath);
};

#endif