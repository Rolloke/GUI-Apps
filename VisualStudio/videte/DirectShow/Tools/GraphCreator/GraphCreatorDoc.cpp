// GraphCreatorDoc.cpp : implementation of the CGraphCreatorDoc class
//

#include "stdafx.h"
#include "GraphCreator.h"
#include "MainFrm.h"

#include "GraphCreatorDoc.h"
#include ".\graphcreatordoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CKeyProvider g_prov;

 
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorDoc
IMPLEMENT_DYNCREATE(CGraphCreatorDoc, CDocument)

BEGIN_MESSAGE_MAP(CGraphCreatorDoc, CDocument)
	//{{AFX_MSG_MAP(CGraphCreatorDoc)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_GRAPH_EXTERNAL, OnGraphExternal)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_EXTERNAL, OnUpdateGraphExternal)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorDoc construction/destruction
CGraphCreatorDoc::CGraphCreatorDoc()
{
	m_pGraph = NULL;
	m_pGraphBuilder = NULL;
	m_pMediaControl = NULL;
	m_pMediaEvent = NULL;
	g_prov.AddRef();  // Don't let COM try to free our static object.
}
/////////////////////////////////////////////////////////////////////////////
CGraphCreatorDoc::~CGraphCreatorDoc()
{
	ReleaseBasicFilters();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGraphCreatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	ReleaseFilters();
	ReleaseBasicFilters();
	CreateBasicFilters();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorDoc serialization
void CGraphCreatorDoc::Serialize(CArchive& ar)
{
	CView *pView = theApp.GetActiveView();
	if (m_pGraph && pView)
	{
		HRESULT hr;
		CString sStreamName = _T("ActiveMovieGraph");
		if (ar.IsStoring())
		{
			ar.GetFile()->Close();
			LPCOLESTR sFile = ar.m_strFileName;
			LPCOLESTR sStream = sStreamName;
			IStorage *pStorage = NULL;
			IStream *pStream = NULL;
			IPersistStream *pPersist = NULL;
			try
			{
                hr = StgCreateDocfile(sFile, STGM_CREATE|STGM_TRANSACTED|STGM_WRITE|STGM_SHARE_EXCLUSIVE, 0, &pStorage);
				HRESULT_EXCEPTION(hr);
				hr = pStorage->CreateStream(sStream, STGM_WRITE|STGM_CREATE|STGM_SHARE_EXCLUSIVE, 0, 0, &pStream);
				HRESULT_EXCEPTION(hr);
				hr = m_pGraph->QueryInterface(IID_IPersistStream, (void**)&pPersist);
				HRESULT_EXCEPTION(hr);
				hr = pPersist->Save(pStream, TRUE);
				HRESULT_EXCEPTION(hr);
				hr = pStorage->Commit(STGC_DEFAULT);
				HRESULT_EXCEPTION(hr);
			}
			catch(ErrorStruct *ps)
			{
				pView->PostMessage(WM_HRESULT_ERROR, (WPARAM)ps, 1);
			}
			RELEASE_OBJECT(pStorage);
			RELEASE_OBJECT(pStream);
			RELEASE_OBJECT(pPersist);
			ar.GetFile()->Open(ar.m_strFileName, CFile::modeRead);
		}
		else
		{
			LPCOLESTR sFile = ar.m_strFileName;
			hr = StgIsStorageFile(sFile);
			BOOL bStorageFile = (hr == S_OK);
			if (!bStorageFile == !theApp.m_bRenderMediaFile)
			{
				theApp.m_bRenderMediaFile = TRUE;
				bStorageFile = -1;
			}
			if (theApp.m_bRenderMediaFile)
			{
				hr = m_pGraph->RenderFile(sFile, NULL);
				if (SUCCEEDED(hr))
				{
					pView->PostMessage(WM_USER, WPARAM_UPDATE_GRAPH);
				}
				else
				{
					pView->PostMessage(WM_HRESULT_ERROR, (WPARAM)HRESULT_ERROR_AT_POS(hr), 1);
				}
			}
			else if (bStorageFile)
			{
				ar.GetFile()->Close();
				LPCOLESTR sFile = ar.m_strFileName;
				IStorage *pStorage = NULL;
				IPersistStream *pPersistStream = NULL;
				IStream *pStream = NULL;
				IEnumSTATSTG *pEnumS = NULL;
				try
				{
					hr = StgOpenStorage(sFile, 0, STGM_TRANSACTED|STGM_READ|STGM_SHARE_DENY_WRITE, 0, 0, &pStorage);
					HRESULT_EXCEPTION(hr);
					hr = m_pGraph->QueryInterface(IID_IPersistStream, reinterpret_cast<void**>(&pPersistStream));
					HRESULT_EXCEPTION(hr);
					hr = pStorage->EnumElements(0, NULL, 0, &pEnumS);
					if (SUCCEEDED(hr))
					{
						STATSTG s;
						ULONG uRet = 1;
						if (SUCCEEDED(pEnumS->Next(1, &s, &uRet)) && uRet)
						{
							sStreamName = CString(s.pwcsName);
						}
					}
					LPCOLESTR sStream = sStreamName;
					hr = pStorage->OpenStream(sStream, 0, STGM_READ|STGM_SHARE_EXCLUSIVE, 0, &pStream);
					HRESULT_EXCEPTION(hr);
					hr = pPersistStream->Load(pStream);
					HRESULT_EXCEPTION(hr);
					pView->PostMessage(WM_USER, WPARAM_UPDATE_GRAPH);
				}
				catch(ErrorStruct *ps)
				{
					pView->PostMessage(WM_HRESULT_ERROR, (WPARAM)ps, 1);
				}
				RELEASE_OBJECT(pStorage);
				RELEASE_OBJECT(pStream);
				RELEASE_OBJECT(pPersistStream);
				RELEASE_OBJECT(pEnumS);
				ar.GetFile()->Open(ar.m_strFileName, CFile::modeRead);
			}
			if (bStorageFile == -1)
			{
				theApp.m_bRenderMediaFile = FALSE;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorDoc::CreateBasicFilters()
{
	HRESULT hr;
	try
	{	
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGraph);
		HRESULT_EXCEPTION(hr);
		// Create the Capture Graph Builder.
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&m_pGraphBuilder);
		HRESULT_EXCEPTION(hr);
		// Associate the graph with the builder.
		hr = m_pGraphBuilder->SetFiltergraph(m_pGraph);
		HRESULT_EXCEPTION(hr);
		hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
		HRESULT_EXCEPTION(hr);
		hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&m_pMediaEvent);
		// event window is set in CView..::OnInitialUpdate()
		HRESULT_EXCEPTION(hr);
		// Give the graph an IObjectWithSite pointer for callbacks and QueryService.
		IObjectWithSite* pObjectWithSite = NULL;
		hr = m_pGraph->QueryInterface(IID_IObjectWithSite, (void**)&pObjectWithSite);
		if (SUCCEEDED(hr))
		{
			// Use the IObjectWithSite pointer to specify our key provider object.
			// The filter graph manager will use this pointer to call
			// QueryService to do the unlocking.
			// If the unlocking succeeds, then we can build our graph.
			pObjectWithSite->SetSite((IUnknown *) (IServiceProvider *) &g_prov);
			pObjectWithSite->Release();
		}
	}
	catch(ErrorStruct *ps)
	{
		CView *pView = theApp.GetActiveView();
		if (pView)
		{
			pView->PostMessage(WM_HRESULT_ERROR, (WPARAM)ps, 1);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorDoc::ReleaseFilters()
{
	CView *pView = theApp.GetActiveView();
	if (m_pGraph && pView)
	{
		pView->SendMessage(WM_USER, WPARAM_CLOSE_DOC);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorDoc::ReleaseBasicFilters()
{
	RELEASE_OBJECT(m_pMediaControl);
	if (m_pMediaEvent)
	{
		m_pMediaEvent->SetNotifyWindow(NULL, 0, 0);
	}
	RELEASE_OBJECT(m_pMediaEvent);
	RELEASE_OBJECT(m_pGraph);
	RELEASE_OBJECT(m_pGraphBuilder);
}
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorDoc diagnostics
#ifdef _DEBUG
void CGraphCreatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGraphCreatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorDoc commands

/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorDoc::OnCloseDocument()
{
	CDocument::OnCloseDocument();
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorDoc::OnGraphExternal()
{
    IMoniker * pMoniker = NULL;
    IRunningObjectTable *pROT = NULL;
    HRESULT hr;
	VARIANT var;
	VariantInit(&var);

	hr = GetRunningObjectTable(0, &pROT);
	if (SUCCEEDED(hr))
	{
		IEnumMoniker*pEnum = NULL;
		hr = pROT->EnumRunning(&pEnum);
		if (SUCCEEDED(hr))
		{
			ULONG uFetched;
			CString strGraphName;
			IGraphBuilder*pGraph = NULL;
			while (SUCCEEDED(pEnum->Next(1, &pMoniker, &uFetched)) && uFetched != 0)
			{
				LPOLESTR pszName = NULL;
				hr = pROT->IsRunning(pMoniker);
				if (hr == S_OK)
				{
					IUnknown*pUnknown = NULL;
					hr = pROT->GetObject(pMoniker, &pUnknown);
					if (SUCCEEDED(hr))
					{
						hr = pUnknown->QueryInterface(IID_IGraphBuilder, (void**)&pGraph);
						if (SUCCEEDED(hr))
						{
							ReleaseBasicFilters();
							m_pGraph = pGraph;
						}
					}
					RELEASE_OBJECT(pUnknown);
				}
				RELEASE_OBJECT(pMoniker);
				if (pGraph)
				{
					CView *pView = theApp.GetActiveView();
					if (pView)
					{
						pView->PostMessage(WM_USER, WPARAM_UPDATE_GRAPH);
					}
					break;
				}
			}
		}
		RELEASE_OBJECT(pEnum);
	}
	RELEASE_OBJECT(pROT);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorDoc::OnUpdateGraphExternal(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
