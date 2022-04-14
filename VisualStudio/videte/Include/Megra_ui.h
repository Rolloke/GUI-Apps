/******************************************************************************/
/* Projekt: MEGRA                                                             */
/* Datei:   MEGRA_UI.H                                                        */
/******************************************************************************/

#define VDPI_VERSION  100  // Aktuelle Versionsnummer des Treibers
#define YUV422        1    // Shiftindex für YUV 4-2-2-Format

/*** Prototypen des VDUI ***/
BOOL	 DLLInitTerm(HMODULE hDLL, DWORD dwReason);
WORD     GetVDPIVersion(void);
WORD     GetVDPIError(WORD NewError);
WORD	 GetVGAMode(void);
void     ReadVLUT(WORD wType, WORD wStartIndex, WORD wNr, LPBYTE lpTable);
void     WriteVLUT(WORD wType, WORD wStartIndex, WORD wNr, LPBYTE lpTable);
VOID     Freeze(HWND hWnd, BOOL bFreeze);
DWORD    SetStartAddress(WORD wType, DWORD lStartAdr, WORD wLineInc);
DWORD    GetStartAddress(WORD wType);
DWORD	 GetMemLineInc(WORD wType);
WORD	 SetVRAMAccessMode(WORD wVRAMMode, WORD wMAnuellMode);
WORD	 GetVRAMAccessMode(void);
WORD     SetMemBaseBank(WORD wBase);
WORD     GetMemBaseBank(VOID);
WORD 	 SetUMAPageSize(WORD wPageSize);
WORD 	 GetUMAPageSize(VOID);
DWORD 	 SetUMABase(WORD wBase);
DWORD 	 GetUMABaseAddr(VOID);
DWORD 	 SetUMABankRange(DWORD dwBankRange);
DWORD 	 GetUMABankRange(VOID);
VOID     SetWindowField(WORD wType, LPRECT lpRect);
VOID     GetWindowField(WORD wTYPE, LPRECT lpRect);
COLORREF SetColorKey(HWND hWnd, WORD wColorKey);
WORD	 GetColorKey(VOID);
WORD     SetKeyMask(WORD wMask);
WORD     GetKeyMask(VOID);
WORD     SetVLUTMode(WORD wMode);
WORD     GetVLUTMode(VOID);
WORD     SetDisplayMode(WORD wMode);
WORD     GetDisplayMode(VOID);
WORD     SetAcqMode(WORD wMode);
WORD     GetAcqMode(VOID);
WORD     SetCKeyMode(WORD wMode, WORD wDelay);
WORD     GetCKeyMode(LPWORD wDelay);
WORD     SetFilterMode(WORD wMode);
WORD     GetFilterMode(VOID);
WORD     SetMatrixMode(WORD wMode);
WORD     GetMatrixMode(VOID);
DWORD    SetMemIndex(DWORD lAddress);
DWORD    GetMemIndex(VOID);
WORD     SetMemInc(WORD wInc);
WORD     GetMemInc(VOID);
DWORD    SetAcqMask(DWORD lMask);
DWORD    GetAcqMask(VOID);
DWORD    SetCPUMask(DWORD lMask);
DWORD    GetCPUMask(VOID);
VOID     SetAcquisitionWindow(HWND hWnd, LPRECT lpRect);
VOID     SetDisplayWindow(HWND hWnd, LPRECT lpRect);
VOID     SetOutputWindow(HWND hWnd, LPRECT lpRect);
WORD     SetJustification(HWND hWnd, WORD wFactor, WORD wValue);
WORD     GetJustification(WORD wFactor);
int      SetLuminance(HWND hWnd, WORD wType, int nValue);
int      GetLuminance(WORD wSource, WORD wType);
int      SetHue(HWND hWnd, int nHue);
int      GetHue(WORD wSource);
int		 SetPllDiv(HWND hWnd, int nPllDiv);
int		 GetPllDiv(void);
int		 SetHScaleCorrection(int nHCorr);
int		 GetHScaleCorrection(void);

WORD     SetZoom(HWND hWnd, WORD wType, WORD wZoom);
WORD     GetZoom(WORD wType);
WORD     SetScale(WORD wType, WORD wScale);
WORD   	 GetScale(WORD wType);
WORD     SetVideoFormat(HWND hWnd, WORD wFormat);
WORD     GetVideoFormat(void);
WORD     SetVideoType(HWND hWnd, WORD wType);
WORD     GetVideoType(WORD wSource);
WORD     SetInputSource(HWND hWndFrom, WORD wSource);
WORD     GetInputSource(VOID);
WORD     SetInputType(HWND hWnd, WORD wType);
WORD     GetInputType(VOID);
WORD     SetFilter(HWND hWnd, WORD wFilter);
WORD     GetFilter(WORD wFilterDir);
VOID     SetScreenWritePosition(WORD wX, WORD wY);
VOID     SetScreenReadPosition(WORD wX, WORD wY);
VOID     RescaleOutputWindow(VOID);
LONG     GetVCAddress(WORD wX,WORD wY);
VOID     SelectFilter(VOID);
BOOL     CheckIfZoomed(VOID);
VOID 	 Scroll(WORD wX, WORD wY);
VOID	 SymetrieSize(LPRECT lpRect);
VOID	 SetDlgText(WORD wType, LPSTR lpszText);
VOID 	 SaveBitmap(UINT nDestFormat);
BOOL	 GetBitmap(UINT nDestFormat, LPCRECT lpRect, HGLOBAL &hBitmap, DWORD &dwSize);

//VOID 	 SendEchoMail(LPCSTR lpszMsg, HWND hWndFrom, LONG lExtra);
VOID 	 SendEchoMail(UINT nMsg, HWND hWndFrom, LONG lExtra);
int		 ActivatePosDialog(HWND hWndParent);                       
int		 ActivateDelayDialog(HWND hWndParent);
int		 ActivateColorDialog(HWND hWndParent);
int		 ActivateAcquisitionDialog(HWND hWndParent);
int		 ActivateFilterDialog(HWND hWndParent);
int		 ActivatePllDialog(HWND hWndParent);

/*** Prototypen einiger Hilfsfunktionen des VDUI's ***/
int      Min(int nValue1, int nValue2);
int      Max(int nValue1, int nValue2);
BOOL     GetWindowsVersion(DWORD &dwVersion);
BOOL     CheckSource(LPWORD lpwSource);
void     Waiting(WORD wCount);
void	 WaitForVGARetrayce(void);
