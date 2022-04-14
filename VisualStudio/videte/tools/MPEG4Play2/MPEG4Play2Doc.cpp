// MPEG4Play2Doc.cpp : Implementierung der Klasse CMPEG4Play2Doc
//

#include "stdafx.h"
#include "MPEG4Play2.h"

#include "MPEG4Play2Doc.h"
#include "MPEG4Play2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMPEG4Play2Doc

IMPLEMENT_DYNCREATE(CMPEG4Play2Doc, CDocument)

BEGIN_MESSAGE_MAP(CMPEG4Play2Doc, CDocument)
END_MESSAGE_MAP()


// CMPEG4Play2Doc Erstellung/Zerstörung

CMPEG4Play2Doc::CMPEG4Play2Doc()
{
	m_pInpFile           = NULL;
	m_bThreadIsRunning   = FALSE;
	m_pThread            = NULL;
	m_pMPEG4Decoder      = NULL;
	m_dwMPEG4FrameLength = 0;
	m_pMPEG4Frame        = NULL;
	m_bIsIFrame          = TRUE;	// jedes File sollte mit einem I-Frame beginnen!
	m_streamEndPos       = 0;
	m_bOpenViaFileDialog = FALSE;
	m_bGoToEnd           = FALSE;

	// MPEG4-Decoder erstellen...
	m_pMPEG4Decoder = new CMPEG4Decoder;
	// ...und initialisieren
	m_pMPEG4Decoder->Init(NULL, 0, 0, 0);
}

CMPEG4Play2Doc::~CMPEG4Play2Doc()
{
	m_bThreadIsRunning = FALSE;

	if(m_pMPEG4Frame)
	{
		delete m_pMPEG4Frame;
		m_pMPEG4Frame = NULL;
	}
	
	if (m_pThread)
	{
		WaitForSingleObject(m_pThread->m_hThread,5000);
		delete m_pThread;

		m_pThread = NULL;
	}

	if(m_pMPEG4Decoder)
	{
		delete m_pMPEG4Decoder;
		m_pMPEG4Decoder = NULL;
	}
}

BOOL CMPEG4Play2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}




// CMPEG4Play2Doc Serialisierung

void CMPEG4Play2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: Hier Code zum Speichern einfügen
	}
	else
	{
		// TODO: Hier Code zum Laden einfügen
	}
}


// CMPEG4Play2Doc Diagnose

#ifdef _DEBUG
void CMPEG4Play2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMPEG4Play2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMPEG4Play2Doc-Befehle

/*------
*
* Name: GetVOPFromStream()
*
* Zweck: einen MPEG4-Frame aus dem komprimierten Datenstrom lesen
*        -> Datei-Variante
*
* Eingabeparameter: 
*
* Rueckgabewert: -
*
* Datum: 25.02.2004
*
* letzte Aenderung: 25.02.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CMPEG4Play2Doc::GetVOPFromStream(BYTE *pFrame, DWORD *dwFrameLength, FILE *fp)
{
	DWORD          rear;
	DWORD		   i;
	DWORD		   startIndex;
	DWORD		   B3Flag;

	rear       = 0;
	i          = 0;
	startIndex = 3;
	B3Flag     = 0;

	if ((*dwFrameLength = (DWORD)fread ((void *) pFrame, 1, COMPRESSED_FRAME_SIZE, fp)) <= 0)
	{
		// Frame konnte nicht aus dem Stream gelesen werden!
		*dwFrameLength = 0;
		return;
	}

	if(*(DWORD*)pFrame == 0xB0010000)
	{
		// 0 0 1 0xB0: ein neuer I-Frame beginnt

		for(i = 3; i < *dwFrameLength; i++)
		{
			if(*(DWORD*)&pFrame[i] == 0xB3010000)
			{
				B3Flag = 1;
			}

			if(*(DWORD*)&pFrame[i] == 0xB0010000)
			{
				// 0 0 1 0xB0: ein neuer I-Frame beginnt
				//rear = i;
				break;
			}

			if(*(DWORD*)&pFrame[i] == 0xB6010000)
			{
				if(B3Flag)
				{
					// suchen bis zum naechsten B6
					B3Flag = 0;
				}
				else
				{
					// 0 0 1 0xB0: ein neuer P-Frame beginnt
					//rear = i;
					break;
				}
			}
		}
		rear = i;
	}
	else
	{
		// 0 0 1 0xB0: ein neuer P-Frame beginnt
		if(*(DWORD*)pFrame == 0xB6010000)
		{
			for(i = 3; i < *dwFrameLength; i++)
			{
				if(    *(DWORD*)&pFrame[i] == 0xB0010000
					|| *(DWORD*)&pFrame[i] == 0xB6010000)
				{
					break;
				}
			}
			rear = i;
		}
	}

	fseek(fp, rear - *dwFrameLength, SEEK_CUR);

	*dwFrameLength = rear;

	return;
}



/*------
*
* Name: FindPrevVOP()
*
* Zweck: -
*
* Eingabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 25.02.2004
*
* letzte Aenderung: 25.02.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CMPEG4Play2Doc::FindPrevVOP()
{
	BOOL bFound;
	BOOL bIsPFrame;
	fpos_t pos;
	fpos_t posCur;
	fpos_t posCurFrame;
	BYTE cData[4];

	bIsPFrame = FALSE;

	// Da der FilePointer bereits auf dem naechsten Bild steht, das dekodiert
	// werden soll, muss dieser zunaechst auf den Anfang des aktuellen Bilds
	// gesetzt werden. Danach wird der vorherige Frame gesucht.

	for(int k=0;k<2;k++)
	{
		bFound = FALSE;

		fgetpos(m_pInpFile, &pos);

		// Ist der Anfang des Streams erreicht worden?
		if(pos == 0)
		{
			fseek(m_pInpFile, 0, SEEK_END);
			fgetpos(m_pInpFile, &pos);
		}

		// Wurde der FilePointer über GoToEnd() an das Ende des Streams gesetzt?
		// -> dann aeussere Schleife nur einmal durchlaufen!
		if(m_bGoToEnd)
		{
			m_bGoToEnd = FALSE;
			k++;
		}

		for(int i=4; !bFound; i++)
		{
			fseek(m_pInpFile, -i, SEEK_CUR);

			cData[0] = (BYTE)fgetc(m_pInpFile);
			cData[1] = (BYTE)fgetc(m_pInpFile);
			cData[2] = (BYTE)fgetc(m_pInpFile);
			cData[3] = (BYTE)fgetc(m_pInpFile);

			if(*((DWORD*)cData) == 0xB6010000)
			{
				// Anfang eines Frames gefunden
				fseek(m_pInpFile, -4, SEEK_CUR);
				fgetpos(m_pInpFile, &pos);

				//TRACE(_T("%d\n"),pos);

				// I- oder P-Frame?
				fseek(m_pInpFile, -STREAM_INFO_LENGTH-7, SEEK_CUR);
				cData[0] = (BYTE)fgetc(m_pInpFile);
				cData[1] = (BYTE)fgetc(m_pInpFile);
				cData[2] = (BYTE)fgetc(m_pInpFile);
				cData[3] = (BYTE)fgetc(m_pInpFile);

				//TRACE(_T("cData: %x\n"),*((DWORD*)cData));

				if(*((DWORD*)cData) == 0xB0010000)
				{
					// I-Frame!
					fseek(m_pInpFile, -4, SEEK_CUR);
					bIsPFrame = FALSE;
				}
				else 
				{
					// P-Frame!
					fsetpos(m_pInpFile, &pos);
					bIsPFrame = TRUE;
				}

				bFound = TRUE;
			}
			else
				fsetpos(m_pInpFile, &pos);
		}
	}

	//TRACE(_T("[CMPEG4Play2Doc::FindPrevVOP] %d\n"), ftell(m_pInpFile));

	// Der vorherige Frame wurde gefunden. Der FilePointer steht auf dem
	// Anfang des gefundenen Frames.
	// Wenn es ein P-Frame ist, so muss das vorherige I-Frame gesucht werden,
	// um dann bis zur aktuellen FilePointer-Position fortlaufend zu dekodieren.

	if(bIsPFrame)
	{
		// aktuellen FilePointer merken
		fgetpos(m_pInpFile, &posCurFrame);
		pos = posCurFrame;

		// I-Frame suchen
		for(int i=4;; i++)
		{
			fseek(m_pInpFile, -i, SEEK_CUR);
			//TRACE(_T("%d\n"),ftell(m_pInpFile));

			cData[0] = (BYTE)fgetc(m_pInpFile);
			cData[1] = (BYTE)fgetc(m_pInpFile);
			cData[2] = (BYTE)fgetc(m_pInpFile);
			cData[3] = (BYTE)fgetc(m_pInpFile);

			if(*((DWORD*)cData) == 0xB0010000)
			{
				// I-Frame gefunden
				fseek(m_pInpFile, -4, SEEK_CUR);
				//TRACE(_T("%d\n"),ftell(m_pInpFile));
				break;
			}
			if(*((DWORD*)cData) == 0xB6010000)
			{
				//TRACE(_T("%d\n"),ftell(m_pInpFile));
			}

			fsetpos(m_pInpFile, &posCurFrame);
		}

		// fortlaufend dekodieren bis pos
		if(fgetpos(m_pInpFile, &posCur) == 0)
		{
			while(posCur < posCurFrame)
			{
				GetVOPFromStream(m_pMPEG4Frame, &m_dwMPEG4FrameLength, m_pInpFile);
				m_pMPEG4Decoder->DecodeMpeg4FromMemory(m_pMPEG4Frame, m_dwMPEG4FrameLength);
				fgetpos(m_pInpFile, &posCur);
			}
		}
		else
			TRACE(_T("[CMPEG4Play2Doc::FindPrevVOP] fgetpos != 0\n"));
	}
}



CMPEG4Decoder* CMPEG4Play2Doc::GetMPEG4Decoder()
{
	return m_pMPEG4Decoder;
}


FILE* CMPEG4Play2Doc::GetFile()
{
	return m_pInpFile;
}

BYTE* CMPEG4Play2Doc::GetMPEG4Frame()
{
	return m_pMPEG4Frame;
}

DWORD CMPEG4Play2Doc::GetMPEG4FrameLength()
{
	return m_dwMPEG4FrameLength;
}


BOOL CMPEG4Play2Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CMPEG4Play2View* pActiveView;

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if(m_pInpFile)
		fclose(m_pInpFile);
	
	m_pInpFile = _tfopen(lpszPathName, _T("rb"));

	if(m_pInpFile)
	{
		// Endposition des Streams ermitteln
		fseek(m_pInpFile, 0, SEEK_END);
		fgetpos(m_pInpFile, &m_streamEndPos);
		fseek(m_pInpFile, 0, SEEK_SET);

		// ersten Frame dekodieren und anzeigen
		// -> dadurch wird StreamInfo ausgelesen und der Anwender
		//    kann die Anzeige des Streams per 'Play' starten
		if(m_pMPEG4Frame)
		{
			delete m_pMPEG4Frame;
			m_pMPEG4Frame = NULL;
		}

		m_pMPEG4Frame = new BYTE[COMPRESSED_FRAME_SIZE];

		if(m_pMPEG4Frame)
		{
			GetVOPFromStream(m_pMPEG4Frame, &m_dwMPEG4FrameLength, m_pInpFile);

			if(*((DWORD*)m_pMPEG4Frame) == 0xB0010000)
			{
				//TRACE(_T("%x\n"),*((DWORD*)&m_pMPEG4Frame[STREAM_INFO_LENGTH]));
				if(*((DWORD*)&m_pMPEG4Frame[STREAM_INFO_LENGTH]) == 0xB3010000)
					m_bIsIFrame = TRUE;
				else
					m_bIsIFrame = FALSE;
			}
			else
				m_bIsIFrame = FALSE;

			if(m_pMPEG4Decoder->DecodeMpeg4FromMemory(m_pMPEG4Frame, m_dwMPEG4FrameLength) && m_bIsIFrame)
			{
				if(m_bOpenViaFileDialog)
				{
					pActiveView = (CMPEG4Play2View*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
					pActiveView->OnDraw(pActiveView->GetDC());

					// reseten
					m_bOpenViaFileDialog = FALSE;
				}

			}
			else
			{
				// Entweder konnte der erste Frame nicht dekodiert werden oder die Datei
				// beginnt nicht mit einem I-Frame!
				fclose(m_pInpFile);
				m_pInpFile = NULL;
			}
		}
		else
			TRACE(_T("[CMPEG4Play2Doc::OnOpenDocument] Fehler bei der MPEG4Frame-Allozierung"));
	}
	else
		TRACE(_T("Fehler beim Oeffnen der Datei: %s\n"),lpszPathName);

	return m_bIsIFrame;
}



/*------
*
* Name: DecodeThreadProc()
*
* Zweck: Decoder-Thread
*
* Eingabeparameter: (LPVOID) pParam
*
* Rueckgabewert: (UINT)
*
* Datum: 25.04.2004
*
* letzte Aenderung: 25.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
UINT CMPEG4Play2Doc::ThreadProc(LPVOID pParam)
{
	CMPEG4Play2Doc* pDoc = (CMPEG4Play2Doc*)pParam;
	CMPEG4Play2View* pActiveView;
	
	if (pDoc)
	{
		while (pDoc->m_bThreadIsRunning)
		{
			if(pDoc->m_pInpFile)
			{
				if(pDoc->m_pMPEG4Frame)
				{
					pDoc->GetVOPFromStream(pDoc->m_pMPEG4Frame, &(pDoc->m_dwMPEG4FrameLength), pDoc->m_pInpFile);

					if(pDoc->m_dwMPEG4FrameLength)
					{
						if(*((DWORD*)pDoc->m_pMPEG4Frame) == 0xB0010000)
							pDoc->m_bIsIFrame = TRUE;
						else
							pDoc->m_bIsIFrame = FALSE;

						if(pDoc->m_pMPEG4Decoder->DecodeMpeg4FromMemory(pDoc->m_pMPEG4Frame, pDoc->m_dwMPEG4FrameLength))
						{
							pActiveView = (CMPEG4Play2View*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
							pActiveView->OnDraw(pActiveView->GetDC());
						}
					}
					else
					{	
						// zurueck zum Anfang der Datei
						fseek(pDoc->m_pInpFile, 0, SEEK_SET);
					}
				}
			}

			Sleep(1);
		}
	}

	return 0x0DEC;
}


BOOL CMPEG4Play2Doc::StartThread()
{
	BOOL bRet = TRUE;

	if(!m_bThreadIsRunning)
	{
		m_bThreadIsRunning = TRUE;
		m_pThread = AfxBeginThread(ThreadProc,(LPVOID)this);

		if(m_pThread == NULL)
		{
			TRACE(_T("[CMPEG4Play2Doc::StartThread] Decoder-Thread konnte nicht erstellt werden!\n"));
			bRet = FALSE;
		}
		else
		{
			// m_pThread soll im Destruktor geloescht werden
			m_pThread->m_bAutoDelete = FALSE;
		}
	}

	return bRet;
}


BOOL CMPEG4Play2Doc::StopThread()
{
	m_bThreadIsRunning = FALSE;

	if (m_pThread)
	{
		WaitForSingleObject(m_pThread->m_hThread,5000);
		delete m_pThread;

		m_pThread = NULL;
	}

	return TRUE;
}

void CMPEG4Play2Doc::ShowNextPicture()
{
	CMPEG4Play2View* pActiveView;
	fpos_t posCur;

	if(m_pMPEG4Frame && m_pInpFile)
	{
		// Ist das Ende der Datei erreicht?
		// -> dann zurueck zum Anfang!
		fgetpos(m_pInpFile, &posCur);
		if(posCur == m_streamEndPos)
			fseek(m_pInpFile, 0, SEEK_SET);

		GetVOPFromStream(m_pMPEG4Frame, &(m_dwMPEG4FrameLength), m_pInpFile);

		if(m_dwMPEG4FrameLength)
		{
			if(*((DWORD*)m_pMPEG4Frame) == 0xB0010000)
				m_bIsIFrame = TRUE;
			else
				m_bIsIFrame = FALSE;

			if(m_pMPEG4Decoder->DecodeMpeg4FromMemory(m_pMPEG4Frame, m_dwMPEG4FrameLength))
			{
				pActiveView = (CMPEG4Play2View*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
				pActiveView->OnDraw(pActiveView->GetDC());
			}
		}
		else
		{	
			TRACE(_T("[CMPEG4Play2Doc::ShowNextPicture] m_dwMPEG4FrameLength = 0\n"));
		}
	}
}



void CMPEG4Play2Doc::ShowPrevPicture()
{
	CMPEG4Play2View* pActiveView;

	if(m_pMPEG4Frame && m_pInpFile)
	{
		// FilePointer auf den Anfang des vorherigen Frames setzen
		FindPrevVOP();

		GetVOPFromStream(m_pMPEG4Frame, &(m_dwMPEG4FrameLength), m_pInpFile);

		if(m_dwMPEG4FrameLength)
		{
			if(*((DWORD*)m_pMPEG4Frame) == 0xB0010000)
				m_bIsIFrame = TRUE;
			else
				m_bIsIFrame = FALSE;

			if(m_pMPEG4Decoder->DecodeMpeg4FromMemory(m_pMPEG4Frame, m_dwMPEG4FrameLength))
			{
				pActiveView = (CMPEG4Play2View*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
				pActiveView->OnDraw(pActiveView->GetDC());
			}
		}
	}
}


void CMPEG4Play2Doc::GoToStart()
{
	CMPEG4Play2View* pActiveView;

	if(m_pMPEG4Frame && m_pInpFile)
	{
		fseek(m_pInpFile, 0, SEEK_SET);

		GetVOPFromStream(m_pMPEG4Frame, &(m_dwMPEG4FrameLength), m_pInpFile);

		if(m_dwMPEG4FrameLength)
		{
			if(*((DWORD*)m_pMPEG4Frame) == 0xB0010000)
				m_bIsIFrame = TRUE;
			else
				m_bIsIFrame = FALSE;

			if(m_pMPEG4Decoder->DecodeMpeg4FromMemory(m_pMPEG4Frame, m_dwMPEG4FrameLength))
			{
				pActiveView = (CMPEG4Play2View*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
				pActiveView->OnDraw(pActiveView->GetDC());
			}
		}
	}
}



void CMPEG4Play2Doc::GoToEnd()
{
	CMPEG4Play2View* pActiveView;

	if(m_pMPEG4Frame && m_pInpFile)
	{
		m_bGoToEnd = TRUE;

		fseek(m_pInpFile, 0, SEEK_END);

		TRACE(_T("[CMPEG4Play2Doc::GoToEnd] %d   %d\n"), ftell(m_pInpFile), m_streamEndPos);

		FindPrevVOP();

		GetVOPFromStream(m_pMPEG4Frame, &m_dwMPEG4FrameLength, m_pInpFile);

		TRACE(_T("[CMPEG4Play2Doc::GoToEnd] %d\n"), ftell(m_pInpFile));

		if(m_dwMPEG4FrameLength)
		{
			if(*((DWORD*)m_pMPEG4Frame) == 0xB0010000)
				m_bIsIFrame = TRUE;
			else
				m_bIsIFrame = FALSE;

			if(m_pMPEG4Decoder->DecodeMpeg4FromMemory(m_pMPEG4Frame, m_dwMPEG4FrameLength))
			{
				pActiveView = (CMPEG4Play2View*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
				pActiveView->OnDraw(pActiveView->GetDC());
			}
		}
		else
		{	
			// zurueck zum Anfang der Datei
			fseek(m_pInpFile, 0, SEEK_SET);
		}
	}
}


BOOL CMPEG4Play2Doc::IsIFrame()
{
	return m_bIsIFrame;
}



fpos_t CMPEG4Play2Doc::GetStreamEndPos()
{
	return m_streamEndPos;
}


BOOL CMPEG4Play2Doc::GetOpenFileViaDialog()
{
	return m_bOpenViaFileDialog;
}


void CMPEG4Play2Doc::SetOpenFileViaDialog(BOOL bOpenFileViaDialog)
{
	m_bOpenViaFileDialog = bOpenFileViaDialog;
}