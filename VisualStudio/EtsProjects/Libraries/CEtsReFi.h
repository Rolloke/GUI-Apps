#ifndef _CETSREFI_
#define _CETSREFI_

/**************************************+****************************************

                                   CEtsReFi.h
                                   ----------

             Declaration der Schnittstellenklasse für die ETSREFI.DLL

                     (C) 2000 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 28.04.2000


                                                   programmed by Oliver Wesnigk
*******************************************************************************/

struct ETSREFI_INIT
{
   char  szAppKey[256];                          // Key für die Registry z.B. "CARACAD\\2.0"
   char  szCurrentExt[256];                      // Extension für das Current Directory wenn erwünscht z.B. "CAD" ohne Backslashes
   int   nBaseCommand;                           // IDM_BASE für Recent File Auswahl z.B. 6000 erzeugt dann die Commands 6000 bis 6007
   HWND  hMain;                                  // Handle auf das Hauptfenster
   int   nPosition;                              // Position des Recent File Menus im Dateimenu (immer ersten im Hauptmenu)
};

class CEtsReFi
{
public:
   ~CEtsReFi();

   static void Destructor();
   static bool Connect(ETSREFI_INIT * psData);
   static void SetNewRecentFile(const char * szFile);
   static bool GetSelectedFile(int command,int nBuffer,char * szFile);
   static void DeleteRecentFile(const char * szFile);
   static void CurrentDirChanged();
};
#endif
