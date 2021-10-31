#ifndef _CCARATRIA_
#define _CCARATRIA_

/*******************************************************************************

                                  CCaraTria.h
                                  ------------

                        Aufrufklasse für die CARATRIA.DLL

                    (C) 1999 ELAC Technische Software GmbH

                         VERSION 1.10 Stand 27.08.1999


benötig noch die Datei CCaraTria.lib

                                                   programmed by Oliver Wesnigk
*******************************************************************************/

#ifndef _FPOINT_
#define _FPOINT_

struct FPOINT        // Struktur für die Ergebnispunkte als float
{
   float x;
   float y;
};

#endif

struct CARATRIA_PTTS // Polygon-Triangle-Tessellation-Structur
{
   int      nPoints;          // Input
   POINT *  pPoints;          // Input
   HANDLE   hMyHeap;          // Input

   int      nIndices;         // Output
   int *    pIndices;         // Output
};

struct CARATRIA_GWS
{
   bool     bWebSize;         // Input
   int      nPoints;          // Input
   POINT *  pPoints;          // Input
   HANDLE   hMyHeap;          // Input

   int      nWebPoints;       // Output
   FPOINT * pWebPoints;       // Output
   int      nFloorIndices;    // Output
   int *    pFloorIndices;    // Output
   int      nConIndices;      // Output
   int *    pConIndices;      // Output
   int      nWebIndices;      // Output
   int *    pWebIndices;      // Output
   double   dMaxWebSpacing;   // Output
};

class CCaraTria
{
public:

   CCaraTria(){};
   ~CCaraTria();

          void   Destructor();                   // neu 27.08.1999 der DLL - Destructor
   static int    GetLastError();
   static bool   Tessellation(CARATRIA_PTTS&);
   static double Area_Sum(CARATRIA_PTTS&);
   static bool   GenerateWeb(CARATRIA_GWS&);
};

#endif