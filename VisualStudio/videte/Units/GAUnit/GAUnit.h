// GAUnit.h : main header file for the GAUNIT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "GeldDaten.h"

class CGAInput;
class CGAWindow;
class CGaRS232;

/////////////////////////////////////////////////////////////////////////////
// CGAUnitApp:
// See GAUnit.cpp for the implementation of this class
//

class CGAUnitApp : public CWinApp
{
public:
	CGAUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGAUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	public:
		void	LoadDebugConfiguration();
		void	CheckAndStoreReadData(LPBYTE lpBuffer, DWORD dwLen);
		void	TransmitDataToSecurity();
		int		OnConnect();
		void	OnDisconnect();
		void	AlarmReset();

		void	ErrorHandler(int nErrorCode);
		DWORD   GetAlarmMask() const {return m_dwAlarmMask;};
		int		CheckAndStoreOneData(LPBYTE lpB, char* pB, int nMax);

		inline const CString& GetCurrency() const;

	protected:
		void	ConfirmCom(BOOL bSuccess=TRUE);
	//{{AFX_MSG(CGAUnitApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:
		CString		m_sCurrency;
		CGAInput*	m_pInput;
		CGaRS232*	m_pSerialCom;

		int			m_nStatus;
		int			m_nOffset;

		// interface 
		// 1 2 3 4
		// 0 no interface -> update data record
		int			m_nInterface;

		// camera 1 or 2
		// 0 no camera -> update data record
		int			m_nCamera;

		// actual read data
		CGeldDaten  m_Data;

		// data for all interface / camera
		CGeldDaten  m_Datas[8];

		BYTE		m_byXORCheckSum;
		DWORD		m_dwAlarmMask;

	int		m_iCOMNumber;
	BOOL	m_bTraceiSTData;
	BOOL	m_bTraceRS232Data;
	BOOL	m_bTraceRS232DataHex;
	BOOL	m_bTraceRS232DataAscii;
	BOOL	m_bTraceRS232Events;
};
/////////////////////////////////////////////////////////
inline const CString& CGAUnitApp::GetCurrency() const
{
	return m_sCurrency;
}
extern CGAUnitApp theApp;

#define GA_STX	1
#define GA_IFN	2
#define GA_DATE 3
#define GA_TIME 4
#define GA_TAN  5
#define GA_AN   6
#define GA_BLZ  7
#define GA_KTN  8
#define GA_BETR 9
#define GA_KAN  10
#define GA_ETX  11
#define GA_US	12
#define GA_OK	13
#define GA_CONF 14
#define GA_CSUM 15

/////////////////////////////////////////////////////////////////////////////
