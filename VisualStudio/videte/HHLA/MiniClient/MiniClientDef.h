
#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))

// Messages vom Server zu den Clients
#define WM_SHOW_IMAGE				WM_USER + 1
#define WM_SET_INPUT_SOURCE			WM_USER + 2
#define WM_SET_WINDOW_POSITION		WM_USER + 3
#define WM_SAVE_PICTURE				WM_USER + 4
#define WM_SET_CLIENT_NUMBER		WM_USER + 5
#define WM_SET_CAMERA_PARAMETER		WM_USER + 6
#define WM_REQ_CAMERA_PARAMETER		WM_USER + 7
#define WM_CHECK_CAMERA				WM_USER + 8

// Messages vom Client zum Server
#define WM_CONF_CAMERA_PARAMETER	WM_USER + 20

//
#define UNKNOWN		0 
#define FREEZE_IT	1
#define SHOW_IT		2
#define REFRESH_IT	3
