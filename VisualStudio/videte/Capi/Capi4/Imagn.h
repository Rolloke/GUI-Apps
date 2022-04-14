//  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ·
//  ³   Copyright (c) 1995-1996, Imagine Software Solututions, Inc.,      º
//  ³                            All rights Reserved.                     º
//  ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼

#ifndef IMAGN_WIN
#define IMAGN_WIN

#ifndef _WIN32
#ifdef  __FLAT__
#define _WIN32
#endif
#else
#ifndef __FLAT__
#define __FLAT__
#endif
#endif

#ifndef IM_ENTRY
#ifdef _WIN32
#define IM_ENTRY __stdcall
#ifndef  __BORLANDC__
#ifndef IM_IMPORT
#define IM_IMPORT __declspec(dllimport)
#endif
#else
#define IM_IMPORT
#endif
#else
#define IM_ENTRY FAR PASCAL
#define IM_IMPORT
#endif
#endif

#ifndef __IM_FUNCTIONS__
#define __IM_FUNCTIONS__
#ifdef __cplusplus
extern "C" {
#endif

IM_IMPORT long IM_ENTRY IM_Load( 
                    long BufId, 
                    const char FAR *FileName 
                    );

IM_IMPORT long IM_ENTRY IM_Open( 
                    long BufId, 
                    const char FAR *FileName 
                    );

IM_IMPORT long IM_ENTRY IM_DefineImage( 
                    long BufId, 
                    const char FAR *FileName 
                    );

IM_IMPORT long IM_ENTRY IM_LoadEx( 
                    long BufId, 
                    const char FAR *FileName, 
                    long Offset, 
                    long Length, 
                    long PageNo 
                    );

IM_IMPORT long IM_ENTRY IM_OpenEx( 
                    long BufId, 
                    const char FAR *FileName, 
                    long Offset, 
                    long Length, 
                    long PageNo 
                    );

IM_IMPORT long IM_ENTRY IM_DefineImageEx( 
                    long BufId, 
                    const char FAR *FileName, 
                    long Offset, 
                    long Length, 
                    long PageNo 
                    );


IM_IMPORT long IM_ENTRY IM_LoadData( 
                    long BufId, 
                    unsigned long Length, 
                    const void FAR *ImageData 
                    );

IM_IMPORT long IM_ENTRY IM_OpenData( 
                    long BufId, 
                    unsigned long Length, 
                    const void FAR *ImageData 
                    );

IM_IMPORT long IM_ENTRY IM_DefineImageData( 
                    long BufId, 
                    unsigned long Length, 
                    const void FAR *ImageData 
                    );


IM_IMPORT long IM_ENTRY IM_LoadDataEx( 
                    long BufId, 
                    unsigned long DataLen, 
                    const void FAR *ImageData, 
                    long Offset, 
                    long Length, 
                    long PageNo 
                    );


IM_IMPORT long IM_ENTRY IM_OpenDataEx( 
                    long BufId, 
                    unsigned long DataLen, 
                    const void FAR *ImageData, 
                    long Offset, 
                    long Length, 
                    long PageNo 
                    );


IM_IMPORT long IM_ENTRY IM_DefineImageDataEx( 
                    long BufId, 
                    unsigned long DataLen, 
                    const void FAR *ImageData, 
                    long Offset, 
                    long Length, 
                    long PageNo 
                    );


#if defined(__FLAT__) && (defined(_tagVARIANT_DEFINED) || defined(__oaidl_h__))

IM_IMPORT long IM_ENTRY IM_DefineImageVar( 
                    long BufId, 
                    unsigned long Length, 
                    VARIANT FAR *ImageData 
                    );

IM_IMPORT long IM_ENTRY IM_OpenVar( 
                    long BufId, 
                    unsigned long Length, 
                    VARIANT FAR *ImageData 
                    );

IM_IMPORT long IM_ENTRY IM_LoadVar( 
                    long BufId, 
                    unsigned long Length, 
                    VARIANT FAR *ImageData 
                    );


#endif

IM_IMPORT void IM_ENTRY IM_WndProc( 
                    HWND hwnd, 
                    unsigned int msg, 
                    WPARAM wParam, 
                    LPARAM lParam 
                    );


IM_IMPORT void IM_ENTRY IM_Paint( 
                    HWND hWnd, 
                    HDC hdc, 
                    const LPRECT rc 
                    );


IM_IMPORT long IM_ENTRY IM_DefineView( 
                    HWND hwnd, 
                    long ViewId, 
                    long BufId, 
                    long locx, 
                    long locy, 
                    long sizex, 
                    long sizey, 
                    long attrib 
                    );


IM_IMPORT long IM_ENTRY IM_SetViewBuffer( 
                    long ViewId, 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_SetViewImage( 
                    long viewid, 
                    char FAR *filename 
                    );

IM_IMPORT void IM_ENTRY IM_SetViewLogSize( 
                    long ViewId, 
                    long sizex, 
                    long sizey 
                    );


IM_IMPORT long IM_ENTRY IM_DeleteView( 
                    long view_nbr 
                    );


IM_IMPORT long IM_ENTRY IM_SetViewPos( 
                    long viewid, 
                    long x_pos, 
                    long y_pos 
                    );


IM_IMPORT long IM_ENTRY IM_GetViewPosX( 
                    long viewid 
                    );

IM_IMPORT long IM_ENTRY IM_GetViewPosY( 
                    long viewid 
                    );

IM_IMPORT long IM_ENTRY IM_SetWindowPos( 
                    HWND hwnd, 
                    long x_pos, 
                    long y_pos 
                    );

IM_IMPORT BOOL IM_ENTRY IM_Init( 
                    HANDLE hInst 
                    );

IM_IMPORT void IM_ENTRY IM_DeInit( void );

IM_IMPORT long IM_ENTRY IM_GetHeight( 
                    long BufId, 
                    long DisplayWidth 
                    );

IM_IMPORT long IM_ENTRY IM_GetWidth( 
                    long BufId, 
                    long DisplayHeight 
                    );

IM_IMPORT long IM_ENTRY IM_SizeX( 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_SizeY( 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_Colors( 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_Pages( 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_BufStatus( 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_RetCode( 
                    long BufId 
                    );

IM_IMPORT BOOL IM_ENTRY IM_SetAlignment( 
                    long ViewId, 
                    long AlignFlags 
                    );

IM_IMPORT BOOL IM_ENTRY IM_SetViewOptions( 
                    long ViewId, 
                    long Options 
                    );

IM_IMPORT void IM_ENTRY IM_Idle( void );


IM_IMPORT long IM_ENTRY IM_SelPal( 
                    HWND hWnd, 
                    long far *BufList 
                    );


IM_IMPORT long IM_ENTRY IM_SelPalA( 
                    HWND hWnd, 
                    char FAR *BufList 
                    );

IM_IMPORT long IM_ENTRY IM_SetPalWindow( 
                    HWND hWnd, 
                    HWND hPalWnd 
                    );

IM_IMPORT long IM_ENTRY IM_SetViewScale( 
                    long Viewid, 
                    long ViewScale 
                    );

IM_IMPORT long IM_ENTRY IM_GetViewScale( 
                    long Viewid 
                    );

IM_IMPORT long IM_ENTRY IM_GetLogHeight( 
                    long Viewid 
                    );

IM_IMPORT long IM_ENTRY IM_GetLogWidth( 
                    long Viewid 
                    );

IM_IMPORT long IM_ENTRY IM_SetAspect( 
                    long BufId, 
                    long AspectX, 
                    long AspectY 
                    );

IM_IMPORT long IM_ENTRY IM_AspectX( 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_AspectY( 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_GetFilename( 
                    long BufId, 
                    char FAR *Dest, 
                    int MaxLen 
                    );


IM_IMPORT HPALETTE IM_ENTRY IM_GetPalette( 
                    HWND hWnd 
                    );

IM_IMPORT HWND IM_ENTRY IM_GetChildWin( 
                    HWND hWnd 
                    );

IM_IMPORT void IM_ENTRY IM_RT( 
                    unsigned long rt1, 
                    unsigned long rt2, 
                    unsigned long rt3 
                    );


IM_IMPORT void IM_ENTRY IM_UL( 
                    unsigned long ul1, 
                    unsigned long ul2, 
                    unsigned long ul3, 
                    unsigned long ul4 
                    );


IM_IMPORT long IM_ENTRY IM_GetUniqueView( void );

IM_IMPORT long IM_ENTRY IM_GetUniqueBuf( void );

IM_IMPORT BOOL IM_ENTRY IM_GetMessage( 
                    long ErrorCode, 
                    char FAR *DestBuf, 
                    int MaxLen 
                    );


IM_IMPORT unsigned long IM_ENTRY IM_PaintDC( 
                    HDC hdc, 
                    long bufid, 
                    const LPRECT view, 
                    const LPRECT LogSize, 
                    const LPRECT Paint, 
                    long Options 
                    );


IM_IMPORT unsigned long IM_ENTRY IM_PaintDCClip( 
                    HDC hdc, 
                    long bufid, 
                    const LPRECT view, 
                    const LPRECT LogSize, 
                    const LPRECT Paint, 
                    long Options, 
                    const LPRECT Clip 
                    );


IM_IMPORT unsigned long IM_ENTRY IM_PaintDCA( 
                    HDC hdc, 
                    long bufid, 
                    const char FAR *view, 
                    const char FAR *LogSize, 
                    const char FAR *rc, 
                    long Options 
                    );


IM_IMPORT unsigned long IM_ENTRY IM_PaintDCAClip( 
                    HDC hdc, 
                    long bufid, 
                    const char FAR *view, 
                    const char FAR *LogSize, 
                    const char FAR *rc, 
                    long Options, 
                    const char FAR *Clip 
                    );


typedef unsigned long (IM_ENTRY *IM_OPENCB)( 
                    unsigned long OpenInfo, 
                    unsigned long mode 
                    );

typedef unsigned long (IM_ENTRY *IM_READCB)( 
                    unsigned long UserHandle, 
                    void FAR *dest, 
                    unsigned long count 
                    );

typedef unsigned long (IM_ENTRY *IM_SEEKCB)( 
                    unsigned long UserHandle, 
                    unsigned long seek, 
                    int SeekType 
                    );

typedef unsigned long (IM_ENTRY *IM_CLOSECB)( 
                    unsigned long UserHandle 
                    );


IM_IMPORT long IM_ENTRY IM_DefineImageCB( 
                    long bufid, 
                    IM_OPENCB OpenCB, 
                    IM_READCB ReadCB, 
                    IM_SEEKCB SeekCB, 
                    IM_CLOSECB CloseCB, 
                    unsigned long Instance 
                    );


IM_IMPORT long IM_ENTRY IM_OpenCB( 
                    long bufid, 
                    IM_OPENCB OpenCB, 
                    IM_READCB ReadCB, 
                    IM_SEEKCB SeekCB, 
                    IM_CLOSECB CloseCB, 
                    unsigned long Instance 
                    );


IM_IMPORT long IM_ENTRY IM_LoadCB( 
                    long bufid, 
                    IM_OPENCB OpenCB, 
                    IM_READCB ReadCB, 
                    IM_SEEKCB SeekCB, 
                    IM_CLOSECB CloseCB, 
                    unsigned long Instance 
                    );


IM_IMPORT long IM_ENTRY IM_DefineImageCBEx( 
                    long bufid, 
                    IM_OPENCB OpenCB, 
                    IM_READCB ReadCB, 
                    IM_SEEKCB SeekCB, 
                    IM_CLOSECB CloseCB, 
                    unsigned long Instance, 
                    long Offset, 
                    long Length, 
                    long PageNo 
                    );


IM_IMPORT long IM_ENTRY IM_OpenCBEx( 
                    long bufid, 
                    IM_OPENCB OpenCB, 
                    IM_READCB ReadCB, 
                    IM_SEEKCB SeekCB, 
                    IM_CLOSECB CloseCB, 
                    unsigned long Instance, 
                    long Offset, 
                    long Length, 
                    long PageNo 
                    );


IM_IMPORT long IM_ENTRY IM_LoadCBEx( 
                    long bufid, 
                    IM_OPENCB OpenCB, 
                    IM_READCB ReadCB, 
                    IM_SEEKCB SeekCB, 
                    IM_CLOSECB CloseCB, 
                    unsigned long Instance, 
                    long Offset, 
                    long Length, 
                    long PageNo 
                    );


IM_IMPORT long IM_ENTRY IM_WriteCBData( 
                    HWND hWnd, 
                    void FAR *ImageData 
                    );


IM_IMPORT void IM_ENTRY IM_SetCBLength( 
                    HWND hWnd, 
                    long Length 
                    );

IM_IMPORT long IM_ENTRY IM_DefineImageCBM( 
                    long BufferId, 
                    HWND hWnd, 
                    unsigned int msg 
                    );

IM_IMPORT long IM_ENTRY IM_OpenCBM( 
                    long BufferId, 
                    HWND hWnd, 
                    unsigned int msg 
                    );

IM_IMPORT long IM_ENTRY IM_LoadCBM( 
                    long BufferId, 
                    HWND hWnd, 
                    unsigned int msg 
                    );

IM_IMPORT long IM_ENTRY IM_GetCBReadLength( 
                    HWND hWnd 
                    );

IM_IMPORT long IM_ENTRY IM_GetCBReadOffset( 
                    HWND hWnd 
                    );


typedef void (IM_ENTRY *IM_VIEWPAINTCB)( 
                    unsigned long instance, 
                    HDC hDC, 
                    RECT FAR *view, 
                    RECT FAR *rc 
                    );

IM_IMPORT long IM_ENTRY IM_SetViewPaintCB( 
                    long ViewId, 
                    IM_VIEWPAINTCB PaintCB, 
                    unsigned long Instance 
                    );


typedef void (IM_ENTRY *IM_BUFSTATCB)( 
                    unsigned long instance, 
                    long BufId, 
                    int status, 
                    int CBData 
                    );

IM_IMPORT long IM_ENTRY IM_SetBufStatusCB( 
                    long bufid, 
                    IM_BUFSTATCB callback, 
                    unsigned long instance 
                    );

typedef void (IM_ENTRY *IM_VIEWSTATCB)( 
                    unsigned long instance, 
                    long View, 
                    int EventType, 
                    int CBData 
                    );

IM_IMPORT long IM_ENTRY IM_SetViewStatusCB( 
                    long ViewId, 
                    IM_VIEWSTATCB callback, 
                    unsigned long instance 
                    );


IM_IMPORT long IM_ENTRY IM_RotateView( 
                    long view_nbr, 
                    int rotation, 
                    int mode 
                    );

IM_IMPORT long IM_ENTRY IM_RotateImage( 
                    long buf, 
                    int rotation, 
                    int mode 
                    );

IM_IMPORT long IM_ENTRY IM_MirrorView( 
                    long view_nbr, 
                    int mode 
                    );

IM_IMPORT long IM_ENTRY IM_MirrorImage( 
                    long Bufid, 
                    int mode 
                    );

IM_IMPORT int IM_ENTRY IM_SetSuspendMask( 
                    int Mask 
                    );


IM_IMPORT BOOL IM_ENTRY IM_GetViewRect( 
                    long ViewId, 
                    RECT FAR *rect 
                    );


IM_IMPORT long IM_ENTRY IM_InvPal( 
                    long BufId 
                    );

IM_IMPORT int IM_ENTRY IM_BeginReport( 
                    const char FAR *ModuleName 
                    );

IM_IMPORT int IM_ENTRY IM_EndReport( void );


IM_IMPORT HPALETTE IM_ENTRY IM_BldPal( 
                    HDC hdc, 
                    long far *BufList 
                    );


IM_IMPORT long IM_ENTRY IM_DefineImageRes( 
                    long BufId, 
                    HANDLE hInstance, 
                    const char FAR *Resource, 
                    const char FAR *ResType 
                    );

IM_IMPORT long IM_ENTRY IM_OpenRes( 
                    long BufId, 
                    HANDLE hInstance, 
                    const char FAR *Resource, 
                    const char FAR *ResType 
                    );

IM_IMPORT long IM_ENTRY IM_LoadRes( 
                    long BufId, 
                    HANDLE hInstance, 
                    const char FAR *Resource, 
                    const char FAR *ResType 
                    );

IM_IMPORT long IM_ENTRY IM_ViewToPixelX( 
                    long viewid, 
                    long offsetx 
                    );

IM_IMPORT long IM_ENTRY IM_ViewToPixelY( 
                    long viewid, 
                    long offsety 
                    );

IM_IMPORT long IM_ENTRY IM_PixelToViewX( 
                    long viewid, 
                    long offsetx 
                    );

IM_IMPORT long IM_ENTRY IM_PixelToViewY( 
                    long viewid, 
                    long offsety 
                    );

IM_IMPORT HWND IM_ENTRY IM_FindhWnd( 
                    HWND hWnd, 
                    char FAR *title, 
                    BOOL FindClassFlag 
                    );


IM_IMPORT HWND IM_ENTRY IM_FindChild( 
                    HWND hWnd, 
                    RECT FAR *rc 
                    );


IM_IMPORT int IM_ENTRY IM_DefineWin( 
                    HWND hWnd 
                    );

IM_IMPORT long IM_ENTRY IM_Resume( 
                    long bufid 
                    );


IM_IMPORT HGLOBAL IM_ENTRY IM_CreateDIB( 
                    long bufid 
                    );


IM_IMPORT HGLOBAL IM_ENTRY IM_CreateDIBEx( 
                    long bufid, 
                    long sizex, 
                    long sizey, 
                    RECT FAR *clip, 
                    long BitsPerPixel, 
                    HPALETTE hPal, 
                    long Options 
                    );

IM_IMPORT long IM_ENTRY IM_SetViewClip( 
                    long viewid, 
                    int left, 
                    int top, 
                    int right, 
                    int bottom 
                    );


IM_IMPORT long IM_ENTRY IM_GetPaintOptions( 
                    long viewid 
                    );

IM_IMPORT long IM_ENTRY IM_SetZoomAdj( 
                    long viewid, 
                    int percentage 
                    );

IM_IMPORT long IM_ENTRY IM_ZoomPercent( 
                    long viewid, 
                    int percentage 
                    );

IM_IMPORT long IM_ENTRY IM_ZoomPercentEx( 
                    long viewid, 
                    int percentage, 
                    int locx, 
                    int locy 
                    );

IM_IMPORT long IM_ENTRY IM_SetTool( 
                    long ViewId, 
                    unsigned int ShiftStatus, 
                    unsigned int Tool 
                    );


IM_IMPORT long IM_ENTRY IM_GetViewOptions( 
                    long ViewId 
                    );


IM_IMPORT HWND IM_ENTRY IM_GetViewChild( 
                    long ViewId 
                    );


IM_IMPORT long IM_ENTRY IM_GetImageType( 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_GetImageCompr( 
                    long BufId 
                    );

IM_IMPORT BOOL IM_ENTRY IM_GetImageTypeStr( 
                    long BufId, 
                    char FAR *DestBuf, 
                    int MaxLen 
                    );

IM_IMPORT BOOL IM_ENTRY IM_GetImageComprStr( 
                    long BufId, 
                    char FAR *DestBuf, 
                    int MaxLen 
                    );


IM_IMPORT long IM_ENTRY IM_GetBitsPerPixel( 
                    long BufId 
                    );

IM_IMPORT long IM_ENTRY IM_ShowImageInfo( 
                    long BufId 
                    );


IM_IMPORT long IM_ENTRY IM_CopyBufToClip( 
                    long BufId 
                    );


IM_IMPORT long IM_ENTRY IM_CopyBufToClipEx( 
                    long BufId, 
                    const LPRECT Clip 
                    );


IM_IMPORT long IM_ENTRY IM_SetWindowPal( 
                    HWND hWnd, 
                    HPALETTE hPal 
                    );


IM_IMPORT long IM_ENTRY IM_InvalidateView( 
                    long ViewId 
                    );

IM_IMPORT long IM_ENTRY IM_InvalidateBuf( 
                    long BufId 
                    );

IM_IMPORT HANDLE IM_ENTRY IM_GetBufHand( 
                    long BufId 
                    );


IM_IMPORT int IM_ENTRY IM_GetComment( 
                    long BufId, 
                    char FAR *DestBuf, 
                    int MaxLen 
                    );


#ifdef __cplusplus
        }
#endif
#endif

#ifndef IMW_BITMAPHEADER
typedef struct _IMW_BITMAPHEADER
    {
    long            Width;
    long            Height;
    long            BitsPerPixel;
    long            TotalBitsPerPixel;
    long            BytesPerLine;
    long            Options;
    long            PalColors;
    PALETTEENTRY    Palette[1];
    } IMW_BITMAPHEADER;
#endif

#define IMWB_FILL_ORDER 0x00000001      
#define IMWB_SIDEWAYS   0x00000002      
#define IMWB_INVERT     0x00000004      
#define IMWB_MIRROR     0x00000008      

// Alignment Codes 

#define ALIGNV_TOP            0x00000001  
#define ALIGNV_BOTTOM         0x00000002  
#define ALIGNV_CENTER         0x00000000  
#define ALIGNH_LEFT           0x00000004  
#define ALIGNH_RIGHT          0x00000008  
#define ALIGNH_CENTER         0x00000000  
#define SCALE_OFF             0x00000010  
#define SCALE_DEST            0x00000020  
#define GROW_FIT              0x00000040  
#define VIEW_PROGRESSIVE      0x00000080  
#define DITHER_ENABLE         0x00000100  
#define DITHER_FAST           0x00000200  
#define ANTI_ALIAS            0x00000400  
#define VIEW_SCROLLBARS       0x00000800  
#define ERASE_OFF             0x00001000  
#define ENABLE_POPUP          0x00002000  
#define ALIGN_MASK            0x00003fff  

// Special View flags 

#define USE_TWIPS             0x00100000  
#define FIND_CHILD            0x00200000  
#define REFERENCE_FORM        0x00400000  
#define HOOK_MDICLIENT        0x00800000  
#define CREATE_CHILD_ALT      0x01000000  
#define HOOK_OWNER            0x02000000  
#define ENABLE_SCROLLBARS     0x04000000  
#define VIEW_MAX              0x08000000  
#define NO_WNDPROC_HOOK       0x10000000  
#define HOOK_PARENT           0x20000000  
#define COMMON_PARENT         0x40000000  
#define CREATE_CHILD          0x80000000  

// Special Option flags available only in IM_PrintDC 

#define IMG_SIDEWAYS          0x00080000  // Draw Image Sideways 
#define IMG_INVERT            0x00100000  // Image is Inverted (bottom up) 
#define IMG_MIRROR            0x00200000  // Image is Mirrored (right to left) 

// IMAGN Error Codes 

#define ERR_READ_ERR          1000        // Read error 
#define ERR_INV_FFMT          1002        // Invalid or unsupported file format 
#define ERR_INT_ERR           1003        // Internal system error 
#define ERR_PMAT_EOF          1004        // Premature end of file 
#define ERR_RNG               1005        // Display location out of bounds 
#define ERR_NOMEM             1008        // Out of memory error 
#define ERR_IN_PROGRESS       1010        // Operation in progress 
#define ERR_TIF_DCD_ERR       1013        // Tiff file decode error. 
#define ERR_PG_NOT_FND        1014        // Page not found. 
#define ERR_SCALE_WIN         1015        // Scaled window size has zero rows or columns. 
#define ERR_BAD_HDR           1016        // Bad image file header. 
#define ERR_INV_WIN_HAN       1017        // Invalid window handle 
#define ERR_INV_VIEW_ID       1018        // Invalid view id 
#define ERR_THUNK             1019        // Thunk initialization error 
#define ERR_SHUTDOWN          1020        // Error shutdown in progress 
#define ERR_NOT_INIT          1021        // Error task not initialized 
#define ERR_INV_ROT           1022        // Error invalid rotation code 
#define ERR_OFS_BOUNDS        1023        // File offset out of bounds 
#define ERR_BUF_TO_SMALL      1024        // Destination buffer to small 
#define ERR_INV_BUF_LIST      1025        // Invalid buffer list 
#define ERR_UNDEF_WIN_ERR     1026        // Undefined Windows Error 
#define ERR_BUF_OVFL          1027        // Internal Buffer Overflow 
#define ERR_BUF_NOTLD         1028        // Error Buffer Not Loaded 
#define ERR_NO_PAL            1029        // Error No Palette 
#define ERR_INV_PAL_LIST      1030        // Error Invalid Palette List 
#define ERR_MEM_LOCK          1031        // Global Memory Lock Error 
#define ERR_BAD_MEM           1032        // Bad Memory Pointer 
#define ERR_INV_DT            1033        // Invalid Data Type 
#define ERR_NOT_FOUND         1034        // File/Resource Not Found 
#define ERR_NOT_SUP           1035        // Function Not Supported 
#define ERR_INV_MIS_PAR       1036        // Invalid or Missing Parameters 
#define ERR_OPEN_ERR          1037        // Open Error 
#define ERR_INV_BUFID         1038        // Invalid Buffer Id 
#define ERR_SEEK_ERR          1039        // Seek Error 
#define ERR_SETCLIPFAILED     1040        // SetClipboardData Failed 
#define ERR_INV_PAL           1041        // Invalid Palette Specified 
#define ERR_INV_HDC           1042        // Invalid HDC 
#define ERR_INCORRECTV        1255        // Imagn version mismatch. 

// Buffer Status Bits 

#define BUF_DEFINED           0x0001      // An image filename has been defined 
#define BUF_OPENED            0x0002      // Image has been opened 
#define BUF_DECODED           0x0004      // Image has been decoded 
#define BUF_ERROR             0x0008      // An error has occured durring open/decode 
#define BUF_DECODING          0x0010      // Decode in progress 
#define BUF_WAIT              0x0020      // Waiting on Data 
#define BUF_PAGE_SCAN         0x0040      // Scaning for Pages 

// Rotation Mode Codes 

#define ROTATE_SET            0x0000      
#define ROTATE_LEFT           0x0001      
#define ROTATE_RIGHT          0x0002      

// Mirror Mode Codes 

#define MIRROR_RESET          0x0000      
#define MIRROR_SET            0x0001      
#define MIRROR_TOGGLE         0x0002      

#define MIRROR_HORIZONTAL     0x0000      
#define MIRROR_VERTICAL       0x0040      

// Tool Selections 

#define TOOL_NONE             0           
#define TOOL_ZOOM             1           
#define TOOL_ZOOM_RECT        2           
#define TOOL_HAND             3           
#define TOOL_MAX              4           

// Tool Shift Status 

#define TOOL_CONTROL          1           
#define TOOL_SHIFT            2           

// View Status Callback Type Codes: 

#define VCB_BUF               0           // Buffer Status has changed 
#define VCB_SCALE             2           // View Scaling has changed 
#define VCB_ORIENTATION       3           // View Orientation has changed 
#define VCB_SIZE              4           // View Size has changed 
#define VCB_OPTIONS           5           // View Options have changed 

// Image type codes 

#define IMG_UNKNOWN           0           // Unknown Image Type 
#define IMG_TIFFI             1           // Adobe TIFF file Intel Format (*.TIF; *.TIFF) 
#define IMG_TIFFM             2           // Adobe TIFF file Motorolia Format (*.TIF; *.TIFF) 
#define IMG_GIF               3           // CompuServe GIF file (*.GIF) 
#define IMG_IGIF              4           // Interlaced CompuServe GIF file (*.GIF) 
#define IMG_JPEG              5           // JPEG file (*.JPG; *.JPEG; *.JFIF) 
#define IMG_PJPEG             6           // Progressive JPEG file (*.JPG; *.JPEG; *.JFIF) 
#define IMG_TARGA             7           // TARGA file (*.TGA) 
#define IMG_PCX               8           // Z-Soft PCX file (*.PCX) 
#define IMG_DCX               9           // Z-Soft DCX file (*.DCX) 
#define IMG_BMP               10          // Windows BMP file (*.BMP) 
#define IMG_PIC               11          // Pegasus PIC file (*.PIC) 
#define IMG_PPIC              12          // Pegasus PIC file (*.PIC) 
#define IMG_PNG               13          // PNG file (*.PNG) 
#define IMG_IMSTAR            14          // Pegasus IMStar file (*.IMX) 
#define IMG_PAL               15          // Image is a Palette File (*.PAL) 
#define IMG_DIB               16          // Image is DIB (*.DIB) 

// Compression type codes 

#define CMP_NONE              0           // Uncompressed 
#define CMP_PB                1           // Packed Bits (runlength encoded) 
#define CMP_CCIT              2           // FAX CCIT 
#define CMP_G31D              3           // CCIT Group 3 1 dimensional coding 
#define CMP_G32D              4           // CCIT Group 3 2 dimensional coding 
#define CMP_G4                5           // CCIT Group 4 
#define CMP_LZW               6           // LZW Compressed 
#define CMP_IMSTAR            7           // Pegasus IMStar compression 
#define CMP_JPEG              8           // Jpeg compression 

#endif

