// QueryRectTracker.cpp: implementation of the CQueryRectTracker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
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

CQueryRectTracker::CQueryRectTracker(CDisplayWindow* pDisplayWnd)
{
	m_pDisplayWnd = pDisplayWnd;
	m_rect.SetRectEmpty();
	m_rcPromilleImage.SetRectEmpty();
	m_rcPicture.SetRectEmpty();
	m_iMacroBlockWidth		= 22; //Anzahl Blöcke der Breite
	m_iMacroBlockHeight		= 18; // Anzahl Blöcke der Höhe
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
	TrackRubberBand((CWnd*)m_pDisplayWnd, point, FALSE);
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
// - kein Verschieben des Rechtecks über den Rand des ClientWindows
// - bei zu kleinem Rechteck wird es auf Mindestgröße gesetzt
void CQueryRectTracker::OnChangedRect(const CRect& rectOld)
{
	CRect rc;
	((CWnd*)m_pDisplayWnd)->GetClientRect(rc);

	//hat sich Client window Größe geändert, neue Größe holen
	if(m_rcLastClientRect != rc)
	{
		TRACE(_T("** rc != ClientRect\n"));
		((CWnd*)m_pDisplayWnd)->GetClientRect(m_rcLastClientRect);
	}

	m_rect.NormalizeRect();
	
	//Rechteck auf Bildbereich des Client Fensters beschränken 
	if(	   (m_rect.left - m_rcLastClientRect.left) < 0
	    || (m_rect.top - m_rcLastClientRect.top) < 0
		|| (m_rect.right - m_rcLastClientRect.right) > 0
		|| (m_rect.bottom - m_rcLastClientRect.bottom) > 0)
	{

		 //ist nicht gezoomt, Rechteck auf ClientWindow beschränken
		 if(m_pDisplayWnd->GetZoom().IsRectEmpty())
		 {
			m_rect = rectOld;
		 }
		 else
		 {
			 //ist gezoomt, Rechteck auf Image beschränken
			CRect rcPromilleClient, rcPromilleImage;
			CRect rcClient;
			((CWnd*)m_pDisplayWnd)->GetClientRect(rcClient);
			CRect rcZoom = m_pDisplayWnd->GetZoom();
			
			//Promille Daten bezüglich Client
			rcPromilleClient.left	= (m_rect.left		* 1000) / rcClient.Width();
			rcPromilleClient.top	= (m_rect.top		* 1000) / rcClient.Height();
			rcPromilleClient.right	= (m_rect.right		* 1000) / rcClient.Width();
			rcPromilleClient.bottom = (m_rect.bottom	* 1000) / rcClient.Height();

			//Promille Daten bezüglich Image
			rcPromilleImage.left	= (((rcPromilleClient.left	 * rcZoom.Width())	/ 1000) + rcZoom.left)	*1000 / m_rcPicture.Width();
			rcPromilleImage.top		= (((rcPromilleClient.top	 * rcZoom.Height()) / 1000) + rcZoom.top)	*1000 / m_rcPicture.Height();
			rcPromilleImage.right	= (((rcPromilleClient.right	 * rcZoom.Width())	/ 1000) + rcZoom.left)	*1000 / m_rcPicture.Width();
			rcPromilleImage.bottom	= (((rcPromilleClient.bottom * rcZoom.Height()) / 1000) + rcZoom.top)	*1000 / m_rcPicture.Height();			

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
//Berechnet die absoluten Rechteck Koordinaten aus gegebenen, auf die Imagegröße
//bezogenen Promille Werten, neu. Berücksichtigt auch den Zoom Faktor
void CQueryRectTracker::RecalcNewRectPositionFromPromille(CRect rcZoom)
{
	CRect rcClient;
	
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

	((CWnd*)m_pDisplayWnd)->GetClientRect(rcClient);
	if(    !m_rcPromilleImage.IsRectNull()
		&& !m_rcPicture.IsRectNull())
	{
/*
		TRACE(_T("***** RecalcNewRectPositionFromPromille: Tracker: 0x%x\n\n"), this);
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

			dm_rectLeft		= (dDummyPromilleLeft	* (DOUBLE)rcClient.Width())  / 1000 + (DOUBLE)rcClient.left;
			dm_rectTop		= (dDummyPromilleTop	* (DOUBLE)rcClient.Height()) / 1000 + (DOUBLE)rcClient.top;
			dm_rectRight	= (dDummyPromilleRight	* (DOUBLE)rcClient.Width())  / 1000 + (DOUBLE)rcClient.left;
			dm_rectBottom	= (dDummyPromilleBottom	* (DOUBLE)rcClient.Height()) / 1000 + (DOUBLE)rcClient.top;

			m_rect.left		= (int)(dm_rectLeft		+ 0.5);
			m_rect.top		= (int)(dm_rectTop		+ 0.5);
			m_rect.right	= (int)(dm_rectRight	+ 0.5);
			m_rect.bottom	= (int)(dm_rectBottom	+ 0.5);
		}
		else
		{
//			TRACE(_T("**** RecalcNewRect ohne Zoom \n"));
			m_rect.left		= (m_rcPromilleImage.left	* rcClient.Width())  / 1000 + rcClient.left;
			m_rect.top		= (m_rcPromilleImage.top		* rcClient.Height()) / 1000 + rcClient.top;
			m_rect.right	= (m_rcPromilleImage.right	* rcClient.Width())  / 1000 + rcClient.left;
			m_rect.bottom	= (m_rcPromilleImage.bottom	* rcClient.Height()) / 1000 + rcClient.top;
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
//			TRACE(_T("**Rect too small\n"));
			SetRectPromille(rcZoom);
//			TRACE(_T("**** RecalcNewRect m_rect mini  left: %i  top: %i  right: %i  bottom: %i\n"),
//				m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//Berechnet die Promille Werte (bezogen auf das ClientWindow) aus gegebenen 
//absoluten Rechteckwerten (auf dem ClientWindow) neu
//Ist gezoomt, werden ebenfalls die neuen Promille Werte bezogen 
//auf das Image berechnte
void CQueryRectTracker::SetRectPromille(CRect rcZoom)
{
	if(!m_rcPicture.IsRectNull() && !m_rcLastClientRect.IsRectEmpty())
	{
//		TRACE(_T("***** SetRectPromille: Tracker: 0x%x\n"), this);
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
			TRACE(_T("** SetRectPromille: m_rcLastClientRect   left: %i  top: %i  right: %i  bottom: %i\n"),
				m_rcLastClientRect.left, m_rcLastClientRect.top, m_rcLastClientRect.right, m_rcLastClientRect.bottom);
*/
			m_rcPromilleImage.left	= m_rect.left * 1000 / m_rcLastClientRect.Width();
			m_rcPromilleImage.top	= m_rect.top * 1000 / m_rcLastClientRect.Height();
			m_rcPromilleImage.right	= m_rect.right * 1000 / m_rcLastClientRect.Width();
			m_rcPromilleImage.bottom = m_rect.bottom * 1000 / m_rcLastClientRect.Height();

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
			CRect rcClient;
			((CWnd*)m_pDisplayWnd)->GetClientRect(rcClient);

			if (!rcClient.IsRectEmpty())
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
				
				//Promille Daten bezüglich Client
				dPromilleClientLeft		= ((DOUBLE)m_rect.left		* 1000) / (DOUBLE)rcClient.Width();
				dPromilleClientTop		= ((DOUBLE)m_rect.top		* 1000) / (DOUBLE)rcClient.Height();
				dPromilleClientRight	= ((DOUBLE)m_rect.right		* 1000) / (DOUBLE)rcClient.Width();
				dPromilleClientBottom	= ((DOUBLE)m_rect.bottom	* 1000) / (DOUBLE)rcClient.Height();

				//Promille Daten bezüglich Image
				drcPromilleLeft		= (((dPromilleClientLeft	* (DOUBLE)rcZoom.Width())  / 1000) + (DOUBLE)rcZoom.left)	*1000 / (DOUBLE)ImageSize.cx;
				drcPromilleTop		= (((dPromilleClientTop		* (DOUBLE)rcZoom.Height()) / 1000) + (DOUBLE)rcZoom.top)	*1000 / (DOUBLE)ImageSize.cy;
				drcPromilleRight	= (((dPromilleClientRight	* (DOUBLE)rcZoom.Width())  / 1000) + (DOUBLE)rcZoom.left)	*1000 / (DOUBLE)ImageSize.cx;
				drcPromilleBottom	= (((dPromilleClientBottom	* (DOUBLE)rcZoom.Height()) / 1000) + (DOUBLE)rcZoom.top)	*1000 / (DOUBLE)ImageSize.cy;			

				m_rcPromilleImage.left	= (int)(drcPromilleLeft + 0.5);
				m_rcPromilleImage.top	= (int)(drcPromilleTop + 0.5);
				m_rcPromilleImage.right	= (int)(drcPromilleRight + 0.5);
				m_rcPromilleImage.bottom = (int)(drcPromilleBottom + 0.5);

//				TRACE(_T("** SetRectPromille: Prom nach   left: %i  top: %i  right: %i  bottom: %i\n"),
//					m_rcPromilleImage.left, m_rcPromilleImage.top, m_rcPromilleImage.right, m_rcPromilleImage.bottom);
			}
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

	CRect rcZoom(0,0,0,0);
	if(m_pDisplayWnd)
	{
		rcZoom = m_pDisplayWnd->GetZoom();

		//ist gezoomt die minimale Größe des Rechtecks erhöhen
		if(!rcZoom.IsRectEmpty())
		{
			CSize pic = m_pDisplayWnd->GetPictureSize();
			CRect rcPicture(0,0,pic.cx, pic.cy);
			iZoom = rcPicture.Width()/m_pDisplayWnd->GetZoom().Width();
		}		
	}

	DOUBLE dCurrentMinWidthRect = (1 + ((DOUBLE)m_rcLastClientRect.Width() / (DOUBLE)m_iMacroBlockWidth)) * iZoom;
	DOUBLE dCurrentMinHeigthRect =(1 + ((DOUBLE)m_rcLastClientRect.Height() / (DOUBLE)m_iMacroBlockHeight)) * iZoom;
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
			SetRectPromille(rcZoom);
		}	
	}

	return bRet;
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
	CRect rc;
	m_pDisplayWnd->GetClientRect(rc);
	SaveLastClientRect(rc);
	RecalcNewRectPositionFromPromille(m_pDisplayWnd->GetZoom());
	Draw(m_pDisplayWnd->GetDC());
}




