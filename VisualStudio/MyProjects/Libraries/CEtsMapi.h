#ifndef _CETSMAPI_
#define _CETSMAPI_

/**************************************+****************************************

                                   CEtsMapi.h
                                   ----------

             Declaration der Schnittstellenklasse für die ETSMAPI.DLL

                     (C) 2000 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 26.04.2000


                                                   programmed by Oliver Wesnigk
*******************************************************************************/

class CEtsMapi
{
public:
   ~CEtsMapi();

   static void Destructor();
   static bool IsAvailable();
   static bool Send(HWND,const char *);
};
#endif
