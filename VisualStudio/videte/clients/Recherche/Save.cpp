// DisplayWindow.cpp : implementation file
//

#include "stdafx.h"

#include "Recherche.h"

#include "RechercheDoc.h"
#include "RechercheView.h"
#include "DisplayWindow.h"
#include "CIPCServerControlRecherche.h"
#include "IPCControlAudioUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayWindow Saving Code
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnFileDib() 
{
	COemFileDialog dlg(this);
	CString sSave,sDIB;

	sSave.LoadString(IDS_SAVE);
	sDIB.LoadString(IDS_DIB);
	dlg.SetProperties(FALSE,sSave,sSave);
	dlg.SetInitialDirectory(_T("c:\\out"));
	dlg.AddFilter(sDIB,_T("bmp"));
	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		BeginWaitCursor();
		SaveDIB(sFileName);
		EndWaitCursor();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnFileDisk() 
{
	DWORD dwDrives = GetLogicalDrives();
	int i,j;
	char ch = _T('a');
	CString sRoot,sFloppies;

	// browse drives for floppies
	for (i=0;i<32;i++)
	{
		if ( (1<<i) & dwDrives ) 
		{
			sRoot.Format(_T("%c:\\"),ch);
			if (GetDriveType((LPCTSTR)sRoot)==DRIVE_REMOVABLE)
			{
				sFloppies += ch;
			}
		}
		ch++;
	}

	if (sFloppies.IsEmpty())
	{
		return;
	}

    DWORD dwSectorsPerCluster;
    DWORD dwBytesPerSector;
    DWORD dwNumberOfFreeClusters;
    DWORD dwTotalNumberOfClusters;
	DWORD dwTotalBytes,dwFreeBytes;
	
	dwTotalBytes = 0;
	dwFreeBytes = 0;

	for (i=0;i<sFloppies.GetLength();i++)
	{
		sRoot.Format(_T("%c:\\"),sFloppies[i]);
		if (GetDiskFreeSpace((LPCTSTR)sRoot,
							&dwSectorsPerCluster,
							&dwBytesPerSector,
							&dwNumberOfFreeClusters,
							&dwTotalNumberOfClusters))
		{
			dwTotalBytes = dwTotalNumberOfClusters * dwSectorsPerCluster * dwBytesPerSector;
			dwFreeBytes = dwNumberOfFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
			break;
		}
	}
	if (dwTotalBytes==0)
	{
		CString sLS,sMes;

		sLS.LoadString(IDP_NO_DISK);
		sMes.Format(sLS,(LPCTSTR)sFloppies);
		COemGuiApi::MessageBox(sMes,20,MB_OK|MB_ICONSTOP);
		return;
	}

	if (dwFreeBytes<70*1024)
	{
		CString sLS,sMes;

		WK_TRACE_ERROR(_T("cannot store on drive %s , not enough space left"),(LPCTSTR)sRoot);

		sLS.LoadString(IDP_NO_DISK_SPACE);
		sMes.Format(sLS,(LPCTSTR)sRoot.Left(1));
		COemGuiApi::MessageBox(sMes,20,MB_OK|MB_ICONSTOP);
		return;
	}

	CStringArray sFiles;
	CString sExt;
#ifdef _DTS
	sExt = _T("htm");
#else
	sExt = _T("jpx");
#endif
	WK_SearchFiles(sFiles,sRoot,_T("*.") + sExt);

	CString sFileName;
	BOOL bFree = TRUE;

	for (i=0;i<32767;i++)
	{
		sFileName.Format(_T("%08d.%s"),i,sExt);
		bFree = TRUE;
		for (j=0;j<sFiles.GetSize();j++)
		{
			if (sFileName == sFiles[j])
			{
				bFree = FALSE;
				break;
			}
		}
		if (bFree)
		{
			break;
		}
	}
	
	if (bFree)
	{
		sFileName = sRoot + sFileName;
		CString sLS,sMes;

#ifdef _DTS
		if (SaveHTML(sFileName))
#else
		if (SaveJPX(sFileName))
#endif
		{
			WK_TRACE(_T("wrote file %s"),(LPCTSTR)sFileName);

			sLS.LoadString(IDP_DISK_WRITTEN);
			sMes.Format(sLS,(LPCTSTR)sFileName);
			COemGuiApi::MessageBox(sMes,20,MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			WK_TRACE_ERROR(_T("Datei %s nicht geschrieben, Diskette defekt oder read only"),(LPCTSTR)sFileName);
			sMes.LoadString(IDP_DISK_RO);
			COemGuiApi::MessageBox(sMes,20,MB_OK|MB_ICONSTOP);
		}
	}
	else
	{
		// should never be reached
		WK_TRACE_ERROR(_T("kein freier Dateiname mehr  auf %s"),(LPCTSTR)sRoot);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnFileHtml() 
{
	COemFileDialog dlg(this);
	CString sSave,sHTML;

	sSave.LoadString(IDS_SAVE);
	sHTML.LoadString(IDS_HTML);
	dlg.SetProperties(FALSE,sSave,sSave);
	dlg.SetInitialDirectory(_T("c:\\out"));
	dlg.AddFilter(sHTML,_T("htm"));
	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		BOOL bWrite = TRUE;
		if (DoesFileExist(sFileName))
		{
			CString s;
			s.Format(IDP_FILE_OVERWRITE,sFileName);
			bWrite = IDYES == AfxMessageBox(s,MB_YESNO);
		}
		if (bWrite)
		{
			BeginWaitCursor();
			SaveHTML(sFileName);
			EndWaitCursor();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnFileJpg() 
{
	COemFileDialog dlg(this);
	CString sSave,sJpeg;

	sSave.LoadString(IDS_SAVE);
	sJpeg.LoadString(IDS_JPEG);
	dlg.SetProperties(FALSE,sSave,sSave);
	dlg.SetInitialDirectory(_T("c:\\out"));
	dlg.AddFilter(sJpeg,_T("jpg"));
	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		BOOL bWrite = TRUE;
		if (DoesFileExist(sFileName))
		{
			CString s;
			s.Format(IDP_FILE_OVERWRITE,sFileName);
			bWrite = IDYES == AfxMessageBox(s,MB_YESNO);
		}
		if (bWrite)
		{
			BOOL bOK = FALSE;
			BeginWaitCursor();
			bOK = SaveJPG(sFileName);
			EndWaitCursor();
			CString s;
			if (bOK)
			{
				s.Format(IDP_DISK_WRITTEN,sFileName);
			}
			else
			{
				s.Format(IDS_WRITE_FAILED,sFileName);
			}
			COemGuiApi::MessageBox(s,20);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnFileJpx() 
{
	COemFileDialog dlg(this);
	CString sSave,sJpx;

	sSave.LoadString(IDS_SAVE);
	sJpx.LoadString(IDS_JPX);
	dlg.SetProperties(FALSE,sSave,sSave);
	dlg.SetInitialDirectory(_T("c:\\out"));
	dlg.AddFilter(sJpx,_T("jpx"));
	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		BOOL bWrite = TRUE;
		if (DoesFileExist(sFileName))
		{
			CString s;
			s.Format(IDP_FILE_OVERWRITE,sFileName);
			bWrite = IDYES == AfxMessageBox(s,MB_YESNO);
		}
		if (bWrite)
		{
			BOOL bOK = FALSE;
			BeginWaitCursor();
			bOK = SaveJPX(sFileName);
			EndWaitCursor();
			CString s;
			if (bOK)
			{
				s.Format(IDP_DISK_WRITTEN,sFileName);
			}
			else
			{
				s.Format(IDS_WRITE_FAILED,sFileName);
			}
			COemGuiApi::MessageBox(s,20);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::SaveJPG(const CString& sFileName)
{
	BOOL bRet = TRUE;
	
	if (m_pPictureRecord)
	{
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{
			// todo uncomment for stretching jpg files
			if (m_pPictureRecord->GetResolution() == RESOLUTION_HIGH)
			{
				HANDLE hDIB = m_Jpeg.CreateDIB();
				if (hDIB)
				{
					bRet = m_Jpeg.EncodeJpegToFile(hDIB, sFileName);
					GlobalFree(hDIB);		
				}
			}
			else
			{
				bRet = SaveRAW(sFileName);
			}
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			CJpeg jpeg;

			HGLOBAL hDIB = m_H263.GetDib()->GetHDIB();
			if (hDIB)
			{
				bRet = jpeg.EncodeJpegToFile(hDIB, sFileName);
				GlobalFree(hDIB);
			}
		}
#ifndef _DTS
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			CDib dib(m_PTDecoder.GetBitmapHandle());
			dib.IncreaseTo24Bit();
			CJpeg jpeg;

			HGLOBAL hDIB = dib.GetHDIB();
			if (hDIB)
			{
				bRet = jpeg.EncodeJpegToFile(hDIB, sFileName);
				GlobalFree(hDIB);
			}
		}
#endif
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::SaveJPX(const CString& sFileName)
{
	CString sTemp = WK_GetTempFile(_T("REC"));

	if (m_pPictureRecord->GetCompression()==COMPRESSION_JPEG)
	{
		if (!SaveRAW(sTemp))
		{
			DeleteFile(sTemp);
			return FALSE;
		}
	}
	else
	{
		if (!SaveJPG(sTemp))
		{
			DeleteFile(sTemp);
			return FALSE;
		}
	}

	BOOL bRet = TRUE; // GF Interessanter Default-Wert ...
	CFile file;
	BYTE  b;
	WORD  w;
	DWORD dw;
	CString sData;
	CFile temp;

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite))
	{
		TRY
		{
			// DateiTyp schreiben
			b = 1;
			file.Write(&b, 1);

			// Anzahl Bilder schreiben
			w = 1;
			file.Write(&w, 2);
			
			// Compression schreiben
			b = (BYTE)COMPRESSION_JPEG;
			file.Write(&b, 1);

			// BildResolution schreiben
			if (m_pPictureRecord->GetCompression()==COMPRESSION_JPEG)
			{
				b = (BYTE)m_pPictureRecord->GetResolution();
			}
			else
			{
				b = (BYTE)RESOLUTION_LOW;
			}
			file.Write(&b, 1);

			// Laenge der ExtraData schreiben
			sData = BuildExtraData();
			dw = sData.GetLength();
			file.Write(&dw, 4);

			// ExtraData schreiben
			file.Write((LPCTSTR)sData, (UINT)sData.GetLength());

			if (temp.Open(sTemp,CFile::modeRead))
			{
				DWORD dwLen;
				BYTE* pBuffer = NULL;
				CFileStatus cfs;

				TRY
				{
					if (temp.GetStatus(cfs))
					{
						dwLen = cfs.m_size;
						pBuffer = new BYTE[dwLen];
						if (dwLen!=temp.Read(pBuffer,dwLen))
						{
							WK_DELETE_ARRAY(pBuffer);
						}
					}
					temp.Close();
				}
				CATCH (CFileException,e)
				{
					// GF Schön ... dass sich hierbei jemand was gedacht hat .. aber was?
					// Kein Fehler? Alles ok?
					// Vielleicht doch lieber FALSE zurück liefern?
					// Ist doch schließlich das Bild, was hier gelesen worden sein muss - oder?
					bRet = FALSE;
				}
				END_CATCH

				if (pBuffer)
				{
					// Laenge der PictData schreiben
					file.Write(&dwLen,4);
					
					// PictData schreiben
					file.Write(pBuffer, (UINT)dwLen);
				}
				WK_DELETE_ARRAY(pBuffer);
			}
			else
			{
				// GF Schön ... dass sich hierbei jemand NICHTS gedacht hat ...
				// Kein Fehler? Alles ok?
				// Vielleicht doch lieber FALSE zurück liefern?
				// Ist doch schließlich das Bild, was hier gelesen worden sein muss - oder?
				bRet = FALSE;
			}
		}
		CATCH (CFileException,e)
		{
			bRet = FALSE;
		}
		END_CATCH
	}
	else
	{
		// GF Schön ... dass sich hierbei jemand NICHTS gedacht hat ...
		// Kein Fehler? Alles ok?
		// Vielleicht doch lieber FALSE zurück liefern?
		// Ist doch schließlich das Bild, was hier hinein geschrieben worden sein sollte ...
		bRet = FALSE;
	}
	
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::BuildExtraData()
{
	// InfoString1@InfoString2@Kommentar@GA oder SDI Daten
	CString s;

	// Check des Kommentarfeldes: Archivname und Kamera Nr ergänzen,
	// so noch nicht vorhanden
	CString sTemp = m_sComment;
	
	if (m_sComment.Find(_T("Archiv:")) == -1)
	{
		sTemp.Format(_T("Archiv: %s\r\nKamera Nr: %d\r\n%s"), 
							(LPCTSTR)m_sArchivName, 
							1+(int)m_pPictureRecord->GetCamID().GetSubID(), 
							(LPCTSTR)m_sComment);
	}

	s = FormatPicture();
	s += _T('@');
	s += FormatData(_T(", "),_T(":"));
	s += _T('@');
	s += sTemp;
	s += _T('@');
	if (m_pPictureRecord)
	{
		s += m_pPictureRecord->GetInfoString();
	}

	return s;
}
/////////////////////////////////////////////////////////////////////////////
int	CDisplayWindow::GetRecNo()
{
	return m_dwCurrentRecord;
}
/////////////////////////////////////////////////////////////////////////////
int	CDisplayWindow::GetRecCount()
{
	return m_SequenceRecord.GetNrOfPictures();
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::FormatComment()
{
	return m_sComment;
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::FormatCommentHtml()
{
	CString sReturn = m_sComment;
	sReturn.Replace(_T("&"), _T("&amp;"));
	sReturn.Replace(_T("<"), _T("&lt;"));
	sReturn.Replace(_T(">"), _T("&gt;"));
	sReturn.Replace(_T("\""), _T("&quot;"));
	sReturn.Replace(_T("\r\n"), _T("<br>")); // check first to avoid two line breaks 
	sReturn.Replace(_T("\n\r"), _T("<br>")); // check first to avoid two line breaks 
	sReturn.Replace(_T("\n"), _T("<br>"));
	sReturn.Replace(_T("  "), _T("&nbsp;&nbsp;"));
	sReturn.Replace(_T("\t"), _T("&nbsp;&nbsp;&nbsp;&nbsp;"));
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetDate()
{
	CString s;

	if (m_pAudioRecord)
	{	
		CSystemTime t;
		m_pAudioRecord->GetMediaTime(t);
		s = t.GetDate();
	}
	else if (m_pPictureRecord)
	{	
		s = m_pPictureRecord->GetTimeStamp().GetDate();
	}

	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetTime()
{
	CString s;

	if (m_pAudioRecord)
	{	
		CSystemTime t;
		m_pAudioRecord->GetMediaTime(t);
		s.Format(_T("%s,%03d"),t.GetTime(),t.GetMilliseconds());
	}
	else if (m_pPictureRecord)
	{	
		s.Format(_T("%s,%03d"),m_pPictureRecord->GetTimeStamp().GetTime(),m_pPictureRecord->GetTimeStamp().GetMilliseconds());
	}

	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetCompression()
{
	CString s;

	if (m_pAudioRecord)
	{
		if (m_bAudioBusy)
		{
			s.LoadString(IDS_AUDIO_BUSY);
			if (m_pPictureRecord == NULL)
			{
				OnNavigateStop();
			}
		}
		else
		{
			if (m_pAudioRecord->IsLongHeader())
			{
				s = CIPCAudioRecherche::GetCompressionType(m_pAudioRecord);
				m_sLastCompressionType = s;
			}
			else
			{
				s = m_sLastCompressionType;
			}
		}
#ifdef _DEBUG
		if (m_pAudioRecord->IsLongHeader())
		{
			s += _T(" LH");
		}
		if (m_pAudioRecord->GetIsSyncPoint())
		{
			s += _T(" SP");
		}
#endif
	}
	else if (m_pPictureRecord)
	{	
		switch (m_pPictureRecord->GetCompressionType())
		{
		case COMPRESSION_H263:
			s = _T("H.263");
#ifdef _DEBUG
			if (m_pPictureRecord->GetPictureType() == SPT_FULL_PICTURE)
			{
				s += _T(" I");
			}
			else if (m_pPictureRecord->GetPictureType() == SPT_DIFF_PICTURE)
			{
				s += _T(" P");
			}
#endif
			break;
		case COMPRESSION_JPEG:
			s = _T("JPEG");
			break;
		case COMPRESSION_RGB_24:
			s = _T("Bitmap");
			break;
		case COMPRESSION_PRESENCE:
			s = _T("PRESENCE");
			break;
		}
	}
	
	return s;
}
/////////////////////////////////////////////////////////////////////////////
int CDisplayWindow::GetCameraNr()
{
	if (m_pPictureRecord)
	{
		return 1+(int)m_pPictureRecord->GetCamID().GetSubID();
	}
	else if (m_pAudioRecord)
	{
		return 1+(int)m_pAudioRecord->GetID().GetSubID();
	}
	return -1;
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::FormatPicture(BOOL bTabbed /*= FALSE*/)
{
	CString s;

	if (m_pPictureRecord)
	{	
		CString sFormat,sDate,sTime;
		sDate = m_pPictureRecord->GetTimeStamp().GetDate();
		sTime = m_pPictureRecord->GetTimeStamp().GetTime();
		sFormat.LoadString(bTabbed ? IDS_PICTURE_TABBED : IDS_PICTURE);
		
		CSize size = GetPictureSize();
		s.Format(sFormat, 1+(int)m_pPictureRecord->GetCamID().GetSubID(),
						  sDate,sTime,size.cx,size.cy);
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::SaveDIB(const CString& sFileName)
{
	BOOL bRet = TRUE;
	if (m_pPictureRecord)
	{
		if (m_pPictureRecord && m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{					 
			CSize s = GetPictureSize();

			HBITMAP hBitmap = m_Jpeg.CreateBitmap();
			CDib dib(hBitmap);
			bRet = dib.Save(sFileName);
			DeleteObject(hBitmap);
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			CDib* pDib;
			pDib = m_H263.GetDib();
			if (pDib)
			{
				bRet = pDib->Save(sFileName);
				WK_DELETE(pDib);
			}
		}					   
#ifndef _DTS
		else if (m_pPictureRecord && m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			CDib dib(m_PTDecoder.GetBitmapHandle());
			dib.IncreaseTo24Bit();
			bRet = dib.Save(sFileName);
		}
#endif
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::SaveRAW(const CString& sFileName)
{
	CFile file;
	BOOL bRet = TRUE;

	TRY
	{
		if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite))
		{
			file.Write((BYTE*)m_pPictureRecord->GetData(),
					   m_pPictureRecord->GetDataLength()
						);
			file.Close();

		}
	}
	CATCH (CFileException, e)
	{
		bRet = FALSE;
	}
	END_CATCH

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::SaveHTML(const CString& sPathName)
{
	CString sName,sJpg;
	int p;
	BOOL bRet = TRUE;

	p = sPathName.ReverseFind(_T('.'));
	sJpg = sPathName.Left(p+1) + _T("jpg");
	sName = sPathName.Left(p);
	p = sName.ReverseFind(_T('\\'));
	sName = sName.Mid(p+1);

	bRet = SaveJPG(sJpg);

	CStdioFile html;
	CFileException fe;
	CString sTemp,sT,sL;
	CString str;
	CSize size;

	str = theApp.GetStationName();
	if (str.IsEmpty())
	{
		str = COemGuiApi::GetProducerName();
	}
	size = GetPictureSize();

	if (!html.Open(sPathName, CFile::modeCreate |
	  CFile::modeReadWrite | CFile::shareExclusive, &fe))
	{
		return FALSE;
	}

	html.WriteString(_T("<html>\n"));
	
	html.WriteString(_T("<head>\n"));
	sTemp.Format(_T("<title>%s</title>\n"),str);
	html.WriteString(sTemp);
	html.WriteString(_T("</head>\n"));
	
	html.WriteString(_T("<body>\n"));

	html.WriteString(_T("<table border=\"1\" cellpadding=\"1\">\n"));

	html.WriteString(_T("<tr><td><h3 align=\"center\">\n"));
	html.WriteString(str);
	html.WriteString(_T("</h3></td></tr>\n"));

	html.WriteString(_T("<tr><td>\n"));
//	sTemp.Format(_T("<img src=\")%s.jpg\_T(" width=\")%d\_T(" height=\")%d\_T(">\n"),
//				 sName,size.cx,size.cy);
	sTemp.Format(_T("<img src=\"%s.jpg\">\n"),sName);
	html.WriteString(sTemp);
	html.WriteString(_T("</tr></td>\n"));

	html.WriteString(_T("<tr><td>\n"));
	html.WriteString(_T("<table border=\"1\" cellpadding=\"1\" width=\"100%\">\n"));

	CString sData,name,value;

	// PC - Date
	// PC - Time
	// Resolution
	name.LoadString(IDS_REC_DATE);
	value = m_pPictureRecord->GetTimeStamp().GetDate();
	sData.Format(_T("<tr><td>%s</td><td>%s</td></tr>\n"),name,value);
	html.WriteString(sData);

	name.LoadString(IDS_RESOLUTION);
	sData.Format(_T("<tr><td>%s</td><td>%dx%d</td></tr>\n"),name,size.cx,size.cy);
	html.WriteString(sData);

	name.LoadString(IDS_REC_TIME);
	value = m_pPictureRecord->GetTimeStamp().GetTime();
	sData.Format(_T("<tr><td>%s</td><td>%s</td></tr>\n"),name,value);
	html.WriteString(sData);

	for (int i=0;i<m_Fields.GetSize();i++)
	{
		CIPCField* pField = m_Fields.GetAt(i);
		if (GetServer())
		{
			name = GetServer()->GetFieldName(pField->GetName());
		}
		if (name.IsEmpty())
		{
			name = theApp.GetMappedString(pField->GetName());
		}
		value = pField->ToString();
		if (!name.IsEmpty() && !value.IsEmpty())
		{

			sData.Format(_T("<tr><td>%s</td><td>%s</td></tr>\n"),name,value);
			html.WriteString(sData);
		}
	}

	// Comment
	name.LoadString(IDS_COMMENT);
	sData.Format(_T("<tr><td>%s</td><td>%s</td></tr>\n"),name, FormatCommentHtml());
	html.WriteString(sData);

	html.WriteString(_T("</table>\n"));
	html.WriteString(_T("</tr></td>\n"));

	html.WriteString(_T("</table>\n"));

	html.WriteString(_T("</body>\n"));
	html.WriteString(_T("</html>\n"));

	html.Close();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////

