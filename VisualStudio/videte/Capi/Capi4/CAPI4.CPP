////////////////////////////////////////////////////////////////////////////
// capi4.cpp
#include <afxwin.h>         // MFC core and standard components

#include "wkclasses\WK_Trace.h"

#include "hdr.h"
#include "capi4.h" 
#ifdef _CAPI20_
#include "20main.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The one and only global pointer...
CCapi*	pCapi = NULL;

// Funktionen die Funktionen aufrufen...
CCapi::CCapi()
{
	m_pC = NULL;
}

int CCapi::Create(HWND hWnd, UINT uMsgCommand, WORD wMaxBCh, WORD wXBuffers, 
	WORD wMyProtocol, WORD wMyMaxMovingBlocks, BOOL bChannelBundeling)
{
	m_pC = (CCp*) new CCp;
	int iResult = m_pC->Create(hWnd, uMsgCommand, wMaxBCh, wXBuffers, 
			wMyProtocol, wMyMaxMovingBlocks, bChannelBundeling,
			FALSE	// no raw data please
			);

	return iResult;
}

int	CCapi::NewCreate(	
					CAbstractCapiClient *pWrapper,
					HWND hWnd,
					UINT uMsgCommand,
					WORD wMaxBCh,
					WORD wXBuffers,
					WORD wMyProtocol,
					WORD wMyMaxMovingBlocks,
					BOOL bChannelBundeling,
					BOOL bClientWantsRawData)
{
	m_pC = (CCp*) new CCp(pWrapper);
	int iResult = m_pC->Create(hWnd, uMsgCommand, wMaxBCh, wXBuffers, 
			wMyProtocol, wMyMaxMovingBlocks, bChannelBundeling,
			bClientWantsRawData
			);

	return iResult;
}

BOOL CCapi::Open(LPCTSTR sAddress, WORD wBCh)
{
	return m_pC->Open(sAddress, wBCh); 
}

void CCapi::Close(BOOL bCloseOnlyOne/*=FALSE*/)
{
	m_pC->Close(bCloseOnlyOne);
}

LPCTSTR CCapi::GetVersion()
{
	return m_pC->GetVersion();
}

LPCTSTR CCapi::GetCapi4Version()
{
	return m_pC->GetCapi4Version();
}

LPCTSTR CCapi::GetManufacturer()
{
	return m_pC->GetManufacturer();
}

UINT CCapi::Reset(BOOL bShutdown/*=FALSE*/)
{
	return m_pC->Reset(bShutdown);
}

BOOL CCapi::CapiActive()
{
	return m_pC->CapiActive();
}

void CCapi::SetPasswort(LPCTSTR szId)
{
	m_pC->SetPasswort(szId);
}

BOOL CCapi::ListenRequest(LPCTSTR pOwnNumber, BYTE byController, DWORD dwServiceMask, DWORD dwInfoMask)
{
	return m_pC->ListenRequest(pOwnNumber, byController, dwServiceMask, dwInfoMask);
}

BOOL CCapi::SendData(LPBYTE lpHdr, LPBYTE lpData, int iPrior)
{
	return m_pC->SendData((LPDEFHDR) lpHdr, lpData, iPrior);
}

BOOL CCapi::SendRawData(LPBYTE lpData, WORD wLen)
{
	if (wLen > MY_BLK_LEN) {
		WK_TRACE(_T("invalid block length %d wLen>%d\n"),wLen,MY_BLK_LEN);
		return FALSE;
	} else {
		BOOL bOkay = m_pC->OnSendDataToCapi(lpData, wLen);
		if (bOkay) {
		} else {
		}
		return bOkay;
	}
}

void CCapi::SetConnectionTimeout(DWORD dwTimeout)
{
	m_pC->SetConnectionTimeout((LONG)dwTimeout);
}

CCapi::~CCapi()
{
	delete m_pC;
}

int CCapi::GetNumOpenRequests() const
{
	return m_pC->GetNumOpenRequests();
}

int CCapi::Poll()
{
	return m_pC->Poll();
}

void CCapi::EnableRawData(BOOL bEnable)
{
	m_pC->EnableRawData(bEnable);
}