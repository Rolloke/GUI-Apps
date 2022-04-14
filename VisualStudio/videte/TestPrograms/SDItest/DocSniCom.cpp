/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocSniCom.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/DocSniCom.cpp $
// CHECKIN:		$Date: 19.12.05 16:30 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 19.12.05 16:28 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "DocSniCom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocSniCom
IMPLEMENT_DYNCREATE(CDocSniCom, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocSniCom::CDocSniCom()
{
	m_eType = SDICT_SNI_DOS_COM;

	m_sDataArray.Add("11:39:02 EARLY TDR            T400     0000101 FCV990518113902045201CN00000000\r\n");
	m_sDataArray.Add("0000600                0100                                            0000000\r\n");
	m_sDataArray.Add("00000000000000106725228300004303828D991210106827500000T0113FFFF104015922151730\r\n");
	m_sDataArray.Add("D00004303824D2809540100000007188013010000020000099125285245054DD1D913800000000\r\n");
	m_sDataArray.Add("5000000000000051FD521031                                                      \r\n");
	m_sDataArray.Add("11:39:02 TRANSAKTIONSANTWORT  00001 00FPN9905181139020452014472522A0          \r\n");
	m_sDataArray.Add("\r\n");
	m_sDataArray.Add("     DDC: BP=0003 OPEN... VEM08SASCSCTAS1 \r\n");
	m_sDataArray.Add("          BX=0300\r\n");
	m_sDataArray.Add("          AX=0000\r\n");
	m_sDataArray.Add("     DDC: BP=0004 CLOSE.. \r\n");
	m_sDataArray.Add("          BX=0300\r\n");
	m_sDataArray.Add("          AX=000011:39:15 AUTORISIERUNG        T400     0000101 FAA990518113914045202IN00000010\r\n");
	m_sDataArray.Add("00006010AD67C0B98266B690101                                            0000000\r\n");
	m_sDataArray.Add("00000000001000106725228300004303828D991210106827500000T0113FFFF104015922151730\r\n");
	m_sDataArray.Add("D00004303824D2809540100000007188013010000020000099125285245054DD1D913800000000\r\n");
	m_sDataArray.Add("500000000000005125EE103F                                                      \r\n");
	m_sDataArray.Add("11:39:16 TRANSAKTIONSANTWORT  00001 10FPN9905181139140452025D00430031  KONTOST\r\n");
	m_sDataArray.Add("AND DEM          546,37 SOLL;#J00760000010452 Auszahlung MEC 22151730;/11:39 0\r\n");
	m_sDataArray.Add("000430382/5 3 18.05.99 12.99 M1;#132C2BBF                                     \r\n");
	m_sDataArray.Add("11:39:31 NORMAL STATUS        T400     0000101 FSN990518113930045203NN00000000\r\n");
	m_sDataArray.Add("00002010AD67C0B98266B690101                                            0000000\r\n");
	m_sDataArray.Add("00000000001000106725228300004303828D991210106827500000T0113FFFF104015922151730\r\n");
	m_sDataArray.Add("D00004303824D2809540100000007188013010000020000099125285245054DD1D913800000000\r\n");
	m_sDataArray.Add("5000000000000051M00100000000100802592AA                                       \r\n");
	m_sDataArray.Add("11:39:33 TRANSAKTIONSANTWORT  00001 00FPN9905181139300452034BFC2A867          \r\n");
	m_sDataArray.Add("11:44:30 INQUIRY              VAR : 1                                         \r\n");
	m_sDataArray.Add("11:44:31 COMMAND RESPONSE     STATUS : 01 OPEN    444400044440000040440000A 08\r\n");
	m_sDataArray.Add("501400460034503450008203180294029410097070306170617100380362031003101000000000\r\n");
	m_sDataArray.Add("00000000C 0100000000000E 0102770000000Z 00029DD6982                           \r\n");

	m_iLastDataIndex = m_sDataArray.GetUpperBound();
	m_iDataIndex = 0;

}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocSniCom::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocSniCom::~CDocSniCom()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocSniCom::OpenCom() 
{
	return m_pCom->Open(CBR_9600, 8, NOPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocSniCom::CreateData() 
{
	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
//			m_iPointOfData = GetRandomNumberBetweenAsInt(0, 9);
			break;
		case 2: // Successive
			if ( !m_bStarted ) {
				m_iDataIndex = 0;
			}
			else {
				// Werte hochzaehlen
				m_iDataIndex++;
				if (m_iLastDataIndex < m_iDataIndex) {
					m_iDataIndex = 0;
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR(_T("CDocSniCom::PreCreateData() WRONG DATA RepeatHow %i\n"),
																			m_iRepeatHow);
			break;
		}
	}

	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocSniCom::CreateDataIntern() 
{
	CString sData = m_sDataArray.GetAt(m_iDataIndex);

	// Datenstring zusammensetzen
	m_byaData.RemoveAll();
	// Daten
	CStringA s(sData);
	for (int i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDocSniCom diagnostics
#ifdef _DEBUG
void CDocSniCom::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocSniCom::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocSniCom serialization
void CDocSniCom::Serialize(CArchive& ar)
{
	// Versionsnummer wird mit abgespeichert,
	// muss bei jeder relevanten Aenderung erhoeht werden
	m_uSerializeVersion = 0;

	CSDITestDoc::Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocSniCom, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocSniCom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocSniCom commands
