// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "colorbutton.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
/////////////////////////////////////////////////////////////////////////////
// CColorButton
/////////////////////////////////////////////////////////////////////////////
CColorButton::CColorButton()
{
	m_bChangeOnlyBrightness = FALSE;
	m_dMaxColorChangeRange	= 64;
}
/////////////////////////////////////////////////////////////////////////////
void CColorButton::DrawRectangleButton(const enumODBS& eODBS, CRect rect, CDC* pDC)
{
	// draw the whole button
	if (m_bDebugTrace)
	{
		TRACE("CColorButton DrawRectangleButton %04u %04x state %i\n",
					GetDlgCtrlID(), GetDlgCtrlID(), eODBS);
	}

	// Aussenrahmen zeichnen
	CBrush brush;
	brush.CreateSolidBrush(GetSysColor(COLOR_WINDOWFRAME));
	pDC->FrameRect(rect,&brush);
	brush.DeleteObject();

	rect.DeflateRect(1,1);

	// Wie soll der Button aussehen?
	// Grundfarbe bestimmen und 3D-Rahmen zeichnen
	COLORREF colBase;
	switch (eODBS)
	{
		case ODBS_DISABLED:
			colBase = ChangeBrightness(m_BaseColor, -32);
			pDC->Draw3dRect(rect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_3DSHADOW));
			break;
		case ODBS_SELECTED:
			colBase = ChangeBrightness(m_BaseColor, 32);
			pDC->Draw3dRect(rect,GetSysColor(COLOR_3DSHADOW),GetSysColor(COLOR_3DHILIGHT));
			break;
		case ODBS_FOCUSED:
			colBase = ChangeBrightness(m_BaseColor, 32);
			pDC->Draw3dRect(rect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_3DSHADOW));
			break;
		case ODBS_NORMAL:
		default:
			colBase = m_BaseColor;
			pDC->Draw3dRect(rect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_3DSHADOW));
			break;
	}

	rect.DeflateRect(1,1);

	if (eODBS == ODBS_SELECTED)
	{
//		rect.OffsetRect(1,1);
//		col = RGB(GetRValue(colBase)-r*rect.Height(),
//				  GetGValue(colBase)-g*rect.Height(),
//				  GetBValue(colBase)-b*rect.Height());
//		r = -r;
//		b = -b;
//		g = -g;

	}							 

	// Button zeichnen
	DrawRectangle(eODBS, rect, colBase, pDC);

	// Buttonbeschriftung zeichnen
	DrawImageList(eODBS, rect, pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CColorButton::DrawRectangle(const enumODBS& eODBS, CRect rect, COLORREF colBase, CDC* pDC)
{
	switch (m_eButtonSurface)
	{
		case Surface3DPlaneEdge15:
			DrawRectangle3DPlaneEdge(eODBS, 15, rect, colBase, pDC);
			break;
		case Surface3DPlaneEdge10:
			DrawRectangle3DPlaneEdge(eODBS, 10, rect, colBase, pDC);
			break;
		case Surface3DPlaneEdge5:
			DrawRectangle3DPlaneEdge(eODBS, 5, rect, colBase, pDC);
			break;
		case SurfaceColorChangeBrushed:
			DrawRectangleColorChangeBrushed(rect, colBase, pDC);
			break;
		case SurfaceColorChangeSimple:
			DrawRectangleColorChangeSimple(rect, colBase, pDC);
			break;
		case SurfacePlaneColor:
		default:
			DrawRectanglePlaneColor(rect, colBase, pDC);
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CColorButton::DrawRectanglePlaneColor(CRect rect, COLORREF colBase, CDC* pDC)
{
	CBrush brush;
	brush.CreateSolidBrush(colBase);
	pDC->FillRect(rect,&brush);
	brush.DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
void CColorButton::DrawRectangleColorChangeSimple(CRect rect, COLORREF colBase, CDC* pDC)
{
	if (m_bChangeOnlyBrightness)
	{
		// Umrechnen in YCbCr-Farbraum
		double dY, dCb, dCr;
		RGB2YCrCb(colBase, dY, dCb, dCr);

		// Beim Farbverlauf sollte nur die Helligkeit Y wechseln
		// Bestimme die kleinste Spanne bei der Farbe, vergleiche mit vorgegebener Begrenzung
		double dYDiff = min(255-dY, dY);
		dYDiff = (dYDiff < m_dMaxColorChangeRange) ? dYDiff : m_dMaxColorChangeRange;
		if (m_bDebugTrace)
		{
			TRACE("dYDiff total %03.4f\n", dYDiff);
		}

		// Verteile die Helligkeitsspanne auf die Abstufungen
		double dSpan = rect.Height()/2;
		dYDiff /= dSpan;
		if (m_bDebugTrace)
		{
			TRACE("dYDiff step %03.4f\n", dYDiff);
		}

		// Fange an bei der hellsten Abstufung
		dY = dY + dSpan * dYDiff;
		CBrush brush;
		CRect rc = rect;
		COLORREF col;
		for (int i = 0;i<rect.Height();i++)
		{
			col = YCrCb2RGB(dY-i*dYDiff, dCb, dCr);
			if (m_bDebugTrace)
			{
				TRACE("CColorButton color change simple r=%03i g=%03i b=%03i\n",
							GetRValue(col), GetGValue(col), GetBValue(col));
			}
			rc.top = rect.top  + i;
			rc.bottom = rect.top + i + 1;
			brush.CreateSolidBrush(col);
			pDC->FillRect(rc,&brush);
			brush.DeleteObject();
		}
	}
	else
	{
		// Farbwerte fuer Farbverlauf bestimmen
		double dRBase = GetRValue(colBase);
		double dGBase = GetGValue(colBase);
		double dBBase = GetBValue(colBase);
		// Verteile die Abstufungen auf das (halbe!) Rechteck
		double dSpan = rect.Height()/2;
		double r,g,b;
		r = min(dRBase, 255-dRBase);
		r = (r < m_dMaxColorChangeRange) ? r : m_dMaxColorChangeRange;
		r /= dSpan;
		g = min(dGBase, 255-dGBase);
		g = (g < m_dMaxColorChangeRange) ? g : m_dMaxColorChangeRange;
		g /= dSpan;
		b = min(dBBase, 255-dBBase);
		b = (b < m_dMaxColorChangeRange) ? b : m_dMaxColorChangeRange;
		b /= dSpan;

		// Fange an bei der hellsten Abstufung an
		dRBase += r*dSpan;
		dGBase += g*dSpan;
		dBBase += b*dSpan;
		CBrush brush;
		CRect rc = rect;
		COLORREF col;
		double dR, dG, dB;
		for (int i = 0 ; i<rect.Height() ; i++)
		{
			dR = dRBase - r*i;
			dG = dGBase - g*i;
			dB = dBBase - b*i;
			col = RGB((int)dR, (int)dG, (int)dB);
			if (m_bDebugTrace)
			{
				TRACE("CColorButton color change simple r=%03f g=%03f b=%03f\n", dR, dG, dB);
			}
			rc.top = rect.top  + i;
			rc.bottom = rect.top + i + 1;
			brush.CreateSolidBrush(col);
			pDC->FillRect(rc,&brush);
			brush.DeleteObject();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CColorButton::DrawRectangleColorChangeBrushed(CRect rect, COLORREF colBase, CDC* pDC)
{
	if (m_bChangeOnlyBrightness)
	{
		// Umrechnen in YCbCr-Farbraum
		double dY, dCb, dCr;
		RGB2YCrCb(colBase, dY, dCb, dCr);

		// Beim Farbverlauf sollte nur die Helligkeit Y wechseln
		// Bestimme die kleinste Spanne bei der Farbe, vergleiche mit vorgegebener Begrenzung
		double dYDiff = min(255-dY, dY);
		dYDiff = (dYDiff < m_dMaxColorChangeRange) ? dYDiff : m_dMaxColorChangeRange;
		if (m_bDebugTrace)
		{
			TRACE("dYDiff total %03.4f\n", dYDiff);
		}

		// Verteile die Helligkeitsspanne auf die Abstufungen
		double dSpan = rect.Height()/2;
		dYDiff /= dSpan;
		if (m_bDebugTrace)
		{
			TRACE("dYDiff step %03.4f\n", dYDiff);
		}

		// Fange an bei der hellsten Abstufung
		dY = dY + dSpan * dYDiff;
		CBrush brush;
		CRect rc = rect;
		COLORREF col;
		int s = 1;
		BOOL bBright = TRUE;
		for (int i=0;i<rect.Height();i+=s)
		{
			if (bBright)
			{
				col = YCrCb2RGB(dY-i*dYDiff, dCb, dCr);
				if (m_bDebugTrace)
				{
					TRACE("CColorButton color change simple r=%03i g=%03i b=%03i\n",
								GetRValue(col), GetGValue(col), GetBValue(col));
				}
				bBright = FALSE;
			}
			else
			{
				col = YCrCb2RGB(dY-(i+5)*dYDiff, dCb, dCr);
				if (m_bDebugTrace)
				{
					TRACE("CColorButton color change simple r=%03i g=%03i b=%03i\n",
								GetRValue(col), GetGValue(col), GetBValue(col));
				}
				bBright = TRUE;
			}
			rc.top = rect.top  + i;
			rc.bottom = rect.top + i + s;
			brush.CreateSolidBrush(col);
			pDC->FillRect(rc,&brush);
			brush.DeleteObject();
		}
	}
	else
	{
		// Farbwerte fuer Farbverlauf bestimmen
		double dRBase = GetRValue(colBase);
		double dGBase = GetGValue(colBase);
		double dBBase = GetBValue(colBase);
		double dSpan = rect.Height()/2;
		double r,g,b;
		r = min(dRBase, 255-dRBase);
		r = (r < m_dMaxColorChangeRange) ? r : m_dMaxColorChangeRange;
		r /= dSpan;
		g = min(dGBase, 255-dGBase);
		g = (g < m_dMaxColorChangeRange) ? g : m_dMaxColorChangeRange;
		g /= dSpan;
		b = min(dBBase, 255-dBBase);
		b = (b < m_dMaxColorChangeRange) ? b : m_dMaxColorChangeRange;
		b /= dSpan;

		// Fange an bei der hellsten Abstufung an
		dRBase += r*dSpan;
		dGBase += g*dSpan;
		dBBase += b*dSpan;

		CBrush brush;
		CRect rc = rect;
		COLORREF col;
		double dR, dG, dB;
		int s = 1;
		BOOL bBright = TRUE;
		for (int i=0;i<rect.Height();i+=s)
		{
			if (bBright)
			{
				dR = dRBase - r*i;
				dG = dGBase - g*i;
				dB = dBBase - b*i;
				col = RGB(dR, dG, dB);
				bBright = FALSE;
			}
			else
			{
				dR = dRBase - r*(i+5);
				dG = dGBase - r*(i+5);
				dB = dBBase - r*(i+5);
				col = RGB(dR, dG, dB);
				bBright = TRUE;
			}
			rc.top = rect.top  + i;
			rc.bottom = rect.top + i + s;
			brush.CreateSolidBrush(col);
			pDC->FillRect(rc,&brush);
			brush.DeleteObject();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CColorButton::DrawRectangle3DPlaneEdge(const enumODBS& eODBS, int iEdge, CRect rect, COLORREF colBase, CDC* pDC)
{
	if (m_bChangeOnlyBrightness)
	{
		CBrush brush;
		// Bei gedrückten Buttons den Button um 1 Pixel nach unten rechts verschieben
		if (eODBS == ODBS_SELECTED)
		{
			// Fuer 3d-Effekt einen weiteren dunklen Rahmen zeichnen
			brush.CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
			pDC->FrameRect(rect,&brush);
			brush.DeleteObject();
			// Den Button um 1 Pixel nach unten rechts verschieben (nur oben und links kuerzen)
			rect.left++;  
			rect.top++;
		}

		// Farbwerte fuer Farbverlauf bestimmen
		// Umrechnen in YCbCr-Farbraum
		double dY, dCb, dCr;
		RGB2YCrCb(colBase, dY, dCb, dCr);

		// Beim Farbverlauf sollte nur die Helligkeit Y wechseln
		// Bestimme die kleinste Spanne
		double dYDiff = min(255-dY, dY);
		dYDiff = (dYDiff < m_dMaxColorChangeRange) ? dYDiff : m_dMaxColorChangeRange;
		if (m_bDebugTrace)
		{
			TRACE("dYDiff total %03.4f\n", dYDiff);
		}

		// Verteile die Helligkeitsspanne auf die Abstufungen
		dYDiff /= iEdge;
		if (m_bDebugTrace)
		{
			TRACE("dYDiff step %03.4f\n", dYDiff);
		}

		// Rechteck-Flanken zeichnen
		CRect rc = rect;
		COLORREF col;
		int j= 0;
		for (int i=0 ; i<iEdge ; i++)
		{
			// Helle Kanten oben und links
			col = YCrCb2RGB(dY+(iEdge-i)*dYDiff, dCb, dCr);
			rc.top = rect.top  + i;
			rc.left = rect.left  + i;
			rc.bottom = rect.bottom - i;
			rc.right = rect.right - i;
			brush.CreateSolidBrush(col);
			pDC->FillRect(rc,&brush);
			brush.DeleteObject();

			j = i;
			if (eODBS == ODBS_SELECTED)
			{
				j--;
			}
			if (j >= 0)
			{
				// Dunkle Kanten unten und rechts
				col = YCrCb2RGB(dY-(iEdge-i)*dYDiff, dCb, dCr);
				brush.CreateSolidBrush(col);

				// Dunkle Kante unten
				rc.top = rect.bottom - j - 1;
				rc.left = rect.left  + j + 1;
				rc.bottom = rect.bottom - j;
				rc.right = rect.right - j;
				pDC->FillRect(rc,&brush);

				// Dunkle Kante rechts
				rc.top = rect.top + j + 1;
				rc.left = rect.right - j - 1;
				rc.bottom = rect.bottom - j;
				rc.right = rect.right - j;
				pDC->FillRect(rc,&brush);
			}
			brush.DeleteObject();
		}
		// Plane Flaeche
		rc.top = rect.top  + i;
		rc.left = rect.left  + i;
		rc.bottom = rect.bottom - j;
		rc.right = rect.right - j;
		col = colBase;
		brush.CreateSolidBrush(col);
		pDC->FillRect(rc,&brush);
		brush.DeleteObject();
	}
	else
	{
		CBrush brush;
		// Bei gedrückten Buttons den Button um 1 Pixel nach unten rechts verschieben
		if (eODBS == ODBS_SELECTED)
		{
			// Fuer 3d-Effekt einen weiteren dunklen Rahmen zeichnen
			brush.CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
			pDC->FrameRect(rect,&brush);
			brush.DeleteObject();
			// Den Button um 1 Pixel nach unten rechts verschieben (nur oben und links kuerzen)
			rect.left++;  
			rect.top++;
		}

		// Farbwerte fuer Farbverlauf bestimmen
		double dR = GetRValue(colBase);
		double dG = GetGValue(colBase);
		double dB = GetBValue(colBase);
		double r,g,b;
		r = min(dR, 255-dR);
		r = (r < m_dMaxColorChangeRange) ? r : m_dMaxColorChangeRange;
		r /= iEdge;
		g = min(dG, 255-dG);
		g = (g < m_dMaxColorChangeRange) ? g : m_dMaxColorChangeRange;
		g /= iEdge;
		b = min(dB, 255-dB);
		b = (b < m_dMaxColorChangeRange) ? b : m_dMaxColorChangeRange;
		b /= iEdge;

		// Rechteck-Flanken zeichnen
		CRect rc = rect;
		COLORREF col;
		int j= 0;
		for (int i=0 ; i<iEdge ; i++)
		{
			// Helle Kanten oben und links
			col = RGB(dR+r*(iEdge-i), dG+g*(iEdge-i), dB+b*(iEdge-i));

			rc.top = rect.top  + i;
			rc.left = rect.left  + i;
			rc.bottom = rect.bottom - i;
			rc.right = rect.right - i;
			brush.CreateSolidBrush(col);
			pDC->FillRect(rc,&brush);
			brush.DeleteObject();

			j = i;
			if (eODBS == ODBS_SELECTED)
			{
				j--;
			}
			if (j >= 0)
			{
				// Dunkle Kanten unten und rechts
				col = RGB(dR-r*(iEdge-j), dG-g*(iEdge-j), dB-b*(iEdge-j));
				brush.CreateSolidBrush(col);

				// Dunkle Kante unten
				rc.top = rect.bottom - j - 1;
				rc.left = rect.left  + j + 1;
				rc.bottom = rect.bottom - j;
				rc.right = rect.right - j;
				pDC->FillRect(rc,&brush);

				// Dunkle Kante rechts
				rc.top = rect.top + j + 1;
				rc.left = rect.right - j - 1;
				rc.bottom = rect.bottom - j;
				rc.right = rect.right - j;
				pDC->FillRect(rc,&brush);
			}

			brush.DeleteObject();
		}
		rc.top = rect.top  + i;
		rc.left = rect.left  + i;
		rc.bottom = rect.bottom - j;
		rc.right = rect.right - j;
		col = colBase;
		brush.CreateSolidBrush(col);
		pDC->FillRect(rc,&brush);
		brush.DeleteObject();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CColorButton::DrawRoundButton(const enumODBS& eODBS, CRect rect, CDC* pDC)
{
//	if (m_bChangeOnlyBrightness)
	{
	}
//	else
	{
		// Offscreensurface anlegen
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		
		// Kompatible Bitmap mit dem Offscreensurface verbinden
		CBitmap Bitmap;
		Bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		CBitmap* pOldBitmap = memDC.SelectObject(&Bitmap);

		// Button Hintergrund in den MemoryDC blitten
		memDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
		
		int nW	= rect.Width();
		int nH	= rect.Height();
		int nDx		= nW / 5;
		int nDy		= nH / 5;

		// Wie soll der Button aussehen?
		// Grundfarbe bestimmen
		COLORREF colBase;
		switch (eODBS)
		{
			case ODBS_DISABLED:
				colBase = RGB(min(GetRValue(m_BaseColor)*2, 255),
							  min(GetGValue(m_BaseColor)*2, 255),
							  min(GetBValue(m_BaseColor)*2, 255));
				nDx		= nW / 20;
				nDy		= nH / 20;
				break;
			case ODBS_SELECTED:
				colBase = ChangeBrightness(m_BaseColor, 32);
//				colBase = RGB(min(GetRValue(m_BaseColor)+127,255),
//							  min(GetGValue(m_BaseColor)+127,255),
//							  min(GetBValue(m_BaseColor)+127,255));
//				break;
			case ODBS_FOCUSED:
				colBase = ChangeBrightness(m_BaseColor, 32);
//				colBase = RGB(min(GetRValue(m_BaseColor)+127,255),
//							  min(GetGValue(m_BaseColor)+127,255),
//							  min(GetBValue(m_BaseColor)+127,255));
//				break;
			case ODBS_NORMAL:
			default:
				colBase = m_BaseColor;
				break;
		}

		// Button zeichnen
		// Farbwerte fuer Farbverlauf bestimmen
		double a = nW/2;
		double b = nH/2;
		double r = 0;
		double r2 = 0;
		double r2Max = (double)((nW/2+nDx)*(nW/2+nDx))/(a*a) + (double)((nH/2+nDy)*(nH/2+nDy))/(b*b);
		double dRBase = GetRValue(colBase);
		double dGBase = GetGValue(colBase);
		double dBBase = GetBValue(colBase);
		double dR, dG, dB;
		COLORREF col;
		int nXX = 0;
		int nYY = 0;
		for (int nY = 0; nY < nH; nY++)
		{
			for (int nX = 0; nX < nW; nX++)
			{
				nXX = nX - nW/2;
				nYY = nY - nH/2;

				r = (double)(nXX*nXX)/(a*a) + (double)(nYY*nYY)/(b*b);
				r2 = (double)((nXX+nDx)*(nXX+nDx))/(a*a) + (double)((nYY+nDy)*(nYY+nDy))/(b*b);
				dR = dRBase*(1-r2/r2Max);
				dG = dGBase*(1-r2/r2Max);
				dB = dBBase*(1-r2/r2Max);
				col = RGB(dR, dG, dB);
				if (r2 > r2Max)
					TRACE("r2=%f r2Max=%f\n", r2, r2Max);

				if (r <= 1.0)
					memDC.SetPixel(nX, nY, col);
			}
		}

		rect.DeflateRect(1,1);

		// Schatten um die Ellipse zeichnen
		CPen pen(PS_SOLID, 2, RGB(40, 40, 40));
		memDC.SelectObject(pen);
		memDC.Arc(rect, CPoint(0,0),CPoint(0,0));

		if (eODBS == ODBS_SELECTED)
		{
			CPen pen(PS_SOLID, 2, RGB(80, 80, 80));
			memDC.SelectObject(pen);
			CRect rect2(rect.left+1, rect.top+1, rect.right, rect.bottom);
			memDC.Arc(rect2, CPoint(0,0),CPoint(0,0));
		}

		rect.InflateRect(1,1);

		// Und das Offscreensurface zeichnen
		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
		
		memDC.SelectObject(pOldBitmap);
		Bitmap.DeleteObject();
		memDC.DeleteDC();

		// Buttonbeschriftung zeichnen
		DrawImageList(eODBS, rect, pDC);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CColorButton::RGB2YCrCb(COLORREF col, double& dY, double& dCb, double& dCr)
{
	double dR = GetRValue(col);
	double dG = GetGValue(col);
	double dB = GetBValue(col);
	dY	=  0.2990*dR + 0.5870*dG + 0.1140*dB;
	dCb	= -0.1687*dR - 0.3313*dG + 0.5000*dB;
	dCr	=  0.5000*dR - 0.4187*dG - 0.0813*dB;
//	TRACE("dY %03.4f   dCb %03.4f   dCr %03.4f\n", dY, dCb, dCr);
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CColorButton::YCrCb2RGB(const double& dY, const double& dCb, const double& dCr)
{
//	TRACE("\ndY %03.4f   dCb %03.4f   dCr %03.4f\n", dY, dCb, dCr);
	double dR = (dY +             + 1.40200*dCr);
	double dG = (dY - 0.34414*dCb - 0.71414*dCr);
	double dB = (dY + 1.72700*dCb              );
	// CAVEAT, RGB jeweils im Bereich 0-255
	dR = max(0, min(255,dR));
	dG = max(0, min(255,dG));
	dB = max(0, min(255,dB));
//	TRACE("R %03.0f   G %03.0f   B %03.0f\n", dR, dG, dB);
	COLORREF col = RGB((int)dR, (int)dG, (int)dB);
	return col;
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CColorButton::ChangeBrightness(COLORREF col, double dYDiff)
{
	double dR = GetRValue(col);
	double dG = GetGValue(col);
	double dB = GetBValue(col);
	if (m_bDebugTrace)
	{
		TRACE("\nChangeBrightness before R %03.0f   G %03.0f   B %03.0f   dY %03.0f\n",
														dR, dG, dB, dYDiff);
	}
	COLORREF colReturn;
	if (m_bChangeOnlyBrightness)
	{
		double dY	=  0.2990*dR + 0.5870*dG + 0.1140*dB;
		double dCb	= -0.1687*dR - 0.3313*dG + 0.5000*dB;
		double dCr	=  0.5000*dR - 0.4187*dG - 0.0813*dB;
		if (m_bDebugTrace)
		{
			TRACE("dY %03.4f   dCb %03.4f   dCr %03.4f   dYDiff %03.4f\n", dY, dCb, dCr, dYDiff);
		}

		// Soll der Wert relativ zur zur Verfuegung stehender Spanne geaendert werden (dYDiff < 1)
		if ((int)dYDiff == 0)
		{
			// Bestimme die Spanne
			double dYDiffSpan = (dYDiff < 0) ? dY : 255-dY;
			dY = dY + dYDiffSpan * dYDiff;
			if (m_bDebugTrace)
			{
				TRACE("dYDiffSpan step %03.4f\n", dYDiffSpan);
			}
		}
		else
		{
			dY = dY + dYDiff;
		}

		dR = (dY +             + 1.40200*dCr);
		dG = (dY - 0.34414*dCb - 0.71414*dCr);
		dB = (dY + 1.72700*dCb              );
		// CAVEAT, RGB jeweils im Bereich 0-255
		dR = max(0, min(255,dR));
		dG = max(0, min(255,dG));
		dB = max(0, min(255,dB));
		if (m_bDebugTrace)
		{
			TRACE("R %03.0f   G %03.0f   B %03.0f\n", dR, dG, dB);
		}
		colReturn = RGB(dR, dG, dB);
	}
	else
	{
		dR += dYDiff;
		dG += dYDiff;
		dB += dYDiff;
		if (dYDiff > 0)
		{
			colReturn = RGB(min(dR, 255),
							min(dG, 255),
							min(dB, 255));
		}
		else
		{
			colReturn = RGB(max(dR, 0),
							max(dG, 0),
							max(dB, 0));
		}
	}
	if (m_bDebugTrace)
	{
		TRACE("ChangeBrightness after R %03.0f   G %03.0f   B %03.0f\n", dR, dG, dB);
	}
	return colReturn;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CColorButton, CSkinButton)
	//{{AFX_MSG_MAP(CColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers
/////////////////////////////////////////////////////////////////////////////
*/