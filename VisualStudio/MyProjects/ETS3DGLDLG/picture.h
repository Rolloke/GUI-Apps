#if !defined(AFX_PICTURE_H__49294C42_6D0E_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_PICTURE_H__49294C42_6D0E_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mit Microsoft Visual C++ automatisch erstellte IDispatch-Kapselungsklasse(n). 

// HINWEIS: Die Inhalte dieser Datei nicht ändern. Wenn Microsoft Visual C++
// diese Klasse erneuert, werden Ihre Änderungen überschrieben.

/////////////////////////////////////////////////////////////////////////////
// Wrapper-Klasse CPicture 

#ifdef _DEBUG

class CPicture : public COleDispatchDriver
{
public:
	CPicture() {}		// Ruft den Standardkonstruktor für COleDispatchDriver auf
	CPicture(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CPicture(const CPicture& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attribute
public:
	long GetHandle();
	long GetHPal();
	void SetHPal(long);
	short GetType();
	long GetWidth();
	long GetHeight();

// Operationen
public:
	// Methode 'Render' wird wegen eines ungültigen Rückgabetyps oder Parametertyps nicht verwendet
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.
#endif // _DEBUG

#endif // AFX_PICTURE_H__49294C42_6D0E_11D4_B6EC_0000B458D891__INCLUDED_
