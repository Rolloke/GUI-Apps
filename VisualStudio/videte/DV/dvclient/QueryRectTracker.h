// QueryRectTracker.h: interface for the CQueryRectTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYRECTTRACKER_H__9D63CC08_703A_11D5_8DE3_0050BF554CAA__INCLUDED_)
#define AFX_QUERYRECTTRACKER_H__9D63CC08_703A_11D5_8DE3_0050BF554CAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDisplayWindow;

//Ermöglicht das Aufziehen eines Rechtecks auf einem Live- oder Datenbankbild
//Die Größe des Rechtecks wird beim Zoomen automatisch angepaßt
class CQueryRectTracker : public CRectTracker  
{
public:
	CQueryRectTracker(CDisplayWindow* pDisplayWnd);
	virtual ~CQueryRectTracker();

// Operations
public:

	void			DrawNewRect(CPoint point); //zeichnet neues Rechteck
	void			SetRectEmpty();			//setzt alle Rechteck Koords NULL
	inline CRect*	GetRect();				//liefert Rechteck Koordinaten bezüglich Client
	inline CRect*	GetRectPromilleImage();	//liefert Promille Werte des Rechtecks 
											//bez. des zugrunde liegenden Bildes
	void			SaveLastRect();			//speichert Rechteck vor dessen Änderung
	void			SaveLastClientRect(CRect rcClient);//speichert Client Window Koords
	inline CRect*	GetLastRect();			//liefert Rechteck vor dessen Änderung
	void			RecalcNewRectPositionFromPromille(CRect rcZoom);
	void			SetRectPromille(CRect rcZoom);
	void			SetRectPromilleEmpty();	//setzt Rechteck auf NULL
	void			SetPictureRect(CRect rcPicture);
	BOOL			SetMinimumRect(BOOL bTrack);
	void			SetInitFromLiveWindow(BOOL bInit);
	inline BOOL		GetInitFromLiveWindow();
	void			ResizeRectangle();

//functions
public:
	virtual void OnChangedRect(const CRect& rectOld);


//members
private:

	CDisplayWindow* m_pDisplayWnd;			//Pointer auf DisplayWindow, auf dem
											//sich das Rechteck befindet
	CRect			m_rcLastRect;			//Rechteck Daten vor einer Rechteck Änderung
	CRect			m_rcLastClientRect;		//ClientWindow Daten des vorherigen Clients
	CRect			m_rcPromilleImage;		//Promille Daten bezüglich Image (720 x 560)
	CRect			m_rcPicture;			//Image Daten (720 x 560)
	int				m_iRectHandleSize;		//Größe der Rechteckmarkierungen zum Resizen
	BOOL			m_bRubberBand;
	BOOL			m_bDrawStatus;
	BOOL			m_bIsInitFromLiveWindow; //ist TRUE, wenn Rechteck im PlayWindow vom
									//LiveWindow aus zwar initialisiert wurde, aber noch
									//kein anzuzeigendes Bild aus der Datenbank geholt
									//wurde. Erst danach können die restlichen Variablen
									//z.B. m_rcPicture initialisiert und 
									//das Rechteck im PlayWindow genutzt werden
	int				m_iMacroBlockWidth;		//Anzahl Blöcke in der Breite des Images
	int				m_iMacroBlockHeight;	//Anzahl Blöcke in der Höhe des Images
};
//
//////////////////////////////////////////////////////////////////////
//Liefert das aktuelle aufgezogene Rechteck
inline CRect* CQueryRectTracker::GetRect()
{
	return &m_rect;
}

//////////////////////////////////////////////////////////////////////
//Liefert das jeweils zuletzt aufgezogene Rechteck vor dessen Änderung
inline CRect* CQueryRectTracker::GetLastRect()
{
	return &m_rcLastRect;
}
//////////////////////////////////////////////////////////////////////
//Liefert die Promille Werte des Rechtecks bez. des zugrunde liegenden Bildes
inline CRect* CQueryRectTracker::GetRectPromilleImage()
{
	return &m_rcPromilleImage;
}
//////////////////////////////////////////////////////////////////////
//Liefert den Status der Initialisierung des PlayWindow Rechtecks
inline BOOL CQueryRectTracker::GetInitFromLiveWindow()
{
	TRACE(_T("***** GetInitFromLiveWindow: %i \n"),m_bIsInitFromLiveWindow);
	TRACE(_T("***** tracker: 0x%x\n"), this);
	return m_bIsInitFromLiveWindow;
}



#endif // !defined(AFX_QUERYRECTTRACKER_H__9D63CC08_703A_11D5_8DE3_0050BF554CAA__INCLUDED_)
