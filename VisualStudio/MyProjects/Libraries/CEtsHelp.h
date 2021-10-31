#ifndef _CETSHELP_
#define _CETSHELP_

/**************************************+****************************************

                                   CEtsHelp.h
                                   ----------

            Declaration der Schnittstellenklasse für die ETSHELP.DLL

                     (C) 1999 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 16.04.1999


                                                   programmed by Oliver Wesnigk
*******************************************************************************/

class CEtsHelp
{
public:
   ~CEtsHelp();

   static void Destructor();
   static void CreateContextWnd(const char*,int,int);
   static void CreateErrorWnd  (const char*,int,int);
};
#endif