// Pins.h: Schnittstelle für die Klasse CPins.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PINS_H__2D99CFA0_32E9_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_PINS_H__2D99CFA0_32E9_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CARA3DDocFriend.h"
#define  NO_OF_PINTYPES       3

#define  PINTYPE_BOX          1
#define  PINTYPE_LISTENER     2
#define  PINTYPE_EXCITER      3

#define  LIGHT_COLOR_FACTOR   0.7f

struct PinType
{
   int      nType;
   int      nPins;
   CVector *pvPositions;
};

class CPins : public CCARA3DDocFriend  
{
public:
	CPins();
	virtual ~CPins();
	void     DeletePins();

   void     DrawPins();
   void     DrawPinsText(HDC hDC, int nOff);

   int      GetNoOfPins(int nType);
   void     ShowPins(int nType, bool bShow);
	bool     ShowPins(int nType);

   bool     ReadPins(CArchive &, bool);
	void     WritePins(CArchive&, bool);

   bool         m_bShow[NO_OF_PINTYPES];
private:
   CPtrList     m_PinTypes;
   static char *gm_szPinText[NO_OF_PINTYPES];
};

#endif // !defined(AFX_PINS_H__2D99CFA0_32E9_11D3_B6EC_0000B458D891__INCLUDED_)
