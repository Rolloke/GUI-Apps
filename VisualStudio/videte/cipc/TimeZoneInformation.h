// TimeZoneInformation.h: interface for the CTimeZoneInformation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEZONEINFORMATION_H__C63280A1_B18C_11D4_994E_004005A19028__INCLUDED_)
#define AFX_TIMEZONEINFORMATION_H__C63280A1_B18C_11D4_994E_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

static _TCHAR SKEY_NT[] = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones");
static _TCHAR SKEY_9X[] = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Time Zones");

///////////////////////////////////////////////////////////////////////////
class CIPC;
class CIPCExtraMemory;
///////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CTimeZoneInformation : public TIME_ZONE_INFORMATION  
{
	// construction / destruction
public:
	CTimeZoneInformation();
	CTimeZoneInformation(const TIME_ZONE_INFORMATION& tzi);
	~CTimeZoneInformation();

	// attributes
public:
	CString GetDisplayName();

	// operations
public:
	void SetDisplayName(const CString& sName);

	// bubble code
public:
	CIPCExtraMemory*  BubbleFrom(CIPC* pCipc) const;
	void FromBubble(const CIPCExtraMemory *pExtraMem);
	static  DWORD GetBubbleLength();
	void    WriteIntoBubble(BYTE*& pByte) const;
	void    ReadFromBubble(const BYTE*& pByte);

public:
	WCHAR DisplayName[64];
	// Achtung keine weiteren Membervariablen hinzufügen !!
};
///////////////////////////////////////////////////////////////////////////
typedef CTimeZoneInformation* CTimeZoneInformationPtr;
WK_PTR_ARRAY_CS(CTimeZoneInformationArrayPlain, CTimeZoneInformationPtr, CTimeZoneInformationArray)
///////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CTimeZoneInformations : public CTimeZoneInformationArray
{
typedef struct
{
   LONG			Bias;
   LONG			StandardBias;
   LONG			DaylightBias;
   SYSTEMTIME	StandardDate;
   SYSTEMTIME	DaylightDate;
}REGTIMEZONEINFORMATION;

	// construction/destruction
public:
	CTimeZoneInformations();
	CTimeZoneInformations(const CTimeZoneInformations& src);
	virtual ~CTimeZoneInformations();
	
	// operators
public:
	const CTimeZoneInformations& operator = (const CTimeZoneInformations& src);

	// operations
public:
	BOOL Load();
	int FindByTzi(const TIME_ZONE_INFORMATION &tzi);
	int FindByDaylightName(const WCHAR* pDaylightName);
	int FindByStandardName(const WCHAR* pStandardName);
	int FindByDisplayName(const WCHAR* pDisplayName);

	// bubble code
public:
	CIPCExtraMemory*  BubbleFrom(CIPC* pCipc);
	void FromBubble(int iNumRecords,const CIPCExtraMemory* pExtraMem);

protected:
	void Sort();
};
///////////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_TIMEZONEINFORMATION_H__C63280A1_B18C_11D4_994E_004005A19028__INCLUDED_)
