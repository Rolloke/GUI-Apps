#ifndef _ETSBINDDLL_                             // mehrfach include verhindern
#define _ETSBINDDLL_

/**************************************+****************************************

                                   ETSBIND.h
                                   ---------

          Load time link dll für das run time linking von allen ETS-DLL´s


                   (C) 1999 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 02.09.1999



                                                   programmed by Oliver Wesnigk
*******************************************************************************/


#ifdef ETSBIND_EXPORTS                           // Funktionen exportieren oder importieren ?
#define ETSBIND_API __declspec(dllexport)
#else
#define ETSBIND_API __declspec(dllimport)
#endif

// Supportfunktionen Libraries (load on call)

ETSBIND_API bool pascal ETSBind      (const char * szDllName,        // Dateinamen der DLL z.B. ETSPRTD.DLL
                                      HMODULE&     hModule,          // Reference auf Modulehandle im LIB-namespace
                                      void**       ppFct,            // Pointer auf erste Funktion im LIB-namespace
                                      int          nCount,           // Anzahl einzubindener Funktionen
                                      bool         bLoadError        // Flag, DLL nicht vorhanden, dann Programmabbruch
                                     );                              // DLL in den Process einbinden wenn nötig

ETSBIND_API void pascal ETSUnBind    (const char * szDllName,        // Dateinamen der DLL z.B. ETSPRTD.DLL
                                      HMODULE&     hModule,          // Reference auf Modulehandle im LIB-namespace
                                      void**       ppFct,            // Pointer auf erste Funktion im LIB-namespace
                                      int          nCount,           // Anzahl einzubindener Funktionen
                                      const int *  pnInstances,      // Pointer auf Instanzzähler im LIB-namespace
                                      void *       pParam            // Optionaler Parameter für den DLL - Destructor
                                     );                              // DLL aus dem Process entfernen wenn nötig

ETSBIND_API void pascal ETSTestUnBind(const char * szDllName,        // Dateinamen der DLL z.B. ETSPRTD.DLL
                                      HMODULE&     hModule,          // Reference auf Modulehandle im LIB-namespace
                                      const int *  pnInstances       // Pointer auf Instanzzähler im LIB-namespace
                                     );                              // Überprüfen DLL Bindung

// Supportfunktionen für Programme

ETSBIND_API bool pascal ETSFoundInDefault(const char * szFileName    // Return: true wenn Datei im Aktuellen oder Commonpath gefunden wurde, sonst false
                                         );                          // Teste ob File im Aktuellen- oder Commonpath existiert

ETSBIND_API bool pascal ETSGetCommonPath(char * szCommon,            // Pointer auf Puffer für den Commonpath
                                         int length                  // länge des Puffers
                                        );                           // Besorge den Commonpath

#endif