#if !defined(ETS_APP_WND_MSG__INCLUDED_)
#define ETS_APP_WND_MSG__INCLUDED_

// ETS3DVIEW.exe
#define WM_UPDATE_ERROR          (WM_APP+  1)
#define WM_APPLY_TO_VIEW         (WM_APP+  3)
#define WM_COLORVIEW_MOUSE_EVENT (WM_APP+  4)

// ETS3DGL.dll
#define WM_RENDER_SCENE       (WM_APP + 100)
#define WM_TRANSFORMPOINTS    (WM_APP + 101)
#define WM_SET_VIEWPORT       (WM_APP + 102)
#define WM_SET_VIEWVOLUME     (WM_APP + 103)
#define WM_VALIDATE_LISTS     (WM_APP + 104)
#define WM_VALIDATE_COMMANDS  (WM_APP + 105)
#define WM_CHANGE_3DMODES     (WM_APP + 106)

#define WM_COPY_FRAME_BUFFER  (WM_APP + 107)
#define COPY_TO_DIB        1     // (CDibSection *), Initialisiert das Objekt und kopiert den Framebufferinhalt
#define COPY_TO_HGLOBAL    2     // (HGLOBAL**)    , Allociert eine globalen Memoryhandle
#define COPY_TO_FILE       3     // (char*)        , Speichert den Framebuffer als Bitmap in einer Datei
#define COPY_TO_HDC        4     // (ETS3DGL_Draw*), Zeichnet den Buffer in einen HDC
#define COPY_TO_ENHMETA    5     // (ETS3DGL_Draw*), Zeichnet den Buffer in einen Enhanced Metafile
#define COPY_TO_PRINTER    6     // (ETS3DGL_Draw*), Zeichnet den Buffer in einen PrinterDC
#define RENDER_TO_DIP      7     // (ETS3DGL_Draw*), Rendert direkt in eine DibSection
#define COPY_TO_MASK       0x0F  // Maske für die Copy Parameter
#define DESTROY_DIBSECTION 0x10  // Flag für DibSection zerstören
#define DELETE_DIBSECTION  0x20  // Flag für DibSection löschen

// CaraWalk.Dll
#define  WM_CHECKPOS                   (WM_APP+110)            // Sendet eine Nachricht zum überprüfen der Position im Raum
// wParam = (WALKER_POSITION, BOX_POSITION, WALKER_STARTPOS),
// lParam = Zeiger auf Vectorfeld mit den Koordinaten für :
// vPos[0] (EA) : neue Position
// vPos[1] (E)  : alte Position
// Aufruf mit SendMessage(...) !! Rückgabewert (0 : nicht Ok, 1 : Ok)
#define  WALKER_POSITION               0x01
#define  BOX_POSITION                  0x02
#define  WALKER_STARTPOS               0x04

#define  WM_3DCHECK                    (WM_APP+114)            // Update-commands für die Menü- und Toolbar-Steuerung
// wParam = ID-Command
// lParam :
#define UNCHECKED    0 // unchecked,
#define CHECKED      1 // checked,
#define RADIOCHECKED 2 // radiochecked
#define DISABLE_ITEM 3 // disable
#define ENABLE_ITEM  4 // enable

#define  WM_SETSTATUSBAR_TEXT          (WM_APP+115)
// wParam = ID-Resource

#define WM_RESET_CURSOR_AFTER_RENDERING  (WM_APP +116)

// ETS3DGL.dll
#define WM_REMOVE_PROPERTY      (WM_APP + 150)
#define WM_THREAD_SIZE          (WM_APP + 151)
#define WM_HIDE_THREAD_WND      (WM_APP + 152)
#define WM_SHOW_THREAD_WND      (WM_APP + 153)
#define WM_ATTACH_THREAD_STATES (WM_APP + 154)
#define WM_DETACH_THREAD_STATES (WM_APP + 155)
#define WM_OPENGL_STATE         (WM_APP + 156)
//      WPARAM 0 : nicht initialisiert, 1 : Initialisiert

// CARABOX.dll
#define WM_SETBOXDATA        (WM_APP+111)
#define WM_SETDLGTOTOP       (WM_APP+112)

// CARACAD
#define WM_DRAWPREVIEW   (WM_APP+4)              // Nachricht zum Zeichnen
#define WM_NEWRANGE      (WM_APP+10)

#define WM_SETMENUSTATE  (WM_APP+1)              // Eigene Nachricht (Menus sollen updatet werden)
#define WM_DOCKCHANGED   (WM_APP+2)              // Eigene Nachricht (Fenster ändern den DockingState)
#define WM_LEAVEPICKED   (WM_APP+3)              // Eigene Nachricht (Ausgewählte Objecte abwählen)
#define WM_BUTTONCONTEXT (WM_APP+4)              // Eigene Nachricht (ToolBarButtonContextMenu aufrufen !)
#define BUTTONCONTEXTOK   0x0fedc                // Antwort auf die Nachricht WM_BUTTONCONTEXT, wenn diese bearbeitet wurde
#define WM_PRINTVIEW     (WM_APP+5)              // Eigene Nachricht (Drucke den View "Dokument")
#define WM_WALLCHANGED   (WM_APP+6)              // Eigene Nachricht (eine andere Wand zum bearbeiten ausgewält)
#define WM_DOCKFRAME     (WM_APP+7)              // Eigene Nachricht (Rahmen zum Andocken von Fenstern holen)
#define WM_SETHELPLINES  (WM_APP+8)              // Eigene Nachricht (X,Y Hilfslinien setzen !)
#define WM_GETMETAFILE   (WM_APP+9)              // Neu 16.03.1999 eigene Nachricht (MetaFile vom View besorgen)
#define WM_LOADDOCUMENT  (WM_APP+10)             // Nue 10.09.1999 eigene Nachricht (Document soll geladen werden)
#define WM_RULERORIGIN   (WM_APP+11)             // neu 10.02.2000 eigene Nachricht (RuhlerOrigin Change für CFullview)
#define WM_GETPICKCOLOR  (WM_APP+12)             // neu 19.05.2000 eigene Nachricht (Farbe für die Auswahl besorgen)

#endif // ETS_APP_WND_MSG__INCLUDED_ 
