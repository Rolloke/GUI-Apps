// idipapi.h

#ifndef _IdipApi_h
#define _IdipApi_h

EXPORT LRESULT DoAbout(HWND hWnd, UINT nResourceID);
EXPORT LRESULT DoLogin(HWND hWnd, UINT nResourceID, 
					   LPSTR pszUser, DWORD dwMaxUserLen,
					   LPSTR pszPassword, DWORD dwMaxPasswordLen);
class CAutoLogout;
///////////////////////////
// Anstatt die beiden Funktionen DoAbout und DoLogin zu exportieren werden nun die 
// folgenden beiden Member exportiert.
class __declspec( dllexport ) CSharedDlg
{
public:
    LRESULT LoginDlg(HWND hWnd, UINT nResourceID, CString &sUser, CString &sPassword);
	LRESULT LoginFailed(HWND hWnd, UINT nResourceID);
	LRESULT DongleFailed(HWND hWnd, UINT nResourceID);
    LRESULT AboutDlg(HWND hWnd, UINT nResourceID, BOOL bShowImagnLicense = FALSE);
	LRESULT AutoConnectionDlg(CWnd* pWnd, 
							  CAutoLogout* pAutoLogout, 
							  UINT uMessage, 
							  WPARAM wParam, 
							  LPARAM lParam);
	LRESULT AutoApplicationDlg(CWnd* pWnd, 
							   CAutoLogout* pAutoLogout, 
							   UINT uMessage, 
							   WPARAM wParam, 
							   LPARAM lParam);
};

#endif
