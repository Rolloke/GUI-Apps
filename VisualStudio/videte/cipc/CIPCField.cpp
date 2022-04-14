/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCField.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCField.cpp $
// CHECKIN:		$Date: 7/19/06 11:39a $
// VERSION:		$Revision: 103 $
// LAST CHANGE:	$Modtime: 7/19/06 11:39a $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//
// CIPCField.cpp: implementation of the CIPCField class.
//
//////////////////////////////////////////////////////////////////////
#include "StdCipc.h"

#include "CIPC.h"
#include "CipcExtraMemory.h"
#include "CIPCField.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define USE_PERFORMANCE_TIMER 1
#ifdef USE_PERFORMANCE_TIMER
 #include "wk_timer.h"
 CWK_Timer theTimer;
#endif

//////////////////////////////////////////////////////////////////////

CString CIPCField::m_sfTime			= _T("DBD_TIME");
CString CIPCField::m_sfDate			= _T("DBD_DATE");
CString CIPCField::m_sfGaNr			= _T("DBD_GANR");
CString CIPCField::m_sfTaNr			= _T("DBD_TANR");
CString CIPCField::m_sfKtNr			= _T("DBD_KTNR");
CString CIPCField::m_sfBcNr			= _T("DBD_BCNR");
CString CIPCField::m_sfAmnt			= _T("DBD_AMNT");
CString CIPCField::m_sfCurr			= _T("DBD_CURR");
CString CIPCField::m_sfStNm			= _T("DBD_STNM");
CString CIPCField::m_sfCaNm			= _T("DBD_CANM");
CString CIPCField::m_sfInNm			= _T("DBD_INNM");
CString CIPCField::m_sfSqNm			= _T("DBD_SQNM");
CString CIPCField::m_sfCustomerId	= _T("DBD_CSID");
CString CIPCField::m_sfFirstName	= _T("DBD_FINM");
CString CIPCField::m_sfLastName		= _T("DBD_LANM");
CString CIPCField::m_sfProcessId	= _T("DBD_PRID");
CString CIPCField::m_sfBoxId		= _T("DBD_BXID");
CString CIPCField::m_sfSoll			= _T("DBD_SOLL");
CString CIPCField::m_sfIst			= _T("DBD_ISTV");
CString CIPCField::m_sfDifferenz	= _T("DBD_DIFF");

// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCField::CIPCField()
{
//	CString m_sName; // used as CodeBase Field
//	CString m_sValue;
	m_wMaxLength = 0;
	m_cType = 'I';
	m_dwFlags = 0;
}
//////////////////////////////////////////////////////////////////////
CIPCField::CIPCField(const CString& sName,
					  const CString& sValue,
					  char  cType)
{
	m_sName = sName;
	m_sValue = sValue;
	m_wMaxLength = 0;
	m_cType = cType;
	m_dwFlags = 0;
	Trim();
}
//////////////////////////////////////////////////////////////////////
CIPCField::CIPCField(const CString& sName,
					  const CString& sValue,
					  WORD  wMaxLength,
					  char  cType)
{
	m_sName = sName;
	m_sValue = sValue;
	m_wMaxLength = wMaxLength;
	m_cType = cType;
	m_dwFlags = 0;
	Trim();
}
//////////////////////////////////////////////////////////////////////
CIPCField::CIPCField(const CIPCField& src)
{
	m_sName = src.m_sName;
	m_sValue = src.m_sValue;
	m_wMaxLength = src.m_wMaxLength;
	m_cType = src.m_cType;
	m_dwFlags = src.m_dwFlags;
}
//////////////////////////////////////////////////////////////////////
CIPCField::~CIPCField()
{

}
//////////////////////////////////////////////////////////////////////
void CIPCField::Trim()
{
	m_sValue.TrimLeft();
	m_sValue.TrimRight();
}
//////////////////////////////////////////////////////////////////////
void CIPCField::LeadingZeros()
{
	Trim();
	if (m_cType == 'N')
	{
		m_sValue = FillLeadingZeros(m_sValue,m_wMaxLength);
	}
}
//////////////////////////////////////////////////////////////////////
CString CIPCField::FillLeadingZeros(const CString& sData, WORD wLen)
{
	CString l0('0', wLen - sData.GetLength());
	return l0 + sData;
}
//////////////////////////////////////////////////////////////////////
CString CIPCField::DropLeadingZeros(const CString& sData)
{
	// Consider Leading Zero in Amount as 0,xx
	TCHAR tchar;
	for (int i=0;i<sData.GetLength();i++)
	{
		tchar = sData[i];
		if (tchar != _T('0'))
		{
			if (   (tchar == _T('.'))
				|| (tchar == _T(','))
				)
			{
				i = max(0, i--);
			}
			return sData.Mid(i);
		}
	}
	return "";
}
//////////////////////////////////////////////////////////////////////
const CString CIPCField::ToString() const
{
	if (   m_cType=='C'
		|| m_cType=='N')
	{
		if (!m_sValue.IsEmpty())
		{
			CString sValue = m_sValue;;
			sValue.TrimLeft();
			sValue.TrimRight();
			return DropLeadingZeros(m_sValue);
		}
	}
	else if (m_cType=='D')
	{
		if (!m_sValue.IsEmpty())
		{
			return m_sValue.Mid(6,2) + '.' + m_sValue.Mid(4,2) + '.' + m_sValue.Left(4);
		}
	}
	else if (m_cType=='T')
	{
		if (!m_sValue.IsEmpty())
		{
			return m_sValue.Left(2) + ':' + m_sValue.Mid(2,2) + ':' + m_sValue.Mid(4,2);
		}
	}
	else if (m_cType=='W')
	{
		if (!m_sValue.IsEmpty())
		{
			CString sValue(m_sValue);
			sValue.TrimLeft();
			sValue.TrimRight();
			sValue = DropLeadingZeros(sValue);
			if (!sValue.IsEmpty() 
				&& (sValue[0] == ','))
			{
				sValue = '0' + sValue;
			}
			return sValue;
		}
	}
	return m_sValue;
}
//////////////////////////////////////////////////////////////////////
void CIPCField::SetName(const CString& sName)
{
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
void CIPCField::SetValue(const CString& sValue)
{
	m_sValue = sValue;
	if (m_dwFlags & FIELD_FLAG_CAN_DEACTIATE)
	{
		m_sValue.Replace(_T('\\'), 0);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCField::SetMaxLength(WORD wMaxLength)
{
	m_wMaxLength = wMaxLength;
}
//////////////////////////////////////////////////////////////////////
void CIPCField::SetType(char cType)
{
	m_cType = cType;
}
//////////////////////////////////////////////////////////////////////
void CIPCField::SetFlags(DWORD dwFlags)
{
	m_dwFlags = dwFlags;
}
//////////////////////////////////////////////////////////////////////
CString CIPCField::Translate4xTo3x(const CString& sData)
{
	if (sData == m_sfAmnt)
   {
		return _T("BETRAG");
	}
	else if (sData == m_sfBcNr)
	{
		return _T("BLZ");
	}
	else if (sData == m_sfCurr)
	{
		return _T("WAEHRUNG");
	}
	else if (sData == m_sfGaNr)
	{
		return _T("GA_NR");
	}
	else if (sData == m_sfTaNr)
	{
		return _T("TA_NR");
	}
	else if (sData == m_sfKtNr)
	{
		return _T("KTO_NR");
	}
	else if (sData == m_sfTime)
	{
		return _T("DTP_ZEIT");
	}
	else if (sData == m_sfDate)
	{
		// take PC Datum, because there is no DTP Date
		return _T("DATUM");
	}
	else if (sData == _T("DBP_DATE"))
	{
		return _T("DATUM");
	}
	else if (sData == _T("DBP_TIME"))
	{
		return _T("ZEIT");
	}
	return _T("");
}
//////////////////////////////////////////////////////////////////////
char CIPCField::TranslateToType(const CString& sData)
{
	if (sData == m_sfAmnt)
   {
		return 'N';
	}
	else if (sData == m_sfBcNr)
	{
		return 'N';
	}
	else if (sData == m_sfCurr)
	{
		return 'W';
	}
	else if (sData == m_sfGaNr)
	{
		return 'N';
	}
	else if (sData == m_sfTaNr)
	{
		return 'N';
	}
	else if (sData == m_sfKtNr)
	{
		return 'N';
	}
	else if (sData == m_sfTime)
	{
		return 'T';
	}
	else if (sData == m_sfDate)
	{
		return 'D';
	}
	else if (sData == _T("DBP_DATE"))
	{
		return 'D';
	}
	else if (sData == _T("DBP_TIME"))
	{
		return 'T';
	}
	return 'U';
}
//////////////////////////////////////////////////////////////////////
CString CIPCField::Translate3xTo4x(const CString& sData)
{
	if (sData == _T("BETRAG"))
	{
		return m_sfAmnt;
	}
	else if (sData == _T("BLZ"))
	{
		return m_sfBcNr;
	}
	else if (sData == _T("WAEHRUNG"))
	{
		return m_sfCurr;
	}
	else if (sData == _T("GA_NR"))
	{
		return m_sfGaNr;
	}
	else if (sData == _T("TA_NR"))
	{
		return m_sfTaNr;
	}
	else if (sData == _T("KTO_NR"))
	{
		return m_sfKtNr;
	}
	else if (sData == _T("DTP_ZEIT"))
	{
		return m_sfTime;
	}
	else if (sData == _T("DATUM"))
	{
		return _T("DBP_DATE");
	}
	else if (sData == _T("ZEIT"))
	{
		return _T("DBP_TIME");
	}
	return _T("");
}
/////////////////////////////////////////////////////////////////
BOOL CIPCField::Load(const CString& sSection,int i,CWK_Profile& wkp)
{
	CString	t = wkp.GetString(sSection,i, _T(""));

	m_sName = wkp.GetStringFromString(t, INI_COMMENT, _T(""));
	m_sValue = wkp.GetStringFromString(t, INI_VALUE, _T(""));
	m_wMaxLength = (WORD)wkp.GetNrFromString(t, INI_MAX_LENGTH, 0);
	m_cType = 'I';
	CString s = wkp.GetStringFromString(t, INI_FIELD_TYPE, _T(""));
	if (!s.IsEmpty())
	{
		m_cType = (char)s[0];
	}
	m_dwFlags = wkp.GetHexFromString(t, INI_FLAG, 0);
	if (m_dwFlags & FIELD_FLAG_UNUSED)
	{
		CString sValue = wkp.GetStringFromString(t, INI_ID, _T(""));
		if (!sValue.IsEmpty())
		{
			m_sValue += _T("\\") + sValue;
			m_sValue.Replace(_T('\\'), 0);
		}
	}
	return m_sName.GetLength()>0;
}
/////////////////////////////////////////////////////////////////
void CIPCField::Save(const CString& sSection,int i,CWK_Profile& wkp)
{
	CString s;
	s.Format(INI_COMMENT _T("%s") INI_VALUE _T("%s") INI_MAX_LENGTH _T("%d") INI_FIELD_TYPE _T("%c"),
		m_sName,m_sValue,m_wMaxLength,m_cType);
	if (m_dwFlags)
	{
		CString sF;
		sF.Format(INI_FLAG _T("%08x"), m_dwFlags);
		s += sF;
		if (m_dwFlags & FIELD_FLAG_UNUSED)
		{
			m_sValue.Replace(0, _T('\\'));
			int nFind = m_sValue.Find(_T("\\"));
			if (nFind != -1)
			{
				s += INI_ID + m_sValue.Mid(nFind+1);
			}
			m_sValue.Replace(_T('\\'), 0);
		}
	}

	wkp.WriteStringIndex(sSection,i,s);
}
/////////////////////////////////////////////////////////////////
// CAVEAT local copy, not from the header
// Definition der Feldlängen
#define	DATUM_LEN	(6)
#define	ZEIT_LEN	(4)
#define	TA_LEN		(4)
#define	GAA_LEN		(6)
#define	BLZ_LEN		(8)
#define	KTONR_LEN	(10)
#define	BETRAG_LEN	(4)

#define	GDC_GESAMT_LEN (DATUM_LEN+ZEIT_LEN+TA_LEN+GAA_LEN+BLZ_LEN+KTONR_LEN+BETRAG_LEN)

#define	SDI_TYP_LEN			(1)
#define	SDI_DATUM_LEN		(6)
#define	SDI_ZEIT_LEN		(6)
#define	SDI_TAN_LEN			(4)
#define	SDI_GA_LEN			(6)
#define	SDI_BLZ_LEN			(8)
#define	SDI_KONTO_LEN		(16)
#define	SDI_BETRAG_LEN		(8)
#define	SDI_WAEHRUNG_LEN	(3)

#define	SDI_GESAMT_LEN (SDI_TYP_LEN+SDI_DATUM_LEN+SDI_ZEIT_LEN+SDI_TAN_LEN+SDI_GA_LEN+SDI_BLZ_LEN+SDI_KONTO_LEN+SDI_BETRAG_LEN+SDI_WAEHRUNG_LEN)		
///////////////////////////////////////////////////////////////////////////////////////
CIPCFields::CIPCFields()
{
}
///////////////////////////////////////////////////////////////////////////////////////
CIPCFields::CIPCFields(const CIPCFields& src)
{
	Lock();
	DeleteAll();
	for (int i=0;i<src.GetSize(); i++)
	{
		CIPCField* pField = src.GetAtFast(i);
		if (pField)
		{
			Add(new CIPCField(*pField));
		}
	}
	Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////
const CIPCFields& CIPCFields::operator = (const CIPCFields& src)
{
	Lock();
	DeleteAll();
	for (int i=0;i<src.GetSize(); i++)
	{
		CIPCField* pField = src.GetAtFast(i);
		if (pField)
		{
			Add(new CIPCField(*pField));
		}
	}
	Unlock();
	return *this;
}
///////////////////////////////////////////////////////////////////////////////////////
CIPCFields::~CIPCFields()
{
	SafeDeleteAll();
}
///////////////////////////////////////////////////////////////////////////////////////
void CIPCFields::FromArray(int iNumRecords,const CIPCField data[])
{
	Lock();
	DeleteAll();
	for (int i=0;i<iNumRecords;i++)
	{
		Add(new CIPCField(data[i]));
	}
	Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////
void CIPCFields::FromString(const CString& sGAorSerialData)
{
	SafeDeleteAll();
	if (!sGAorSerialData.IsEmpty())
	{
		if (sGAorSerialData.GetLength()<=GDC_GESAMT_LEN) 
		{
			FromGeld(sGAorSerialData);
		}
		else if (sGAorSerialData.GetLength()>=SDI_GESAMT_LEN) 
		{
			FromSerial(sGAorSerialData);
		}
		else 
		{
			WK_TRACE_ERROR(_T("cannot decide data format GeldDaten or Serial '%s'\n"),
				LPCTSTR(sGAorSerialData));
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
void CIPCFields::FromGeld(const CString& sData)
{
	// NOT YET perform any verifications on the data itself

	if (sData.GetLength()>GDC_GESAMT_LEN) 
	{
		WK_TRACE_ERROR(_T("Wrong GeldDaten string '%s'\n"), LPCTSTR(sData));
	}

	int nAnfang		=	0;
	CIPCField* pField = NULL;

	CString sDatum	=	sData.Left(DATUM_LEN);
	nAnfang			+=	DATUM_LEN;

	sDatum.TrimLeft();
	if (!sDatum.IsEmpty())
	{
		CString sDateTmp;
		CString sYear;
		int nYear;
		sYear = sDatum.Left(2);
		nYear = _ttoi(sYear);
		if (nYear<100)
		{
			if (nYear<70)
			{
				nYear += 2000;
			}
			else
			{
				nYear += 1900;
			}
			sYear.Format(_T("%d"),nYear);
		}
		
		CString sDate;
		
		sDate = sYear + sDatum.Mid(2,2) + sDatum.Mid(4,2);
		pField = new CIPCField(CIPCField::m_sfDate,sDate,8,'D');
		if (!pField->IsEmpty())
		{
			SafeAdd(pField);
		}
		else
		{
			WK_DELETE(pField);
		}
	}

	// GA Zeit
	CString sZeit	=	sData.Mid(nAnfang, ZEIT_LEN);
	nAnfang			+=	ZEIT_LEN;
	sZeit.TrimLeft();
	if (!sZeit.IsEmpty())
	{
		CString sHour,sMinute,sSecond;
		CString sTime;
		sHour   = sZeit.Left(2);
		sMinute = sZeit.Mid(2,2);
		sSecond = sZeit.Mid(4,2);
		if (sSecond.IsEmpty())
		{
			sSecond = _T("00");
		}
		sTime += sHour + sMinute + sSecond;

		pField = new CIPCField(CIPCField::m_sfTime,sTime,6,'T');
		if (!pField->IsEmpty())
		{
			SafeAdd(pField);
		}
		else
		{
			WK_DELETE(pField);
		}
	}

	CString sTransaktion	=	sData.Mid(nAnfang, TA_LEN);
	nAnfang			+=	TA_LEN;
	pField = new CIPCField(CIPCField::m_sfTaNr,sTransaktion,TA_LEN,'N');
	if (!pField->IsEmpty())
	{
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}

	CString sAutomat=	sData.Mid(nAnfang, GAA_LEN);
	nAnfang			+=	GAA_LEN;
	pField = new CIPCField(CIPCField::m_sfGaNr,sAutomat,GAA_LEN,'N');
	if (!pField->IsEmpty())
	{
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}

	CString sBankleitzahl	=	sData.Mid(nAnfang, BLZ_LEN);
	nAnfang			+=	BLZ_LEN;
	pField = new CIPCField(CIPCField::m_sfBcNr,sBankleitzahl,BLZ_LEN,'N');
	if (!pField->IsEmpty())
	{
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}

	CString sKontonummer=	sData.Mid(nAnfang, KTONR_LEN);
	nAnfang			+=	KTONR_LEN;
	pField = new CIPCField(CIPCField::m_sfKtNr,sKontonummer,KTONR_LEN,'N');
	if (!pField->IsEmpty())
	{
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}

	CString sBetrag	=	sData.Mid(nAnfang, BETRAG_LEN);
	nAnfang			+=	BETRAG_LEN;
	pField = new CIPCField(CIPCField::m_sfAmnt,sBetrag,BETRAG_LEN,'N');
	if (!pField->IsEmpty())
	{
		SafeAdd(pField);
		// only if there is an amount
		CString sWaehrung=	_T("DEM");
		pField = new CIPCField(CIPCField::m_sfCurr,sWaehrung,SDI_WAEHRUNG_LEN,'C');
		if (!pField->IsEmpty())
		{
			SafeAdd(pField);
		}
		else
		{
			WK_DELETE(pField);
		}
	}
	else
	{
		WK_DELETE(pField);
	}

}
///////////////////////////////////////////////////////////////////////////////////////
void CIPCFields::FromSerial(const CString& sData)
{
	// NOT YET perform any verifications on the data itself

	if (sData.GetLength()!=SDI_GESAMT_LEN) 
	{
		WK_TRACE_ERROR(_T("Wrong SDIDaten string '%s'\n"),LPCTSTR(sData));
	}

	int nAnfang		=	1;
	CIPCField* pField = NULL;

	CString sDatum	=	sData.Mid(nAnfang, SDI_DATUM_LEN);
	nAnfang			+=	SDI_DATUM_LEN;

	sDatum.TrimLeft();

	if (!sDatum.IsEmpty())
	{
		CString sYear;
		int nYear;
		sYear = sDatum.Left(2);
		nYear = _ttoi(sYear);
		if (nYear<100)
		{
			if (nYear<70)
			{
				nYear += 2000;
			}
			else
			{
				nYear += 1900;
			}
			sYear.Format(_T("%d"),nYear);
		}
		
		CString sDate;
		
		sDate = sYear + sDatum.Mid(2,2) + sDatum.Mid(4,2);
		pField = new CIPCField(CIPCField::m_sfDate,sDate,8,'D');
		if (!pField->IsEmpty())
		{
			SafeAdd(pField);
		}
		else
		{
			WK_DELETE(pField);
		}
	}

	CString sZeit	=	sData.Mid(nAnfang, SDI_ZEIT_LEN);
	nAnfang			+=	SDI_ZEIT_LEN;
	sZeit.TrimLeft();
	if (!sZeit.IsEmpty())
	{
		CString sTime;
		sTime += sZeit.Left(2) + sZeit.Mid(2,2) + sZeit.Mid(4,2);
		pField = new CIPCField(CIPCField::m_sfTime,sTime,8,'T');
		if (!pField->IsEmpty())
		{
			SafeAdd(pField);
		}
		else
		{
			WK_DELETE(pField);
		}
	}
	
	CString sTransaktion = sData.Mid(nAnfang, SDI_TAN_LEN);
	nAnfang	+=	SDI_TAN_LEN;
	pField = new CIPCField(CIPCField::m_sfTaNr,sTransaktion,SDI_TAN_LEN,'N');
	if (!pField->IsEmpty())
	{
		pField->LeadingZeros();
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}
	
	CString sAutomat=	sData.Mid(nAnfang, SDI_GA_LEN);
	nAnfang			+=	SDI_GA_LEN;
	pField = new CIPCField(CIPCField::m_sfGaNr,sAutomat,SDI_GA_LEN,'N');
	if (!pField->IsEmpty())
	{
		pField->LeadingZeros();
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}

	CString sBankleitzahl	=	sData.Mid(nAnfang, SDI_BLZ_LEN);
	nAnfang			+=	SDI_BLZ_LEN;
	pField = new CIPCField(CIPCField::m_sfBcNr,sBankleitzahl,SDI_BLZ_LEN,'N');
	if (!pField->IsEmpty())
	{
		pField->LeadingZeros();
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}
	
	CString  sKontonummer	=	sData.Mid(nAnfang, SDI_KONTO_LEN);
	nAnfang			+=	SDI_KONTO_LEN;
	pField = new CIPCField(CIPCField::m_sfKtNr,sKontonummer,SDI_KONTO_LEN,'N');
	if (!pField->IsEmpty())
	{
		pField->LeadingZeros();
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}
	
	CString sBetrag	=	sData.Mid(nAnfang, SDI_BETRAG_LEN);
	nAnfang			+=	SDI_BETRAG_LEN;
	pField = new CIPCField(CIPCField::m_sfAmnt,sBetrag,SDI_BETRAG_LEN,'N');
	if (!pField->IsEmpty())
	{
		pField->LeadingZeros();
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}
	
	CString sWaehrung=	sData.Mid(nAnfang, SDI_WAEHRUNG_LEN);
	pField = new CIPCField(CIPCField::m_sfCurr,sWaehrung,SDI_WAEHRUNG_LEN,'C');
	if (!pField->IsEmpty())
	{
		pField->Trim();
		SafeAdd(pField);
	}
	else
	{
		WK_DELETE(pField);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
DWORD CIPCFields::GetBubbleLength(WORD wCodePage) const
#else
DWORD CIPCFields::GetBubbleLength() const
#endif								 
{
	// 2 max len
	// 2 name len
	// name
	// 2 value len
	// value
	// 1 type
	CIPCFields* pThis = (CIPCFields*)this;
	pThis->Lock();

	int i;
	DWORD dwSum = 4;
#ifdef _UNICODE
	BYTE* pByte = NULL;
	// calc size of bubble
	for (i=0;i<GetSize();i++) 
	{
		dwSum += sizeof(WORD) // max len
				+sizeof(WORD) // name len
				+CIPCExtraMemory::CString2Memory(pByte, GetAtFast(i)->GetName() , wCodePage)
				+sizeof(WORD) // value len
				+CIPCExtraMemory::CString2Memory(pByte, GetAtFast(i)->GetValue(), wCodePage)
				+sizeof(BYTE);  // typ
	}
#else
	// calc size of bubble
	for (i=0;i<GetSize();i++) 
	{
		dwSum += sizeof(WORD) // max len
				+sizeof(WORD) // name len
				+GetAtFast(i)->GetName().GetLength()+
				+sizeof(WORD) // value len
				+GetAtFast(i)->GetValue().GetLength()+
				+sizeof(BYTE);  // typ
	}
#endif
	pThis->Unlock();
	return dwSum;
}
///////////////////////////////////////////////
DWORD CIPCFields::ToMemory(BYTE*& pByte
#ifdef _UNICODE
								 , WORD wCodePage
#endif								 
									) const
{
	CIPCFields* pThis = (CIPCFields*)this;
	pThis->Lock();
	BYTE *pStart = pByte;
//	BYTE *pbLen;
//	WORD  wLen;
#ifdef _UNICODE
	DWORD_TO_MEMORY(pByte, GetSize());

	for (int i=0;i<GetSize();i++) 
	{
		WORD_TO_MEMORY(pByte, GetAtFast(i)->GetMaxLength());

		CIPCExtraMemory::CString2Memory(pByte, GetAtFast(i)->GetName() , wCodePage);
		CIPCExtraMemory::CString2Memory(pByte, GetAtFast(i)->GetValue(), wCodePage);
		*pByte++ = GetAtFast(i)->GetType();
	}
#else
	DWORD dw;
	WORD w;
	dw = (DWORD)GetSize();
	*pByte++ = HIBYTE(HIWORD(dw));
	*pByte++ = LOBYTE(HIWORD(dw));
	*pByte++ = HIBYTE(LOWORD(dw));
	*pByte++ = LOBYTE(LOWORD(dw));

	for (int i=0;i<GetSize();i++) 
	{
		w = GetAtFast(i)->GetMaxLength();
		*pByte++ = HIBYTE(w);
		*pByte++ = LOBYTE(w);
		w = (WORD)GetAtFast(i)->GetName().GetLength();
		*pByte++ = HIBYTE(w);
		*pByte++ = LOBYTE(w);
		CopyMemory(pByte, LPCTSTR(GetAtFast(i)->GetName()), w);
		pByte += w;
		w = (WORD)GetAtFast(i)->GetValue().GetLength();
		*pByte++ = HIBYTE(w);
		*pByte++ = LOBYTE(w);
		CopyMemory(pByte,LPCTSTR(GetAtFast(i)->GetValue()),w);
		pByte += w;
		*pByte++ = GetAtFast(i)->GetType();
	}
#endif

	DWORD dwDelta=pByte-pStart;
	pThis->Unlock();
	return dwDelta;
}
///////////////////////////////////////////////////////////////////////////////////////
CIPCExtraMemory* CIPCFields::BubbleFromFields(CIPC *pCipc) const
{
#ifdef _UNICODE
	DWORD dwSum = GetBubbleLength(pCipc->GetCodePage());
#else
	DWORD dwSum = GetBubbleLength();
#endif
	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(pCipc,dwSum))
	{
		BYTE *pByte = (BYTE*) pBubble->GetAddressForWrite();
#ifdef _UNICODE
		DWORD dwDelta = ToMemory(pByte, pCipc->GetCodePage());
#else
		DWORD dwDelta = ToMemory(pByte);
#endif
		WK_ASSERT(dwDelta==dwSum);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in BubbleFromFields\n"));
	}
	return pBubble;
}
///////////////////////////////////////////////////////////////////////////////////////
CIPCField* CIPCFields::FromMemory(DWORD dwMaxRead,
								  const BYTE*& pByte,
								  int& iNumRecords
#ifdef _UNICODE
								 , WORD wCodePage
#endif								 
								  )
{
	// 2 max len
	// 2 name len
	// name
	// 2 value len
	// value
	// 1 type
	WORD w = 0;
	DWORD dw = 0;

	MEMORY_TO_DWORD(pByte, dw);
	iNumRecords = dw;

	if (iNumRecords==0)
	{
		return NULL;
	}

#ifdef _UNICODE
	CIPCField *pData = new CIPCField[dw];
	CWK_String sTemp;
	// loop over records
	for (DWORD i=0;i<dw;i++) 
	{
		CIPCField & field = pData[i];
		MEMORY_TO_WORD(pByte, w);
		field.SetMaxLength(w);

		MEMORY_TO_WORD(pByte, w);
		if (w>0)
		{
			sTemp.InitFromMemory(wCodePage == CODEPAGE_UNICODE, (void*)pByte, w, wCodePage);
//			TRACE(_T("FromMemory:%s, "), sTemp);
			pByte += w;
		}
		field.SetName(sTemp);

		MEMORY_TO_WORD(pByte, w);
		sTemp.Empty();
		if (w>0)
		{
			sTemp.InitFromMemory(wCodePage == CODEPAGE_UNICODE, (void*)pByte, w, wCodePage);
//			TRACE(_T("%s\n"), sTemp);
			pByte += w;
		}
		field.SetValue(sTemp);

		field.SetType(*pByte++);
#else
	CIPCField *pData = new CIPCField[dw];
	// loop over records
	for (DWORD i=0;i<dw;i++) 
	{
		CIPCField & field = pData[i];
		w = (WORD)(((WORD)pByte[0])<<8 | pByte[1]);
		pByte += 2;
		field.SetMaxLength(w);
		w = (WORD)(((WORD)pByte[0])<<8 | pByte[1]);
		pByte += 2;
		CString sTemp;
		if (w>0)
		{
			_TCHAR* pChar = sTemp.GetBuffer(w);
			CopyMemory(pChar,pByte,w);
			sTemp.ReleaseBuffer();
			pByte += w;
		}
		field.SetName(sTemp);
		w = (WORD)(((WORD)pByte[0])<<8 | pByte[1]);
		pByte += 2;
		sTemp.Empty();
		if (w>0)
		{
			_TCHAR* pChar = sTemp.GetBuffer(w);
			CopyMemory(pChar,pByte,w);
			sTemp.ReleaseBuffer();
			pByte += w;
		}
		field.SetValue(sTemp);
		field.SetType(*pByte++);
#endif
	}
	return pData;
}
///////////////////////////////////////////////////////////////////////////////////////
CIPCField* CIPCFields::FieldsFromBubble(int& iNumRecords,const CIPCExtraMemory *pExtraMem)
{
	const BYTE *pByte = (const BYTE*)pExtraMem->GetAddress();
#ifdef _UNICODE
	ASSERT(pExtraMem->GetCIPC() != NULL);
	return FromMemory(pExtraMem->GetLength(), pByte, iNumRecords,pExtraMem->GetCIPC()->GetCodePage());
#else
	return FromMemory(pExtraMem->GetLength(), pByte,iNumRecords);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
//CIPCField* CIPCFields::GetCIPCField(const CString& sName) const
CIPCField* CIPCFields::GetCIPCField(const _TCHAR sName[]) const
{
#ifdef USE_PERFORMANCE_TIMER
	LARGE_INTEGER liOld, liTick;
	liOld = theTimer.GetMicroTicks();
#endif
	CIPCFields* pThis = (CIPCFields*)this;
	pThis->Lock();

	CIPCField* pField = NULL, *pTemp;
    int i=0;
	for (i=0; i<GetSize(); i++)
	{
		pTemp = GetAtFast(i);
		if (pTemp->GetName() == sName)
		{
			pField = pTemp;
			break;
		}
	}

#ifdef USE_PERFORMANCE_TIMER
	liTick = theTimer.GetMicroTicks();
	if (pField)
	{
		TRACE(_T("Field %s (%s) found at pos %d in %d µs\n"), sName, pField->GetName(), i+1, (int)(liTick.QuadPart - liOld.QuadPart));
	}
#endif
	pThis->Unlock();
	return pField;
}
///////////////////////////////////////////////////////////////////////////////////////
void CIPCFields::AddNoDuplicates(const CIPCField& field)
{
	CIPCField* pField = NULL;
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		pField = GetAtFast(i);
		if (pField->GetName() == field.GetName())
		{
			*pField = field;
			Unlock();
			return;
		}
	}
	Unlock();
	SafeAdd(new CIPCField(field));
}
///////////////////////////////////////////////////////////////////////////////////////
void CIPCFields::AddNoDuplicates(const CIPCFields& fields)
{
	Lock();
	for (int i=0;i<fields.GetSize();i++)
	{
		CIPCField* pField = fields.GetAtFast(i);
		if (pField)
		{
			AddNoDuplicates(*pField);
		}
	}
	Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////
CString CIPCFields::ToSerial() const
{
	CString r;
	CString sDate,sTime;
	CIPCField* pField;

	r = _T("1");
	// Datum
	pField = GetCIPCField(CIPCField::m_sfDate);
	if (pField)
	{
		sDate = pField->GetValue(); // YYYYMMDD -> YYMMDD
		if (sDate.GetLength()==8)
		{
			r += sDate.Mid(2);
		}
		else
		{
			r += CString(' ',SDI_DATUM_LEN);
		}
	}
	else
	{
		r += CString(' ',SDI_DATUM_LEN);
	}
	// Zeit
	pField = GetCIPCField(CIPCField::m_sfTime);
	if (pField)
	{
		sTime = pField->GetValue();
		if (sTime.GetLength()==6)
		{
			r += sTime;
		}
		else
		{
			r += CString(' ',SDI_DATUM_LEN);
		}
	}
	else
	{
		r += CString(' ',SDI_ZEIT_LEN);
	}
	
	// Transaktion
	pField = GetCIPCField(CIPCField::m_sfTaNr);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->GetValue(),SDI_TAN_LEN);
	}
	else
	{
		r += CString(' ',SDI_TAN_LEN);
	}
	
	// Ga Nr
	pField = GetCIPCField(CIPCField::m_sfGaNr);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->GetValue(),SDI_GA_LEN);
	}
	else
	{
		r += CString(' ',SDI_GA_LEN);
	}

	// Bc Nr
	pField = GetCIPCField(CIPCField::m_sfBcNr);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->GetValue(),SDI_BLZ_LEN);
	}
	else
	{
		r += CString(' ',SDI_BLZ_LEN);
	}

	// Kt Nr
	pField = GetCIPCField(CIPCField::m_sfKtNr);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->GetValue(),SDI_KONTO_LEN);
	}
	else
	{
		r += CString(' ',SDI_KONTO_LEN);
	}
	
	// Amnt
	pField = GetCIPCField(CIPCField::m_sfAmnt);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->GetValue(),SDI_BETRAG_LEN);
	}
	else
	{
		r += CString(' ',SDI_BETRAG_LEN);
	}
	
	// Curr
	pField = GetCIPCField(CIPCField::m_sfCurr);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->GetValue(),SDI_WAEHRUNG_LEN);
	}
	else
	{
		r += CString(' ',SDI_WAEHRUNG_LEN);
	}
	return r;
}
///////////////////////////////////////////////////////////////////////////////////////
CString CIPCFields::ToGeld() const
{
	CString r;
	CString sDate,sTime;
	CIPCField* pField;

	// Datum
	pField = GetCIPCField(CIPCField::m_sfDate);
	if (pField)
	{
		sDate = pField->GetValue(); // YYYYMMDD -> YYMMDD
		if (sDate.GetLength()==8)
		{
			r += sDate.Mid(2);
		}
		else
		{
			r += CString(' ',DATUM_LEN);
		}
	}
	else
	{
		r += CString(' ',DATUM_LEN);
	}
	// Zeit
	pField = GetCIPCField(CIPCField::m_sfTime);
	if (pField)
	{
		sTime = pField->GetValue();
		if (sTime.GetLength()==6)
		{
			r += sTime.Left(4); // no seconds
		}
		else
		{
			r += CString(' ',ZEIT_LEN);
		}
	}
	else
	{
		r += CString(' ',ZEIT_LEN);
	}
	
	// Transaktion
	pField = GetCIPCField(CIPCField::m_sfTaNr);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->ToString(),TA_LEN);
	}
	else
	{
		r += CString(' ',TA_LEN);
	}
	
	// Ga Nr
	pField = GetCIPCField(CIPCField::m_sfGaNr);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->ToString(),GAA_LEN);
	}
	else
	{
		r += CString(' ',GAA_LEN);
	}

	// Bc Nr
	pField = GetCIPCField(CIPCField::m_sfBcNr);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->ToString(),BLZ_LEN);
	}
	else
	{
		r += CString(' ',BLZ_LEN);
	}

	// Kt Nr
	pField = GetCIPCField(CIPCField::m_sfKtNr);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->ToString(),KTONR_LEN);
	}
	else
	{
		r += CString(' ',KTONR_LEN);
	}
	
	// Amnt
	pField = GetCIPCField(CIPCField::m_sfAmnt);
	if (pField)
	{
		r += CIPCField::FillLeadingZeros(pField->ToString(),BETRAG_LEN);
	}
	else
	{
		r += CString(' ',BETRAG_LEN);
	}
	return r;
}
///////////////////////////////////////////////////////////////////////////////////////
void CIPCFields::Load(const CString& sSection, CWK_Profile& wkp)
{
	CString t;
	CIPCField* pField;

	Lock();
	int i,c = wkp.GetInt(sSection, _T("NrOfFields"), 0);
	for (i=0;i<c;i++)
	{
		pField = new CIPCField();
		if (pField->Load(sSection,i,wkp))
		{
			if (GetCIPCField(pField->GetName()) == NULL)
			{
				Add(pField);
				pField = NULL;
			}
			else
			{
				WK_TRACE_ERROR(_T("Field %s is already in list (Section: %s)\n"), pField->GetName(), sSection);
			}
		}
		WK_DELETE(pField);
	}
	Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////
void CIPCFields::Save(const CString& sSection, CWK_Profile& wkp)
{
	CIPCField* pField;

	int i; 
	Lock();
	wkp.WriteInt(sSection, _T("NrOfFields"), GetSize());
	for (i=0;i<GetSize();i++)
	{
		pField = GetAtFast(i);
		pField->Save(sSection,i,wkp);
	}
	
	// if the list is smaller than before, delete the values after list size
	CString str;
	for (;i<10000;i++)
	{
		str.Format(_T("%d"), i);
		if (!wkp.DeleteEntry(sSection, str))
		{
			break;
		}
	}

	Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////
CString CIPCFields::BuildOldQueryString() const
{
	CString r,o;
	CString s,v;
	int i;
	CIPCFields* pThis = (CIPCFields*)this;
	pThis->Lock();

	for (i=0;i<GetSize();i++)
	{
		o = _T("=");
		if (!r.IsEmpty())
		{
			r += _T(" .AND. ");
		}
		s = CIPCField::Translate4xTo3x(GetAtFast(i)->GetName());
		if (   (s == _T("DTP_ZEIT")) 
			|| (s == _T("ZEIT"))
			)
		{
			// convert time to old format
			// HHMMSS -> ('HH:MM:SS')
			CString t = GetAtFast(i)->GetValue(); 
			if (t.GetLength()==6)
			{
				// assume a correct time
				v = '\'' + t.Mid(0,2) + ':' + t.Mid(2,2) + ':' + t.Mid(4,2) + '\'';
			}
			else
			{
				WK_TRACE_ERROR(_T("time format %s\n"),t);
			}
		}
		else if (s == _T("DATUM"))
		{
			// convert date to old format
			CString t = GetAtFast(i)->GetValue(); 
			// CCYYMMDD to 'YY.MM.DD'
			if (t.GetLength()==8)
			{
				// assume a correct date
				v = '\'' + t.Mid(2,2) + '.' + t.Mid(4,2) + '.' + t.Mid(6,2) + '\'';
			}
			else
			{
				WK_TRACE_ERROR(_T("date format %s\n"),t);
			}
		}
		else if ((s==_T("TA_NR")) || (s==_T("BLZ")) || (s==_T("BETRAG")))
		{
			v = GetAtFast(i)->ToString();
		}
		else
		{
			v = '\'' + GetAtFast(i)->ToString() + '\'';
		}
		if ((GetAtFast(i)->GetType() == '<') ||	(GetAtFast(i)->GetType() == '>'))
		{
			o = GetAtFast(i)->GetType();
		}
		else if (GetAtFast(i)->GetType() == '{')
		{
			o = _T("<=");
		}
		else if (GetAtFast(i)->GetType() == '}')
		{
			o = _T(">=");
		}
		r += _T("(") + s + o + v + _T(")");
	}

	pThis->Unlock();
	return r;
}
///////////////////////////////////////////////////////////////////////////////////////
CString CIPCFields::ToString() const
{
	CString s; 
	CIPCField* pField;
	CIPCFields* pThis = (CIPCFields*)this;
	pThis->Lock();

	for (int i=0;i<GetSize();i++)
	{
		pField = GetAtFast(i);
		if (!s.IsEmpty())
		{
			s += _T(", ");
		}
		s += pField->GetName() + _T("=") + pField->GetValue(); 
	}
	pThis->Unlock();
	return s;
}


