/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCField.h $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCField.h $
// CHECKIN:		$Date: 7/19/06 11:02a $
// VERSION:		$Revision: 61 $
// LAST CHANGE:	$Modtime: 7/19/06 10:55a $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
// CIPCField.h: interface for the CIPCField class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCFIELD_H__E639AA85_98B3_11D2_B541_004005A19028__INCLUDED_)
#define AFX_CIPCFIELD_H__E639AA85_98B3_11D2_B541_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wkclasses\wk_profile.h"
//////////////////////////////////////////////////////////////////////
class CIPC;
class CIPCExtraMemory;
//////////////////////////////////////////////////////////////////////
// Types
// 'U' undefined
// 'N' numeric value
// 'C' character array
// 'F' float value
// 'L' logical value
// 'D' date
// 'T' time
// 'W' currency währung
// 'R' whole record as string
//////////////////////////////////////////////////////////////////////
#define FIELD_DBD_STNM _T("DBD_STNM")
#define FIELD_DBD_CANM _T("DBD_CANM")
#define FIELD_DBD_INNM _T("DBD_INNM")

#define FIELD_DBD_BFR _T("DBD_DBFR")
#define FIELD_DBD_TCC _T("DBD_DTCC")
#define FIELD_DBD_ANR _T("DBD_DANR")

#define FIELD_DBD_TSN			_T("DBD_DTSN")
#define FIELD_DBD_SUBSCRIBER	_T("DBD_SUBN")
#define FIELD_DBD_COMMENT		_T("DBD_COMM")

#define FIELD_DBD_POS			_T("DBD_POS")

#define FIELD_FLAG_CAN_DEACTIATE	0x00000001
#define FIELD_FLAG_UNUSED			0x00000002
#define FIELD_FLAG_NO_EDIT_NAME		0x00000010
#define FIELD_FLAG_NO_EDIT_VALUE	0x00000020
#define FIELD_FLAG_NO_EDIT_TYPE		0x00000040
#define FIELD_FLAG_NO_EDIT_MAXLEN	0x00000080

class AFX_EXT_CLASS CIPCField  
{
	// construction / destruction
public:
	CIPCField();
	CIPCField(const CString& sName,
			  const CString& sValue,
			  char  cType);
	CIPCField(const CString& sName,
			  const CString& sValue,
			  WORD  wMaxLength,
			  char  cType);
	CIPCField(const CIPCField& src);
	virtual ~CIPCField();

	// attributes
public:
	inline const CString& GetName() const;
	inline const CString& GetValue() const;
	inline WORD	 GetMaxLength() const;
	inline char	 GetType() const;
	inline BOOL	 IsNumeric() const;
	inline BOOL	 IsFloat() const;
	inline BOOL	 IsString() const;
	inline BOOL	 IsLogical() const;
	inline BOOL	 IsDate() const;
	inline BOOL	 IsTime() const;
	inline BOOL	 IsCurrency() const;
	inline BOOL	 IsEmpty() const;
		   const CString  ToString() const;
	inline DWORD GetFlags() const;

	// operations
public:
	void SetName(const CString& sName);
	void SetValue(const CString& sValue);
	void SetMaxLength(WORD dwMaxLength);
	void SetType(char cType);
	void SetFlags(DWORD dwFlags);
	void Trim();
	void LeadingZeros();

	BOOL Load(const CString& sSection,int i,CWK_Profile& wkp);
	void Save(const CString& sSection,int i,CWK_Profile& wkp);

	// implementation
public:
	static CString FillLeadingZeros(const CString& sData, WORD wLen);
	static CString DropLeadingZeros(const CString& sData);
	static CString Translate3xTo4x(const CString& sData);
	static CString Translate4xTo3x(const CString& sData);
	static char TranslateToType(const CString& sData);

	// data
public:
   // Geldautomat Zeit

   // Geldautomat Zeit
	static CString m_sfTime;
	// Geldautomat Datum
	static CString m_sfDate;
	// Geldautomat Nummer
	static CString m_sfGaNr;
	// Transaktionsnummer
	static CString m_sfTaNr;
	// Kontonummer
	static CString m_sfKtNr;
	// Bankleitzahl
	static CString m_sfBcNr;
	// Betrag
	static CString m_sfAmnt;
	// Waehrung
	static CString m_sfCurr;
	// Stationsname
	static CString m_sfStNm;
	// Kameraname
	static CString m_sfCaNm;
	// Meldername
	static CString m_sfInNm;
	// Sequencename
	static CString m_sfSqNm;
	// Kundennummer
	static CString m_sfCustomerId;
	// Vorname
	static CString m_sfFirstName;
	// Nachname
	static CString m_sfLastName;
	// Vorgangsnummer
	static CString m_sfProcessId;
	// Fachnummer
	static CString m_sfBoxId;
	// Sollbetrag
	static CString m_sfSoll;
	// Istbetrag
	static CString m_sfIst;
	// Differenzbetrag
	static CString m_sfDifferenz;

	// data
private:
	CString m_sName; // used as CodeBase Field
	CString m_sValue;
	DWORD	m_dwFlags;
	WORD	m_wMaxLength;
	char	m_cType;
};

//////////////////////////////////////////////////////////////////////
#include "CIPCField.inl"

//////////////////////////////////////////////////////////////////////
typedef CIPCField* CIPCFieldPtr;

WK_PTR_ARRAY_CS(CIPCFieldArray, CIPCFieldPtr, CIPCFieldArrayCS)
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCFields : public CIPCFieldArrayCS
{
	// construction/destruction
public:
	CIPCFields();
	CIPCFields(const CIPCFields& src);
	virtual ~CIPCFields();

	// operators
public:
	const CIPCFields& operator = (const CIPCFields& src);

	// operations
public:
	void		FromString(const CString& sGAorSerialData);
	void		FromArray(int iNumRecords,const CIPCField data[]);
	
	void		AddNoDuplicates(const CIPCField& field);
	void		AddNoDuplicates(const CIPCFields& fields);
//	CIPCField*	GetCIPCField(const CString& sName) const;
	CIPCField*	GetCIPCField(const _TCHAR sName[]) const;
//	inline CIPCField* GetAtFast(int nPos) const;
	
	CString     ToSerial() const;
	CString     ToGeld() const;

	CString		BuildOldQueryString() const;

	void		Load(const CString& sSection, CWK_Profile& wkp);
	void		Save(const CString& sSection, CWK_Profile& wkp);

	CString	    ToString() const;

	// bubble code
public:
#ifdef _UNICODE
	DWORD GetBubbleLength(WORD wCodePage) const;
	DWORD ToMemory(BYTE*& pByte, WORD wCodePage) const;
	static CIPCField* FromMemory(DWORD dwMaxRead,
								 const BYTE*& pByte,
								 int& iNumRecords,
								 WORD wCodePage);
#else
	DWORD GetBubbleLength() const;
	DWORD ToMemory(BYTE*& pByte) const;
	static CIPCField* FromMemory(DWORD dwMaxRead,const BYTE*& pByte,int& iNumRecords);
#endif
	CIPCExtraMemory* BubbleFromFields(CIPC *pCipc) const;
	static CIPCField* FieldsFromBubble(int& iNumRecords, const CIPCExtraMemory *pExtraMem);

private:
	void FromGeld(const CString& sData);
	void FromSerial(const CString& sData);
};
//////////////////////////////////////////////////////////////////////
//inline CIPCField* CIPCFields::GetAtFast(int nPos) const
//{	// RKE: to be used within a loop. The field range of the array is not validated,
	// because the range is validated by the loops control variable.
//	return (CIPCField*) m_pData[nPos];
//}
//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_CIPCFIELD_H__E639AA85_98B3_11D2_B541_004005A19028__INCLUDED_)
