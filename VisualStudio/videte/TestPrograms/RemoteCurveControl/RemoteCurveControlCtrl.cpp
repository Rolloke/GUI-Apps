// RemoteCurveControlCtrl.cpp : Implementation of the CRemoteCurveControlCtrl ActiveX Control class.

#include "stdafx.h"
#include "RemoteCurveControl.h"
#include "RemoteCurveControlCtrl.h"
#include "RemoteCurveControlPropPage.h"
#include "AboutDlg.h"

#include "CurveLabel.h"
#include "caramenu.h"
#include "CaraViewProperties.h"
#include ".\remotecurvecontrolctrl.h"

#pragma warning (disable : 4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class AutoCS
{
public:
    AutoCS(CRITICAL_SECTION*pcs)
    {
        mpcs = pcs;
        EnterCriticalSection(mpcs);
    }
    ~AutoCS()
    {
		Release();
    }

	void Release()
    {
		if (mpcs)
		{
	        LeaveCriticalSection(mpcs);
		}
        mpcs = NULL;
    }

private:
    CRITICAL_SECTION *mpcs;
};


void CReceivingSocket::OnReceive(int nErrorCode)
{
   m_pCtrl->OnReceive(this, nErrorCode);
   CAsyncSocket::OnReceive(nErrorCode);
}

void CReceivingSocket::OnAccept(int nErrorCode)
{
   TRACE("OnAccept(%d)\n", nErrorCode);

   SOCKADDR addr;
   int nlen = sizeof(addr);
   CSocket soc;
   Accept(soc, &addr, &nlen);
   Close();
   Attach(soc.Detach());
   AsyncSelect(FD_READ|FD_CLOSE);
   CAsyncSocket::OnAccept(nErrorCode);
   mListening = false;
   mConnected = true;
}

void CReceivingSocket::OnConnect(int nErrorCode)
{
    mConnected = true;
    CAsyncSocket::OnConnect(nErrorCode);
}

void CReceivingSocket::OnClose(int nErrorCode)
{
   TRACE("OnClose(%d)\n", nErrorCode);
   Close();
   mConnected = false;
   CAsyncSocket::OnClose(nErrorCode);
}

#define IDT_FIRST_UPDATE 100
#define IDT_CHECK_UPDATE 101

IMPLEMENT_DYNCREATE(CRemoteCurveControlCtrl, COleControl)

// Message map
BEGIN_MESSAGE_MAP(CRemoteCurveControlCtrl, COleControl)
    ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
    ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEWHEEL()
    ON_WM_MBUTTONUP()
    ON_WM_MBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_CONTEXT_PROPERTIES, &CRemoteCurveControlCtrl::OnUpdateContextProperties)
    ON_COMMAND(ID_HELP_ABOUT, &CRemoteCurveControlCtrl::OnHelpAbout)
	ON_UPDATE_COMMAND_UI(ID_HELP_ABOUT, &CRemoteCurveControlCtrl::OnUpdateHelpAbout)
	ON_COMMAND(ID_CONTEXT_PROPERTIES, &CRemoteCurveControlCtrl::OnContextProperties)
	ON_UPDATE_COMMAND_UI(ID_CONTEXT_SAVEPLOTASFILE, &CRemoteCurveControlCtrl::OnUpdateContextSaveplotasfile)
	ON_COMMAND(ID_CONTEXT_SAVEPLOTASFILE, &CRemoteCurveControlCtrl::OnContextSaveplotasfile)
	ON_COMMAND_RANGE(ID_CURVES_1ST, ID_CONTEXT_RANGES, &CRemoteCurveControlCtrl::OnCurves1st)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CURVES_1ST, ID_CONTEXT_RANGES, &CRemoteCurveControlCtrl::OnUpdateCurves1st)
	ON_COMMAND(ID_RANGES_NEXT, &CRemoteCurveControlCtrl::OnRangesNext)
	ON_UPDATE_COMMAND_UI(ID_RANGES_NEXT, &CRemoteCurveControlCtrl::OnUpdateRangesNext)
	ON_COMMAND(ID_RANGES_PREVIOUS, &CRemoteCurveControlCtrl::OnRangesPrevious)
	ON_UPDATE_COMMAND_UI(ID_RANGES_PREVIOUS, &CRemoteCurveControlCtrl::OnUpdateRangesPrevious)
	ON_COMMAND(ID_RANGES_DELETE_THIS, &CRemoteCurveControlCtrl::OnRangesDeleteThis)
	ON_UPDATE_COMMAND_UI(ID_RANGES_DELETE_THIS, &CRemoteCurveControlCtrl::OnUpdateRangesDeleteThis)
	ON_COMMAND(ID_RANGES_DELETEALL, &CRemoteCurveControlCtrl::OnRangesDeleteall)
	ON_UPDATE_COMMAND_UI(ID_RANGES_DELETEALL, &CRemoteCurveControlCtrl::OnUpdateRangesDeleteall)
END_MESSAGE_MAP()



// Dispatch map
BEGIN_DISPATCH_MAP(CRemoteCurveControlCtrl, COleControl)
    DISP_STOCKPROP_READYSTATE()
END_DISPATCH_MAP()



// Event map
BEGIN_EVENT_MAP(CRemoteCurveControlCtrl, COleControl)
END_EVENT_MAP()



// Property pages
// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CRemoteCurveControlCtrl, 1)
    PROPPAGEID(CRemoteCurveControlPropPage::guid)
END_PROPPAGEIDS(CRemoteCurveControlCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRemoteCurveControlCtrl, "REMOTECURVECONTR.RemoteCurveContrCtrl.1",
    0x3cc27841, 0x69c4, 0x477a, 0x94, 0x39, 0xe7, 0x5e, 0xf7, 0x7f, 0xcc, 0xf3)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CRemoteCurveControlCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DRemoteCurveControl =
        { 0x558E0221, 0x7485, 0x41F5, { 0xB7, 0x39, 0x20, 0x95, 0xCA, 0xE6, 0x49, 0x50 } };
const IID BASED_CODE IID_DRemoteCurveControlEvents =
        { 0x9377B6FC, 0x4B40, 0x4A3B, { 0xBA, 0xF1, 0xDD, 0xB4, 0xA7, 0x57, 0xB8, 0x5C } };

// Control type information

static const DWORD BASED_CODE _dwRemoteCurveControlOleMisc =
    OLEMISC_ACTIVATEWHENVISIBLE |
    OLEMISC_IGNOREACTIVATEWHENVISIBLE |
    OLEMISC_SETCLIENTSITEFIRST |
    OLEMISC_INSIDEOUT |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CRemoteCurveControlCtrl, IDS_REMOTECURVECONTROL, _dwRemoteCurveControlOleMisc)



// CRemoteCurveControlCtrl::CRemoteCurveControlCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CRemoteCurveControlCtrl

BOOL CRemoteCurveControlCtrl::CRemoteCurveControlCtrlFactory::UpdateRegistry(BOOL bRegister)
{
    // TODO: Verify that your control follows apartment-model threading rules.
    // Refer to MFC TechNote 64 for more information.
    // If your control does not conform to the apartment-model rules, then
    // you must modify the code below, changing the 6th parameter from
    // afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.
    if (bRegister)
    {
        return AfxOleRegisterControlClass(
            AfxGetInstanceHandle(),
            m_clsid,
            m_lpszProgID,
            IDS_REMOTECURVECONTROL,
            IDB_REMOTECURVECONTROL,
            afxRegInsertable | afxRegApartmentThreading,
            _dwRemoteCurveControlOleMisc,
            _tlid,
            _wVerMajor,
            _wVerMinor);
    }
    else
    {
        return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
    }
}


// Licensing strings
static const TCHAR BASED_CODE _szLicFileName[] = _T("RemoteCurveControl.lic");
//static const WCHAR BASED_CODE _szLicString[]   = L"Copyright (c) 2009 ";
static const WCHAR BASED_CODE _szLicString[]   = L"Copyright (c) L3-communications ELAC Nautik 2009";

// CRemoteCurveControlCtrl::CRemoteCurveControlCtrlFactory::VerifyUserLicense -
// Checks for existence of a user license
BOOL CRemoteCurveControlCtrl::CRemoteCurveControlCtrlFactory::VerifyUserLicense()
{
    TRACE("VerifyUserLicense\n");
    return TRUE;
    //return AfxVerifyLicFile(AfxGetInstanceHandle(), _szLicFileName, _szLicString);
}


// CRemoteCurveControlCtrl::CRemoteCurveControlCtrlFactory::GetLicenseKey -
// Returns a runtime licensing key
BOOL CRemoteCurveControlCtrl::CRemoteCurveControlCtrlFactory::GetLicenseKey(DWORD dwReserved, BSTR FAR* pbstrKey)
{
    TRACE("GetLicenseKey\n");
    if (pbstrKey == NULL)
        return FALSE;

    *pbstrKey = SysAllocString(_szLicString);
    return (*pbstrKey != NULL);
}

// CRemoteCurveControlCtrl::CRemoteCurveControlCtrl - Constructor
CRemoteCurveControlCtrl::CRemoteCurveControlCtrl():
mpPlot(NULL), mpPickList(NULL), mnPort(0), muFirstUpdate(0), muCheckUpdate(0), mbUpdateControl(FALSE), mbLbuttonDown(FALSE)
{
    InitializeIIDs(&IID_DRemoteCurveControl, &IID_DRemoteCurveControlEvents);
    mReceiver.SetControl(this);
    InitializeCriticalSection(&mcsPlot);

    InternalSetReadyState(READYSTATE_UNINITIALIZED);
    //m_lReadyState = READYSTATE_LOADING;
    TRACE("CRemoteCurveControlCtrl\n");
}



// CRemoteCurveControlCtrl::~CRemoteCurveControlCtrl - Destructor

CRemoteCurveControlCtrl::~CRemoteCurveControlCtrl()
{
    delete mpPlot;
    delete mpPickList;
    DeleteCriticalSection(&mcsPlot);
    CLabel::DeleteGraficObjects();
    TRACE("~CRemoteCurveControlCtrl()\n");
}


int CRemoteCurveControlCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (COleControl::OnCreate(lpCreateStruct) == -1)
        return -1;

    muFirstUpdate = SetTimer(IDT_FIRST_UPDATE, 200, NULL);
    muCheckUpdate = SetTimer(IDT_CHECK_UPDATE, 500, NULL);
//    GetDataSource()->DelayRenderFileData(CF_DIF);

    TRACE("OnCreate %d, %d\n", muFirstUpdate, muCheckUpdate);
    return 0;
}

void CRemoteCurveControlCtrl::OnTimer(UINT nIDEvent)
{
    //TRACE("OnTimer(%d, %d, %d)\n", nIDEvent, mReceiver.IsConnected(), mReceiver.IsListening());
    if (nIDEvent == muFirstUpdate)
    {
        TRACE("OnTimer(%d)\n", nIDEvent);
        KillTimer(muFirstUpdate);
        muFirstUpdate = 0;
        InvalidateControl();
    }
    if (nIDEvent == muCheckUpdate)
    {
        if (mbUpdateControl && !mbLbuttonDown)
        {
            if (mpPlot) 
            {
                AutoCS cs(&mcsPlot);
                if (mpPlot->GetNoOfRanges() <= 1)
                {
                    mpPlot->DeterminDefaultRange();
                }
                mpPlot->CheckCurveColors();
            }
            InvalidateControl();
            mbUpdateControl = FALSE;
        }

        if (mReceiver.IsConnected())
        {
            mReceiver.AsyncSelect(FD_READ|FD_CLOSE);
        }
        else
        {
            CreateReceivingSocket();
        }
    }

    COleControl::OnTimer(nIDEvent);
}

// CRemoteCurveControlCtrl::OnDraw - Drawing function
void CRemoteCurveControlCtrl::OnPrepareDC(CDC*pDC)
{
    pDC->SetMapMode(MM_LOMETRIC);
}

void CRemoteCurveControlCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
    InternalSetReadyState(READYSTATE_COMPLETE );
    if (!pdc) return;
    OnPrepareDC(pdc);
    TRACE("OnDraw\n");
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
        mpPlot->Draw(pdc);
    }
}

void CRemoteCurveControlCtrl::OnSize(UINT nType, int cx, int cy)
{
    COleControl::OnSize(nType, cx, cy);
    CDC     *pDC = GetDC();
    OnPrepareDC(pDC);
    CPoint size  = CLabel::PixelToLogical(pDC, cx-1, cy-1);
    TRACE("OnSize(%d, %d\n", cx, cy);

    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
        mpPlot->SetRect(CRect(0, 0, size.x, size.y));
    }
    ReleaseDC(pDC);
}


// CRemoteCurveControlCtrl::DoPropExchange - Persistence support
BOOL CRemoteCurveControlCtrl::ReadColor(CPropExchange*pPX, const CString& name, COLORREF &color)
{
    CString txt;
    long red=0, green=0, blue=0;

    PX_String(pPX, name, txt, NULL);
    LPCTSTR pstr = txt;
    if (!txt.IsEmpty())
    {
        if (txt.GetAt(0) == '#')
        {
            sscanf(txt.Mid(1), "%x", &color);
        }
        else if (sscanf(pstr, "%d,%d,%d", &red, &green, &blue) == 3)
        {
            color = RGB(red, green, blue);
        }
        else if (sscanf(pstr, "%d%%,%d%%,%d%%", &red, &green, &blue) == 3)
        {
            red = MulDiv(red, 255, 100);
            green = MulDiv(green, 255, 100);
            blue = MulDiv(blue, 255, 100);
            color = RGB(red, green, blue);
        }
        TRACE("ReadColor(%s, %d, %d, %d)\n", LPCTSTR(name), GetRValue(color), GetGValue(color), GetBValue(color));
        return TRUE;
    }
    return FALSE;
}

BOOL CRemoteCurveControlCtrl::ReadGridStep(CPropExchange*pPX, const CString& name, WORD &wStep)
{
    CString txt;
    PX_String(pPX, name, txt, NULL);
    if (!txt.IsEmpty())
    {
        float dist;
        if (sscanf(txt, "%f", &dist) == 1)
        {
            wStep = (WORD)(dist*10.0f);
        }
        if (txt.Find("NoText") != -1) wStep |= HIDE_GRID_TEXT;
        if (txt.Find("NoLines") != -1) wStep |= HIDE_GRID_LINE;
        return TRUE;
    }
    return FALSE;
}

void CRemoteCurveControlCtrl::CreateReceivingSocket()
{
    if (   mnPort 
        && theApp.InitSocket() 
        && !mReceiver.IsConnected())
    {
        if (   msUrl.IsEmpty() 
            && !mReceiver.IsListening())
        {
            if (mReceiver.Create(mnPort))
            {
                mReceiver.AsyncSelect(FD_READ|FD_ACCEPT|FD_CLOSE);
                if (mReceiver.Listen())
                {
                    mReceiver.SetListening();
                    TRACE("Lintening on Port %d\n", mnPort);
                }
            }
        }
        else
        {
            mReceiver.Connect(msUrl, mnPort);
        }
    }
}

void CRemoteCurveControlCtrl::DoPropExchange(CPropExchange* pPX)
{
    InternalSetReadyState(READYSTATE_LOADING);
    ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
    COleControl::DoPropExchange(pPX);
    unsigned long  n = 0;

    PX_String(pPX, "URL"    , msUrl    , NULL);
    PX_ULong( pPX, "Port"   , mnPort   , 0);

    CreateReceivingSocket();

    TRACE("DoPropExchange\n");
    CCurveLabel *pcurve;
    // read number of curves
    PX_ULong( pPX, "Curves", n, 0);

    if (n != 0)
    {
        CString fmt, txt, yval, xval;
        LPCTSTR ptxt;
        unsigned long i, j, m, commas, ncolors = 0;
        WORD wStep;
        LOGBRUSH lb;
        int angleformat = CCURVE_RAD;
        // create a plot object
        AutoCS cs(&mcsPlot);
        mpPlot = new CPlotLabel();
        mpPickList = new CLabelContainer();
    
        PX_String(pPX, "data", txt, NULL);
        
        // read heading
        PX_String(pPX, "Title", txt, NULL);
        mpPlot->SetHeading(txt);

        // read background color
        if (!ReadColor(pPX, "BackGroundColor", lb.lbColor))
        {
            lb.lbColor = RGB(255,255,255);
        }
        lb.lbStyle = BS_SOLID;
        mpPlot->SetLogBrush(&lb);

        // read grid color
        if (ReadColor(pPX, "GridColor", lb.lbColor))
        {
            mpPlot->SetGridLineColor(lb.lbColor);
        }
        
        // read text color
        if (ReadColor(pPX, "TextColor", lb.lbColor))
        {
            mpPlot->SetGridTextColor(lb.lbColor);
        }

        // read heading color
        if (ReadColor(pPX, "HeadingColor", lb.lbColor))
        {
            mpPlot->SetHeadingColor(lb.lbColor);
        }

        // read grid line settings
        wStep = mpPlot->GetXGridStep();
        if (ReadGridStep(pPX, "XGridLineDistance", wStep))
        {
            mpPlot->SetXGridStep(wStep);
        }
        wStep = mpPlot->GetYGridStep();
        if (ReadGridStep(pPX, "YGridLineDistance", wStep))
        {
            mpPlot->SetYGridStep(wStep);
        }

        // x axis division
        txt.Empty();
        PX_String(pPX, "XDivision", txt, NULL);
        if      (txt == "logarithmic") mpPlot->SetXDivision(ETSDIV_LOGARITHMIC);
        else if (txt == "frequency")   mpPlot->SetXDivision(ETSDIV_FREQUENCY);
        else if (txt == "polar")       mpPlot->SetXDivision(ETSDIV_POLAR);
        
        txt.Empty();
        PX_String(pPX, "Angle", txt, NULL);
        if      (txt == "grad") angleformat = CCURVE_GRAD;
        else if (txt == "gon")  angleformat = CCURVE_GON;
        else if (txt == "rad")  angleformat = CCURVE_RAD;

        // y axis division
        txt.Empty();
        PX_String(pPX, "YDivision", txt, NULL);
        if      (txt == "logarithmic") mpPlot->SetYDivision(ETSDIV_LOGARITHMIC);
        else if (txt == "frequency")   mpPlot->SetYDivision(ETSDIV_FREQUENCY);

        // read fonts ?

        // read curves
        for (i=0; i<n; i++)
        {
            // read n-th y values
            fmt.Format("YValues%d", i+1);
            yval.Empty();
            PX_String(pPX, fmt, yval, NULL);
            m = yval.GetLength();
            ptxt = yval;
            if (m == 0) break; // verify content

            // determine number of values
            commas=0;
            for (j=0; j<m; j++)
            {
                if (ptxt[j] == ',') commas++;
            }
            if (commas == 0) break; // at least 2 values
            commas++;

            // create curve object
            pcurve = new CCurveLabel();
            pcurve->SetXDivision(mpPlot->GetXDivision());
            pcurve->SetYDivision(mpPlot->GetYDivision());
            pcurve->SetAngleFormat(angleformat);

            // read format of curve
            fmt.Format("Format%d", i+1);
            txt.Empty();
            PX_String(pPX, fmt, txt, NULL);
            if      (txt == "double") pcurve->SetFormat(CF_SINGLE_DOUBLE);
            else if (txt == "short" ) pcurve->SetFormat(CF_SINGLE_SHORT);
            else /* default: float */ pcurve->SetFormat(CF_SINGLE_FLOAT);

            // read n-th xvalues if available
            fmt.Format("XValues%d", i+1);
            xval.Empty();
            PX_String(pPX, fmt, xval, NULL);
            if (xval.GetLength())
            {
                switch (pcurve->GetFormat())
                {
                    case CF_SINGLE_SHORT:  pcurve->SetFormat(CF_SHORT);  break;
                    case CF_SINGLE_FLOAT:  pcurve->SetFormat(CF_FLOAT);  break;
                    case CF_SINGLE_DOUBLE: pcurve->SetFormat(CF_DOUBLE); break;
                }
                pcurve->SetSize(commas); // set size after format
                int comma = 0;
                m = xval.GetLength();   // apply x values
                ptxt = xval;
                pcurve->SetX_Value(comma++, atof(ptxt));
                for (j=1; j<m; j++)
                {
                    if (ptxt[j] == ',')
                        pcurve->SetX_Value(comma++, atof(&ptxt[j+1]));
                }
            }
            else
            {
                pcurve->SetSize(commas); // set size with single value format
            }

            // read x range if available
            fmt.Format("XRange%d", i+1);
            xval.Empty();
            PX_String(pPX, fmt, xval, NULL);
            if (xval.GetLength())
            {
                m = xval.GetLength();   // apply x range
                ptxt = xval;
                pcurve->SetX_Value(0, atof(ptxt));
                for (j=1; j<m; j++)
                {
                    if (ptxt[j] == ',')
                    {
                        pcurve->SetX_Value(commas-1, atof(&ptxt[j+1]));
                        break;
                    }
                }
            }

            m = yval.GetLength();   // apply y values
            ptxt = yval;
            commas = 0;
            pcurve->SetY_Value(commas++, atof(ptxt));
            for (j=1; j<m; j++)
            {
                if (ptxt[j] == ',')
                    pcurve->SetY_Value(commas++, atof(&ptxt[j+1]));
            }

            // for range determination
            pcurve->SetSorted(false);
            pcurve->TestXSorted();

            // read name of curve
            fmt.Format("Name%d", i+1);
            txt.Empty();
            PX_String(pPX, fmt, txt, NULL);
            if (txt.GetLength())
            {
                pcurve->SetText(txt);
            }

            // read color of curve
            fmt.Format("Color%d", i+1);
            if (ReadColor(pPX, fmt, lb.lbColor))
            {
                pcurve->SetColor(lb.lbColor);
                ncolors++;
            }
            mpPlot->InsertCurve(pcurve);
        }

        // arrange curves
        mpPlot->DeterminDefaultRange();
        if (ncolors == 0)
        {
            mpPlot->CheckCurveColors();
        }
        // read Units
        txt.Empty();
        PX_String(pPX, "XUnit", txt, NULL);
        mpPlot->SetXUnit((char*)(LPCTSTR)txt);
        txt.Empty();
        PX_String(pPX, "YUnit", txt, NULL);
        mpPlot->SetYUnit((char*)(LPCTSTR)txt);
    }
    InternalSetReadyState(READYSTATE_LOADED);
}

struct CurveData
{
    long identifier;
    short name_length;
    short curve_format;
    long  no_of_data;
    int GetID(bool &bBigEndian)
    {
        switch (identifier)
        {
            case 0x010c0b0a: return 1;  // 
            case 0x020c0b0a: return 2;  // extended single with x range
            default: break;
        }
        if (!bBigEndian)
        {
            bBigEndian = true;
            identifier   = htonl(identifier);
            curve_format = htons(curve_format);
            name_length  = htons(name_length);
            no_of_data   = htonl(no_of_data);
            return GetID(bBigEndian);
        }
        return 0;
    }
};

void endianconvert(BYTE *buf, int size)
{
    BYTE temp;
    int i, j, n = size / 2;
    for (i=0, j=size-1; i<n; i++, j--)
    {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
    }
}

void SplitStrings(const CString &name, const CString &delimiter, CStringArray &names)
{
    int n1 = name.Find(delimiter), n2 = 0, count = name.GetLength();
    int delimiterlength = delimiter.GetLength();
    while (n1 != -1)
    {
        names.Add(name.Mid(n2, n1- n2));
        n2 = n1+delimiterlength;
        count = name.GetLength() - n2;
        n1 = name.Find(delimiter, n2);
    }
    names.Add(name.Right(count));
}

void CRemoteCurveControlCtrl::OnReceive(CReceivingSocket*pSock, int nErrorcode)
{
    CString name;
    CStringArray names;
    CurveData cd;
    int received;
    int nID;
    bool bigendian = false;
    AutoCS cs(&mcsPlot);
    do
    {
        received = pSock->Receive(&cd, sizeof(CurveData));
		TRACE("OnReceive(%d)\n", received);
        if (received == sizeof(CurveData))
        {
            nID = cd.GetID(bigendian);
            if (nID)
            {
                CCurveLabel *pcurve = 0;
                CLabelContainer*plc = mpPlot->GetCurveList();
                int i, size, n = 0, factor = 1;
                BYTE array[sizeof(double)*2];
                if (!plc->IsEmpty()) n = plc->GetCount();

                switch (cd.curve_format)
                {
                    case CF_DOUBLE:        size = sizeof(double); factor = 2; break;
                    case CF_SINGLE_DOUBLE: size = sizeof(double); break;
                    case CF_FLOAT:         size = sizeof(float);  factor = 2; break;
                    case CF_SINGLE_FLOAT:  size = sizeof(float);  break;
                    case CF_SHORT:         size = sizeof(short);  factor = 2; break;
                    case CF_SINGLE_SHORT:  size = sizeof(short);  break;
                }

                pSock->Receive(name.GetBufferSetLength(cd.name_length), cd.name_length);
                name.ReleaseBuffer();
                SplitStrings(name, ";", names);

                for (i=0; i<n; i++)
                {
                    CLabel*pl = mpPlot->GetCurveList()->GetLabel(i);
                    if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
                    {
                        if (name == ((CCurveLabel*)pl)->GetText())
                        {
                            pcurve = (CCurveLabel*) pl;
                            break;
                        }
                    }
                }

                if (pcurve == NULL)
                {
                    pcurve = new CCurveLabel;
                    pcurve->SetText(names[0]);
                    mpPlot->InsertCurve(pcurve);
                    mpPlot->SetChanged(true);
                    if (names.GetCount() >= 2)
                    {
                        pcurve->SetXUnit(names[1]);
                    }
                    if (names.GetCount() >= 3)
                    {
                        pcurve->SetYUnit(names[2]);
                    }
                }
                pcurve->SetFormat(cd.curve_format);
                pcurve->SetSize(cd.no_of_data);
                n = pcurve->GetArraySize();

                BYTE*buf = (BYTE*)pcurve->GetArrayPointer();
                i = 0;
                do
                {
                    i += pSock->Receive((void*)&buf[i], n-i);
                }while (i < n);

                if (nID == 2)
                {
                    pSock->Receive(&array[0], size*2);
                }

                if (bigendian)
                {
                    int n = cd.no_of_data*factor;
                    buf = (BYTE*)pcurve->GetArrayPointer();
                    for (i=0; i<n; i++)
                    {
                        endianconvert(buf, size);
                        buf += size;
                    }
                    if (nID == 2)
                    {
                        buf = &array[0];
                        endianconvert(buf, size);
                        buf += size;
                        endianconvert(buf, size);
                    }
                }

                if (nID == 2)
                {
                    buf = &array[0];
                    switch (cd.curve_format)
                    {
                        case CF_SINGLE_DOUBLE: 
                            pcurve->SetX_Value(0, *(double*)buf);
                            pcurve->SetX_Value(cd.no_of_data-1, *(double*)(buf+size));
                            break;
                        case CF_SINGLE_FLOAT:
                            pcurve->SetX_Value(0, *(float*)buf);
                            pcurve->SetX_Value(cd.no_of_data-1, *(float*)(buf+size));
                            break;
                        case CF_SINGLE_SHORT:
                            pcurve->SetX_Value(0, *(short*)buf);
                            pcurve->SetX_Value(cd.no_of_data-1, *(short*)(buf+size));
                            break;
                    }
                }
            }
        }
    }
    while (received != sizeof(CurveData));
    mbUpdateControl = TRUE;
}

BOOL CRemoteCurveControlCtrl::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
    CString str;
    pFile->Read(str.GetBufferSetLength(20), 20);
    str.Empty();
    return TRUE;
}

// CRemoteCurveControlCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
DWORD CRemoteCurveControlCtrl::GetControlFlags()
{
    DWORD dwFlags = COleControl::GetControlFlags();

    // The control will not be redrawn when making the transition
    // between the active and inactivate state.
    //dwFlags |= fastBeginPaint;
    dwFlags |= clipPaintDC;
    dwFlags |= pointerInactive;
    //dwFlags |= noFlickerActivate;
    dwFlags |= canOptimizeDraw;
    //TRACE("GetControlFlags: %x\n", dwFlags);

    // The control can receive mouse notifications when inactive.
    // TODO: if you write handlers for WM_SETCURSOR and WM_MOUSEMOVE,
    //        avoid using the m_hWnd member variable without first
    //        checking that its value is non-NULL.
    //dwFlags |= pointerInactive;
    return dwFlags;
}



// CRemoteCurveControlCtrl::OnResetState - Reset control to default state

void CRemoteCurveControlCtrl::OnResetState()
{
    COleControl::OnResetState();  // Resets defaults found in DoPropExchange
    TRACE("OnResetState()\n");
    // TODO: Reset any other control state here.
}



// CRemoteCurveControlCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CRemoteCurveControlCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.lpszClass = _T("STATIC");
    cs.style |= SS_NOTIFY;
    return COleControl::PreCreateWindow(cs);
}



// CRemoteCurveControlCtrl::IsSubclassedControl - This is a subclassed control

BOOL CRemoteCurveControlCtrl::IsSubclassedControl()
{
    return TRUE;
}

#ifdef USE_IOBJECTSAFETY
// CRemoteCurveControlCtrl message handlers
STDMETHODIMP CRemoteCurveControlCtrl::XObjectSafety::GetInterfaceSafetyOptions(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions)
{
    if (pdwSupportedOptions == NULL) return E_POINTER;
    if (pdwEnabledOptions == NULL)   return E_POINTER;

    *pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER|INTERFACESAFE_FOR_UNTRUSTED_DATA;
    *pdwEnabledOptions   = INTERFACESAFE_FOR_UNTRUSTED_CALLER|INTERFACESAFE_FOR_UNTRUSTED_DATA;
    TRACE("GetInterfaceSafetyOptions()\n");
     
    return S_OK;
}

STDMETHODIMP CRemoteCurveControlCtrl::XObjectSafety::SetInterfaceSafetyOptions(REFIID riid, DWORD dwSupportedOptions, DWORD dwEnabledOptions)
{
    TRACE("SetInterfaceSafetyOptions(%x,%x)", dwSupportedOptions, dwEnabledOptions);
    return S_OK;
}

STDMETHODIMP_(ULONG) CRemoteCurveControlCtrl::XObjectSafety::Release(void)
{
    TRACE("Release()\n");
	METHOD_PROLOGUE_EX_(CRemoteCurveControlCtrl, ObjectSafety)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP_(ULONG) CRemoteCurveControlCtrl::XObjectSafety::AddRef(void)
{
    TRACE("AddRef\n");
	METHOD_PROLOGUE_EX_(CRemoteCurveControlCtrl, ObjectSafety)
	return (ULONG)pThis->ExternalAddRef();
}
STDMETHODIMP CRemoteCurveControlCtrl::XObjectSafety::QueryInterface(struct _GUID const &iid, void **ppvObj)
{
    TRACE("QueryInterface\n");
	METHOD_PROLOGUE_EX_(CRemoteCurveControlCtrl, ObjectSafety)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}
#endif

BEGIN_INTERFACE_MAP(CRemoteCurveControlCtrl, COleControl)
#ifdef USE_IOBJECTSAFETY
    INTERFACE_PART(CRemoteCurveControlCtrl, IID_IObjectSafety, ObjectSafety)
#endif
END_INTERFACE_MAP()

inline void CRemoteCurveControlCtrl::SetMouseCapture()
{
   if (GetCapture() != this) SetCapture();
}

inline void CRemoteCurveControlCtrl::ReleaseMouseCapture()
{
   if (GetCapture() == this) ReleaseCapture();
}

void CRemoteCurveControlCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_hWnd == 0) return;
    CDC     *pDC = GetDC();
    OnPrepareDC(pDC);
    pDC->DPtoLP(&point);
    mbLbuttonDown = TRUE;

    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
        if (mpPlot->GetXDivision() == ETSDIV_POLAR)
        {
            mpPlot->ZoomPolarPlot(pDC, point, WM_LBUTTONDOWN);
        }
        else
        {
            mpPickList->RemoveAll();
            ((CLabel*)mpPickList)->SetPoint(point, 0);
            ((CLabel*)mpPickList)->SetPoint(point, 1);
            mpPickList->SetChanged(false);
            mpPickList->SetPicked(true);
            mpPickList->DrawShape(pDC, true);
        }
    }

    ReleaseDC(pDC);
    SetMouseCapture();

    COleControl::OnLButtonDown(nFlags, point);
}

void CRemoteCurveControlCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    CPoint ptDiff = point - m_OldMousePoint;

    if (m_hWnd == 0) return;
    CDC     *pDC = GetDC();
    OnPrepareDC(pDC);
    pDC->DPtoLP(&point);

    if (nFlags&MK_LBUTTON)
    {
        AutoCS cs(&mcsPlot);
        if (mpPlot && mpPlot->GetXDivision() == ETSDIV_POLAR)
        {
            mpPlot->ZoomPolarPlot(pDC, point, WM_MOUSEMOVE);
        }
        else
        {
            ptDiff = CLabel::PixelToLogical(pDC, ptDiff.x, ptDiff.y);
            mpPickList->EditLabelPoints(&point, &ptDiff, pDC);
        }
    }
    ReleaseDC(pDC);
    m_OldMousePoint = point;
    COleControl::OnMouseMove(nFlags, point);
}

void CRemoteCurveControlCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_hWnd == 0) return;
    ReleaseMouseCapture();
    CDC     *pDC = GetDC();
    OnPrepareDC(pDC);
    pDC->DPtoLP(&point);
 
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
        if (mpPlot->GetXDivision() == ETSDIV_POLAR)
        {
            if (mpPlot->ZoomPolarPlot(pDC, point, WM_LBUTTONUP))
            {
                cs.Release();
                InvalidateControl();
            }
        }
        else if (mpPickList->IsPicked())
        {
            mpPickList->OnPickRect(&point, NULL, pDC, 0, NULL, true);
            mpPickList->SetPicked(false);
            CRect zoomrect = mpPickList->GetRect(pDC);                // das durch PickList gezogene Rechteck ermitteln
            if (!zoomrect.IsRectEmpty())                              // ist das Zoom Rechteck leer: Ende
            {
                CRect viewrect = mpPlot->GetView();                   // Plotrechteck ermitteln
                if (zoomrect.IntersectRect(&zoomrect, &viewrect))     // Ist der Mauspunkt nicht im Plotrechteck: Ende
                {
                    if (mpPlot->SetRangeInLogical(pDC, zoomrect))     // und setzen des Bereichs in logischen Koordinaten
                    {
                        cs.Release();
                        InvalidateControl();
                    }
                }
            }
        }
    }

    ReleaseDC(pDC);

    mbLbuttonDown = FALSE;
    COleControl::OnLButtonUp(nFlags, point);
}

BOOL CRemoteCurveControlCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Add your message handler code here and/or call default

    return COleControl::OnMouseWheel(nFlags, zDelta, pt);
}

void CRemoteCurveControlCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    COleControl::OnMButtonDown(nFlags, point);
}

void CRemoteCurveControlCtrl::OnMButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    COleControl::OnMButtonUp(nFlags, point);
}

void CRemoteCurveControlCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CCaraMenu  ContextMenu;
	ContextMenu.LoadMenuA(IDR_CONTEXT1);
	ClientToScreen(&point);
	CMenu*pSub = ContextMenu.GetSubMenu(0);
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		CMenu*pCurves = pSub->GetSubMenu(0);
		CLabelContainer *plc = mpPlot->GetCurveList();
		if (plc && plc->GetCount())
		{
			int i, n = plc->GetCount();
			const char *txt = ((CCurveLabel*)plc->GetLabel(0))->GetText();
			pCurves->ModifyMenuA(0, MF_BYPOSITION|MF_STRING, ID_CURVES_1ST, txt);
			for (i=1; i<n; i++)
			{
				txt = ((CCurveLabel*)plc->GetLabel(i))->GetText();
				pCurves->AppendMenuA(MF_STRING, ID_CURVES_1ST+i, txt);
			}
		}
    }
	cs.Release();
    pSub->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
    ContextMenu.DeleteStrings();
    ContextMenu.DestroyMenu(); 
    COleControl::OnRButtonDown(nFlags, point);
}

void CRemoteCurveControlCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    COleControl::OnRButtonUp(nFlags, point);
}

void CRemoteCurveControlCtrl::OnContextProperties()
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		HINSTANCE hOld = AfxGetResourceHandle();
		AfxSetResourceHandle(LoadLibrary("ETSPREV.dll"));
		CCaraViewProperties properties("", this);
		properties.InitDialog(2, mpPlot);
		if (   properties.DoModal() == IDOK
			&& mpPlot->IsChanged())
		{
			cs.Release();
			InvalidateControl();
		}
		AfxSetResourceHandle(hOld);
    }
}

void CRemoteCurveControlCtrl::OnUpdateContextProperties(CCmdUI *pCmdUI)
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		pCmdUI->Enable();
    }
}

void CRemoteCurveControlCtrl::OnHelpAbout()
{
    CAboutDlg about;
    about.DoModal();
}

void CRemoteCurveControlCtrl::OnUpdateHelpAbout(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

void CRemoteCurveControlCtrl::OnContextSaveplotasfile()
{
	CFileDialog dlg(FALSE, "plt");
	if (dlg.DoModal() == IDOK)
	{
		CFile file;
		if (file.Open(dlg.GetPathName( ), CFile::modeCreate|CFile::modeWrite))
		{
			AutoCS cs(&mcsPlot);
			if (mpPlot)
			{
				mpPlot->Write(file.m_hFile);
			}
			file.Close();
		}
	}

}

void CRemoteCurveControlCtrl::OnUpdateContextSaveplotasfile(CCmdUI *pCmdUI)
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		pCmdUI->Enable();
    }
}


void CRemoteCurveControlCtrl::OnCurves1st(UINT nID)
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		CLabel*pl = mpPlot->GetCurveList()->GetLabel(nID-ID_CURVES_1ST);
		if (pl)
		{
			pl->SetVisible(!pl->IsVisible());
			cs.Release();
			InvalidateControl();
		}
    }
}

void CRemoteCurveControlCtrl::OnUpdateCurves1st(CCmdUI *pCmdUI)
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		CLabel*pl = mpPlot->GetCurveList()->GetLabel(pCmdUI->m_nID-ID_CURVES_1ST);
		pCmdUI->Enable(pl != NULL);
		pCmdUI->SetCheck(pl && pl->IsVisible());
    }
}

void CRemoteCurveControlCtrl::OnRangesNext()
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		mpPlot->Zoom();
		cs.Release();
		InvalidateControl();
    }
}

void CRemoteCurveControlCtrl::OnUpdateRangesNext(CCmdUI *pCmdUI)
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		pCmdUI->Enable(mpPlot->CheckZoom());
    }
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRemoteCurveControlCtrl::OnRangesPrevious()
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		mpPlot->ReZoom();
		cs.Release();
		InvalidateControl();
    }
}

void CRemoteCurveControlCtrl::OnUpdateRangesPrevious(CCmdUI *pCmdUI)
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		pCmdUI->Enable(mpPlot->CheckReZoom());
    }
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRemoteCurveControlCtrl::OnRangesDeleteThis()
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
        mpPlot->DeleteCurrentRange();
		cs.Release();
		InvalidateControl();
    }
}

void CRemoteCurveControlCtrl::OnUpdateRangesDeleteThis(CCmdUI *pCmdUI)
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		pCmdUI->Enable(mpPlot->GetNoOfRanges() > 1);
    }
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRemoteCurveControlCtrl::OnRangesDeleteall()
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		mpPlot->DeterminDefaultRange();
		cs.Release();
		InvalidateControl();
    }
}

void CRemoteCurveControlCtrl::OnUpdateRangesDeleteall(CCmdUI *pCmdUI)
{
    AutoCS cs(&mcsPlot);
    if (mpPlot)
    {
		pCmdUI->Enable(mpPlot->GetNoOfRanges() > 1);
    }
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
