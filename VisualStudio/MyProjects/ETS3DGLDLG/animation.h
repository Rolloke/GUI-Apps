#if !defined(AFX_ANIMATION_H__A4310CA3_6C6D_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ANIMATION_H__A4310CA3_6C6D_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mit Microsoft Visual C++ automatisch erstellte IDispatch-Kapselungsklasse(n). 

// HINWEIS: Die Inhalte dieser Datei nicht ändern. Wenn Microsoft Visual C++
// diese Klasse erneuert, werden Ihre Änderungen überschrieben.

/////////////////////////////////////////////////////////////////////////////
// Wrapper-Klasse CAnimation 
#ifdef _DEBUG

class CAnimation : public CWnd
{
protected:
	DECLARE_DYNCREATE(CAnimation)
public:
	static CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xb09de715, 0x87c1, 0x11d1, { 0x8b, 0xe3, 0x0, 0x0, 0xf8, 0x75, 0x4d, 0xa1 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attribute
public:

// Operationen
public:
	void SetAutoPlay(BOOL bNewValue);
	BOOL GetAutoPlay();
	void SetBackStyle(long nNewValue);
	long GetBackStyle();
	void SetCenter(BOOL bNewValue);
	BOOL GetCenter();
	void SetEnabled(BOOL bNewValue);
	BOOL GetEnabled();
	long GetHWnd();
	void SetBackColor(unsigned long newValue);
	unsigned long GetBackColor();
	long GetOLEDropMode();
	void SetOLEDropMode(long nNewValue);
	void Close();
	void Open(LPCTSTR bstrFilename);
	void Play(const VARIANT& varRepeatCount, const VARIANT& varStartFrame, const VARIANT& varEndFrame);
	void Stop();
	void OLEDrag();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.
#endif // _DEBUG

#endif // AFX_ANIMATION_H__A4310CA3_6C6D_11D4_B6EC_0000B458D891__INCLUDED_
