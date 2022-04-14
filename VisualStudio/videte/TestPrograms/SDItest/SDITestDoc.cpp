/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDITestDoc.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/SDITestDoc.cpp $
// CHECKIN:		$Date: 6.01.06 13:16 $
// VERSION:		$Revision: 39 $
// LAST CHANGE:	$Modtime: 5.01.06 14:10 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"

#include "ComDlg.h"
#include "IPDlg.h"

#include "SDITestDoc.h"
#include "SDITestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDITestDoc
IMPLEMENT_DYNCREATE(CSDITestDoc, CDocument)
/////////////////////////////////////////////////////////////////////////////
// CSDITestDoc construction/destruction
CSDITestDoc::CSDITestDoc()
{
	m_eType = SDICT_UNKNOWN;
	m_pCom = NULL;
	m_bTraceRS232DataHex = FALSE;
	m_bTraceRS232DataAscii = TRUE;
	m_bTraceRS232Events = FALSE;
	m_uSerializeVersion = 0;
	m_sDirectory = _T("C:\\LOG");
	
	m_iCom = 0;
	m_iIPPort = 0;
	//m_sIPAddress;
	m_iIPType = SOCK_DGRAM;
	m_Socket.SetDocument(this);

	LoadDebugConfiguration();
	InitFirstAndLastNameArray();

//	for (int i=0 ; i<1000 ; i++) {
//		WK_TRACE( "%i\n", GetRandomDigitInt(0, 9) );
//	}

}
/////////////////////////////////////////////////////////////////////////////
CSDITestDoc::~CSDITestDoc()
{
	WK_DELETE(m_pCom);
	m_Socket.Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::LoadDebugConfiguration() 
{
	// Einlesen der cfg-Datei
	CWKDebugOptions debugOptions;
	// check for C:\SDIUnit.cfg first, then in current directory
	BOOL bFoundFile = debugOptions.ReadFromFile(_T("c:\\SDITest.cfg"));
	if (bFoundFile==FALSE) {
		bFoundFile = debugOptions.ReadFromFile(_T("SDITest.cfg"));
	}

	// Setzen der Variablen
	m_bTraceRS232DataHex = debugOptions.GetValue(_T("TraceRS232DataHex"),m_bTraceRS232DataHex);
	m_bTraceRS232DataAscii = debugOptions.GetValue(_T("TraceRS232DataAscii"),m_bTraceRS232DataAscii);
	m_bTraceRS232Events = debugOptions.GetValue(_T("TraceRS232Events"),m_bTraceRS232Events);

	// Im Debug-Modus einige Werte standartmaessig TRUE setzen
#ifdef _DEBUG
	WK_TRACE(_T("===> Debug-Version <===\n"));
#endif

	WK_TRACE(_T("TraceRS232DataHex=%d\n"),m_bTraceRS232DataHex);
	WK_TRACE(_T("TraceRS232DataAscii=%d\n"),m_bTraceRS232DataAscii);
	WK_TRACE(_T("TraceRS232Events=%d\n"),m_bTraceRS232Events);

	Sleep(100);
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::InitFirstAndLastNameArray() 
{
	m_sFirstNameArray.Add(_T("Jo"));
	m_sFirstNameArray.Add(_T("Uwe"));
	m_sFirstNameArray.Add(_T("Jens"));
	m_sFirstNameArray.Add(_T("Klaus"));
	m_sFirstNameArray.Add(_T("Werner"));
	m_sFirstNameArray.Add(_T("Dimitri"));
	m_sFirstNameArray.Add(_T("Thorsten"));
	m_sFirstNameArray.Add(_T("Friedrich"));
	m_sFirstNameArray.Add(_T("Hans-Georg"));
	m_sFirstNameArray.Add(_T("Hans-Jürgen"));
	m_sFirstNameArray.Add(_T("Horst-Dieter"));
	m_sFirstNameArray.Add(_T("Werner-Johann"));
	m_sFirstNameArray.Add(_T("Hans-Sigismund"));
	m_sFirstNameArray.Add(_T("Georg-Ferdinand"));
	m_sFirstNameArray.Add(_T("Johann-Friedrich"));

	m_sLastNameArray.Add(_T("Xi"));
	m_sLastNameArray.Add(_T("Ter"));
	m_sLastNameArray.Add(_T("Maus"));
	m_sLastNameArray.Add(_T("Groth"));
	m_sLastNameArray.Add(_T("Jansen"));
	m_sLastNameArray.Add(_T("Tribich"));
	m_sLastNameArray.Add(_T("Ottensen"));
	m_sLastNameArray.Add(_T("Ueckersen"));
	m_sLastNameArray.Add(_T("Trauthmann"));
	m_sLastNameArray.Add(_T("Wranitschek"));
	m_sLastNameArray.Add(_T("Gross-Kersig"));
	m_sLastNameArray.Add(_T("Feodorowitsch"));
	m_sLastNameArray.Add(_T("Schmitz-Essert"));
	m_sLastNameArray.Add(_T("Graumann-Zeisig"));
	m_sLastNameArray.Add(_T("Hancke-Feddersen"));
}
/////////////////////////////////////////////////////////////////////////////
// CSDITestDoc serialization
void CSDITestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_uSerializeVersion;
		ar << m_iCom;
		ar << m_bRepeat;
		ar << m_uRepeatTime;
		ar << m_iRepeatHow;
		ar << m_bLimited;
		ar << m_uLimited;
		ar << m_bBitError;
		ar << m_bSendSingleBytes;
		ar << m_uSendDelay;
		ar << m_sDate;
		ar << m_sTime;
		ar << m_sBLZ;
		ar << m_sKonto;
		ar << m_sBetrag;
		ar << m_sTAN;
		ar << m_sWSID;
		ar << m_sCurrency;

		ar << m_iIPPort;
		ar << m_iIPType;
		ar << m_sIPAddress;
	}
	else
	{
		// TODO: add loading code here
		ar >> m_uSerializeVersion;
		ar >> m_iCom;
		ar >> m_bRepeat;
		ar >> m_uRepeatTime;
		ar >> m_iRepeatHow;
		ar >> m_bLimited;
		ar >> m_uLimited;
		ar >> m_bBitError;
		ar >> m_bSendSingleBytes;
		ar >> m_uSendDelay;
		ar >> m_sDate;
		ar >> m_sTime;
		ar >> m_sBLZ;
		ar >> m_sKonto;
		ar >> m_sBetrag;
		ar >> m_sTAN;
		ar >> m_sWSID;
		ar >> m_sCurrency;

		ar >> m_iIPPort;
		ar >> m_iIPType;
		ar >> m_sIPAddress;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CSDITestDoc diagnostics
#ifdef _DEBUG
void CSDITestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSDITestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::DeleteContents() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bRepeat = FALSE;
	m_uRepeatTime = 1000;
	m_iRepeatHow = 0;
	m_bLimited = FALSE;
	m_uLimited = 2;
	m_bBitError = FALSE;
	m_bSendSingleBytes = FALSE;
	m_uSendDelay = 6;
	m_sDate.Empty();
	m_sTime.Empty();
	m_sBLZ.Empty();
	m_sKonto.Empty();
	m_sBetrag.Empty();
	m_sTAN.Empty();
	m_sWSID.Empty();
	m_sCurrency.Empty();
	
	WK_DELETE(m_pCom);
	m_iCom = 0;
	m_bStarted = FALSE;
	m_uCounter = 0;
	m_iTAN = 0;
	m_uRandom = 0;
	m_byaData.RemoveAll();

	m_iIPPort = 0;
	m_sIPAddress.Empty();
	m_iIPType = 0;
	m_Socket.Close();

	CDocument::DeleteContents();
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::ConfigCom() 
{
	CString sTitle, sFormat;
	if (m_iCom) 
	{
		WK_DELETE(m_pCom);
		m_bTraceRS232DataHex = FALSE;
		m_bTraceRS232DataAscii = TRUE;
		m_bTraceRS232Events = FALSE;
		m_pCom = new CSDIRS232(m_iCom,
							   m_bTraceRS232DataAscii,
							   m_bTraceRS232DataHex,
							   m_bTraceRS232Events);
		sTitle = GetTitle();
		if ( -1 != sTitle.Find(_T(" - Com")) ) {
			sTitle = sTitle.Left( sTitle.Find(_T(" - Com")) );
		}
		if (m_pCom && OpenCom() ) {
			sFormat.Format(_T(" - Com %i"), m_iCom);
			sTitle = sTitle + sFormat;
		}
		else {
			m_iCom = 0;
		}
	}
	else if (m_iIPPort>0)
	{
		m_Socket.Close();
		CString sType;
		if (m_iIPType == SOCK_DGRAM)
		{
			sType = _T("UDP");
		}
		else if (m_iIPType == SOCK_STREAM)
		{
			sType = _T("TCP");
		}
		sFormat.Format(_T("%s %d %s"),sType,m_iIPPort,m_sIPAddress);
		if (m_Socket.Create(0,m_iIPType,FD_CONNECT|FD_WRITE))
		{
			WK_TRACE(_T("socket %s created\n"),sFormat);
			sTitle = sTitle + sFormat;
		}
		else
		{
			int e = WSAGetLastError();
			CString s = GetLastErrorString(e);
			WK_TRACE(_T("cannot create socket %s %s\n"),sFormat,s);
		}
	}
	SetTitle(sTitle);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::OpenCom() 
{
	// MUSS von den abgeleiteten Klassen ueberschrieben werden!!!
	BOOL bRet = FALSE;
	if (m_iCom>0)
	{
		WK_TRACE_ERROR(_T("CSDITestDoc::OpenCom() NOT OVERWRITTEN \n"));
	}
	else if (m_iIPPort>0)
	{
		if (m_iIPType == SOCK_STREAM)
		{
			bRet = m_Socket.Connect(m_sIPAddress,m_iIPPort);
			if (bRet)
			{
				WK_TRACE(_T("connected to %s\n"),m_sIPAddress);
			}
			else
			{
				int e = WSAGetLastError();
				CString s = GetLastErrorString(e);
				WK_TRACE(_T("cannot connect to %s %s\n"),m_sIPAddress,s);
			}
		}
		else if (m_iIPType == SOCK_DGRAM)
		{
			bRet = TRUE;
		}
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::CreateData() 
{
	// MUSS von den abgeleiteten Klassen ueberschrieben werden!!!

	WK_TRACE_ERROR(_T("CSDITestDoc::CreateData() NOT OVERWRITTEN \n"));
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::GenerateBitError()
{
	// get a random byte of the array
	int iByte = GetRandomNumberBetweenAsInt(0, m_byaData.GetUpperBound());
	BYTE byRandom = m_byaData.GetAt(iByte);
	// invert a single bit in the byte
	BYTE byBit = (BYTE)GetRandomNumberBetweenAsInt(0, 7);
	byRandom = (BYTE)(byRandom ^ (1<<byBit));
	// set the byte in the array
	m_byaData.SetAt(iByte, byRandom);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::IsComOK()
{
	if (m_iCom>0)
	{
		return (m_pCom && m_pCom->IsOpen() );
	}
	else if (m_iIPPort>0)
	{
		return m_Socket.m_hSocket != INVALID_SOCKET;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CSDITestDoc::GetCOM()
{
	return m_iCom;
}
/////////////////////////////////////////////////////////////////////////////
int	CSDITestDoc::GetPort()
{
	return m_iIPPort;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetAddress()
{
	return m_sIPAddress;
}
/////////////////////////////////////////////////////////////////////////////
UINT CSDITestDoc::GetIPType()
{
	return m_iIPType;
}
/////////////////////////////////////////////////////////////////////////////
SDIControlType CSDITestDoc::GetType()
{
	return (m_eType);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::IsDataOK()
{
	BOOL bReturn = FALSE;

	if ( FALSE == IsDateOK() ) {
		bReturn = FALSE;
	}
	else if ( FALSE == IsTimeOK() ) {
		bReturn = FALSE;
	}
	else if ( FALSE == IsBlzOK() ) {
		bReturn = FALSE;
	}
	else if ( FALSE == IsKontoOK() ) {
		bReturn = FALSE;
	}
	else {
		bReturn = TRUE;
	}

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::IsDateOK()
{
	BOOL bReturn = TRUE;

	if ( m_sDate.IsEmpty() ) {
		// brauchen nichts zu pruefen
	}
	else {
		// Das Datum ist immer 8-stellig
		BOOL bReturn = FALSE;
		int iDigits = 8;
		if (m_sDate.GetLength() == iDigits) {
#if 1
			bReturn = TRUE;
#elif
			int iNumber = 0;
			CString sNumber;
			sNumber = m_sDate.GetLeft(2);
			iNumber = atoi(sNumber);
			if (1 <= iNumber <= 31) {
				sNumber = m_sDate.GetMid(2, 2);
				iNumber = atoi(sNumber);
				if (1 <= iNumber <= 12) {
					sNumber = m_sDate.GetMid(4, 2);
					iNumber = atoi(sNumber);
					if (0 <= iNumber <= 9999) {
						bReturn = TRUE;
					}
				}
			}
#endif
		}
		if (!bReturn) {
			m_sDate.Empty();
			AfxMessageBox(IDS_INVALID_DATE);
			UpdateAllViews(NULL);
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::IsTimeOK()
{
	BOOL bReturn = TRUE;

	if ( m_sTime.IsEmpty() ) {
		// brauchen nichts zu pruefen
	}
	else {
		// Die Zeit ist immer 6-stellig
		int iDigits = 6;
#if 1
		if (m_sTime.GetLength() < iDigits) {
			// Ggf. auf iDigits-Stellen mit 0 auffuellen
			int iNullen = 0;
			iNullen = iDigits - m_sTime.GetLength();
			CString sNullen('0', iNullen); 
			m_sTime = m_sTime + sNullen;
		}
#elif
		int iTwoDigit = 0;
		CString sTwoDigit;
		sTwoDigit = m_sTime.GetLeft(2);
		iTwoDigit = atoi(sTwoDigit);
		if (0 <= iTwoDigit <= 23) {
			sTwoDigit = m_sTime.GetMid(2, 2);
			iTwoDigit = atoi(sTwoDigit);
			if (0 <= iTwoDigit <= 59) {
				sTwoDigit = m_sTime.GetMid(4, 2);
				iTwoDigit = atoi(sTwoDigit);
				if (0 <= iTwoDigit <= 59) {
					bReturn = TRUE;
				}
			}
		}
		if (!bReturn) {
			m_sTime.Empty();
			UpdateAllViews(NULL);
		}
#endif
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::IsBlzOK()
{
	BOOL bReturn = TRUE;

	if ( m_sBLZ.IsEmpty() ) {
		// brauchen nichts zu pruefen
	}
	else {
		// Die BLZ ist max. 8-stellig
		int iDigits = 8;
		if ( m_sBLZ.GetLength() > iDigits ) {
			m_sBLZ = m_sBLZ.Left(iDigits);
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::IsKontoOK()
{
	BOOL bReturn = TRUE;

	if ( m_sKonto.IsEmpty() ) {
		// brauchen nichts zu pruefen
	}
	else {
		// Das Konto ist maximal 16-stellig
		// GF new version 07.08.2003 maximal 34-stellig
		int iDigits = 34;
		if ( m_sKonto.GetLength() > iDigits ) {
			m_sKonto = m_sKonto.Left(iDigits);
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::Send() 
{
	m_byaData.RemoveAll();
	CreateData();

	if (m_bBitError) 
	{
		GenerateBitError();
	}

//	TRACE("Data:%s\n", m_byaData);
	if (m_pCom)
	{
//		TRACE(_T("Start send data %010lu\n"), GetCurrentTime());
		BYTE byte;
		for (int i=0 ; i<m_byaData.GetSize() ; i++)
		{
			byte = m_byaData.GetAt(i);
			m_pCom->Write( (LPVOID)&byte, 1);
			// give the RS232 a chance to send
			if (m_bSendSingleBytes)
			{
				Sleep(m_uSendDelay);
			}
			else
			{
				Sleep(0);
			}
		}
//		TRACE(_T("End send data %010lu\n"), GetCurrentTime());
	}
	else if (m_Socket.m_hSocket != INVALID_SOCKET)
	{
		m_Socket.AddSendData(m_byaData);
		if (m_iIPType == SOCK_STREAM)
		{
			m_Socket.AsyncSelect(FD_WRITE);
		}
		else if (m_iIPType == SOCK_DGRAM)
		{
			m_Socket.Connect(m_sIPAddress,m_iIPPort);
		}
	}
	else 
	{
		WK_TRACE_ERROR(_T("CSDITestDoc::SendData() NO COM\n"));
	}

	m_bStarted = TRUE;
	if ( m_bRepeat) {
		m_uCounter++;
		if (m_bLimited && m_uCounter >= m_uLimited) {
			OnStopSend();
		}
	}
	else {
		OnStopSend();
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetDate(const CString& sFormat)
{
	CString sDateDDMMYYYY;
	if ( m_sDate.IsEmpty() ) {
		CTime time = CTime::GetCurrentTime();
		sDateDDMMYYYY = time.Format(_T("%d%m%Y"));
	}
	else {
		sDateDDMMYYYY = m_sDate;
	}
	CString sDate = sFormat;
	int i, iIndex;
	if ( (iIndex = sDate.Find(_T("YYYY"))) != -1) {
		for (i=0 ; i<4 ; i++) {
			sDate.SetAt(iIndex+i, sDateDDMMYYYY.GetAt(4+i));
		}
	}
	else if ( (iIndex = sDate.Find(_T("YY"))) != -1) {
		for (i=0 ; i<2 ; i++) {
			sDate.SetAt(iIndex+i, sDateDDMMYYYY.GetAt(6+i));
		}
	}
	if ( (iIndex = sDate.Find(_T("MM"))) != -1) {
		for (i=0 ; i<2 ; i++) {
			sDate.SetAt(iIndex+i, sDateDDMMYYYY.GetAt(2+i));
		}
	}
	if ( (iIndex = sDate.Find(_T("DD"))) != -1) {
		for (i=0 ; i<2 ; i++) {
			sDate.SetAt(iIndex+i, sDateDDMMYYYY.GetAt(0+i));
		}
	}
	return sDate;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetTime(const CString& sFormat)
{
	CString sTimeHHMMSS;
	if ( m_sTime.IsEmpty() ) {
		CTime time = CTime::GetCurrentTime();
		sTimeHHMMSS = time.Format(_T("%H%M%S"));
	}
	else {
		sTimeHHMMSS = m_sTime;
	}
	CString sTime = sFormat;
	int i, iIndex;
	if ( (iIndex = sTime.Find(_T("HH"))) != -1) {
		for (i=0 ; i<2 ; i++) {
			sTime.SetAt(iIndex+i, sTimeHHMMSS.GetAt(0+i));
		}
	}
	if ( (iIndex = sTime.Find(_T("MM"))) != -1) {
		for (i=0 ; i<2 ; i++) {
			sTime.SetAt(iIndex+i, sTimeHHMMSS.GetAt(2+i));
		}
	}
	if ( (iIndex = sTime.Find(_T("SS"))) != -1) {
		for (i=0 ; i<2 ; i++) {
			sTime.SetAt(iIndex+i, sTimeHHMMSS.GetAt(4+i));
		}
	}
	return sTime;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetWSID(int iLen, BOOL bTrimLeadingZeros/*=FALSE*/)
{
	CString sReturn;
	if ( m_sWSID.IsEmpty() ) {
		sReturn = GetRandomNumberWithLen(iLen, bTrimLeadingZeros);
	}
	else {
		sReturn = TrimDataToLength(m_sWSID, iLen, bTrimLeadingZeros);
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetTAN(int iLen, BOOL bIncrease, BOOL bTrimLeadingZeros/*=FALSE*/)
{
	CString sReturn;
	if ( m_sTAN.IsEmpty() ) {
		sReturn = GetInternTAN(iLen, bIncrease, bTrimLeadingZeros);
	}
	else {
		sReturn = TrimDataToLength(m_sTAN, iLen, bTrimLeadingZeros);
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetBLZ(int iLen, BOOL bTrimLeadingZeros/*=FALSE*/)
{
	CString sReturn;
	if ( m_sBLZ.IsEmpty() ) {
		sReturn = GetRandomNumberWithLen(iLen, bTrimLeadingZeros);
	}
	else {
		sReturn = TrimDataToLength(m_sBLZ, iLen, bTrimLeadingZeros);
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetKonto(int iLen, BOOL bTrimLeadingZeros/*=FALSE*/)
{
	CString sReturn;
	if ( m_sKonto.IsEmpty() ) {
		sReturn = GetRandomNumberWithLen(iLen, bTrimLeadingZeros);
	}
	else {
		sReturn = TrimDataToLength(m_sKonto, iLen, bTrimLeadingZeros);
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetBetrag(int iLen, BOOL bTrimLeadingZeros/*=FALSE*/)
{
	CString sReturn;
	if ( m_sBetrag.IsEmpty() ) {
		sReturn += GetRandomBetrag(iLen, bTrimLeadingZeros);
	}
	else {
		sReturn = TrimDataToLength(m_sBetrag, iLen, bTrimLeadingZeros);
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetCurrency()
{
	CString sReturn;
	if ( m_sCurrency.IsEmpty() ) {
		sReturn = _T("DEM");
	}
	else {
		sReturn = m_sCurrency;
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetFirstName()
{
	CString sReturn;
	if ( m_sBLZ.IsEmpty() ) {
		int iRandom = GetRandomNumberBetweenAsInt(0, m_sFirstNameArray.GetUpperBound());
		sReturn = m_sFirstNameArray.GetAt(iRandom);
	}
	else {
		sReturn = m_sBLZ;
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetLastName()
{
	CString sReturn;
	if ( m_sKonto.IsEmpty() ) {
		int iRandom = GetRandomNumberBetweenAsInt(0, m_sLastNameArray.GetUpperBound());
		sReturn = m_sLastNameArray.GetAt(iRandom);
	}
	else {
		sReturn = m_sKonto;
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::TrimDataToLength(CString sData, int iLen, BOOL bTrimLeadingZeros)
{
	CString sReturn;
	// Ggf. links mit Nullen auffuellen, falls zu kurz
	int iLead = iLen - sData.GetLength();
	if (bTrimLeadingZeros) {
		iLead = min (iLead, 0);
		int iStart = sData.FindOneOf(_T("123456789"));
		if (iStart != -1) {
			sData = sData.Mid(iStart);
		}
	}
	if (iLead > 0) {
		CString sNullen('0', iLead);
		sReturn = sNullen + sData;
	}
	else {
		// Ggf. kuerzen, falls zu lang
		sReturn = sData.Left(iLen);
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetDay(int iChar, BOOL bGerman) 
{
	CTime time = CTime::GetCurrentTime();
	CString sReturn = time.Format(_T("%A"));
	if (bGerman) {
		if (sReturn == _T("Monday")) {
			sReturn = _T("Montag");
		}
		else if (sReturn == _T("Tuesday")) {
			sReturn = _T("Dienstag");
		}
		else if (sReturn == _T("Wednesday")) {
			sReturn = _T("Mittwoch");
		}
		else if (sReturn == _T("Thursday")) {
			sReturn = _T("Donnerstag");
		}
		else if (sReturn == _T("Friday")) {
			sReturn = _T("Freitag");
		}
		else if (sReturn == _T("Saturday")) {
			sReturn = _T("Samstag");
		}
		else if (sReturn == _T("Sunday")) {
			sReturn = _T("Sonntag");
		}
	}
	if ( iChar>0 ) {
		sReturn = sReturn.Left(iChar);
	}
	return sReturn;
}
/*
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetTimeHHMM() 
{
	CTime time = CTime::GetCurrentTime();
	return time.Format(_T("%H%M"));
}
*/
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetTimeHHMMSS() 
{
	CTime time = CTime::GetCurrentTime();
	return time.Format(_T("%H%M%S"));
}
/*
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetTimeHH_MM_SSWithDoublePoint() 
{
	CTime time = CTime::GetCurrentTime();
	return time.Format(_T("%H:%M:%S"));
}
*/
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetInternTAN(int iLen, BOOL bIncrease, BOOL bTrimLeadingZeros/*=FALSE*/) 
{
	if (bIncrease) {
		m_iTAN++;
	}
	CString sTAN;
	sTAN.Format(_T("%i"),m_iTAN);

	int iLead = iLen - sTAN.GetLength();
	if (bTrimLeadingZeros) {
		iLead = min(iLead, 0);
	}
	if (iLead>0) {	
		CString sLead(_T('0'), iLead);
		sTAN = sLead + sTAN;
	}
	else if (iLead < 0) {
		 sTAN = sTAN.Right(iLen);
	}
	
	return sTAN;
}
/////////////////////////////////////////////////////////////////////////////
int CSDITestDoc::GetRandomNumberBetweenAsInt(int iMin, int iMax)
{
	// neue Startziffer setzen
	m_uRandom += (unsigned)GetCurrentTime();
	srand(m_uRandom);
	int iRandom = rand();
	float fRandom = (float)iRandom / (float)RAND_MAX;
	iRandom = (int)( (iMax - iMin + 1) * fRandom);
	if (iRandom>iMax){
		iRandom = iMax;
	}

	return iRandom;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetRandomNumberBetweenAsString(int iMin, int iMax)
{
	CString sRandom;
	sRandom.Format( _T("%i"), GetRandomNumberBetweenAsInt(iMin, iMax) );
	return sRandom;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetRandomNumberWithLen(int iLen, BOOL bTrimLeadingZeros/*=FALSE*/)
{
	CString sRandom;
	CString sDigit;
	for (int i=1 ; i<=iLen ; i++) {
		sDigit = GetRandomNumberBetweenAsString(0, 9);
		if (sRandom.IsEmpty() && bTrimLeadingZeros && (sDigit == _T('0'))) {
			// skip leading zero
		}
		else {
			sRandom += sDigit;
		}
	}
	return sRandom;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDITestDoc::GetRandomBetrag(int iLen, BOOL bTrimLeadingZeros/*=FALSE*/)
{
	CString sBetrag;
	// Letzte Ziffer ist immer 0
	sBetrag = GetRandomNumberWithLen(iLen-1, bTrimLeadingZeros);
	sBetrag += _T("0");
	return sBetrag;
}
/////////////////////////////////////////////////////////////////////////////
BYTE CSDITestDoc::GetXORCrcByte(CByteArray& byaData, int iStart/*=0*/, int iEnd/*=-1*/) 
{
	if (-1 > iEnd) {
		ASSERT(0);
		MessageBeep((UINT)-1);
		WK_TRACE_ERROR(_T("CSDITestDoc::GetXORCrcByte(CByteArray) Wrong NEGATIVE End %i\n"), iEnd);
		iEnd = byaData.GetUpperBound();
	}
	else if ( iEnd > byaData.GetUpperBound() ) {
		ASSERT(0);
		MessageBeep((UINT)-1);
		WK_TRACE_ERROR(_T("CSDITestDoc::GetXORCrcByte(CByteArray) Wrong End %i > %i\n"),
														iEnd, byaData.GetUpperBound());
		iEnd = byaData.GetUpperBound();
	}
	else if (-1 == iEnd){
		// Alles ok, iEnd auf Standart setzen
		iEnd = byaData.GetUpperBound();
	}
	else {
		// Alles ok, iEnd gueltig
	}

	BYTE byCRC = 0;
	for (int i=iStart ; i<=iEnd ; i++) {
		byCRC = (BYTE)( byCRC ^ byaData.GetAt(i) );
	}
	return byCRC;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDITestDoc, CDocument)
	//{{AFX_MSG_MAP(CSDITestDoc)
	ON_COMMAND(ID_CONFIG_COM, OnConfigCom)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_COM, OnUpdateConfigCom)
	ON_COMMAND(ID_CONFIG_IP, OnConfigIP)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_IP, OnUpdateConfigIP)
	ON_COMMAND(ID_SEND, OnSend)
	ON_UPDATE_COMMAND_UI(ID_SEND, OnUpdateSend)
	ON_COMMAND(ID_STOP_SEND, OnStopSend)
	ON_UPDATE_COMMAND_UI(ID_STOP_SEND, OnUpdateStopSend)
	ON_COMMAND(ID_TRACE_RS232_ASCII, OnTraceRs232Ascii)
	ON_UPDATE_COMMAND_UI(ID_TRACE_RS232_ASCII, OnUpdateTraceRs232Ascii)
	ON_COMMAND(ID_TRACE_RS232_HEX, OnTraceRs232Hex)
	ON_UPDATE_COMMAND_UI(ID_TRACE_RS232_HEX, OnUpdateTraceRs232Hex)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSDITestDoc commands
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnConfigCom() 
{
	CComDlg	dlg(m_iCom-1);
	if ( IDOK == dlg.DoModal() ) 
	{
		m_iCom = dlg.m_iCom + 1;
		ConfigCom();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnUpdateConfigCom(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bStarted);
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnConfigIP() 
{
	CIPDlg	dlg(m_iIPPort,m_sIPAddress,m_iIPType);
	if ( IDOK == dlg.DoModal() ) 
	{
		m_iIPPort = dlg.m_iIPPort;
		m_iIPType = dlg.m_iIPType;
		m_sIPAddress = dlg.m_sIPAddress;
		ConfigCom();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnUpdateConfigIP(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bStarted);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	BOOL bReturn = FALSE;
	TCHAR szOldDir[_MAX_PATH];
	memset(szOldDir, 0, _MAX_PATH);
	if ( 0 != GetCurrentDirectory(_MAX_PATH, szOldDir) ) {
		SetCurrentDirectory(m_sDirectory);
		if (CSDITestDoc::OnOpenDocument(lpszPathName)) {
			bReturn = TRUE;
		}
		SetCurrentDirectory(szOldDir);
	}
	
	// TODO: Add your specialized creation code here
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnFileSave() 
{
	TCHAR szOldDir[_MAX_PATH];
	memset(szOldDir, 0, _MAX_PATH);
	if ( 0 != GetCurrentDirectory(_MAX_PATH, szOldDir) ) 
	{
		SetCurrentDirectory(m_sDirectory);
		CDocument::OnFileSave();	
		SetCurrentDirectory(szOldDir);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnFileSaveAs() 
{
	TCHAR szOldDir[_MAX_PATH];
	memset(szOldDir, 0, _MAX_PATH);
	if ( 0 != GetCurrentDirectory(_MAX_PATH, szOldDir) ) {
		SetCurrentDirectory(m_sDirectory);
		CDocument::OnFileSaveAs();	
		SetCurrentDirectory(szOldDir);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	// Hier steht der neue Name drin
	CString sPathName = lpszPathName;
	m_sDirectory = sPathName.Left( sPathName.ReverseFind(_T('\\')) );
	
	return CDocument::OnSaveDocument(lpszPathName);
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnSend() 
{
	if ( IsDataOK() ) 
	{
		m_uCounter = 0;
		CString sTitle, sActive;
		sTitle = GetTitle();
		sActive.LoadString(IDS_ACTIVE);
		sTitle = sTitle + sActive;
		SetTitle(sTitle);

		Send();

		if (m_bRepeat) 
		{
			UpdateAllViews(NULL);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnUpdateSend(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	bEnable = IsComOK() && !m_bStarted;
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnStopSend() 
{
	m_bStarted = FALSE;
	CString sTitle, sActive;
	sTitle = GetTitle();
	sActive.LoadString(IDS_ACTIVE);
	if ( -1 != sTitle.Find(sActive) ) {
		sTitle = sTitle.Left( sTitle.Find(sActive) );
		SetTitle(sTitle);
	}

	UpdateAllViews(NULL);
/*	if ( KillTimer(m_uTimerID) ) {
	}
	else {
		WK_TRACE_ERROR(_T("CSDITestDoc::OnStopSend() TIMER NOT KILLED\n"));
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnUpdateStopSend(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	bEnable = m_bStarted;
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnTraceRs232Ascii() 
{
	m_bTraceRS232DataAscii = !m_bTraceRS232DataAscii;
	if (m_pCom) {
		m_pCom->SetTraceRS232DataAscii(m_bTraceRS232DataAscii);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnUpdateTraceRs232Ascii(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceRS232DataAscii);
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnTraceRs232Hex() 
{
	m_bTraceRS232DataHex = !m_bTraceRS232DataHex;
	if (m_pCom) {
		m_pCom->SetTraceRS232DataHex(m_bTraceRS232DataHex);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestDoc::OnUpdateTraceRs232Hex(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceRS232DataHex);
}
