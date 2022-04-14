
#ifndef __LSTNSOCK_H__
#define __LSTNSOCK_H__

#include "wk.h"


class CSocketUnitDoc;


class CControlSocket : public CSocket
{
	DECLARE_DYNAMIC(CControlSocket)

public:
	CControlSocket(CSocketUnitDoc* pDoc);
// Attributes
public:
	CSocketUnitDoc* m_pDoc;

// Overridable callbacks
protected:
	virtual void OnAccept(int nErrorCode);

// Implementation
public:
	virtual ~CControlSocket();


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CControlSocket(const CControlSocket& rSrc);         // no implementation
	void operator=(const CControlSocket& rSrc);  // no implementation
};


#endif // __LSTNSOCK_H__
