// CDRBurn.h: interface for the CCDRBurn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDRBURN_H__81D06101_218A_11D4_AD14_004005A1D890__INCLUDED_)
#define AFX_CDRBURN_H__81D06101_218A_11D4_AD14_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInitPacketCD;

class CCDRBurn
{
	// construction / destruction
public:
	CCDRBurn(CIPCDrive* pCD_Drive);
	virtual ~CCDRBurn();

	// attributes
public:
	BOOL IsFinalized();
	BOOL IsPacketCDWindow();
	BOOL IsCDRemounting();
	BOOL IsFormatted();
	void SetTickCountStart(DWORD dwStart);
	inline DWORD GetTickCountStart() const;
	inline DWORD GetTickCountEnd() const;
	inline CInitPacketCD* GetInitPCD() const;

	// operations
public:
	BOOL IsCDFinalizing();
	BOOL StartExplMenuFinalizeCD();
	BOOL StartFormatCD();
	BOOL StartExplMenuRemountCD();
	BOOL StartExplMenuFormatCD();
	BOOL StartExplMenuEjectCD();
	void SetTickCountEnd(DWORD dwEnd);
	CExplorerMenuThread* GetThreadRemount();
	BOOL TerminateExplMenuRemountCD();

	// implementation
protected:
	
	// data member
private:
	HWND m_hFinalizeProgressDlg;
	BOOL m_bFinalizeInProgress;
	BOOL m_bFinalizeDlgOnDesktop;
	BOOL m_bFertigButtonClicked;
	DWORD m_dwTickCountEnd;
	DWORD m_dwTickCountStart;
	CInitPacketCD* m_pInitPCD;
	CIPCDrive* m_pCD_Drive;

	CExplorerMenuThread m_RemountThread;
	CExplorerMenuThread m_FormatThread;
	CExplorerMenuThread m_FinalizeThread;
	CExplorerMenuThread	m_EjectThread;

};

//////////////////////////////////////////////////////////////////////
inline DWORD CCDRBurn::GetTickCountStart() const
{
	return m_dwTickCountStart;
}

//////////////////////////////////////////////////////////////////////
inline DWORD CCDRBurn::GetTickCountEnd() const
{
	return m_dwTickCountEnd;
}

//////////////////////////////////////////////////////////////////////
inline CInitPacketCD* CCDRBurn::GetInitPCD() const
{
	return m_pInitPCD;
}

//////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_CDRBURN_H__81D06101_218A_11D4_AD14_004005A1D890__INCLUDED_)
