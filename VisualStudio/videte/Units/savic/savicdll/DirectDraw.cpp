/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: directdraw.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/directdraw.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirectDraw.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CDirectDraw::CDirectDraw(int nHSize, int nVSize)
{
	m_lpDD					= NULL;
	m_lpDDSPrimary			= NULL;
	m_lpDDSOverlay			= NULL;
	m_lpDDSClipper			= NULL;

	m_pPrimarySurface		= NULL;
	m_pOverlaySurface		= NULL;

	m_dwStretchFactor1000	= 0;
	m_dwDestSizeAlign		= 0;
	m_dwSrcSizeAlign		= 0;
	m_nHSize				= nHSize;
	m_nVSize				= nVSize;
}

//////////////////////////////////////////////////////////////////////
CDirectDraw::~CDirectDraw()
{
	UnlockPrimarySurface();
	UnlockOverlaySurface();

	if (m_lpDDSClipper != NULL)
	{
		m_lpDDSPrimary->SetClipper(NULL);
		m_lpDDSClipper->Release();
		m_lpDDSClipper = NULL;
	}

	if (m_lpDDSOverlay != NULL)
	{
		m_lpDDSOverlay->Release();
		m_lpDDSOverlay = NULL;
	}

	if (m_lpDDSPrimary != NULL)
	{
		m_lpDDSPrimary->Release();
		m_lpDDSPrimary = NULL;
	}

	if (m_lpDD != NULL)
	{
		m_lpDD->Release();
		m_lpDD = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectDraw::Create()
{
	BOOL	bRet = FALSE;
	HRESULT	ddrval;

	ddrval = DirectDrawCreate(NULL, &m_lpDD, NULL);
	if (CheckValue(ddrval) == DD_OK)
	{
		ddrval = m_lpDD->SetCooperativeLevel(NULL, DDSCL_NORMAL);
		if (CheckValue(ddrval) == DD_OK)
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectDraw::CreatePrimarySurface()
{
	BOOL	bRet = FALSE;
	HRESULT	ddrval;
    DDSURFACEDESC ddsd;
	
	if (m_lpDDSPrimary)
		return TRUE;

	if (m_lpDD == NULL)
	{
		if (!Create())
			return FALSE;
	}

	if (m_lpDD)
	{
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));

		// Create the primary surface
        ddsd.dwSize = sizeof( ddsd );
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;

        ddrval = m_lpDD->CreateSurface( &ddsd, &m_lpDDSPrimary, NULL);
        if (CheckValue(ddrval) == DD_OK)
		{
			bRet = TRUE;
		}
	}

 	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectDraw::CreateClipper(HWND hWnd)
{
	BOOL	bRet = FALSE;
	HRESULT	ddrval;
	
	if (m_lpDDSClipper)
		return TRUE;

	if (m_lpDD == NULL)
	{
		if (!Create())
			return FALSE;
	}

	if (m_lpDD && m_lpDDSPrimary)
	{
		ddrval = m_lpDD->CreateClipper(0, &m_lpDDSClipper, NULL);
        if (CheckValue(ddrval) == DD_OK)
		{
			ddrval = m_lpDDSClipper->SetHWnd(0, hWnd);
			if (CheckValue(ddrval) == DD_OK)
			{
				ddrval = m_lpDDSPrimary->SetClipper(m_lpDDSClipper);
				if (CheckValue(ddrval) == DD_OK)
				{
					bRet = TRUE;
				}
			}
		}
	}

 	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectDraw::CreateOverlaySurface(DWORD dwColorKey)
{
	HRESULT	ddrval;
    DDCAPS	ddcaps;
    DDSURFACEDESC ddsd;
	BOOL	bHasOverlay			= FALSE;
	BOOL	bHasColorKey		= FALSE;
	BOOL	bCanStretch			= FALSE;
	BOOL	bCanOverlayFourCC	= FALSE;
	BOOL	bRet				= FALSE;

	if (m_lpDDSOverlay)
		return TRUE;

	m_dwColorKey = dwColorKey;

	if (m_lpDD == NULL)
	{
		if (!Create())
			return FALSE;
	}
	
	if (m_lpDDSPrimary == NULL)
	{
		if (!CreatePrimarySurface())
			return FALSE;
	}

	if (m_lpDD && m_lpDDSPrimary)
	{
		ZeroMemory(&ddcaps, sizeof(DDCAPS));
		ddcaps.dwSize = sizeof(ddcaps);
		ddrval = m_lpDD->GetCaps(&ddcaps, NULL);
		if(CheckValue(ddrval) == DD_OK )
		{
			// Determine if the hardware supports overlay surfaces
			bHasOverlay  = ((ddcaps.dwCaps & DDCAPS_OVERLAY) == DDCAPS_OVERLAY) ? TRUE : FALSE;     /* Determine if the hardware supports colorkeying */
			bHasColorKey = ((ddcaps.dwCaps & DDCAPS_COLORKEY) == DDCAPS_COLORKEY) ? TRUE : FALSE;
			
			// Determine if the hardware supports scaling of the overlay surface
			bCanStretch = ((ddcaps.dwCaps & DDCAPS_OVERLAYSTRETCH) == DDCAPS_OVERLAYSTRETCH) ? TRUE : FALSE;
			
			// Is overlay hardware capable of color space conversions during the overlay operation.
			bCanOverlayFourCC = ((ddcaps.dwCaps & DDCAPS_OVERLAYFOURCC) == DDCAPS_OVERLAYFOURCC) ? TRUE : FALSE;
			
			if (!bHasOverlay || !bHasColorKey || !bCanStretch || !bCanOverlayFourCC)
				return FALSE;

		    m_dwStretchFactor1000 = ddcaps.dwMinOverlayStretch>1000 ? ddcaps.dwMinOverlayStretch : 1000; 
		    m_dwDestSizeAlign	  = ddcaps.dwAlignSizeDest; 
			m_dwSrcSizeAlign	  = ddcaps.dwAlignSizeSrc; 

			ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
			
			// Create an scaleable overlay surface
			ddsd.dwSize = sizeof(DDSURFACEDESC);
			ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
			ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY | DDSCAPS_LIVEVIDEO;
			ddsd.dwWidth  = GetSystemMetrics(SM_CXSCREEN);
			ddsd.dwHeight = GetSystemMetrics(SM_CYSCREEN);
			ddsd.ddpfPixelFormat.dwSize			= sizeof(DDPIXELFORMAT);
			ddsd.ddpfPixelFormat.dwFlags		= DDPF_FOURCC | DDPF_YUV;
			ddsd.ddpfPixelFormat.dwFourCC		= mmioFOURCC('Y','U','Y','2');
			ddsd.ddpfPixelFormat.dwYUVBitCount	= 16;
		   
			ddrval = m_lpDD->CreateSurface(&ddsd, &m_lpDDSOverlay, NULL);
			if (CheckValue(ddrval) == DD_OK)
			{
				bRet = TRUE;
			}
		}
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectDraw::UpdateOverlay(const CRect &rc)
{
	HRESULT	ddrval;
	RECT	rectOverlaySource;
	RECT	rectOverlayDest;
	DDOVERLAYFX ddofx;
	BOOL	bRet = FALSE;

	if (m_lpDDSPrimary && m_lpDDSOverlay)
	{
		ZeroMemory(&ddofx, sizeof(DDOVERLAYFX));
		
		// Set the overlay source rect
		rectOverlaySource.left	= 0;
		rectOverlaySource.top	= 0;

		// Bei Darstellungen die größer als das von der Kamera gelieferte
		// Format sind, wird die Quellgröße auf dieses Format beschränkt. Der
		// Grafikchip skaliert dann das Bild rauf.
		if (rc.Width() > m_nHSize)
			rectOverlaySource.right = m_nHSize;
		else
			rectOverlaySource.right = rc.right	- rc.left;

		if (rc.Height() > m_nVSize)
			rectOverlaySource.bottom = m_nVSize;
		else
			rectOverlaySource.bottom = rc.bottom	- rc.top;

rectOverlaySource = CRect(0,0,m_nHSize,m_nVSize);

		//Apply any size alignment restrictions if necessary. 
		if (m_dwSrcSizeAlign != 0)
			rectOverlaySource.right -= rectOverlaySource.right % m_dwSrcSizeAlign; 

		// Fill the entire display with the overlay
		rectOverlayDest = rc;

	    // UpdateOverlay will fail unless the minimum stretch value is observed. 
		rectOverlayDest.right  = (rectOverlayDest.right*m_dwStretchFactor1000+999)/1000; // adding 999 takes care of integer truncation problems. 
		rectOverlayDest.bottom = rectOverlayDest.bottom*m_dwStretchFactor1000/1000; 

		if (m_dwDestSizeAlign != 0)
	        rectOverlayDest.right = (int)((rectOverlayDest.right+m_dwDestSizeAlign-1)/m_dwDestSizeAlign)*m_dwDestSizeAlign; 
		
		ddofx.dwSize = sizeof(DDOVERLAYFX);
		 
		// These values are arbitrary, in this example set to black.
		// For your application, use a reserved color that will not
		// appear in other graphic elements. One technique that works
		// nicely is to use the upper left pixel of your primary surface
		// elements to specify the color key. Read in your offscreen
		// elements, lock them down, and read the first two bytes
		// (for 16-bpp). For this example, force black to be the colorkey.
		ddofx.dckDestColorkey.dwColorSpaceLowValue  = m_dwColorKey;
		ddofx.dckDestColorkey.dwColorSpaceHighValue = m_dwColorKey;

		// Position, set colorkey values, and show the front overlay buffer
		ddrval = m_lpDDSOverlay->UpdateOverlay(&rectOverlaySource,
										  m_lpDDSPrimary,
										  &rectOverlayDest,
										  DDOVER_SHOW | DDOVER_KEYDESTOVERRIDE | DDOVER_DDFX,
										  &ddofx);
		 
		 // At this point, the overlay will appear wherever the color key color
		 // is painted in the graphics plane. The application must paint the color
		 // key value into the primary surface using either DirectDraw colorfillBLT
		 // or GDI calls, or by loading a background into the primary surface that
		 // contains the color key value
		 if(CheckValue(ddrval) == DD_OK)
		 {
			 bRet =  TRUE;
		 }
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////
void* CDirectDraw::LockPrimarySurface(RECT *pRect)
{
	HRESULT	ddrval;
    DDSURFACEDESC ddsd;

	if (m_lpDDSPrimary && !m_pPrimarySurface)
	{
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddrval = m_lpDDSPrimary->Lock(pRect, &ddsd, DDLOCK_SURFACEMEMORYPTR, NULL);
		if (CheckValue(ddrval) == DD_OK)
		{
			m_pPrimarySurface = ddsd.lpSurface;
		}
	}

	return m_pPrimarySurface;
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectDraw::UnlockPrimarySurface()
{
	HRESULT	ddrval;
	BOOL	bRet = FALSE;

	if (m_lpDDSPrimary && m_pPrimarySurface)
	{
		ddrval = m_lpDDSPrimary->Unlock(m_pPrimarySurface);
		if (CheckValue(ddrval) == DD_OK)
		{
			m_pPrimarySurface = NULL;
			bRet = TRUE;
		}
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
void* CDirectDraw::LockOverlaySurface(RECT *pRect)
{
	HRESULT	ddrval;
    DDSURFACEDESC ddsd;

	if (m_lpDDSOverlay && !m_pOverlaySurface)
	{
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddrval = m_lpDDSOverlay->Lock(pRect, &ddsd, DDLOCK_SURFACEMEMORYPTR, NULL);
		if (CheckValue(ddrval) == DD_OK)
		{
			m_pOverlaySurface = ddsd.lpSurface;
		}
	}

	return m_pOverlaySurface;
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectDraw::UnlockOverlaySurface()
{
	HRESULT	ddrval;
	BOOL	bRet = FALSE;

	if (m_lpDDSOverlay && m_pOverlaySurface)
	{
		ddrval = m_lpDDSOverlay->Unlock(m_pOverlaySurface);
		if (CheckValue(ddrval) == DD_OK)
		{
			m_pOverlaySurface = NULL;
			bRet = TRUE;
		}
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
HRESULT CDirectDraw::CheckValue(HRESULT ddrval)
{
	CString sMsg = _T("");	
	
	switch (ddrval)
	{
		case DD_OK:
			sMsg = _T("");
			break;
		case DDERR_ALREADYINITIALIZED:
			sMsg = _T("This object is already initialized\n");
			break;		 
		case DDERR_CANNOTATTACHSURFACE:
			sMsg = _T("This surface can not be attached to the requested surface.\n");
			break; 
		case DDERR_CANNOTDETACHSURFACE:
			sMsg = _T("This surface can not be detached from the requested surface.\n");
			break; 
		case DDERR_CURRENTLYNOTAVAIL:
			sMsg = _T("Support is currently not available.\n");
			break; 
		case DDERR_EXCEPTION:
			sMsg = _T("An exception was encountered while performing the requested operation\n");
			break; 
		case DDERR_GENERIC:
			sMsg = _T("Generic failure.\n");
			break; 
		case DDERR_HEIGHTALIGN:
			sMsg = _T("Height of rectangle provided is not a multiple of reqd alignment\n");
			break; 
		case DDERR_INCOMPATIBLEPRIMARY:
			sMsg = _T("Unable to match primary surface creation request with existing primary surface.\n");
			break; 
		case DDERR_INVALIDCAPS:
			sMsg = _T("One or more of the caps bits passed to the callback are incorrect.\n");
			break; 
		case DDERR_INVALIDCLIPLIST:
			sMsg = _T("DirectDraw does not support provided Cliplist.\n");
			break; 
		case DDERR_INVALIDMODE:
			sMsg = _T("DirectDraw does not support the requested mode\n");
			break; 
		case DDERR_INVALIDOBJECT:
			sMsg = _T("DirectDraw received a pointer that was an invalid DIRECTDRAW object.\n");
			break; 
		case DDERR_INVALIDPARAMS:
			sMsg = _T("One or more of the parameters passed to the callback function are incorrect.\n");
			break; 
		case DDERR_INVALIDPIXELFORMAT:
			sMsg = _T("pixel format was invalid as specified\n");
			break; 
		case DDERR_INVALIDRECT:
			sMsg = _T("Rectangle provided was invalid.\n");
			break; 
		case DDERR_LOCKEDSURFACES:
			sMsg = _T("Operation could not be carried out because one or more surfaces are locked\n");
			break; 
		case DDERR_NO3D:
			sMsg = _T("There is no 3D present.\n");
			break; 
		case DDERR_NOALPHAHW:
			sMsg = _T("Operation could not be carried out because there is no alpha accleration hardware present or available.\n");
			break; 
		case DDERR_NOCLIPLIST:
			sMsg = _T("no clip list available\n");
			break; 
		case DDERR_NOCOLORCONVHW:
			sMsg = _T("Operation could not be carried out because there is no color conversion hardware present or available.\n");
			break; 
		case DDERR_NOCOOPERATIVELEVELSET:
			sMsg = _T("Create function called without DirectDraw object method SetCooperativeLevel being called.\n");
			break; 
		case DDERR_NOCOLORKEY:
			sMsg = _T("Surface doesn't currently have a color key\n");
			break;
		case DDERR_NOCOLORKEYHW:
			sMsg = _T("Operation could not be carried out because there is no hardware support of the dest color key.\n");
			break; 
		case DDERR_NODIRECTDRAWSUPPORT:
			sMsg = _T("No DirectDraw support possible with current display driver\n");
			break; 
		case DDERR_NOEXCLUSIVEMODE:
			sMsg = _T("Operation requires the application to have exclusive mode but the application does not have exclusive mode.\n");
			break; 
		case DDERR_NOFLIPHW:
			sMsg = _T("Flipping visible surfaces is not supported.\n");
			break; 
		case DDERR_NOGDI:
			sMsg = _T("There is no GDI present.\n");
			break; 
		case DDERR_NOMIRRORHW:
			sMsg = _T("Operation could not be carried out because there is no hardware present or available.\n");
			break; 
		case DDERR_NOTFOUND:
			sMsg = _T("Requested item was not found\n");
			break; 
		case DDERR_NOOVERLAYHW:
			sMsg = _T("Operation could not be carried out because there is no overlay hardware present or available.\n");
			break; 
		case DDERR_NORASTEROPHW:
			sMsg = _T("Operation could not be carried out because there is no appropriate raster op hardware present or available.\n");
			break; 
		case DDERR_NOROTATIONHW:
			sMsg = _T("Operation could not be carried out because there is no rotation hardware present or available.\n");
			break; 
		case DDERR_NOSTRETCHHW:
			sMsg = _T("Operation could not be carried out because there is no hardware support for stretching\n");
			break; 
		case DDERR_NOT4BITCOLOR:
			sMsg = _T("DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\n");
			break; 
		case DDERR_NOT4BITCOLORINDEX:
			sMsg = _T("DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\n");
			break; 
		case DDERR_NOT8BITCOLOR:
			sMsg = _T("DirectDraw Surface is not in 8 bit color mode and the requested operation requires 8 bit color.\n");
			break; 
		case DDERR_NOTEXTUREHW:
			sMsg = _T("Operation could not be carried out because there is no texture mapping hardware present or available.\n");
			break; 
		case DDERR_NOVSYNCHW:
			sMsg = _T("Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\n");
			break; 
		case DDERR_NOZBUFFERHW:
			sMsg = _T("Operation could not be carried out because there is no hardware support for zbuffer blting.\n");
			break; 
		case DDERR_NOZOVERLAYHW:
			sMsg = _T("Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\n");
			break; 
		case DDERR_OUTOFCAPS:
			sMsg = _T("The hardware needed for the requested operation has already been allocated.\n");
			break;
		case DDERR_OUTOFMEMORY:
			sMsg = _T("DirectDraw does not have enough memory to perform the operation.\n");
			break; 
		case DDERR_OUTOFVIDEOMEMORY:
			sMsg = _T("DirectDraw does not have enough memory to perform the operation.\n");
 			break;
		case DDERR_OVERLAYCANTCLIP:
			sMsg = _T("hardware does not support clipped overlays\n");
			break; 
		case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
			sMsg = _T("Can only have ony color key active at one time for overlays\n");
			break; 
		case DDERR_PALETTEBUSY:
			sMsg = _T("Access to this palette is being refused because the palette is already locked by another thread.\n");
			break; 
		case DDERR_COLORKEYNOTSET:
			sMsg = _T("No src color key specified for this operation.\n");
			break; 
		case DDERR_SURFACEALREADYATTACHED:
			sMsg = _T("This surface is already attached to the surface it is being attached to.\n");
			break; 
		case DDERR_SURFACEALREADYDEPENDENT:
			sMsg = _T("This surface is already a dependency of the surface it is being made a dependency of.\n");
			break; 
		case DDERR_SURFACEBUSY:
			sMsg = _T("Access to this surface is being refused because the surface is already locked by another thread.\n");
			break; 
		case DDERR_CANTLOCKSURFACE:
			sMsg  = _T("Access to this surface is being refused because no driver exists\n");
			sMsg +=	_T("which can supply a pointer to the surface.\n");
			sMsg += _T("This is most likely to happen when attempting to lock the primary\n");
			sMsg += _T("surface when no DCI provider is present.\n");
			sMsg +=	_T("Will also happen on attempts to lock an optimized surface.\n");
			break;
		case DDERR_SURFACEISOBSCURED:
			sMsg = _T("Access to Surface refused because Surface is obscured.\n");
			break;
		case DDERR_SURFACELOST:
			sMsg  = _T("Access to this surface is being refused because the surface is gone.\n");
			sMsg +=	_T("The DIRECTDRAWSURFACE object representing this surface should.\n");
			sMsg +=	_T("have Restore called on it.\n");
			break;
		case DDERR_SURFACENOTATTACHED:
			sMsg = _T("The requested surface is not attached.\n");
			break; 
		case DDERR_TOOBIGHEIGHT:
			sMsg = _T("Height requested by DirectDraw is too large.\n");
			break; 
		case DDERR_TOOBIGSIZE:
			sMsg = _T("Size requested by DirectDraw is too large -- The individual height and width are OK.\n");
			break; 
		case DDERR_TOOBIGWIDTH:
			sMsg = _T("Width requested by DirectDraw is too large.\n");
			break;
		case DDERR_UNSUPPORTED:
			sMsg = _T("Action not supported.\n");
			break;	
		case DDERR_UNSUPPORTEDFORMAT:
			sMsg = _T("FOURCC format requested is unsupported by DirectDraw\n");
			break;
		case DDERR_UNSUPPORTEDMASK:
			sMsg = _T("Bitmask in the pixel format requested is unsupported by DirectDraw\n");
			break;
		case DDERR_VERTICALBLANKINPROGRESS:
			sMsg = _T("vertical blank is in progress\n");
			break;
		case DDERR_WASSTILLDRAWING:
			sMsg = _T("Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\n");
			break;
		case DDERR_XALIGN:
			sMsg = _T("Rectangle provided was not horizontally aligned on reqd. boundary\n");
			break;
		case DDERR_INVALIDDIRECTDRAWGUID:
			sMsg = _T("The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\n");
			break;
		case DDERR_DIRECTDRAWALREADYCREATED:
			sMsg = _T("A DirectDraw object representing this driver has already been created for this process.\n");
			break;
		case DDERR_NODIRECTDRAWHW:
			sMsg = _T("A hardware only DirectDraw object creation was attempted but the driver did not support any hardware.\n");
			break;
		case DDERR_PRIMARYSURFACEALREADYEXISTS:
			sMsg = _T("this process already has created a primary surface\n");
			break;
		case DDERR_NOEMULATION:
			sMsg = _T("software emulation not available.\n");
			break;
		case DDERR_REGIONTOOSMALL:
			sMsg = _T("region passed to Clipper::GetClipList is too small.\n");
			break;
		case DDERR_CLIPPERISUSINGHWND:
			sMsg = _T("an attempt was made to set a clip list for a clipper objec that is already monitoring an hwnd.\n");
			break;
		case DDERR_NOCLIPPERATTACHED:
			sMsg = _T("No clipper object attached to surface object\n");
			break;
		case DDERR_NOHWND:
			sMsg = _T("Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\n");
			break;
		case DDERR_HWNDSUBCLASSED:
			sMsg = _T("HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\n");
			break;
		case DDERR_HWNDALREADYSET:
			sMsg = _T("The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\n");
			break;
		case DDERR_NOPALETTEATTACHED:
			sMsg = _T("No palette object attached to this surface.\n");
			break;
		case DDERR_NOPALETTEHW:
			sMsg = _T("No hardware support for 16 or 256 color palettes.\n");
			break;
		case DDERR_BLTFASTCANTCLIP:
			sMsg = _T("If a clipper object is attached to the source surface passed into a BltFast call.\n");
			break;
		case DDERR_NOBLTHW:
			sMsg = _T("No blter.\n");
			break;
		case DDERR_NODDROPSHW:
			sMsg = _T("No DirectDraw ROP hardware.\n");
			break;
		case DDERR_OVERLAYNOTVISIBLE:
			sMsg = _T("returned when GetOverlayPosition is called on a hidden overlay\n");
			break;
		case DDERR_NOOVERLAYDEST:
			sMsg = _T("returned when GetOverlayPosition is called on a overlay that UpdateOverlay has never been called on to establish a destionation.\n");
			break;
		case DDERR_INVALIDPOSITION:
			sMsg = _T("returned when the position of the overlay on the destionation is no longer legal for that destionation.\n");
			break;
		case DDERR_NOTAOVERLAYSURFACE:
			sMsg = _T("returned when an overlay member is called for a non-overlay surface\n");
			break;
		case DDERR_EXCLUSIVEMODEALREADYSET:
			sMsg = _T("An attempt was made to set the cooperative level when it was already set to exclusive.\n");
			break;
		case DDERR_NOTFLIPPABLE:
			sMsg = _T("An attempt has been made to flip a surface that is not flippable.\n");
			break;
		case DDERR_CANTDUPLICATE:
			sMsg = _T("Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\n");
			break;
		case DDERR_NOTLOCKED:
			sMsg = _T("Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\n");
			break;
		case DDERR_CANTCREATEDC:
			sMsg = _T("Windows can not create any more DCs\n");
			break;
		case DDERR_NODC:
			sMsg = _T("No DC was ever created for this surface.\n");
			break;
		case DDERR_WRONGMODE:
			sMsg = _T("This surface can not be restored because it was created in a different mode.\n");
			break;
		case DDERR_IMPLICITLYCREATED:
			sMsg = _T("This surface can not be restored because it is an implicitly created surface.\n");
			break;
		case DDERR_NOTPALETTIZED:
			sMsg = _T("The surface being used is not a palette-based surface\n");
			break;
		case DDERR_UNSUPPORTEDMODE:
			sMsg = _T("The display is currently in an unsupported mode\n");
			break;
		case DDERR_NOMIPMAPHW:
			sMsg = _T("Operation could not be carried out because there is no mip-map texture mapping hardware present or available.\n");
			break;
		case DDERR_INVALIDSURFACETYPE:
			sMsg = _T("The requested action could not be performed because the surface was of the wrong type.\n");
			break;
		case DDERR_NOOPTIMIZEHW:
			sMsg = _T("Device does not support optimized surfaces, therefore no video memory optimized surfaces\n");
			break;
		case DDERR_NOTLOADED:
			sMsg = _T("Surface is an optimized surface, but has not yet been allocated any memory\n");
			break;
		case DDERR_NOFOCUSWINDOW:
			sMsg = _T("Attempt was made to create or set a device window without first setting the focus window\n");
			break;
		case DDERR_DCALREADYCREATED:
			sMsg = _T("A DC has already been returned for this surface. Only one DC can be retrieved per surface.\n");
			break;
		case DDERR_NONONLOCALVIDMEM:
			sMsg = _T("An attempt was made to allocate non-local video memory from a device that does not support non-local video memory.\n");
			break;
		case DDERR_CANTPAGELOCK:
			sMsg = _T("The attempt to page lock a surface failed.\n");
			break;
		case DDERR_CANTPAGEUNLOCK:
			sMsg = _T("The attempt to page unlock a surface failed.\n");
			break;
		case DDERR_NOTPAGELOCKED:
			sMsg = _T("An attempt was made to page unlock a surface with no outstanding page locks.\n");
			break;
		case DDERR_MOREDATA:
			sMsg = _T("There is more data available than the specified buffer size could hold\n");
			break;
		case DDERR_VIDEONOTACTIVE:
			sMsg = _T("The video port is not active\n");
			break;
		case DDERR_DEVICEDOESNTOWNSURFACE:
			sMsg = _T("Surfaces created by one direct draw device cannot be used directly by another direct draw device.\n");
			break;
		default:
			sMsg = _T("Unbekannter DirectX-Fehler\n");
	}

	if (!sMsg.IsEmpty())
		ML_TRACE(_T("%s"), sMsg);

	return ddrval;
}
