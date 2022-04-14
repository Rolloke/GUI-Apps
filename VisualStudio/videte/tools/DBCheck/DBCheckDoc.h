// DBCheckDoc.h : interface of the CDBCheckDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBCHECKDOC_H__46CD594B_BF55_489E_89EB_97572984EDBA__INCLUDED_)
#define AFX_DBCHECKDOC_H__46CD594B_BF55_489E_89EB_97572984EDBA__INCLUDED_

#include "server.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define VDH_DATABASE				0x01
#define VDH_ADD_FILE				0x02
#define VDH_REMOVE_SERVER			0x03
#define VDH_REMOVE_FILE				0x04
#define VDH_OPEN_FILE				0x05
#define VDH_ADD_ARCHIV				0x06
#define VDH_DEL_ARCHIV				0x07
#define VDH_ADD_SEQUENCE			0x08
#define VDH_OPEN_SEQUENCE			0x09

#define VDH_BACKUP					0x0A
#define VDH_BACKUP_ADD_SEQUENCE		0x0B
#define VDH_BACKUP_DEL_SEQUENCE		0x0C
#define VDH_BACKUP_ADD_ARCHIVE		0x0D
#define VDH_BACKUP_DEL_ARCHIVE		0x0F

#define VDH_SYNC_DEL_ARCHIVE		0x10

class CLeftView;
class CDBCheckView;

class CDBCheckDoc : public CDocument
{
protected: // create from serialization only
	CDBCheckDoc();
	DECLARE_DYNCREATE(CDBCheckDoc)

// Attributes
public:
	inline WORD     GetServerID();
	inline CServer* GetServer(WORD);

   CLeftView*      GetLeftView();
   CDBCheckView*   GetDBCheckView();

// Operations
public:
	void ActualizeDatabase(CIPCDatabaseDBCheck* pDatabase);
	void UpdateMyViews(CView* pSender, LPARAM lHint, CObject* pHint);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBCheckDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnIdle();
	//}}AFX_VIRTUAL

// Implementation
public:
	void ServerError();
	virtual ~CDBCheckDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDBCheckDoc)
	afx_msg void OnConnectLocal();
	afx_msg void OnUpdateConnectLocal(CCmdUI* pCmdUI);
	afx_msg void OnConnectRemote();
	afx_msg void OnUpdateConnectRemote(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CServer m_Server;
};

/////////////////////////////////////////////////////////////////////////////

inline WORD CDBCheckDoc::GetServerID()
{
   return m_Server.GetID();
}

inline CServer* CDBCheckDoc::GetServer(WORD wID)
{
   return &m_Server;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBCHECKDOC_H__46CD594B_BF55_489E_89EB_97572984EDBA__INCLUDED_)
