// DibData.cpp: implementation of the CDibData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdipClient.h"
#include "DibData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CString g_Format;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDibData::CDibData(CDib* pDib,
					 int iNr,
					 const CString sPicture,
					 const CString& sData,
					 const CString& sComment,
					 WORD  wSeqNr /* = 0 */)
{
	m_sPicture = sPicture;
	m_sData = sData;
	m_sComment = sComment;
	m_iPicNr = iNr;
	m_wSeqNr = wSeqNr;
	m_pDib = pDib;

	CString s,f;
	f.LoadString(IDS_PICTURE_NR);
	s.Format(_T("%s:\t%d\n"),f,m_iPicNr);
	m_sToolTip = s + sPicture;
	if (!sData.IsEmpty())
	{
		m_sToolTip += _T('\n');
		m_sToolTip += sData;
	}

}
//////////////////////////////////////////////////////////////////////
CDibData::CDibData(CDib* pDib,
					 const CString sPicture,
					 const CString& sData,
					 const CString& sComment)
{
	m_iPicNr = 0;
	m_wSeqNr = 0;
	m_pDib = pDib;
	m_sPicture = sPicture;
	m_sComment = sComment;
	m_sData = sData;
}
//////////////////////////////////////////////////////////////////////
CDibData::CDibData(CDib* pDib)
{
	m_iPicNr = 0;
	m_wSeqNr = 0;
	m_pDib = pDib;
}
//////////////////////////////////////////////////////////////////////
CDibData::~CDibData()
{
	WK_DELETE(m_pDib);
}
//////////////////////////////////////////////////////////////////////
LPCTSTR  CDibData::GetTooltipText()
{
	return (LPCTSTR) m_sToolTip;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CDibDatas::CDibDatas()
{
	m_iCurrentPicNr = -1;
	m_iCurrentSeqNr = -1;
}
//////////////////////////////////////////////////////////////////////
CDibDatas::~CDibDatas()
{
	DeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CDibDatas::Add(CDibData* pDibData, int nNewSize)
{
	m_iCurrentPicNr = pDibData->GetPicNr();
	m_iCurrentSeqNr = pDibData->GetSeqNr();

	CDibData* pDD = NULL;

	for (int i=0;i<GetSize();i++)
	{
		int iSeqNr = GetAtFast(i)->GetSeqNr();

		if(m_iCurrentSeqNr == iSeqNr) 
		{
			//selbe Sequenz, ordne Bild hinten bzw. vorne ein
			if (pDibData->GetPicNr() < GetAtFast(i)->GetPicNr())
			{
				InsertAt(i,pDibData);
				while (GetSize()>nNewSize)
				{
					pDD = GetAtFast(GetSize()-1);
					WK_DELETE(pDD);
					RemoveAt(GetSize()-1);
				}
				return;
			}
			if (pDibData->GetPicNr() == GetAtFast(i)->GetPicNr())
			{
				pDD = GetAtFast(i);
				WK_DELETE(pDD);
				SetAt(i,pDibData);
				return;
			}			
		}
		else if (m_iCurrentSeqNr > iSeqNr)
		{
			if(GetSize() == nNewSize) //nur wenn alle fenster angezeigt sind
			{
				if (pDibData->GetPicNr() > GetAtFast(i)->GetPicNr())
				{
					if(i == nNewSize-1)
					{
						CDibDataArray::Add(pDibData);
						pDD = GetAtFast(0);
						WK_DELETE(pDD);
						RemoveAt(0);
						return;
					}
				}
				else
				{
					//aktuelle Bildnr größer als die gerade zu prüfende im Array
					//gehe alle angezeigten Bilder durch
					for (int j = 0; j < nNewSize; j++)
					{
						if(pDibData->GetPicNr() == GetAtFast(j)->GetPicNr())
						{
							if(pDibData->GetSeqNr() == GetAtFast(j)->GetSeqNr())
							//gerade wird geblättert, da das aktuelle Bild
							//schon angezeigt wird, also kein neues Bild einfügen
							return;
						}
					}

					//vorwärts abspielen, hänge neues, kleineres Bild (neue Sequenz) an
					CDibDataArray::Add(pDibData);
					pDD = GetAtFast(0);
					WK_DELETE(pDD);
					RemoveAt(0);
					return;
				}
			}
		}
		else
		{
			if(GetSize() == nNewSize) //nur wenn alle fenster angezeigt sind
			{
				//rückwärts abspielen, neues Bild (der neuen Sequenz) nach vorne, letzes Löschen
				InsertAt(0,pDibData);
				pDD = GetAtFast(GetSize()-1);
				WK_DELETE(pDD);
				RemoveAt(GetSize()-1);
				return;
			}
			else
			{
				//rückwärts abspielen, neues Bild (der neuen Sequenz) vorne einfüge
				//kein Bild löschen, da noch nicht die volle Bildzahl erreicht ist
				InsertAt(0,pDibData);
				while (GetSize()>nNewSize)
				{
					pDD = GetAtFast(GetSize()-1);
					WK_DELETE(pDD);
					RemoveAt(GetSize()-1);
				}
				return;
			}
		}
	}

	// append new item
	CDibDataArray::Add(pDibData);
	while (GetSize()>nNewSize)
	{
		pDD = GetAtFast(0);
		WK_DELETE(pDD);
		RemoveAt(0);
	}
}
//////////////////////////////////////////////////////////////////////
void CDibDatas::DeleteAllExceptCurrent()
{
	CDibData* pDD;
	for (int i=GetSize()-1;i>=0;i--)
	{
		pDD = GetAtFast(i);
		if (pDD->GetPicNr()!=m_iCurrentPicNr)
		{
			WK_DELETE(pDD);
			RemoveAt(i);
		}
	}
}
