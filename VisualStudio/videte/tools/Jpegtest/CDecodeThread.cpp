// CDecodeThread.cpp: implementation of the CDecodeThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Jpegtest.h"
#include "CDecodeThread.h"
#include "ChildView.h"
#include "IntelPicture.h"
#include "MyH26xEncoder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CDecodeThread::CDecodeThread(CChildView* pView) 
	: CWK_Thread("DecodeThread", pView)
{
	CFont font;
	font.CreatePointFont(12*10, "Courier New");
	m_Jpeg.SetFont1(font);
	m_Jpeg.SetTextColor(RGB(0,0,0));
	m_Jpeg.SetBackgroundColor(RGB(249, 254, 188));
//	m_Jpeg.EnableLuminanceCorrection();
	m_Jpeg.EnableNoiseReduction();


	m_pPerfDecode = NULL;

//	m_pPerf1 = new CWK_PerfMon("Loading&Decoding");
//	m_pPerf2 = new CWK_PerfMon("Blitting");

	m_dwDecodedFrames = 0;
	m_bIsRunning = FALSE;
	m_bAutoDelete = TRUE;
	m_dwTC1 = 0;
	m_dwTC2 = 0;
	m_pH26xEncoder = NULL;
	m_pView = pView;

	CSystemTime t;
	t.GetLocalTime();
	m_sOutputFilename = t.GetDBDate() + t.GetDBTime() + ".263";
}

//////////////////////////////////////////////////////////////////////
CDecodeThread::~CDecodeThread()
{
	WK_DELETE(m_pPerfDecode);
}

//////////////////////////////////////////////////////////////////////
BOOL CDecodeThread::StartThread()
{
	m_dwDecodedFrames = 0;
	m_dwTC1 = GetTickCount();
	m_dwTC2 = m_dwTC1;

	return CWK_Thread::StartThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CDecodeThread::StopThread()
{
	return CWK_Thread::StopThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CDecodeThread::Run(LPVOID lpContext)
{
	CChildView* pView		= (CChildView*)lpContext;
	double	dFps			= 0.0;
	CRect rc(0,0,0,0);
	CIntelPicture ip;
//   static CMemoryState m1;


	pView->GetClientRect(rc);	    
	CWindowDC  dc(pView);

	m_sJPEGPath = pView->GetJPEGPath();
	m_sFileName	= GetNextFileName();

	m_Jpeg.SetOSDText1(0,0, m_sTitle);


	if (!m_sFileName.IsEmpty())
	{
		TRACE("DECODING %s\n",m_sFileName);

		if (m_pPerfDecode)
			m_pPerfDecode->Start();
		if (pView->AllowNoiseReduction())
			m_Jpeg.EnableNoiseReduction();
		else
			m_Jpeg.DisableNoiseReduction();

		if (pView->AllowLumCorrection())
			m_Jpeg.EnableLuminanceCorrection();
		else
			m_Jpeg.DisableLuminanceCorrection();

//		if (pView->H263())
		{
			if (pView->DecodeToScreen())
				m_Jpeg.SetProperties(768,576,IJL_BGR);
			else
				m_Jpeg.SetProperties(768,576,IJL_YCBCR);
		}

		CFile o;
		if (o.Open(m_sFileName,CFile::modeRead))
		{
			DWORD dwBufferLength = o.GetLength();
			PBYTE pBuffer = new BYTE[dwBufferLength];

			o.Read(pBuffer,dwBufferLength);

			m_Jpeg.DecodeJpegFromMemory(pBuffer,
										dwBufferLength,
										pView->AllowDoubleFieldMode(),
										pView->IsInterpolating() ? CJ_INTERPOLATE : CJ_NOTHING,
										!pView->DecodeToScreen());
			WK_DELETE_ARRAY(pBuffer);
		}

		if (m_pPerfDecode)
			m_pPerfDecode->Stop();

/*      
		if (pView->GetScale()!=0)
		{
			int iWidth = pView->GetWidth();
			int iHeight = pView->GetHeight();
			TRACE("Resizing image to %d,%d\n",iWidth,iHeight);
			ip.SetData(m_Jpeg.GetImageDims().cx,
					   m_Jpeg.GetImageDims().cy,
					   m_Jpeg.GetDibBuffer(),
					   m_Jpeg.GetColorSpace());
			ip.Resize(iWidth,iHeight);
		}
*/
		if (pView->DecodeToScreen())
		{
			pView->PostMessage(WM_USER,WPARAM_IMAGE_DECODED);
		}
		
		if (pView->DecodeToHDD())
		{
			CString sBmp = m_sFileName;
			sBmp.Replace(".jpg",".bmp");
			if (ip.IsEmpty())
			{
				CDib dib(m_Jpeg.CreateDIB());
				dib.Save(sBmp);
			}
			else
			{
				if (ip.GetColorSpace() == IJL_BGR)
				{
					BITMAPINFOHEADER bmi;
					ZeroMemory(&bmi,sizeof(bmi));
					bmi.biSize = sizeof(bmi);
					bmi.biHeight = ip.GetHeight();
					bmi.biWidth = ip.GetWidth();
					CDib dib(&bmi,ip.GetData());
					dib.Save(sBmp);
				}
			}
		}

	
		if (pView->H263())
		{
			if (!m_pH26xEncoder)
			{
//            m1.Checkpoint();
				m_pH26xEncoder = new CMyH26xEncoder(this);
				if (ip.IsEmpty())
            {
//					m_pH26xEncoder->Init(180,140,128000,pView->GetFPS());
					m_pH26xEncoder->Init(720,280,128000,pView->GetFPS(),TRUE);
				}
				else
				{
					m_pH26xEncoder->Init(ip.GetWidth(),ip.GetHeight(),32000,pView->GetFPS(),TRUE);
				}
//            m1.DumpAllObjectsSince();
			}

			if (ip.IsEmpty())
			{
				if (m_Jpeg.GetColorSpace() == IJL_YCBCR)
					m_pH26xEncoder->SetYUV422Data(SECID_NO_ID,m_Jpeg.GetImageDims().cx,m_Jpeg.GetImageDims().cy,m_Jpeg.GetDibBuffer());
				else if (m_Jpeg.GetColorSpace() == IJL_BGR)
					m_pH26xEncoder->SetRGBData(SECID_NO_ID,m_Jpeg.GetImageDims().cx,m_Jpeg.GetImageDims().cy,m_Jpeg.GetDibBuffer());
			}
			else
			{
				if (ip.GetColorSpace() == IJL_YCBCR)
					m_pH26xEncoder->SetYUV422Data(SECID_NO_ID,m_Jpeg.GetImageDims().cx,m_Jpeg.GetImageDims().cy,ip.GetData());
				else if(ip.GetColorSpace() == IJL_RGB)
					m_pH26xEncoder->SetBGRData(SECID_NO_ID,m_Jpeg.GetImageDims().cx,m_Jpeg.GetImageDims().cy,ip.GetData());
				else if(ip.GetColorSpace() == IJL_BGR)
					m_pH26xEncoder->SetRGBData(SECID_NO_ID,m_Jpeg.GetImageDims().cx,m_Jpeg.GetImageDims().cy,ip.GetData());
			}

		}

		DWORD dwTick = GetTickCount();
		int nDT  = GetTimeSpan(m_dwTC1,dwTick);
		if (nDT >= 1000)
		{
			dFps = 	(1000* m_dwDecodedFrames / nDT + 0.5);		
		}	
		m_sTitle.Format("Speed:%02.02f fps, Frame:%d", dFps, m_dwDecodedFrames);

		if (pView->GetFPS()>0)
		{
			DWORD dwOK = 1000/pView->GetFPS();
			DWORD dwUsed = GetTimeSpan(m_dwTC2,dwTick);
//			TRACE("used %d ms for frame, %d ms are OK %d\n",dwUsed,dwOK,m_dwTC2-m_dwTC1);
			if (dwOK>dwUsed)
			{
				DWORD dwSleep = dwOK-dwUsed;
				Sleep(dwSleep);
			}
			m_dwTC2 = GetTickCount();
		}

		return TRUE;
	}
	else
	{
//      CMemoryState m2, mr;

      TRACE("no more file to decode stopping decode thread\n");
//      m1.DumpAllObjectsSince();
		WK_DELETE(m_pH26xEncoder);
/*
      m2.Checkpoint();
      mr.Difference(m1, m2);
      mr.DumpStatistics();
*/
		AfxGetMainWnd()->PostMessage(WM_USER);
		MessageBeep(0);
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
CString CDecodeThread::GetNextFileName()
{
	CString s = "";
	
	s.Format("%sJ_%06lu.jpg", m_sJPEGPath, m_dwDecodedFrames);
	if (!DoesFileExist(s))
	{
		s.Format("%s%06lu.diff", m_sJPEGPath, m_dwDecodedFrames);
		if (!DoesFileExist(s))
			s = "";
	}
	m_dwDecodedFrames++;

	return s;
}

//////////////////////////////////////////////////////////////////////
void CDecodeThread::OnDraw( CDC* pDC, const CRect &rectDest, const CRect &rectSrc/*=CRect(0,0,0,0)*/)
{
	m_Jpeg.OnDraw(pDC, rectDest, rectSrc);
}

//////////////////////////////////////////////////////////////////////
void CDecodeThread::SetSelectionRect(const CRect &rcSelection)
{
	m_Jpeg.SetSelectionRect(rcSelection);
}
//////////////////////////////////////////////////////////////////////
void CDecodeThread::OnEncodedFrame()
{
	CString sH263 = m_sFileName.Left(m_sFileName.ReverseFind('\\'));
	sH263 += "\\" + m_sOutputFilename;

	if (m_dwDecodedFrames == 1)
	{
		TRACE("deleting %s\n",sH263);
		DeleteFile(sH263);
	}
	CFile f;
	if (f.Open(sH263,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
	{
		TRACE("H.263 image is %d bytes\n",m_pH26xEncoder->GetH263BufferLength());
		f.SeekToEnd();
		f.Write(m_pH26xEncoder->GetH263Buffer(),m_pH26xEncoder->GetH263BufferLength());
	}
	m_pView->PostMessage(WM_USER,WPARAM_IMAGE_DECODED);
}
