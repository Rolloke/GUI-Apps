/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (C) 1992 - 1996 Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/
/****************************************************************************
 *
 *  WRITEAVI.C
 *
 *  Creates the file OUTPUT.AVI, an AVI file consisting of a rotating clock
 *  face.  This program demonstrates using the functions in AVIFILE.DLL
 *  to make writing AVI files simple.
 *
 *  This is a stripped-down example; a real application would have a user
 *  interface and check for errors.
 *
 ***************************************************************************/
#include "stdafx.h"
#include <windowsx.h>
#include <memory.h>
#include <mmsystem.h>
#include <vfw.h>

#include "writeavi.h"

/////////////////////////////////////////////////////////////////////////////
static HANDLE  MakeDib( HBITMAP hbitmap, UINT bits )
{
	HANDLE              hdib ;
	HDC                 hdc ;
	BITMAP              bitmap ;
	UINT                wLineLen ;
	DWORD               dwSize ;
	DWORD               wColSize ;
	LPBITMAPINFOHEADER  lpbi ;
	LPBYTE              lpBits ;
	
	GetObject(hbitmap,sizeof(BITMAP),&bitmap) ;

	//
	// DWORD align the width of the DIB
	// Figure out the size of the colour table
	// Calculate the size of the DIB
	//
	wLineLen = (bitmap.bmWidth*bits+31)/32 * 4;
	wColSize = sizeof(RGBQUAD)*((bits <= 8) ? 1<<bits : 0);
	dwSize = sizeof(BITMAPINFOHEADER) + wColSize +
		(DWORD)(UINT)wLineLen*(DWORD)(UINT)bitmap.bmHeight;

	//
	// Allocate room for a DIB and set the LPBI fields
	//
	hdib = GlobalAlloc(GHND,dwSize);
	if (!hdib)
		return hdib ;

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib) ;

	lpbi->biSize = sizeof(BITMAPINFOHEADER) ;
	lpbi->biWidth = bitmap.bmWidth ;
	lpbi->biHeight = bitmap.bmHeight ;
	lpbi->biPlanes = 1 ;
	lpbi->biBitCount = (WORD) bits ;
	lpbi->biCompression = BI_RGB ;
	lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize ;
	lpbi->biXPelsPerMeter = 0 ;
	lpbi->biYPelsPerMeter = 0 ;
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
	lpbi->biClrImportant = 0 ;

	//
	// Get the bits from the bitmap and stuff them after the LPBI
	//
	lpBits = (LPBYTE)(lpbi+1)+wColSize ;

	hdc = CreateCompatibleDC(NULL) ;

	GetDIBits(hdc,hbitmap,0,bitmap.bmHeight,lpBits,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	// Fix this if GetDIBits messed it up....
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;

	DeleteDC(hdc) ;
	GlobalUnlock(hdib);

	return hdib ;
}
/////////////////////////////////////////////////////////////////////////////
CAVIFile::CAVIFile(LPCTSTR szFileName, int iFps, int iXDim/*=-1*/, int iYDim/*=-1*/)
{
	m_sFileName	= szFileName;
	m_iXDim		= iXDim;
	m_iYDim		= iYDim;
	m_bOK		= TRUE;
	m_iFrames	= 0;
	m_iFps		= iFps;

	m_pAviFile = NULL;
	m_pAviStream = NULL;
	m_pAviStreamCompressed = NULL;
	m_pAviStreamText = NULL;
	aopts[0] = &m_AviCompressOptions;
	WORD wVer = HIWORD(VideoForWindowsVersion());
	if (wVer < 0x010A)
	{
		// oops, we are too old, blow out of here
		m_bOK = false;
	}
	else
	{
		AVIFileInit();
	}
}
/////////////////////////////////////////////////////////////////////////////
CAVIFile::~CAVIFile()
{
	if (m_pAviStream)
		AVIStreamClose(m_pAviStream);

	if (m_pAviStreamCompressed)
		AVIStreamClose(m_pAviStreamCompressed);

	if (m_pAviStreamText)
		AVIStreamClose(m_pAviStreamText);

	if (m_pAviFile)
		AVIFileClose(m_pAviFile);

	WORD wVer = HIWORD(VideoForWindowsVersion());
	if (wVer >= 0x010A)
	{
		AVIFileExit();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAVIFile::IsOK() const
{
	return m_bOK;
}
/////////////////////////////////////////////////////////////////////////////
CString CAVIFile::GetFileName() const
{
	return m_sFileName;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAVIFile::AddFrame(CBitmap& bmp)
{
	HRESULT hr;

	if (!m_bOK)
		return false;
	LPBITMAPINFOHEADER alpbi = (LPBITMAPINFOHEADER)GlobalLock(MakeDib(bmp, 8));
	if (alpbi == NULL)
		return false;
	if (m_iXDim>=0 && m_iXDim != alpbi->biWidth)
	{
		GlobalFreePtr(alpbi);
		return false;
	}
	if (m_iYDim>=0 && m_iYDim != alpbi->biHeight)
	{
		GlobalFreePtr(alpbi);
		return false;
	}
	m_iXDim = alpbi->biWidth;
	m_iYDim = alpbi->biHeight;
	if (m_iFrames == 0)
	{
		hr = AVIFileOpen(&m_pAviFile,		    // returned file pointer
						 m_sFileName,			// file name
						 OF_WRITE | OF_CREATE,	// mode to open file with
						 NULL					// use handler determined from file extension....
						 );
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			m_bOK = false;
			return false;
		}
		_fmemset(&m_AviStreamInfo, 0, sizeof(m_AviStreamInfo));
		m_AviStreamInfo.fccType                = streamtypeVIDEO;// stream type
		m_AviStreamInfo.fccHandler             = 0;
		m_AviStreamInfo.dwScale                = 1;
		m_AviStreamInfo.dwRate                 = m_iFps;
		m_AviStreamInfo.dwSuggestedBufferSize  = alpbi->biSizeImage;
		SetRect(&m_AviStreamInfo.rcFrame, 0, 0,		    // rectangle for stream
			(int) alpbi->biWidth,
			(int) alpbi->biHeight);

		// And create the stream;
		hr = AVIFileCreateStream(m_pAviFile,		    // file pointer
						         &m_pAviStream,		    // returned stream pointer
								 &m_AviStreamInfo);	    // stream header
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			m_bOK = false;
			return false;
		}

		_fmemset(&m_AviCompressOptions, 0, sizeof(m_AviCompressOptions));

		if (!AVISaveOptions(NULL, 0, 1, &m_pAviStream, (LPAVICOMPRESSOPTIONS FAR *) &aopts))
		{
			GlobalFreePtr(alpbi);
			m_bOK = false;
			return false;
		}

		hr = AVIMakeCompressedStream(&m_pAviStreamCompressed,
									 m_pAviStream,
									 &m_AviCompressOptions,
									 NULL);
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			m_bOK = false;
			return false;
		}

		hr = AVIStreamSetFormat(m_pAviStreamCompressed,
								0,
								alpbi,					// stream format
								alpbi->biSize +			// format size
								alpbi->biClrUsed * sizeof(RGBQUAD)
								);
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			m_bOK = false;
			return false;
		}

		// Fill in the stream header for the text stream....

		// The text stream is in 60ths of a second....
/*
		_fmemset(&m_AviStreamInfo, 0, sizeof(m_AviStreamInfo));
		m_AviStreamInfo.fccType                = streamtypeTEXT;
		m_AviStreamInfo.fccHandler             = mmioFOURCC('D', 'R', 'A', 'W');
		m_AviStreamInfo.dwScale                = 1;
		m_AviStreamInfo.dwRate                 = 60;
		m_AviStreamInfo.dwSuggestedBufferSize  = sizeof(szText);
		SetRect(&m_AviStreamInfo.rcFrame, 0, (int) alpbi->biHeight,
			(int) alpbi->biWidth, (int) alpbi->biHeight + TEXT_HEIGHT);

		// ....and create the stream.
		hr = AVIFileCreateStream(m_pAviFile, &m_pAviStreamText, &m_AviStreamInfo);
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			m_bOK = false;
			return false;
		}

		dwTextFormat = sizeof(dwTextFormat);
		hr = AVIStreamSetFormat(m_pAviStreamText, 0, &dwTextFormat, sizeof(dwTextFormat));
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			m_bOK = false;
			return false;
		}
*/
	}

	// Jetzt eigentliches Schreiben
	hr = AVIStreamWrite(m_pAviStreamCompressed,	// stream pointer
						m_iFrames,				// time of this frame
						1,						// number to write
						(LPBYTE) alpbi +		// pointer to data
						alpbi->biSize +
						alpbi->biClrUsed * sizeof(RGBQUAD),
						alpbi->biSizeImage,		// size of this frame
						AVIIF_KEYFRAME,			// flags....
						NULL,
						NULL
						);
	if (hr != AVIERR_OK)
	{
		GlobalFreePtr(alpbi);
		m_bOK = false;
		return false;
	}

	// Make some text to put in the file ...
	//char szMessage[BUFSIZE];
	//LoadString(hInstance, IDS_TEXTFORMAT, szMessage, BUFSIZE );
	/*
	strcpy(szMessage, "This is frame #%d");
		
	int iLen = wsprintf(szText, szMessage, (int)(m_iFrames + 1));

	// ... and write it as well.
	hr = AVIStreamWrite(m_pAviStreamText,
			m_iFrames * 40,
			1,
			szText,
			iLen + 1,
			AVIIF_KEYFRAME,
			NULL,
			NULL);
	if (hr != AVIERR_OK)
	{
		GlobalFreePtr(alpbi);
		m_bOK = false;
		return false;
	}
	*/
	GlobalFreePtr(alpbi);

	m_iFrames++;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAVIFile::Compress()
{
	if (AVIERR_OK != AVIMakeCompressedStream(&m_pAviStreamCompressed,
											 m_pAviStream,
											 &m_AviCompressOptions,
											 NULL)
		)
	{
		return FALSE;
	}
	AVIFileRelease(m_pAviFile);
	return TRUE;

}