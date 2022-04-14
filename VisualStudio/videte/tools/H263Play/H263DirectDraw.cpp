// H263Decoder.cpp: implementation of the CH263Decoder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "H263Play.h"
#include "H263Decoder.h"
#include "H263DirectDraw.h"

#include "DDError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CString GetDDError(HRESULT ddrval);

#define INIT_DIRECTDRAW_STRUCT(x) (ZeroMemory(&x, sizeof(x)), x.dwSize=sizeof(x))
// These are the pixel formats this app supports.  Most display adapters
// with overlay support will recognize one or more of these formats.
// The first on our list is the 16-bit RGB formats.  These have the widest
// support.
DDPIXELFORMAT g_ddpfOverlayFormats[] = 
{   
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','U','Y','2'),0,0,0,0,0}, // YUY2
    {sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('U','Y','V','Y'),0,0,0,0,0}, // UYVY
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0x7C00, 0x03e0, 0x001F, 0},  // 16-bit RGB 5:5:5
    {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0xF800, 0x07e0, 0x001F, 0},  // 16-bit RGB 5:6:5
 };  

#define NUM_OVERLAY_FORMATS (sizeof(g_ddpfOverlayFormats) / sizeof(g_ddpfOverlayFormats[0]))
//////////////////////////////////////////////////////////////////////
// CH263DirectDraw Class
int CH263DirectDraw::m_iInstances = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CH263DirectDraw::CH263DirectDraw()
{
	m_iInstances++;

	m_lpdd = NULL;
	m_lpddsPrimary = NULL;
	m_lpddsOverlay = NULL;
	m_lpClipper	   = NULL;
	m_lpddsBlitter = NULL;

	m_hWnd		= NULL;
	m_bFit		= FALSE;
	m_bOverlay	= TRUE;
//	m_bOverlay	= FALSE;
	m_iZoom		= 1;
}
//////////////////////////////////////////////////////////////////////
CH263DirectDraw::~CH263DirectDraw()
{
	m_iInstances--;
}
//////////////////////////////////////////////////////////////////////
BOOL CH263DirectDraw::GetFit()
{
	return m_bFit;
}
//////////////////////////////////////////////////////////////////////
void CH263DirectDraw::SetFit(BOOL bFit)
{
	m_bFit = bFit;
}
//////////////////////////////////////////////////////////////////////
int	CH263DirectDraw::GetZoom()
{
	return m_iZoom;
}
//////////////////////////////////////////////////////////////////////
void CH263DirectDraw::SetZoom(int iZoom)
{
	if ((iZoom==1) || (iZoom==2))
	{
		m_iZoom = iZoom;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CH263DirectDraw::Init(HWND hWnd)
{
	m_hWnd = hWnd;

    HRESULT         ddrval;

	ddrval = DirectDrawCreate( NULL, &m_lpdd, NULL );
	if(ddrval!=DD_OK)
	{
		TRACE("DirectDrawCreate %s\n",GetDDError(ddrval));
    	return FALSE;
	}

	ddrval = m_lpdd->SetCooperativeLevel(this->m_hWnd, DDSCL_NORMAL );
	if(ddrval!=DD_OK)
	{
		TRACE("SetCooperativeLevel %s\n",GetDDError(ddrval));
		return FALSE;
	}

	ddrval= CreatePrimarySurface();
	if(ddrval!=DD_OK)
	{
		TRACE("CreatePrimarySurface %s\n",GetDDError(ddrval));
		return FALSE;
	}

    ddrval = m_lpdd->CreateClipper( 0, &m_lpClipper, NULL );
    if( ddrval != DD_OK )
    {
		TRACE("CreateClipper %s\n",GetDDError(ddrval));
		return FALSE;
    }
    
    ddrval = m_lpClipper->SetHWnd( 0, m_hWnd );
    if( ddrval != DD_OK )
    {
		TRACE("SetHWnd %s\n",GetDDError(ddrval));
		return FALSE;
    }

    ddrval = m_lpddsPrimary->SetClipper( m_lpClipper );
    if( ddrval != DD_OK )
    {
		TRACE("SetClipper %s\n",GetDDError(ddrval));
		return FALSE;
    }

	if (m_bOverlay)
	{
		if (!CreateOverlay())
		{
			return FALSE;
		}
	}
	else
	{
		if (!CreateBlitter())
		{
			return FALSE;
		}
	}

	TRACE("DirectDrawInit SUCCESS\n");

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
HRESULT CH263DirectDraw::CreatePrimarySurface()
{
	DDSURFACEDESC   ddsd;
    HRESULT         ddrval;

    if (!m_lpdd) return E_FAIL;

    INIT_DIRECTDRAW_STRUCT(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    ddrval = m_lpdd->CreateSurface(&ddsd, &m_lpddsPrimary, NULL );
    return ddrval;
}
//////////////////////////////////////////////////////////////////////
BOOL CH263DirectDraw::CreateOverlay()
{
    DDSURFACEDESC   ddsdOverlay;
    HRESULT         ddrval;
    int             i;

    if (!m_lpdd || !m_lpddsPrimary)
	{
        return FALSE;
	}
    
    // It's currently not possible to query for pixel formats supported by the
    // overlay hardware (though GetFourCCCodes() usually provides a partial 
    // list).  Instead you need to call CreateSurface() to try a variety of  
    // formats till one works.  
    INIT_DIRECTDRAW_STRUCT(ddsdOverlay);
	ddsdOverlay.ddsCaps.dwCaps=DDSCAPS_OVERLAY | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;
    ddsdOverlay.dwFlags= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_BACKBUFFERCOUNT| DDSD_PIXELFORMAT;
	ddsdOverlay.dwWidth=352;
	ddsdOverlay.dwHeight=288;
    ddsdOverlay.dwBackBufferCount=2;

    // Try to create an overlay surface using one of the pixel formats in our
    // global list.
    i=0;
    do 
    {
		ddsdOverlay.ddpfPixelFormat=g_ddpfOverlayFormats[i];
		// Try to create the overlay surface
		ddrval = m_lpdd->CreateSurface(&ddsdOverlay, &m_lpddsOverlay, NULL);
    } while( FAILED(ddrval) && (++i < NUM_OVERLAY_FORMATS) );
	
    // If we failed to create an overlay surface, let's try again with a single
    // (non-flippable) buffer.
    if(FAILED(ddrval))
    {
        // Just in case we're short on video memory or the hardware doesn't like flippable
        // overlay surfaces, let's make another pass using a single buffer.
        ddsdOverlay.dwBackBufferCount=0;
        ddsdOverlay.ddsCaps.dwCaps=DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY;
        ddsdOverlay.dwFlags= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
        // Try to create the overlay surface
        ddrval = m_lpdd->CreateSurface(&ddsdOverlay, &m_lpddsOverlay, NULL);
        i=0;
        do 
		{
            ddsdOverlay.ddpfPixelFormat=g_ddpfOverlayFormats[i];
            ddrval = m_lpdd->CreateSurface(&ddsdOverlay, &m_lpddsOverlay, NULL);
        } while( FAILED(ddrval) && (++i < NUM_OVERLAY_FORMATS) );
		
        // We just couldn't create an overlay surface.  Let's exit.
        if (FAILED(ddrval))
		{
			TRACE(" %s\n",GetDDError(ddrval));
            return FALSE;
		}
    }
	
    return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CH263DirectDraw::DestroyOverlay()
{
	if (m_lpddsOverlay)
	{
        // Use UpdateOverlay() with the DDOVER_HIDE flag to remove an overlay 
        // from the display.
		TRACE("destroying Overlay\n");
        m_lpddsOverlay->UpdateOverlay(NULL, m_lpddsPrimary, NULL, DDOVER_HIDE, NULL);
		m_lpddsOverlay->Release();
		m_lpddsOverlay=NULL;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CH263DirectDraw::CreateBlitter()
{
    DDSURFACEDESC   ddsdBlitter;
    HRESULT         ddrval;
    int             i;

    if (!m_lpdd || !m_lpddsPrimary)
	{
        return FALSE;
	}
    
    // It's currently not possible to query for pixel formats supported by the
    // overlay hardware (though GetFourCCCodes() usually provides a partial 
    // list).  Instead you need to call CreateSurface() to try a variety of  
    // formats till one works.  
    INIT_DIRECTDRAW_STRUCT(ddsdBlitter);
	ddsdBlitter.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;// | DDSCAPS_BACKBUFFER | DDSCAPS_VIDEOMEMORY;
    ddsdBlitter.dwFlags = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_BACKBUFFERCOUNT;//| DDSD_PIXELFORMAT;
	ddsdBlitter.dwWidth = 352;
	ddsdBlitter.dwHeight = 288;
    ddsdBlitter.dwBackBufferCount = 0;
//    ddsdBlitter.dwBackBufferCount = 2;

    // Try to create an overlay surface using one of the pixel formats in our
    // global list.
    i=0;
    do 
    {
		ddsdBlitter.ddpfPixelFormat=g_ddpfOverlayFormats[i];
		// Try to create the overlay surface
		ddrval = m_lpdd->CreateSurface(&ddsdBlitter, &m_lpddsBlitter, NULL);
    } while( FAILED(ddrval) && (++i < NUM_OVERLAY_FORMATS) );
	
    // If we failed to create an overlay surface, let's try again with a single
    // (non-flippable) buffer.
    if(FAILED(ddrval))
    {
		TRACE("CreateBlitter failed %s\n",GetDDError(ddrval));

        ddsdBlitter.dwBackBufferCount = 0;
        ddsdBlitter.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        ddsdBlitter.dwFlags= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH;
        // Try to create the blitter surface
        ddrval = m_lpdd->CreateSurface(&ddsdBlitter, &m_lpddsBlitter, NULL);
        i=0;
/*        do 
		{
            ddsdBlitter.ddpfPixelFormat=g_ddpfOverlayFormats[i];
            ddrval = m_lpdd->CreateSurface(&ddsdBlitter, &m_lpddsBlitter, NULL);
        } 
		while( FAILED(ddrval) && (++i < NUM_OVERLAY_FORMATS) );*/

		TRACE("CreateBlitter failed %s\n",GetDDError(ddrval));
        return FALSE;
    }
	
    return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CH263DirectDraw::DestroyBlitter()
{
	if (m_lpddsBlitter)
	{
		TRACE("destroying Blitter\n");
		m_lpddsBlitter->Release();
		m_lpddsBlitter=NULL;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CH263DirectDraw::Exit()
{
	if (m_iInstances==1)
	{
		if (m_lpdd != NULL)
		{
			DestroyOverlay();
			DestroyBlitter();
			if (m_lpddsPrimary != NULL)
			{
				m_lpddsPrimary->Release();
				TRACE("Release Primary DD\n");
				m_lpddsPrimary = NULL;
			}
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CH263DirectDraw::DrawFrameYUV(unsigned char *src0,
								  unsigned char *src1,
								  unsigned char *src2,
								  int width,
								  int height
								  )
{
    HRESULT             ddrval;
    DDSURFACEDESC       ddsd;

	INIT_DIRECTDRAW_STRUCT(ddsd);

	if (m_bOverlay)
	{
		ddrval = m_lpddsOverlay->Lock( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL);
	    if(ddrval!=DD_OK)
		{
			TRACE("failed to lock Overlay Surface %s\n",GetDDError(ddrval));
			return FALSE;
		}
	}
	else
	{
		ddrval = m_lpddsBlitter->Lock( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL);
	    if(ddrval!=DD_OK)
		{
			TRACE("failed to lock Blitter Surface %s\n",GetDDError(ddrval));
			return FALSE;
		}
	}

	switch (ddsd.ddpfPixelFormat.dwFourCC)
	{
		case MAKEFOURCC('Y','U','Y','2'): 
			CopyYUY2ToSurface(src0, src1, src2, 
							 (DWORD*)ddsd.lpSurface, 
							 width, height,
							 ddsd.lPitch);
			break;
		case MAKEFOURCC('U','Y','V','Y'): 
			CopyUYVYToSurface(src0, src1, src2, 
							  (DWORD*)ddsd.lpSurface, 
							 width, height,
							 ddsd.lPitch);
			break;
	}

	if (m_bOverlay)
	{
		m_lpddsOverlay->Unlock(NULL);     
		DisplayOverlay(width,height);
	}
	else
	{
		m_lpddsBlitter->Unlock(NULL);     
		DisplayBlitter(width,height);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CH263DirectDraw::CopyYUY2ToSurface(BYTE* srcY, BYTE *srcU, BYTE *srcV,
										 DWORD* destS,
										 int width,
										 int height,
										 DWORD dwPitch)
{
	BYTE *py0,*py1,*pu,*pv;
	int i,j;

	DWORD u1,v1,u2,v2;
	DWORD y01,y02,y03,y04;
	DWORD y11,y12,y13,y14;
	DWORD *id1, *id2;
    DWORD dwNextLine;   

	py0 = srcY; 
	py1 = py0 + width;
	pu  = srcU; 
	pv  = srcV;

	dwNextLine = (dwPitch>>1) - (width>>1);
	id1 = destS;

	for (j = 0; j < height; j+=2) 
	{ 
		id2 = id1 + (dwPitch>>2);

		for (i = 0; i < width; i += 4) 
		{
			u1 = *pu++;
			u2 = *pu++;

			v1 = *pv++;
			v2 = *pv++;

			y01 = *py0++;
			y02 = *py0++;
			y03 = *py0++;
			y04 = *py0++;

			y11 = *py1++;
			y12 = *py1++;
			y13 = *py1++;
			y14 = *py1++;
					
			//       	 V          Y1         U        Y0
			*id1++ = (v1<<24) | (y03<<16) | (u1<<8) | (y01);
			*id1++ = (v2<<24) | (y04<<16) | (u2<<8) | (y02);

			*id2++ = (v1<<24) | (y13<<16) | (u1<<8) | (y11);
			*id2++ = (v2<<24) | (y14<<16) | (u2<<8) | (y12);

		}
		py0 += width;
		py1 += width;
		id1 += dwNextLine;
	}
}
//////////////////////////////////////////////////////////////////////
void CH263DirectDraw::CopyUYVYToSurface(BYTE* srcY, BYTE *srcU, BYTE *srcV,
										 DWORD* destS,
										 int width,
										 int height,
										 DWORD dwPitch)
{
	BYTE *py0,*py1,*pu,*pv;
	int i,j;

	DWORD u1,v1,u2,v2;
	DWORD y01,y02,y03,y04;
	DWORD y11,y12,y13,y14;
	DWORD *id1, *id2;
    DWORD dwNextLine;   

	py0 = srcY; 
	py1 = py0 + width;
	pu  = srcU; 
	pv  = srcV;

	dwNextLine = (dwPitch>>1) - (width>>1);
	id1 = destS;

	for (j = 0; j < height; j+=2) 
	{ 
		id2 = id1 + (dwPitch>>2);

		for (i = 0; i < width; i += 4) 
		{
			u1 = *pu++;
			u2 = *pu++;

			v1 = *pv++;
			v2 = *pv++;

			y01 = *py0++;
			y02 = *py0++;
			y03 = *py0++;
			y04 = *py0++;

			y11 = *py1++;
			y12 = *py1++;
			y13 = *py1++;
			y14 = *py1++;

			//       	 Y1         V          Y0        U
			*id1++ = (y03 << 24) | (v1<<16) | (y01<<8) | (u1);
			*id1++ = (y04 << 24) | (v2<<16) | (y02<<8) | (u2);

			*id2++ = (y13 << 24) | (v1<<16) | (y11<<8) | (u1);
			*id2++ = (y14 << 24) | (v2<<16) | (y12<<8) | (u2);


		}
		py0 += width;
		py1 += width;
		id1 += dwNextLine;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CH263DirectDraw::DisplayOverlay(int width, int height)
{
    HRESULT         ddrval;
    RECT            rs, rd;
    DDOVERLAYFX     ovfx;
    DDCAPS          capsDrv;
    unsigned int    uStretchFactor1000;
    unsigned int    uDestSizeAlign, uSrcSizeAlign;
    DWORD           dwUpdateFlags;
	
	GetWindowRect(m_hWnd,&rd);

	if (!m_bFit)
	{
		rd.right  = rd.left + width*m_iZoom;
		rd.bottom = rd.top + height*m_iZoom;
	}

    if(!m_lpdd || !m_lpddsPrimary || !m_lpddsOverlay)
	{
        return FALSE;
	}

    // Get driver capabilities
    INIT_DIRECTDRAW_STRUCT(capsDrv);
    ddrval = m_lpdd->GetCaps(&capsDrv, NULL);
    if(ddrval!=DD_OK)
	{
		TRACE("GetCaps failed %s\n",GetDDError(ddrval));
        return FALSE;
	}

    // We need to check the minimum stretch.  Many display adpators require that
    // the overlay be stretched by a minimum amount.  The stretch factor will 
    // usually vary with the display mode (including changes in refresh rate).
    // The stretch factor is returned x1000.
    uStretchFactor1000 = capsDrv.dwMinOverlayStretch>1000 ? capsDrv.dwMinOverlayStretch : 1000;

    // Grab any alignment restrictions.  The DDCAPS struct contains a series of
    // alignment fields that are not clearly defined. They are intended for
    // overlay use.  It's important to observe alignment restrictions.
    // Many adapters with overlay capabilities require the overlay image be
    // located on 4 or even 8 byte boundaries, and have similar restrictions
    // on the overlay width (for both source and destination areas).
    uDestSizeAlign = capsDrv.dwAlignSizeDest;
    uSrcSizeAlign =  capsDrv.dwAlignSizeSrc;

    // Set the flags we'll send to UpdateOverlay
    dwUpdateFlags = DDOVER_SHOW | DDOVER_DDFX;

    // Does the overlay hardware support source color keying?
    // If so, we can hide the black background around the image.
    // This probably won't work with YUV formats
    if (capsDrv.dwCKeyCaps & DDCKEYCAPS_SRCOVERLAY)
        dwUpdateFlags |= DDOVER_KEYSRCOVERRIDE;

    // Create an overlay FX structure so we can specify a source color key.
    // This information is ignored if the DDOVER_SRCKEYOVERRIDE flag isn't set.
    INIT_DIRECTDRAW_STRUCT(ovfx);
    ovfx.dckSrcColorkey.dwColorSpaceLowValue=0; // Specify black as the color key
    ovfx.dckSrcColorkey.dwColorSpaceHighValue=0;

    // Set up our Source Rect. This is the area of the overlay surface we
    // want to display.  If you want to display your entire surface and
    // happen to know for certain that your surface width meets any alignment
    // restrictions you can go ahead and pass NULL for the source rect in your
    // calls to UpdateOverlay().  Our surface width of 320 probably will meet
    // every alignment restriction, but just in case we'll create a source rect
    // and check for it.
    rs.left=0; rs.top=0; // position 0,0 is already position (boundary aligned)
    rs.right = width;
    rs.bottom = height;
    //Apply any size alignment restrictions if necessary.
    if (capsDrv.dwCaps & DDCAPS_ALIGNSIZESRC && uSrcSizeAlign)
        rs.right -= rs.right % uSrcSizeAlign;

    // Set up our destination rect, indicating where we want the overlay to 
    // appear on the primary surface.  This is where we have to take into 
    // account any stretch factor which may be needed to ensure the overlay is
    // displayed.  Really only the destination width must be stretched for the
    // overlay hardware to work, but we stretch the height as well just to
    // maintain a proper aspect ratio.


    // Note: We use the source rect dimensions, not the surface dimensions in
    // case they differ.
    // UpdateOverlay will fail unless the minimum stretch value is observed.
    
//    rd.right  = (rs.right*uStretchFactor1000+999)/1000; // adding 999 takes care of integer truncation problems.
//    rd.bottom = rs.bottom*uStretchFactor1000/1000;

    // It's also important to observe any alignment restrictions on size and
    // position with respect to the destination rect. Tweak the destination 
    // width a bit more to get the size alignment correct (Be sure to round up
    // to keep any minimum stretch restrictions met). we'll assume the 
    // position 0,0 is already "position aligned".
//    if (capsDrv.dwCaps & DDCAPS_ALIGNSIZEDEST && uDestSizeAlign)
//        rd.right = (int)((rd.right+uDestSizeAlign-1)/uDestSizeAlign)*uDestSizeAlign;

    // Make the call to UpdateOverlay() which actually displays the overlay on
    // the screen.
    ddrval = m_lpddsOverlay->UpdateOverlay(&rs, m_lpddsPrimary, &rd, dwUpdateFlags, &ovfx);

    if(ddrval!=DD_OK)
    {
        // Ok, the call to UpdateOVerlay() failed.  A likely cause is the
        // driver lied about the minimum stretch needed. 
        // Ideally we should try upping the destination size a bit, or
        // perhaps shrinking the source size so the destination stretch
        // is effectively higher.   For this sample, however, we'll just
        // bail!
		TRACE("UpdateOverlay %s\n",GetDDError(ddrval));
        return FALSE;
    }


    return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CH263DirectDraw::DisplayBlitter(int width, int height)
{
    HRESULT         ddrval;
    RECT            rs, rd;
    DDCAPS          capsDrv;
    unsigned int    uDestSizeAlign, uSrcSizeAlign;

	GetWindowRect(m_hWnd,&rd);

	rs.left = 0;
	rs.top = 0;
	rs.right = width;
	rs.bottom = height;

	if (!m_bFit)
	{
		rd.right  = rd.left + width*m_iZoom;
		rd.bottom = rd.top + height*m_iZoom;
	}


    if (!m_lpdd || !m_lpddsPrimary || !m_lpddsBlitter)
	{
        return FALSE;
	}
   
    // Get driver capabilities
    INIT_DIRECTDRAW_STRUCT(capsDrv);
    ddrval = m_lpdd->GetCaps(&capsDrv, NULL);
    if(ddrval!=DD_OK)
	{
		TRACE("GetCaps failed %s\n",GetDDError(ddrval));
        return FALSE;
	}

    uDestSizeAlign = capsDrv.dwAlignSizeDest;
    uSrcSizeAlign =  capsDrv.dwAlignSizeSrc;

    if (capsDrv.dwCaps & DDCAPS_ALIGNSIZESRC && uSrcSizeAlign)
	{
        rs.right -= rs.right % uSrcSizeAlign;
		TRACE("source aligned");
	}
    if (capsDrv.dwCaps & DDCAPS_ALIGNSIZEDEST && uDestSizeAlign)
	{
        rd.left -= rd.left % uDestSizeAlign;
        rd.top -= rd.top % uDestSizeAlign;
		TRACE("dest aligned");
	}



/*	TRACE("rd.left=%d ,rd.top=%d\n",rd.left,rd.top);
	TRACE("rs.left=%d ,rs.top=%d\n",rs.left,rs.top);
	TRACE("rs.right=%d ,rs.bottom=%d\n",rs.right,rs.bottom);*/

#if 0
    DWORD           dwTrans;
	dwTrans = DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY;

	ddrval = m_lpddsPrimary->BltFast(0,0,//rd.left,rd.top, 
								     m_lpddsBlitter, &rs, 
									 dwTrans);
    if(ddrval!=DD_OK)
    {
		TRACE("BltFast failed %s\n",GetDDError(ddrval));
        return FALSE;
    }

#else
	DDBLTFX			bltfx;
	DWORD			dwFlags;
	RECT			rt;

    INIT_DIRECTDRAW_STRUCT(bltfx);
	dwFlags = DDBLT_WAIT | DDBLT_DDFX;
	rt.left  = 0;
	rt.top   = 0;
	rt.right = width;
	rt.bottom = height;

	ddrval = m_lpddsPrimary->Blt(&rt, 
								 m_lpddsBlitter, 
								 &rd, 
								 dwFlags,
								 &bltfx);
    if(ddrval!=DD_OK)
    {
		TRACE("Blt failed %s\n",GetDDError(ddrval));
        return FALSE;
    }
#endif

	return TRUE;
}
