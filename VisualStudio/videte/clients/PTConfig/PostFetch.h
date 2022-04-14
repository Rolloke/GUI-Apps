
#ifndef _CPostFetchResult_H
#define _CPostFetchResult_H

#include "CIPCServerControlClientSide.h"

#define THE_MESSAGE (WM_USER+555)

enum FetchCallType { 
	FCT_INVALID=0,
	FCT_INPUT=1,
	FCT_OUTPUT=2
};

class CPostFetchResult
{
public:
	inline CPostFetchResult(
			FetchCallType t,
			const CIPCFetchResult &result, 
			DWORD dwUserdata
			);
	// public data
	FetchCallType m_type;
	CIPCFetchResult m_result;
	DWORD m_dwUserData;
};

extern void PostFetchInputConnection(const CConnectionRecord &c, DWORD dwUserData);
extern void PostFetchOutputConnection(const CConnectionRecord &c, DWORD dwUserData);

//////////////////////
inline CPostFetchResult::CPostFetchResult(FetchCallType t,
										  const CIPCFetchResult &result, 
										  DWORD dwUserData)
{
	m_result = result;
	m_dwUserData = dwUserData;
	m_type = t;
}
#endif