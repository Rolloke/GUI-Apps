struct PREVIEWPARAMETER
{
   bool bValid;                                  // sind diese Daten gültig ?
   SIZE siPageSize;                              // die Seitengröße in [LOMETRIC]
   RECT rcMargin;                                // die Ränder in [LOMETRIC]
   bool bMono;
};

bool InitPreViewWnd();
void ReleasePreViewWnd();
HWND CreatePreViewWnd(HWND,int,int,int,int);
void SetPreViewWndPageParameter(const PREVIEWPARAMETER *);
void SetPreViewMetaFile(HENHMETAFILE,bool);
void SetPreViewCallback(void *,bool);
