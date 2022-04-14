// MPEG4Play2Doc.h : Schnittstelle der Klasse CMPEG4Play2Doc
//


#pragma once

#include <VImage\MPEG4Decoder.h>

#define COMPRESSED_FRAME_SIZE  300000     // maximale Groesse eines komprimierten Video-Fames
#define STREAM_INFO_LENGTH     28         // Annahme: Groesse des Streaminfo ist immer 28 Byte

class CMPEG4Play2Doc : public CDocument
{
// Konstruktor / Destruktor/////////////////////////////////////////////////////////
protected: // Nur aus Serialisierung erstellen
	CMPEG4Play2Doc();
	DECLARE_DYNCREATE(CMPEG4Play2Doc)

public:
	virtual ~CMPEG4Play2Doc();

// Attribute ///////////////////////////////////////////////////////////////////////
private:
	CMPEG4Decoder* m_pMPEG4Decoder;
	FILE*          m_pInpFile;
	BOOL           m_bThreadIsRunning;
	CWinThread*    m_pThread;
	BYTE*          m_pMPEG4Frame;
	DWORD          m_dwMPEG4FrameLength;
	BOOL           m_bIsIFrame;
	fpos_t         m_streamEndPos;
	BOOL           m_bOpenViaFileDialog;
	BOOL           m_bGoToEnd;

// Methoden ////////////////////////////////////////////////////////////////////////
public:
	CMPEG4Decoder* GetMPEG4Decoder();
	FILE* GetFile();
	static UINT ThreadProc(LPVOID pParam);
	void GetVOPFromStream(BYTE *buffer, DWORD *length, FILE *fp);
	BYTE* GetMPEG4Frame();
	DWORD GetMPEG4FrameLength();
	BOOL StartThread();
	BOOL StopThread();
	void ShowNextPicture();
	void ShowPrevPicture();
	void FindPrevVOP();
	void GoToStart();
	void GoToEnd();
	BOOL IsIFrame();
	fpos_t GetStreamEndPos();
	void SetOpenFileViaDialog(BOOL bOpenFileViaDialog);
	BOOL GetOpenFileViaDialog();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Überschreibungen /////////////////////////////////////////////////////////////////
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

// Generierte Funktionen für die Meldungstabellen ///////////////////////////////////
protected:
	DECLARE_MESSAGE_MAP()
};


