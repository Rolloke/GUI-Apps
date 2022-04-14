#ifndef _CETSTRIA_
#define _CETSTRIA_

/*******************************************************************************

                                  CEtsTria.h
                                  ------------

                        Aufrufklasse für die ETSTRIA.DLL

                    (C) 1999 ELAC Technische Software GmbH

                         VERSION 1.00 Stand 06.09.1999


benötig noch die Datei CEtsTria.lib

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

struct ETSTRIA_PTTS // Polygon-Triangle-Tessellation-Structur
{
   int      nPoints;          // Input
   POINT *  pPoints;          // Input
   HANDLE   hMyHeap;          // Input

   int      nIndices;         // Output
   int *    pIndices;         // Output
};

struct ETSTRIA_GWS
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

class CEtsTria
{
public:

   CEtsTria(){};
   ~CEtsTria();

          void   Destructor();                   // neu 27.08.1999 der DLL - Destructor
   static int    GetLastError();
   static bool   Tessellation(ETSTRIA_PTTS&);
   static double Area_Sum(ETSTRIA_PTTS&);
   static bool   GenerateWeb(ETSTRIA_GWS&);
};

#endif