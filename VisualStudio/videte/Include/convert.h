//---------------------------------------------------------------------
//      Projekt:        Convert
//
//      Name der Datei: convert.h
//      Zweck:          Definitionen der Mitgliedsfunktionen
//                      der Modulklasse ccApplikation.
//      Datum Beginn:   8.3.1994
//      Datum Änderung: 8.3.1994
//
//---------------------------------------------------------------------
//

#ifndef     wk_convert_h
#define     wk_convert_h

//#ifdef  __cplusplus
//extern "C"
//{
//#endif

//---------------------------------------------------------------------
//  Strukur YUV_Header
//---------------------------------------------------------------------
                   
#define     YUV_IDENTITY    0x0167      //Standardkennung für YUV422-Header                   
                   
#define     YUV_NOTVALID    0
#define     YUV_FMT422      1
#define     YUV_FMT411      2

                      
typedef struct  plaqYUV_Header
{
    WORD    nIdent;             //Muß immer auf YUV_IDENTITY gesetzt werden.
    WORD    nFormat;
    WORD    nWidth;
    WORD    nHeight;
}YUV_Header,*PYUV_Header;

  
//---------------------------------------------------------------------
//  Schlüssel Quellformat (Parameter nSrcFormat der Funktion Convert)
//---------------------------------------------------------------------

#define     SRC_YUV         0
#define     SRC_DIB24       1
#define     SRC_DIB8        2
#define     SRC_TIFF24      3
#define     SRC_PCX24       4
                               
#define     SRC_COUNT       5

                               
//---------------------------------------------------------------------
//  Schlüssel Zielformat (Parameter nDestFormat der Funktion Convert)
//---------------------------------------------------------------------

#define     DEST_YUV        0
#define     DEST_DIB24      1
#define     DEST_DIB8       2
#define     DEST_TIFF24     3
#define     DEST_PCX24      4

#define     DEST_COUNT      5

  
//---------------------------------------------------------------------
//      Funktion Convert          
//
//---------------------------------------------------------------------

HGLOBAL __export Converter(UINT    nSrcFormat, UINT    nDestFormat, HGLOBAL hMem);                 
BOOL   DLLInitTerm(HMODULE hDLL, DWORD dwReason);

//#ifdef  __cplusplus
//}
//#endif

#endif      //wk_convert_h
