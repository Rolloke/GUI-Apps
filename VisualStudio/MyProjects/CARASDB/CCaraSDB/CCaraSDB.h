/*******************************************************************************
                                 CCaraSdb
  Zweck:
   Auswahl der Boxen aus einer Boxendatenbank. 3D-Darstellung der Boxen.
  Header:            Bibliotheken:        DLL´s:
  CCaraConf.h        ----                 ----
   ----              EtsBind.lib          ETSBIND.dll
   ----              CCaraSDB.lib         CARASDB.dll
   ----              ----                 CARAWALK.dll
   ----              ----                 ETS3DGL.dll
  Benutzung:
   Die ETSBIND.dll ist eine statisch gebundene Dll. Die weiteren sind sog.
   "load on call-Dll´s". D.h., sie werden erst geladen, wenn sie das erste 
   mal benötigt werden.
  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.10 Stand 22.10.1999
                                                   programmed by Rolf Ehlers
*******************************************************************************/
#ifndef _CCARASDB_H_
#define _CCARASDB_H_

#ifndef STRICT
   #define  STRICT
#endif

#include <WINDOWS.H>

#include "CCaraConf.h"

#define CARASDB_MAX_BOXES            8
#define CARASDB_BOX_NAME_LENGTH    512

#define CARASDB_NO_PARAMETER        10
#define CARASDB_NO_BOXES            11

struct CARASDB_PARAMS
{
   SourceConfiguration Initial;
   int                 nCount;
   SourceConfiguration Selected[CARASDB_MAX_BOXES];
   char                szFileName[CARASDB_MAX_BOXES][CARASDB_BOX_NAME_LENGTH];
   HWND                hwndParent;
};

class CCaraSdb
{
public:
   CCaraSdb();
   ~CCaraSdb();
   void Destructor();
   static bool DoModal(CARASDB_PARAMS &);
   static void UpdateDataBase(char *pszBoxName);
};

#endif