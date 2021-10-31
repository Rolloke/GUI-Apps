
         if (0 == (MIXERLINE_LINEF_SOURCE & mxl.fdwLine))         // Keine Quelle sondern Destination
         {
            switch (mxl.dwComponentType)                          // Was
            {
               case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:         // Lautsprecher Ok
                  break;
               case MIXERLINE_COMPONENTTYPE_DST_DIGITAL:          // Digital   ?
               case MIXERLINE_COMPONENTTYPE_DST_HEADPHONES:       // Kopfhörer ?
               case MIXERLINE_COMPONENTTYPE_DST_LINE:             // Line      ?
               case MIXERLINE_COMPONENTTYPE_DST_MONITOR:          // Monitor   ?
               case MIXERLINE_COMPONENTTYPE_DST_WAVEIN:           // WaveIn    ?
               case MIXERLINE_COMPONENTTYPE_DST_VOICEIN:          // VoiceIn   ?
               case MIXERLINE_COMPONENTTYPE_DST_TELEPHONE:        // Telefon   ?
               case MIXERLINE_COMPONENTTYPE_DST_UNDEFINED:        // wat denn nu
               default:
                   continue;
                   break;
            }
         }

  MIXERLINECONTROLS   mxlc;
    MMRESULT            mmr;
    BOOL                f;
    MACONTROLINSTANCE   maci;


    //
    //
    //
    //
    mxlc.cbStruct    = sizeof(mxlc);
////mxlc.dwLineID    = 0;
    mxlc.dwControlID = dwControlID;
////mxlc.cControls   = 1;
    mxlc.cbmxctrl    = sizeof(gmxctrl);
    mxlc.pamxctrl    = &gmxctrl;

    mmr = mixerGetLineControls((HMIXEROBJ)ghmx, &mxlc, MIXER_GETLINECONTROLSF_ONEBYID);
    if (MMSYSERR_NOERROR != mmr)
    {
        AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                  "mixerGetLineControls(ctrlid=%.08lXh) failed on hmx=%.04Xh, mmr=%u!",
                  dwControlID, ghmx, mmr);
        return (FALSE);
    }


    //
    //
    //
    if (MIXERCONTROL_CONTROLF_DISABLED & gmxctrl.fdwControl)
    {
        AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                  "This control (ctrlid=%.08lXh) is disabled.",
                  dwControlID);
        return (FALSE);
    }


    //
    //
    //
    gfDisplayingControl = TRUE;
    gdwControlID        = dwControlID;


    //
    //
    //
    maci.hmx        = ghmx;
    maci.pmxl       = &gmxl;
    maci.pmxctrl    = &gmxctrl;



    //
    //
    //
    //
    f = FALSE;
    switch (MIXERCONTROL_CT_CLASS_MASK & gmxctrl.dwControlType)
    {
        case MIXERCONTROL_CT_CLASS_CUSTOM:
            AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                      "Control type class CUSTOM not done yet!");
            break;

        case MIXERCONTROL_CT_CLASS_METER:
            f = DialogBoxParam(ghinst,
                               DLG_MIXAPP_CONTROL,
                               hwnd,
                               MixAppDlgProcControlMeter,
                               (LPARAM)(LPVOID)&maci);
            break;

        case MIXERCONTROL_CT_CLASS_SWITCH:
            f = DialogBoxParam(ghinst,
                               DLG_MIXAPP_CONTROL,
                               hwnd,
                               MixAppDlgProcControlSwitch,
                               (LPARAM)(LPVOID)&maci);
            break;

        case MIXERCONTROL_CT_CLASS_NUMBER:
            AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                      "Control type class NUMBER not done yet!");
            break;

        case MIXERCONTROL_CT_CLASS_SLIDER:
            AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                      "Control type class SLIDER not done yet!");
            break;

        case MIXERCONTROL_CT_CLASS_FADER:
            f = DialogBoxParam(ghinst,
                               DLG_MIXAPP_CONTROL,
                               hwnd,
                               MixAppDlgProcControlFader,
                               (LPARAM)(LPVOID)&maci);
            break;

        case MIXERCONTROL_CT_CLASS_LIST:
            f = DialogBoxParam(ghinst,
                               DLG_MIXAPP_CONTROL,
                               hwnd,
                               MixAppDlgProcControlList,
                               (LPARAM)(LPVOID)&maci);
            break;

        default:
            AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                      "Unknown control type class=%.08lXh!",
                      gmxctrl.dwControlType);
            break;
    }

    gfDisplayingControl = FALSE;

    //
    //
    //
    if (!f)
    {
        AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                  "Do not know how to deal with control type=%.08lXh!",
                  gmxctrl.dwControlType);
    }

    return (f);
} // MixAppDisplayControl()

BOOL CMixerDlg::MixAppInitDialogFader(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    LPMACONTROLINSTANCE         pmaci;
    PMACONTROLINSTANCE_FADER    pmaci_fader;

    LPMIXERLINE                 pmxl;
    LPMIXERCONTROL              pmxctrl;

    HWND                        htxt;
    UINT                        u;
    UINT                        v;
    HWND                        hsb;
    RECT                        rcM;
    RECT                        rcU;
    int                         cxvsb;
    UINT                        cb;
    UINT                        cChannels;
    UINT                        cMultipleItems;
    UINT                        uIndex;
    int                         nRange;
    int                         nPageInc;

    //
    //
    //
    pmaci = (LPMACONTROLINSTANCE)lParam;
    if (NULL == pmaci)
    {
        DPF(0, "!MixAppInitDialogFader: pmaci passed in lParam is NULL!?!");
        return (FALSE);
    }

    pmxl    = pmaci->pmxl;
    pmxctrl = pmaci->pmxctrl;

    if (!MixAppGetControlTypeName(pmxctrl, szControlType))
    {
        return (FALSE);
    }

    //
    //
    //
    switch (pmxctrl->dwControlType)
    {
        case MIXERCONTROL_CONTROLTYPE_FADER:
        case MIXERCONTROL_CONTROLTYPE_VOLUME:
        case MIXERCONTROL_CONTROLTYPE_BASS:
        case MIXERCONTROL_CONTROLTYPE_TREBLE:
        case MIXERCONTROL_CONTROLTYPE_EQUALIZER:
            break;

        default:
            DPF(0, "!MixAppInitDialogFader: pmxctrl->dwControlType=%.08lXh not valid fader!", pmxctrl->dwControlType);
            return (FALSE);
    }




    //
    //
    //
    AppSetWindowText(hwnd, szTitle, (LPTSTR)szControlType);
    SetDlgItemText(hwnd, IDD_MACONTROL_TXT_SHORT_NAME, pmxctrl->szShortName);
    SetDlgItemText(hwnd, IDD_MACONTROL_TXT_LONG_NAME,  pmxctrl->szName);

    htxt = GetDlgItem(hwnd, IDD_MACONTROL_TXT_BOUNDS);
    AppSetWindowText(htxt, "dwMinimum=%lu, dwMaximum=%lu",
                     pmxctrl->Bounds.dwMinimum,
                     pmxctrl->Bounds.dwMaximum);

    htxt = GetDlgItem(hwnd, IDD_MACONTROL_TXT_METRICS);
    AppSetWindowText(htxt, "cSteps=%lu",
                     pmxctrl->Metrics.cSteps);



    //
    //
    //
    cChannels = (UINT)pmxl->cChannels;
    if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl->fdwControl)
        cChannels = 1;

    cMultipleItems = 1;
    if (MIXERCONTROL_CONTROLF_MULTIPLE & pmxctrl->fdwControl)
        cMultipleItems = (UINT)pmxctrl->cMultipleItems;

    cb  = sizeof(*pmaci_fader);
    cb += cChannels * cMultipleItems * sizeof(pmaci_fader->pmxcd_u[0]);
    pmaci_fader = (PMACONTROLINSTANCE_FADER)LocalAlloc(LPTR, cb);
    if (NULL == pmaci_fader)
    {
        DPF(0, "!MixAppInitDialogFader: failed trying to alloc %u bytes for control instance!", cb);
        return (FALSE);
    }


    //
    //
    //
    SetWindowLong(hwnd, DWL_USER, (LPARAM)(UINT)pmaci_fader);

    nRange   = (int)min(32767, pmxctrl->Metrics.cSteps - 1);
    nPageInc = nRange / 10;
    if (0 == nPageInc)
        nPageInc = 1;

    pmaci_fader->pmaci    = pmaci;
    pmaci_fader->nRange   = nRange;
    pmaci_fader->nPageInc = nPageInc;


    //
    //
    //
    //
    cxvsb = GetSystemMetrics(SM_CXVSCROLL);

    hsb = GetDlgItem(hwnd, IDD_MACONTROL_GRP_MULTICHANNEL);
    GetWindowRect(hsb, &rcM);

    InflateRect(&rcM, -10, -20);
    ScreenToClient(hwnd, (LPPOINT)&rcM.left);
    ScreenToClient(hwnd, (LPPOINT)&rcM.right);

    rcM.right = rcM.left + cxvsb;

    for (u = 0; u < cChannels; u++)
    {
        for (v = 0; v < cMultipleItems; v++)
        {
            uIndex = (u * cMultipleItems) + v;

            hsb = CreateWindow(szScrollBar, gszNull, FSB_DEF_STYLE,
                               rcM.left, rcM.top,
                               rcM.right - rcM.left,
                               rcM.bottom - rcM.top,
                               hwnd, (HMENU)(IDD_MACONTROL_MULTICHANNEL_BASE + uIndex),
                               ghinst, NULL);

            SetScrollRange(hsb, SB_CTL, 0, nRange, FALSE);

            rcM.left  += cxvsb + 4;
            rcM.right += cxvsb + 4;
        }

        //
        //  add more separation between channels
        //
        rcM.left  += cxvsb;
        rcM.right += cxvsb;
    }


    //
    //
    //
    //
    hsb = GetDlgItem(hwnd, IDD_MACONTROL_GRP_UNIFORM);
    GetWindowRect(hsb, &rcU);

    InflateRect(&rcU, -10, -20);
    ScreenToClient(hwnd, (LPPOINT)&rcU.left);
    ScreenToClient(hwnd, (LPPOINT)&rcU.right);

    rcU.right = rcU.left + cxvsb;

    for (v = 0; v < cMultipleItems; v++)
    {
        hsb = CreateWindow(szScrollBar, gszNull, FSB_DEF_STYLE,
                            rcU.left, rcU.top,
                            rcU.right - rcU.left,
                            rcU.bottom - rcU.top,
                            hwnd, (HMENU)(IDD_MACONTROL_UNIFORM_BASE + v),
                            ghinst, NULL);

        SetScrollRange(hsb, SB_CTL, 0, nRange, FALSE);

        rcU.left  += cxvsb + 4;
        rcU.right += cxvsb + 4;
    }


    //
    //
    //
    SendMessage(hwnd,
                MM_MIXM_LINE_CHANGE,
                (WPARAM)pmaci->hmx,
                pmxl->dwLineID);

    SendMessage(hwnd,
                MM_MIXM_CONTROL_CHANGE,
                (WPARAM)pmaci->hmx,
                pmxctrl->dwControlID);

    return (TRUE);
} // MixAppInitDialogFader()
BOOL FNLOCAL MixAppControlSetFader(HWND hwnd, HWND hsb, UINT uCode, int nPos)
{
    PMACONTROLINSTANCE_FADER        pmaci_fader;
    LPMACONTROLINSTANCE             pmaci;
    LPMIXERLINE                     pmxl;
    LPMIXERCONTROL                  pmxctrl;
    PMIXERCONTROLDETAILS_UNSIGNED   pmxcd_u;
    MMRESULT                        mmr;
    int                             nRange;
    int                             nPageInc;
    int                             nValue;
    UINT                            cChannels;
    UINT                            uIndex;
    MIXERCONTROLDETAILS             mxcd;

    //
    //
    //
//    pmaci_fader = (PMACONTROLINSTANCE_FADER)(UINT)GetWindowLong(hwnd, DWL_USER);
    pmaci       = pmaci_fader->pmaci;
    pmxl        = pmaci->pmxl;
    pmxctrl     = pmaci->pmxctrl;
    pmxcd_u     = &pmaci_fader->pmxcd_u[0];
    nRange      = pmaci_fader->nRange;
    nPageInc    = pmaci_fader->nPageInc;


    uIndex = GetDlgCtrlID(hsb);

    if (uIndex < IDD_MACONTROL_UNIFORM_BASE)
    {
        cChannels = (UINT)pmxl->cChannels;
        if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl->fdwControl)
            cChannels = 1;

        uIndex -= IDD_MACONTROL_MULTICHANNEL_BASE;
    }
    else
    {
        cChannels = 1;

        uIndex -= IDD_MACONTROL_UNIFORM_BASE;
    }


    nValue = GetScrollPos(hsb, SB_CTL);

    switch (uCode)
    {
        case SB_PAGEDOWN:
            nValue = (int)min(nRange, (LONG)nValue + nPageInc);
            break;

        case SB_LINEDOWN:
            nValue = (int)min(nRange, (LONG)nValue + 1);
            break;

        case SB_PAGEUP:
            nValue -= nPageInc;

            //-- fall through --//

        case SB_LINEUP:
            nValue = (nValue < 1) ? 0 : (nValue - 1);
            break;


        case SB_TOP:
            //
            //  !!! would be a VERY BAD IDEA to go full volume !!!
            //
            nValue = nRange / 2;
            break;

        case SB_BOTTOM:
            nValue = nRange;
            break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            nValue = nPos;
            break;

        default:
            return (FALSE);
    }


    //
    //
    //
    //
    mxcd.cbStruct       = sizeof(mxcd);
    mxcd.dwControlID    = pmxctrl->dwControlID;
    mxcd.cChannels      = cChannels;
    mxcd.cMultipleItems = pmxctrl->cMultipleItems;
    mxcd.cbDetails      = sizeof(*pmxcd_u);
    mxcd.paDetails      = pmxcd_u;

    mmr = mixerGetControlDetails((HMIXEROBJ)pmaci->hmx, &mxcd, 0L);
    if (MMSYSERR_NOERROR != mmr)
    {
        AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                  "mixerGetControlDetails(ctrlid=%.08lXh) failed on hmx=%.04Xh, mmr=%u!",
                  pmxctrl->dwControlID, pmaci->hmx, mmr);
        return (FALSE);
    }

    pmxcd_u[uIndex].dwValue  = (DWORD)MulDivRN((nRange - nValue), 0xFFFF, nRange);

    mmr = mixerSetControlDetails((HMIXEROBJ)pmaci->hmx, &mxcd, 0L);
    if (MMSYSERR_NOERROR != mmr)
    {
        AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                  "mixerSetControlDetails(ctrlid=%.08lXh) failed on hmx=%.04Xh, mmr=%u!",
                  pmxctrl->dwControlID, pmaci->hmx, mmr);
        return (FALSE);
    }

// * commented out to stop sliders getting out of synch    *
// * between different instances of the app. AGuy 22.12.93 *
//  SetScrollPos(hsb, SB_CTL, nValue, TRUE);

#if 0
    if (0 == uIndex)
    {
        htxt = GetDlgItem(hwnd, IDD_MACONTROL_TXT_VALUE);
        AppSetWindowText(htxt, "mapped=%d, dwValue=%lu",
                         nValue,
                         pmxcd_u[0].dwValue);
    }
#endif

    return (TRUE);
} // MixAppControlSetFader()
