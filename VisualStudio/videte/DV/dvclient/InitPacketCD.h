// InitPacketCD.h: interface for the CInitPacketCD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITPACKETCD_H__89AAC2A3_27D5_11D4_AD24_004005A1D890__INCLUDED_)
#define AFX_INITPACKETCD_H__89AAC2A3_27D5_11D4_AD24_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInitPacketCD  
{
	// construction / destruction
public:
	CInitPacketCD();
	virtual ~CInitPacketCD();

	// attributes
public:
	void SetHandlePacketCDChildWnd(HWND hWnd);
	void SetHandlePacketCDWnd(HWND hWnd);
	void SetHandleFirstFinalizeDlg(HWND hWnd);
	inline WORD GetWordYesButtonID_FirstFinalizeDlg() const;
	inline DWORD GetDwordMaxCount() const;
	inline WORD GetWordEdit_FieldID() const;
	inline WORD GetWordFormate_ButtonID() const;
	inline WORD GetWordFertig_ButtonID() const;
	inline CString GetStringWndTxtPacketCD() const;
	inline CString GetStringTxtSecondFinalizeDlg() const;
	inline CString GetStringTxtFirstFinalizeDlg() const;
	inline CString GetStringNewCDname() const;
	inline CString GetStringFormate_ButtonTxt() const;
	inline CString GetStringFertig_ButtonTxt() const;
	inline HWND GetHandlePacketCDWnd() const;
	inline HWND GetHandlePacketCDChildWnd() const;
	inline HWND GetHandleFirstFinalizeDlg() const;


	// data member
private:
	
	WORD m_wFertig_ButtonID;
	WORD m_wFormate_ButtonID;
	WORD m_wEdit_FieldID;
	WORD m_wYesButtonID_FirstFinalizeDlg;
	
	DWORD m_dwMaxCount;

	CString m_sTxtSecondFinalizeDlg;
	CString m_sNewCDName;
	CString m_sFertig_ButtonTxt;
	CString m_sFormate_ButtonTxt;
	CString m_sWndTxtPacketCD;
	CString m_sTxtFirstFinalizeDlg;

	HWND m_hFirstFinalizeDlg;
	HWND m_hPacketCDChildWnd;
	HWND m_hPacketCDWnd;
};
//////////////////////////////////////////////////////////////////////
inline HWND CInitPacketCD::GetHandleFirstFinalizeDlg() const
{
	return m_hFirstFinalizeDlg;
}

//////////////////////////////////////////////////////////////////////
inline HWND CInitPacketCD::GetHandlePacketCDChildWnd() const
{
	return m_hPacketCDChildWnd;
}

//////////////////////////////////////////////////////////////////////
inline HWND CInitPacketCD::GetHandlePacketCDWnd() const
{
	return m_hPacketCDWnd;
}

//////////////////////////////////////////////////////////////////////
inline CString CInitPacketCD::GetStringFertig_ButtonTxt() const
{
	return m_sFertig_ButtonTxt;
}

//////////////////////////////////////////////////////////////////////
inline CString CInitPacketCD::GetStringFormate_ButtonTxt() const
{
	return m_sFormate_ButtonTxt;
}

//////////////////////////////////////////////////////////////////////
inline CString CInitPacketCD::GetStringNewCDname() const
{
	return m_sNewCDName;
}

//////////////////////////////////////////////////////////////////////
inline CString CInitPacketCD::GetStringTxtFirstFinalizeDlg() const
{
	return m_sTxtFirstFinalizeDlg;
}

//////////////////////////////////////////////////////////////////////
inline CString CInitPacketCD::GetStringTxtSecondFinalizeDlg() const
{
	return m_sTxtSecondFinalizeDlg;
}

//////////////////////////////////////////////////////////////////////
inline CString CInitPacketCD::GetStringWndTxtPacketCD() const
{
	return m_sWndTxtPacketCD;
}

//////////////////////////////////////////////////////////////////////
inline WORD CInitPacketCD::GetWordFertig_ButtonID() const
{
	return m_wFertig_ButtonID;
}

//////////////////////////////////////////////////////////////////////
inline WORD CInitPacketCD::GetWordFormate_ButtonID() const
{
	return m_wFormate_ButtonID;
}

//////////////////////////////////////////////////////////////////////
inline WORD CInitPacketCD::GetWordEdit_FieldID() const
{
	return m_wEdit_FieldID;
}

//////////////////////////////////////////////////////////////////////
inline DWORD CInitPacketCD::GetDwordMaxCount() const
{
	return m_dwMaxCount;
}

//////////////////////////////////////////////////////////////////////
inline WORD CInitPacketCD::GetWordYesButtonID_FirstFinalizeDlg() const
{
	return m_wYesButtonID_FirstFinalizeDlg;
}




#endif // !defined(AFX_INITPACKETCD_H__89AAC2A3_27D5_11D4_AD24_004005A1D890__INCLUDED_)
