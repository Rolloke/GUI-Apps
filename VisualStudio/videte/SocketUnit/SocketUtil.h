

#ifndef _SocketUtil_H
#define _SocketUtil_H


#include "wk.h"
#include "WK_Template.h"


extern CString WSAGetLastErrorString();

class CWK_LookUpPair
{
public:
	inline CWK_LookUpPair(const CString &sIn, const CString &sOut);
	// access
	inline const CString & GetInput() const;
	inline const CString & GetLookUpResult() const;
private:
	CString m_sName;
	CString m_sLookUp;
};

inline CWK_LookUpPair::CWK_LookUpPair(const CString &sIn, const CString &sOut)
{
	m_sName = sIn;
	m_sLookUp=sOut;
}


inline const CString & CWK_LookUpPair::GetInput() const
{
	return m_sName;
}

inline const CString & CWK_LookUpPair::GetLookUpResult() const
{
	return m_sLookUp;
}



typedef CWK_LookUpPair * CWK_LookUpPairPtr;
// NOT YET auto autodelete
WK_PTR_ARRAY_CS(CWK_LookUpPairsPlain,CWK_LookUpPairPtr,CWK_LookUpPairsCS);

// add Index() with string input
class CWK_LookUpPairs : public CWK_LookUpPairsCS
{
public:
	int Index(const CString &sIn)
	{
		int ixResult = -1;
		Lock();
		for (int i=0;ixResult==-1 && i<GetSize();i++) {
			const CWK_LookUpPair & onePair = * GetAt(i);
			if (onePair.GetInput()==sIn) {
				ixResult = i;	// found
			}
		}	// end of loop over all records
		Unlock();
		return ixResult;
	}

};





// OOPS what about pure stars ?
enum IPNameType {
	INT_UNKNOWN,
	INT_DOTTED_IP_NUMBER, // 1.2.3.4
	INT_DOTTED_IP_NUMBER_WITH_STAR,	// Number.*
	INT_SINGLE_HOST_NAME,	// single word
	INT_FULL_HOST_NAME,	// a.b.c
	INT_FULL_HOST_NAME_WITH_STAR	// Word.*
};


extern LPCTSTR NameOfEnum(IPNameType x);

class CWK_DNS
{
public:
	static void Reset();	// also useful for Cleanup
	static CString LookUp(const CString &sIn);
	// options
	static BOOL m_bUseDNS;	// default FALSE
	static BOOL m_bDoCacheKnownNames;	// default TRUE
	static BOOL m_bDoCacheUnknownNames;	// default TRUE
	//
	static IPNameType CalcIPNameType(const CString &sName, CString &sCorrected);

private:
	static void AddUnknown(const CString &sUnknwon);
	static void AddKnown(const CString &sIn, const CString &sOut);
	// cache data
	static CWK_LookUpPairs m_knownNames;
	static CWK_LookUpPairs m_unknownNames;
};	// end of WK_DNS


#endif