/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 26.08.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __SPORT_DEFINED
	#define __SPORT_DEFINED

///////////////////////////////////////////////////////////////////////////////	
// Definitionen 
///////////////////////////////////////////////////////////////////////////////	
// SPORT0 word length
#define SLEN_32	0x001f
#define SLEN_16	0x000f
#define SLEN_8	0x0007
 
///////////////////////////////////////////////////////////////////////////////	
// Prototypes
///////////////////////////////////////////////////////////////////////////////
void sport_ini();
void SportEnable();
void SportDisable();
void SetSport();
void SetSportDMASorce(BYTE* pSorce, BYTE bySorce);
bool DoTrace(char *pString);
void CreateImageHeader(BYTE2* pBuffer, int nHSize, int nVSize, int nChannel, bool bHelp);
///////////////////////////////////////////////////////////////////////////////

#endif //__SPORT_DEFINED
