#if !defined __3D_VIEW_DLG_H_INCLUDED_
#define __3D_VIEW_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>

#include "CEtsDlg.h"
#include "BoxStructures.h"
#include "COpenGLView.h"	// Hinzugefügt von der Klassenansicht

#define CBN_CONTENT_CHANGED   20
#define BN_LIST_CHANGED       21
#define VIEW2DN_SEGMENT_MOVED  1

#define NOTIFY_KILLFOCUS_CAB_SIZE 0xffff

class C3DViewDlg: public CEtsDialog
{
public:
   C3DViewDlg();
   virtual ~C3DViewDlg();
   C3DViewDlg(HINSTANCE, int, HWND);

   void SetPropertySheet(CBoxPropertySheet*p){m_pPropertySheet=p;}
	double *GetTransModeVar();
	void    UpdateTransMode(bool bUpdate=false, int nMode=-1);
	void    UpdateCtrlStates();

   HWND  m_hwndCabinetPage;

private:
   virtual bool OnInitDialog(HWND hWnd);                       // zur initialisierung der Dialogfelder
   virtual int  OnOk(WORD nNotify);                            // Wird durch die OK-Taste ausgelöst
   virtual int  OnCommand(WORD, WORD, HWND);                   // verarbeitet WM_COMMAND´s an das Dialogfenster
   virtual int  OnMessage(UINT, WPARAM, LPARAM);               // verarbeitet Nachrichten an das Dialogfenster
   virtual void OnEndDialog(int);                              // Beendet den Dialog

   void Constructor();
	void InitCabinetData();
   void UpdateVolume();
	void InitCabinetLists();

	void ModifyDlgItemStyle(int, DWORD, DWORD);
	void NotifyCabPageSegmentMoved();
   void CatchCursorPos(POINTS);
   bool SetMenuItem(UINT nItem, UINT nCheck, UINT nEnable, HMENU hMenu=NULL);
	void SetStatusText(int, char*);

	void OnClickPhiScale();
	void OnCabLstContentChanged(HWND);
	void OnClickXbutton(HWND);
	void OnClickYbutton(HWND);
	void OnClickZbutton(HWND);
	void OnClickXScale();
	void OnClickYScale();
	void OnClickZScale();
	void OnClickSpecularColor();
	void OnClickAmbientColor();
	void OnClickLightColor();
	void OnClickPerspective();
	void OnClickAlphaBlend();
	void OnDblClickXbutton(HWND);
	void OnDblClickYbutton(HWND);
   void OnDblClickZbutton(HWND);
	void OnDblClickPhiScale();
	void OnDblClickZScale();
	void OnDblClickYScale();
	void OnDblClickXScale();

	void OnNewCabinet();
	void OnChangeCabinet(WORD, WORD, HWND);
   void OnCheckCanContainChassis();
	void OnCheckWall(int);
	void OnDeleteCabinet();

	static LRESULT CALLBACK ButtonSubClass(HWND, UINT, WPARAM, LPARAM);
   static LRESULT CALLBACK EditSubClass(HWND, UINT, WPARAM, LPARAM);

   CBoxPropertySheet *m_pPropertySheet;
	COpenGLView        m_GLView;
   double             m_dFactor[3];    // Änderungsfaktor für Maussteuerung
   int                m_nBmpWnd[3];    // IconId und HelpId für den Button
   int                m_nTxtWnd[3];    // TextId für Textfeld statt Editfeld
   int                m_nCheckBtn[3];  // abweichende Buttonstyles
   int                m_nAccuracy1;    // Genauigkeitstufe (Nachkommastellen) für X- und Y-Feld
   int                m_nAccuracy2;    // Genauigkeitstufe (Nachkommastellen) für Z-Feld
   bool               m_bSetCursor;
};

#endif //__3D_VIEW_DLG_H_INCLUDED_