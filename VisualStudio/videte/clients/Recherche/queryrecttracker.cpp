// QueryRectTracker.cpp: implementation of the CQueryRectTracker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Recherche.h"
#include "QueryRectTracker.h"
#include "DisplayWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryRectTracker::CQueryRectTracker(CSmallWindow* pSmallWindow)
{
	m_pSmallWindow = pSmallWindow;
	m_rect.SetRectEmpty();
	m_rcPromilleImage.SetRectEmpty();
	m_rcPicture.SetRectEmpty();
	m_iMacroBlockWidth		= 22; //Anzahl Blöcke der Breite
	m_iMacroBlockHeight		= 18; // Anzahl Blöcke der Höhe
	m_iRectMinSizeDefault	= 30;
	m_iRectMinSizeCurrent	= 30;
	m_iRectHandleSize		= 6;

	m_nHandleSize			= m_iRectHandleSize;
	m_bRubberBand				= FALSE;

	m_bDrawStatus			= TRUE;
	m_bIsInitFromLiveWindow = FALSE;
}

CQueryRectTracker::~CQueryRectTracker()
{

}

/////////////////////////////////////////////////////////////////////////////
//Läßt den User ein neues Rechteck aufziehen
void CQueryRectTracker::DrawNewRect(CPoint point)
{
	m_nStyle = CRectTracker::dottedLine;
	m_bRubberBand = TRUE;
	TrackRubberBand((CWnd*)m_pSmallWindow, point, FALSE);
	m_bRubberBand = FALSE;

	//wenn Rechteck zu klein, auf minimale Größe vergrößern
	SetMinimumRect(TRUE);

	GetTrueRect(&m_rect);
	m_nStyle = CRectTracker::hatchedBorder;
	m_nStyle ^= CRectTracker::resizeOutside;
}
/////////////////////////////////////////////////////////////////////////////
//Setzt Rechteck und Promille Werte auf NULL
void CQueryRectTracker::SetRectEmpty()
{
	m_rect.SetRectEmpty();
	m_rcPromilleImage.SetRectEmpty();
	m_nStyle = CRectTracker::dottedLine;

}
/////////////////////////////////////////////////////////////////////////////
//verhindert Folgendes:
// - Aufziehen des Rechtecks nur im Bildbereich des ClientWindows
// - kein Verschieben des Rechtecks über den Rand des ClientWindows
// - bei zu kleinem Rechteck wird es auf Mindestgröße gesetzt
void CQueryRectTracker::OnChangedRect(const CRect& rectOld)
{
//	TRACE(_T("tkr ---- OnChangedRect() \n"));
	CRect rc;
	((CImageWindow*)m_pSmallWindow)->GetVideoClientRect(rc);

	//hat sich VideoClient window Größe geändert, neue Größe holen und sichern
	if(m_rcLastVideoClientRect != rc)
	{
		((CImageWindow*)m_pSmallWindow)->GetVideoClientRect(m_rcLastVideoClientRect);
	}

	m_rect.NormalizeRect();
	
	//Rechteck auf Bildbereich des VideoClient Fensters beschränken 
	if(	   (m_rect.left - m_rcLastVideoClientRect.left) < 0
	    || (m_rect.top - m_rcLastVideoClientRect.top) < 0
		|| (m_rect.right - m_rcLastVideoClientRect.right) > 0
		|| (m_rect.bottom - m_rcLastVideoClientRect.bottom) > 0)
	{

		 //ist nicht gezoomt, Rechteck auf VideoClientWindow beschränken
		 if(((CDisplayWindow*)m_pSmallWindow)->GetZoom().IsRectEmpty())
		 {
			m_rect = rectOld;
		 }
		 else
		 {
			 //ist gezoomt, Rechteck auf Image beschränken
			CRect rcPromilleClient, rcPromilleImage;
			CRect rcVideoClient;
			((CImageWindow*)m_pSmallWindow)->GetVideoClientRect(rcVideoClient);
			CRect rcZoom = ((CDisplayWindow*)m_pSmallWindow)->GetZoom();
	
			TRACE(_T("**** OnChangedRect m_rect           left: %i  top: %i  right: %i  bottom: %i\n"),
			m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
			
			//Promille Daten bezüglich Client
			rcPromilleClient.left	= (m_rect.left		* 1000) / rcVideoClient.Width();
			rcPromilleClient.top	= (m_rect.top		* 1000) / rcVideoClient.Height();
			rcPromilleClient.right	= (m_rect.right		* 1000) / rcVideoClient.Width();
			rcPromilleClient.bottom = (m_rect.bottom	* 1000) / rcVideoClient.Height();
	
			TRACE(_T("**** OnChangedRect rcPromilleClient left: %i  top: %i  right: %i  bottom: %i\n"),
			rcPromilleClient.left, rcPromilleClient.top, rcPromilleClient.right, rcPromilleClient.bottom);

			//Promille Daten bezüglich Image
			rcPromilleImage.left	= (((rcPromilleClient.left	 * rcZoom.Width())	/ 1000) + rcZoom.left)	*1000 / m_rcPicture.Width();
			rcPromilleImage.top		= (((rcPromilleClient.top	 * rcZoom.Height()) / 1000) + rcZoom.top)	*1000 / m_rcPicture.Height();
			rcPromilleImage.right	= (((rcPromilleClient.right	 * rcZoom.Width())	/ 1000) + rcZoom.left)	*1000 / m_rcPicture.Width();
			rcPromilleImage.bottom	= (((rcPromilleClient.bottom * rcZoom.Height()) / 1000) + rcZoom.top)	*1000 / m_rcPicture.Height();			

			TRACE(_T("**** OnChangedRect rcPromilleImage  left: %i  top: %i  right: %i  bottom: %i\n"),
			rcPromilleImage.left, rcPromilleImage.top, rcPromilleImage.right, rcPromilleImage.bottom);


			CRect rcImageAbs; // absolute Rechteck-Werte bezüglich Image
		
			rcImageAbs.left		= rcPromilleImage.left		* m_rcPicture.Width() / 1000;
			rcImageAbs.top		= rcPromilleImage.top		* m_rcPicture.Height() / 1000;
			rcImageAbs.right	= rcPromilleImage.right		* m_rcPicture.Width() / 1000;
			rcImageAbs.bottom	= rcPromilleImage.bottom	* m_rcPicture.Height() / 1000;

			TRACE(_T("**** rcImageAbs  left: %i  top: %i  right: %i  bottom: %i\n"),
				rcImageAbs.left, rcImageAbs.top, rcImageAbs.right, rcImageAbs.bottom);
			TRACE(_T("**** m_rcPicture left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rcPicture.left, m_rcPicture.top, m_rcPicture.right, m_rcPicture.bottom);

			if(	   (rcImageAbs.left - m_rcPicture.left) <= 1
				|| (rcImageAbs.top - m_rcPicture.top) <= 1
				|| (rcImageAbs.right - m_rcPicture.right) >= -1
				|| (rcImageAbs.bottom - m_rcPicture.bottom) >= -1)
			{
				TRACE(_T("** außerhalb Image\n"));
				m_rect = rectOld;
				TRACE(_T("**** rectOld   left: %i  top: %i  right: %i  bottom: %i\n"),
					rectOld.left, rectOld.top, rectOld.right, rectOld.bottom);
			}
		 }

	}

	//wenn Rechteck zu klein, auf minimale Größe vergrößern
	if(!m_bRubberBand) 
	{
		//hier nur setzten, wenn Rechteck vorhanden,
		//nicht wenn es gerade neu aufgezogen wird
		SetMinimumRect(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
//Speichert das Rechteck. Ist vor einer Rechteck-Änderung aufzurufen
void CQueryRectTracker::SaveLastRect()
{
	m_rcLastRect = m_rect;
}

/////////////////////////////////////////////////////////////////////////////
//Speichert die Größe des ClientWindows. Ist vor einer Window-Änderung aufzurufen
void CQueryRectTracker::SaveLastClientRect(CRect rcClient)
{
	m_rcLastClientRect = rcClient;
}

/////////////////////////////////////////////////////////////////////////////
//Speichert die Größe des VideoClientWindows. Ist vor einer Window-Änderung aufzurufen
void CQueryRectTracker::SaveLastVideoClientRect(CRect rcVideoClient)
{
	m_rcLastVideoClientRect = rcVideoClient;
}

/////////////////////////////////////////////////////////////////////////////
//Berechnet die absoluten Rechteck Koordinaten aus gegebenen, auf die Imagegröße
//bezogenen Promille Werten, neu. Berücksichtigt auch den Zoom Faktor
void CQueryRectTracker::RecalcNewRectPositionFromPromille(CRect rcZoom)
{
	CRect rcClient, rcVideoClient;
	
	//mit int-Werten ist Berechnung zu ungenau
	DOUBLE dLeft, dTop, dRight, dBottom;

	DOUBLE dDummyPromilleLeft = 0;
	DOUBLE dDummyPromilleTop = 0; 
	DOUBLE dDummyPromilleRight = 0;
	DOUBLE dDummyPromilleBottom = 0;

	DOUBLE dm_rectLeft;
	DOUBLE dm_rectTop;
	DOUBLE dm_rectRight;
	DOUBLE dm_rectBottom;

	((CWnd*)m_pSmallWindow)->GetClientRect(rcClient);
	((CImageWindow*)m_pSmallWindow)->GetVideoClientRect(rcVideoClient);
	if(    !m_rcPromilleImage.IsRectNull()
		&& !m_rcPicture.IsRectNull())
	{

/*		TRACE(_T("***** RecalcNewRectPositionFromPromille: Tracker: 0x%x\n"), this);
		TRACE(_T("**** RecalcNewRect Promille vorher left: %i  top: %i  right: %i  bottom: %i\n"),
			m_rcPromilleImage.left, m_rcPromilleImage.top, m_rcPromilleImage.right, m_rcPromilleImage.bottom);
		TRACE(_T("**** RecalcNewRect Zoom     vorher left: %i  top: %i  right: %i  bottom: %i\n"),
			rcZoom.left, rcZoom.top, rcZoom.right, rcZoom.bottom);
		TRACE(_T("**** RecalcNewRect m_rect   vorher left: %i  top: %i  right: %i  bottom: %i\n"),
			m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
*/	
		// Den Zoom berücksichtigen.
		if ((!rcZoom.IsRectEmpty()))
		{
//			TRACE(_T("**** RecalcNewRect mit Zoom \n"));
			//Umrechnung von Promille des Bildes auf absolute Daten auf dem Bild (720 x 560)
			CSize ImageSize = m_rcPicture.Size();
			dLeft	= ((DOUBLE)m_rcPromilleImage.left	* (DOUBLE)ImageSize.cx) / 1000 - (DOUBLE)rcZoom.left;
			dTop	= ((DOUBLE)m_rcPromilleImage.top		* (DOUBLE)ImageSize.cy) / 1000 - (DOUBLE)rcZoom.top;
			dRight	= ((DOUBLE)m_rcPromilleImage.right	* (DOUBLE)ImageSize.cx) / 1000 - (DOUBLE)rcZoom.left;
			dBottom = ((DOUBLE)m_rcPromilleImage.bottom	* (DOUBLE)ImageSize.cy) / 1000 - (DOUBLE)rcZoom.top;

			dDummyPromilleLeft	 = 1000 * dLeft	  / (DOUBLE)rcZoom.Width();
			dDummyPromilleTop	 = 1000 * dTop	  / (DOUBLE)rcZoom.Height();
			dDummyPromilleRight	 = 1000 * dRight  / (DOUBLE)rcZoom.Width();
			dDummyPromilleBottom = 1000 * dBottom / (DOUBLE)rcZoom.Height();

			dm_rectLeft		= (dDummyPromilleLeft	* (DOUBLE)rcVideoClient.Width())  / 1000 + (DOUBLE)rcVideoClient.left;
			dm_rectTop		= (dDummyPromilleTop	* (DOUBLE)rcVideoClient.Height()) / 1000 + (DOUBLE)rcVideoClient.top;
			dm_rectRight	= (dDummyPromilleRight	* (DOUBLE)rcVideoClient.Width())  / 1000 + (DOUBLE)rcVideoClient.left;
			dm_rectBottom	= (dDummyPromilleBottom	* (DOUBLE)rcVideoClient.Height()) / 1000 + (DOUBLE)rcVideoClient.top;

			m_rect.left		= (int)(dm_rectLeft		+ 0.5);
			m_rect.top		= (int)(dm_rectTop		+ 0.5);
			m_rect.right	= (int)(dm_rectRight	+ 0.5);
			m_rect.bottom	= (int)(dm_rectBottom	+ 0.5);

			//die minimale Rechteck Größe an den Zoom anpassen
			//ist einmal gezoomt ist minimales Rechteck doppelt so groß wie vorher
			m_iRectMinSizeCurrent = m_iRectMinSizeDefault * ImageSize.cx / rcZoom.Width();

		}
		else
		{
//			TRACE(_T("**** RecalcNewRect ohne Zoom \n"));
			m_rect.left		= (m_rcPromilleImage.left	* rcVideoClient.Width())  / 1000 + rcVideoClient.left;
			m_rect.top		= (m_rcPromilleImage.top	* rcVideoClient.Height()) / 1000 + rcVideoClient.top;
			m_rect.right	= (m_rcPromilleImage.right	* rcVideoClient.Width())  / 1000 + rcVideoClient.left;
			m_rect.bottom	= (m_rcPromilleImage.bottom	* rcVideoClient.Height()) / 1000 + rcVideoClient.top;


		}
/*		TRACE(_T("**** RecalcNewRect Promille nachher left: %i  top: %i  right: %i  bottom: %i\n"),
			m_rcPromilleImage.left, m_rcPromilleImage.top, m_rcPromilleImage.right, m_rcPromilleImage.bottom);
		TRACE(_T("**** RecalcNewRect Dummy    nachher left: %d  top: %d  right: %d  bottom: %d\n"),
			dDummyPromilleLeft, dDummyPromilleTop, dDummyPromilleRight, dDummyPromilleBottom);
		TRACE(_T("**** RecalcNewRect m_rect   nachher left: %i  top: %i  right: %i  bottom: %i\n"),
			m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
*/
		
		//ist Rechteck zu klein, auf minimale Größe setzten
		if(SetMinimumRect(FALSE))
		{
			TRACE(_T("**Rect too small\n"));
			SetRectPromille(rcZoom);
//			TRACE(_T("**** RecalcNewRect m_rect mini  left: %i  top: %i  right: %i  bottom: %i\n"),
//				m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//Berechnet die Promille Werte (bezogen auf das VideoClientWindow) aus gegebenen 
//absoluten Rechteckwerten (auf dem VideoClientWindow) neu
//Ist gezoomt, werden ebenfalls die neuen Promille Werte bezogen 
//auf das Image berechnte
void CQueryRectTracker::SetRectPromille(CRect rcZoom)
{
	if(!m_rcPicture.IsRectNull())
	{
		TRACE(_T("***** SetRectPromille: Tracker: 0x%x\n"), this);
		if(rcZoom.IsRectEmpty())
		{

			//falls eine Rechteckseite negativ, auf 0 setzten
			m_rect.left < 0 ? m_rect.left = 0 : m_rect.left;  
			m_rect.top < 0 ? m_rect.top = 0 : m_rect.top; 
			m_rect.right < 0 ? m_rect.right = 0 : m_rect.right; 
			m_rect.bottom < 0 ? m_rect.bottom = 0 : m_rect.bottom; 
/*	
			TRACE(_T("** SetRectPromille ohne ZOOM\n"));

			TRACE(_T("** SetRectPromille m_rect: left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
			
			TRACE(_T("** SetRectPromille: vorher left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rcPromilleImage.left, m_rcPromilleImage.top, m_rcPromilleImage.right, m_rcPromilleImage.bottom);
			
			TRACE(_T("** SetRectPromille: m_rcLastClientRect        left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rcLastClientRect.left, m_rcLastClientRect.top, m_rcLastClientRect.right, m_rcLastClientRect.bottom);
			
			TRACE(_T("** SetRectPromille: m_rcLastVideoClientRect   left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rcLastVideoClientRect.left, m_rcLastVideoClientRect.top, m_rcLastVideoClientRect.right, m_rcLastVideoClientRect.bottom);
*/
			if(m_rcLastVideoClientRect.Width() > 0 && m_rcLastVideoClientRect.Height() > 0)
			{
				m_rcPromilleImage.left	= (m_rect.left - m_rcLastVideoClientRect.left)* 1000 / m_rcLastVideoClientRect.Width();
				m_rcPromilleImage.top	= (m_rect.top - m_rcLastVideoClientRect.top) * 1000 / m_rcLastVideoClientRect.Height();
				m_rcPromilleImage.right	= (m_rect.right - m_rcLastVideoClientRect.left) * 1000 / m_rcLastVideoClientRect.Width();
				m_rcPromilleImage.bottom = (m_rect.bottom - m_rcLastVideoClientRect.top) * 1000 / m_rcLastVideoClientRect.Height();
			}
			if(m_rcPromilleImage.right > 1000)
			{
				m_rcPromilleImage.right = 1000;
			}

			if(m_rcPromilleImage.bottom > 1000)
			{
				m_rcPromilleImage.bottom = 1000;
			}
//			TRACE(_T("** SetRectPromille:        left: %i  top: %i  right: %i  bottom: %i\n"),
//				m_rcPromilleImage.left, m_rcPromilleImage.top, m_rcPromilleImage.right, m_rcPromilleImage.bottom);
		}
		else
		{
/*
			TRACE(_T("** SetRectPromille mit  ZOOM\n"));
			TRACE(_T("** SetRectPromille:m_rcLastRect left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rcLastRect.left, m_rcLastRect.top, m_rcLastRect.right, m_rcLastRect.bottom);
			TRACE(_T("** SetRectPromille:    m_rect   left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
			TRACE(_T("** SetRectPromille: Prom vorher left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rcPromilleImage.left, m_rcPromilleImage.top, m_rcPromilleImage.right, m_rcPromilleImage.bottom);
*/
			CSize ImageSize = m_rcPicture.Size();
		
			//mit int-Werten ist Berechnung zu ungenau
			DOUBLE dPromilleClientLeft;
			DOUBLE dPromilleClientTop;
			DOUBLE dPromilleClientRight;
			DOUBLE dPromilleClientBottom;

			DOUBLE drcPromilleLeft;
			DOUBLE drcPromilleTop;
			DOUBLE drcPromilleRight;
			DOUBLE drcPromilleBottom;
	
			TRACE(_T("** SetRectPromille: m_rcLastVideoClientRect left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rcLastVideoClientRect.left, m_rcLastVideoClientRect.top, m_rcLastVideoClientRect.right, m_rcLastVideoClientRect.bottom);
			
			if(    m_rcLastVideoClientRect.Width() > 0 
				&& m_rcLastVideoClientRect.Height() > 0
				&& ImageSize.cx > 0
				&& ImageSize.cy > 0)
			{
				//Promille Daten bezüglich Client
				dPromilleClientLeft		= ((DOUBLE)(m_rect.left	- m_rcLastVideoClientRect.left)	* 1000) / (DOUBLE)m_rcLastVideoClientRect.Width();
				dPromilleClientTop		= ((DOUBLE)(m_rect.top	- m_rcLastVideoClientRect.top) * 1000) / (DOUBLE)m_rcLastVideoClientRect.Height();
				dPromilleClientRight	= ((DOUBLE)(m_rect.right - m_rcLastVideoClientRect.left) * 1000) / (DOUBLE)m_rcLastVideoClientRect.Width();
				dPromilleClientBottom	= ((DOUBLE)(m_rect.bottom	- m_rcLastVideoClientRect.top) * 1000) / (DOUBLE)m_rcLastVideoClientRect.Height();

				//Promille Daten bezüglich Image
				drcPromilleLeft		= (((dPromilleClientLeft	* (DOUBLE)rcZoom.Width())  / 1000) + (DOUBLE)rcZoom.left)	*1000 / (DOUBLE)ImageSize.cx;
				drcPromilleTop		= (((dPromilleClientTop		* (DOUBLE)rcZoom.Height()) / 1000) + (DOUBLE)rcZoom.top)	*1000 / (DOUBLE)ImageSize.cy;
				drcPromilleRight	= (((dPromilleClientRight	* (DOUBLE)rcZoom.Width())  / 1000) + (DOUBLE)rcZoom.left)	*1000 / (DOUBLE)ImageSize.cx;
				drcPromilleBottom	= (((dPromilleClientBottom	* (DOUBLE)rcZoom.Height()) / 1000) + (DOUBLE)rcZoom.top)	*1000 / (DOUBLE)ImageSize.cy;			

				m_rcPromilleImage.left	= (int)(drcPromilleLeft + 0.5);
				m_rcPromilleImage.top	= (int)(drcPromilleTop + 0.5);
				m_rcPromilleImage.right	= (int)(drcPromilleRight + 0.5);
				m_rcPromilleImage.bottom = (int)(drcPromilleBottom + 0.5);				
			}

//			TRACE(_T("** SetRectPromille: Prom nach   left: %i  top: %i  right: %i  bottom: %i\n"),
//				m_rcPromilleImage.left, m_rcPromilleImage.top, m_rcPromilleImage.right, m_rcPromilleImage.bottom);

		}
	}
}
/////////////////////////////////////////////////////////////////////////////
//Speichert die Image Daten
void CQueryRectTracker::SetPictureRect(CRect rcPicture)
{
	m_rcPicture = rcPicture;
}

/////////////////////////////////////////////////////////////////////////////
//Setzt das Rechteck auf seine minimale Größe
// - bei bTrack = TRUE,  beginnend beim Startpunkt topleft
// - bei bTrack = FALSE, um den Rechteck Mittelpunkt herum
BOOL CQueryRectTracker::SetMinimumRect(BOOL bTrack)
{

	BOOL bRet = FALSE;
	int iDoNotResizeIfSmallerThan = 8;
	int iZoom = 1;
	
	//ist gezoomt die minimale Größe des Rechtecks erhöhen
	if(!((CDisplayWindow*)m_pSmallWindow)->GetZoom().IsRectEmpty())
	{
		CSize pic = ((CDisplayWindow*)m_pSmallWindow)->GetPictureSize();
		CRect rcPicture(0,0,pic.cx, pic.cy);
		iZoom = rcPicture.Width()/((CDisplayWindow*)m_pSmallWindow)->GetZoom().Width();
	}

	DOUBLE dCurrentMinWidthRect = (1 + ((DOUBLE)m_rcLastVideoClientRect.Width() / (DOUBLE)m_iMacroBlockWidth)) * iZoom;
	DOUBLE dCurrentMinHeigthRect =(1 + ((DOUBLE)m_rcLastVideoClientRect.Height() / (DOUBLE)m_iMacroBlockHeight)) * iZoom;

//	TRACE(_T("TKR ??????????????????? RectWidht: %d CurrMinWidth: %d RectHeight: %d  CurrMinHeight: %d\n"),
//							m_rect.Width(), (int)(dCurrentMinWidthRect), m_rect.Height(), (int)(dCurrentMinHeigthRect));
	BOOL bWidthTooSmall	= m_rect.Width() <= (int)(dCurrentMinWidthRect) && (int)(dCurrentMinWidthRect) >= iDoNotResizeIfSmallerThan;
	BOOL bHeightTooSmall = m_rect.Height() <= (int)(dCurrentMinHeigthRect) && (int)(dCurrentMinHeigthRect) >= iDoNotResizeIfSmallerThan;

	if(bTrack)
	{
		//resize Rechteck bezüglich topleft
		if(bHeightTooSmall)
		{
			m_rect.bottom = m_rect.top + (int)dCurrentMinHeigthRect+1;
			bRet = TRUE;
		}
		if(bWidthTooSmall)
		{
			m_rect.right = m_rect.left + (int)dCurrentMinWidthRect+1;
			bRet = TRUE;
		}
	}
	else
	{
		//resize Rechteck um den Mittelpunkt
		//wird beim Rauszoomen und beim Wechsel von Live nach PlayWindow benötigt
		CPoint center = m_rect.CenterPoint();
		if(bHeightTooSmall)
		{
			m_rect.left		= center.x - (int)((dCurrentMinHeigthRect / 2) + 1.0);
			m_rect.top		= center.y - (int)((dCurrentMinHeigthRect / 2) + 1.0);
			bRet = TRUE;
		}

		if(bWidthTooSmall)
		{
			m_rect.right	= center.x + (int)((dCurrentMinWidthRect / 2) + 1.0);
			m_rect.bottom	= center.y + (int)((dCurrentMinWidthRect / 2) + 1.0);
			bRet = TRUE;
		}
	}

	//wenn Rechteck nun über ClientWindow Grenzen hinausgeht -> verschieben
	//allerdings nur im nicht gezoomten Zustand
	CRect rcZoom = ((CDisplayWindow*)m_pSmallWindow)->GetZoom();
	if(rcZoom.IsRectEmpty())
	{
		BOOL bRectPositionChanged = FALSE;
		if(m_rect.left < 0)
		{
			m_rect.OffsetRect(-m_rect.left, 0);
			bRectPositionChanged = TRUE;
		}
		if(m_rect.top < 0)
		{
			m_rect.OffsetRect(0, -m_rect.top);
			bRectPositionChanged = TRUE;
		}
		if(m_rect.right > m_rcLastClientRect.right)
		{
			m_rect.OffsetRect(m_rcLastClientRect.right-m_rect.right, 0);
			bRectPositionChanged = TRUE;
		}
		if(m_rect.bottom > m_rcLastClientRect.bottom)
		{
			m_rect.OffsetRect(0, m_rcLastClientRect.bottom-m_rect.bottom);
			bRectPositionChanged = TRUE;
		}

		if(bRectPositionChanged)
		{
			TRACE(_T("TKR --------- PositionChanged\n"));
			SetRectPromille(rcZoom);
		}	
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
//Setzt die minimale Rechteckgröße auf default Wert
void CQueryRectTracker::ResetMinRectSize()
{
	m_iRectMinSizeCurrent = m_iRectMinSizeDefault;
}
/////////////////////////////////////////////////////////////////////////////
//Setzt die Rechteckgröße des Promille Rechtecks auf NULL
void CQueryRectTracker::SetRectPromilleEmpty()
{
	m_rcPromilleImage.SetRectEmpty();
}

/////////////////////////////////////////////////////////////////////////////
//Setzt LiveWindow Init Variable
//TRUE, wenn Rechteck vom LiveWindow aus im PlayWindow initialisiert wurde
//	    aber das datenbank Bild noch nicht angezeigt wird
//FALSE, wenn das datenbank Bild angezeigt wird
void CQueryRectTracker::SetInitFromLiveWindow(BOOL bInit)
{
	TRACE(_T("***** SetInitFromLiveWindow: %i\n"), bInit);
	TRACE(_T("***** tracker: 0x%x\n"), this);
	m_bIsInitFromLiveWindow = bInit;
}

/////////////////////////////////////////////////////////////////////////////
//resize rectangle with new client size
void CQueryRectTracker::ResizeRectangle()
{
	CRect rcClient, rcVideoClient;
	m_pSmallWindow->GetClientRect(rcClient);
	((CDisplayWindow*)m_pSmallWindow)->GetVideoClientRect(rcVideoClient);
	SaveLastClientRect(rcClient);
	SaveLastVideoClientRect(rcVideoClient);
	
	TRACE(_T("TKR ------ ResizeRectangle() Recalc\n"));
	RecalcNewRectPositionFromPromille(((CDisplayWindow*)m_pSmallWindow)->GetZoom());
	Draw(((CDisplayWindow*)m_pSmallWindow)->GetDC());
}




