// ClientOverlayProcess.h: interface for the CClientOverlayProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTOVERLAYPROCESS_H__D8D52265_1135_11D4_A0EA_004005A19028__INCLUDED_)
#define AFX_CLIENTOVERLAYPROCESS_H__D8D52265_1135_11D4_A0EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClientProcess.h"

class CClientOverlayProcess : public CClientProcess  
{
	// construction / destruction
public:
	CClientOverlayProcess(CProcessScheduler* pScheduler,
						  CSecID idClient,
						  CSecID camID,
						  const CRect &rect);
	virtual ~CClientOverlayProcess();

	// attributes
public:
   inline const CRect& GetOverlayRect() const;

   // operations
public:
	void Modify(CSecID camID, const CRect &rect);

	// data member
protected:
	CRect m_rectOverlay;

};
//////////////////////////////////////////////////////////////////////
inline const CRect &CClientOverlayProcess::GetOverlayRect() const
{
	return m_rectOverlay;
}

#endif // !defined(AFX_CLIENTOVERLAYPROCESS_H__D8D52265_1135_11D4_A0EA_004005A19028__INCLUDED_)
