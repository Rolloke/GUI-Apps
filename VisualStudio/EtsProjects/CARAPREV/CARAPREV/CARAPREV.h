#if !defined __CARAPREV_H_INCLUDED
#define __CARAPREV_H_INCLUDED

class CGetAppName
{
public:
   CGetAppName();
   ~CGetAppName();
};

extern "C" __declspec(dllexport) TCHAR g_szAppName[16];

extern "C" __declspec(dllexport) HINSTANCE g_hCaraPreviewInstance;
extern "C" __declspec(dllexport) int       g_nFileVersion;
extern "C" __declspec(dllexport) void     *g_AfxCommDlgProc;
extern "C" __declspec(dllexport) COLORREF  g_CustomColors[16];
extern "C" __declspec(dllexport) UINT CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport) UINT CALLBACK CFHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport) bool      g_bDoReport;

extern "C" __declspec(dllexport) BOOL DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);


#endif // __CARAPREV_H_INCLUDED
