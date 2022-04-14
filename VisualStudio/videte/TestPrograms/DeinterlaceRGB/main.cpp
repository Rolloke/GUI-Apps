/*
    Interlaced RGB filter for virtualdub - Copyright 2002 Valentim Batista
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be contacted at:
    Valentim Batista
    vb@timsara.freeservers.com
    http://timsara.freeservers.com/


    This filter operates on even and odd lines and r, g and b channels in separate as follows
	and converting rgb to y on r, g, b as y 

    even_clone_r: out_odd_r = in_even_r
    even_clone_g: out_odd_g = in_even_g
    even_clone_b: out_odd_b = in_even_b

    even_cancel_r: out_even_r = 0
    even_cancel_g: out_even_g = 0
    even_cancel_b: out_even_b = 0

    even_blend_r: out_even_r = average( in_even_r, in_odd_r )
    even_blend_g: out_even_g = average( in_even_g, in_odd_g )
    even_blend_b: out_even_b = average( in_even_b, in_odd_b )

    even_blur_r: out_even_r = average( in_even_r, prev_in_even_r )
    even_blur_g: out_even_g = average( in_even_g, prev_in_even_g )
    even_blur_b: out_even_b = average( in_even_b, prev_in_even_b )

    odd_clone_r: out_even_r = in_odd_r
    odd_clone_g: out_even_g = in_odd_g
    odd_clone_b: out_even_b = in_odd_b

    odd_cancel_r: out_odd_r = 0
    odd_cancel_g: out_odd_g = 0
    odd_cancel_b: out_odd_b = 0

    odd_blend_r: out_odd_r = average( in_even_r, in_odd_r )
    odd_blend_g: out_odd_g = average( in_even_g, in_odd_g )
    odd_blend_b: out_odd_b = average( in_even_b, in_odd_b )

    odd_blur_r: out_odd_r = average( in_odd_r, prev_in_odd_r )
    odd_blur_g: out_odd_g = average( in_odd_g, prev_in_odd_g )
    odd_blur_b: out_odd_b = average( in_odd_b, prev_in_odd_b )

    even_y = rgb2y(even_rgb)

    odd_y = rgb2y(odd_rgb)

    The filters operate in this order !

*/

#include <windows.h>
#include <stdio.h>
#include <commctrl.h>

#include "ScriptInterpreter.h"
#include "ScriptError.h"
#include "ScriptValue.h"

#include "resource.h"
#include "filter.h"
#include "resrc1.h"


///////////////////////////////////////////////////////////////////////////

int myFilterRunProc(const FilterActivation *fa, const FilterFunctions *ff);
int myFilterStartProc(FilterActivation *fa, const FilterFunctions *ff);
int myFilterEndProc(FilterActivation *fa, const FilterFunctions *ff);
long myFilterParamProc(FilterActivation *fa, const FilterFunctions *ff);
int myFilterConfigProc(FilterActivation *fa, const FilterFunctions *ff, HWND hwnd);
void myFilterStringProc(const FilterActivation *fa, const FilterFunctions *ff, char *str);
void myFilterScriptProc(IScriptInterpreter *isi, void *lpVoid, CScriptValue *argv, int argc);
bool myFilterFssProc(FilterActivation *fa, const FilterFunctions *ff, char *buf, int buflen);

///////////////////////////////////////////////////////////////////////////

#define uint unsigned int

union argb
{
   unsigned long v;
   struct {
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
   };
};

struct chks {
	bool r1, g1, b1;
	bool r2, g2, b2;
	bool r3, g3, b3;
	bool r4, g4, b4;
	bool r5, g5, b5;
	bool r6, g6, b6;
	bool r7, g7, b7;
	bool r8, g8, b8;
};


struct interlacedrgbData {
    chks cc;		
	bool rgb2y1, rgb2y2;
    chks oc;		
	bool orgb2y1, orgb2y2;


	bool o1, o2, o3, o4, o5, o6, o7, o8;
	unsigned long m1, m2, m3, m4, m5, m6, m7, m8;
	unsigned long n1, n3, n4, n6, n7, n8;

	IFilterPreview *ifp;
};

ScriptFunctionDef interlacedrgb_func_defs[]={
    { (ScriptFunctionPtr)myFilterScriptProc, "Config", "0iiiiiiiiiiiiiiiiiiiiiiii" },
    { NULL },
};

CScriptObject myFilter_obj={
    NULL, interlacedrgb_func_defs
};


struct FilterDefinition filterDef_interlacedrgb = {

    NULL, NULL, NULL,       // next, prev, module
    "interlaced rgb (0.3)",			// name
    "clones, cancels or blends rgb channels (and converts rgb to y) on interlaced frames.\n",
                            // desc
    "Valentim Batista",     // maker
    NULL,                   // private_data
    sizeof(interlacedrgbData),	    // inst_data_size
	NULL,			        // initProc
	NULL,                   // deinitProc
    myFilterRunProc,          // runProc
    myFilterParamProc,	    // paramProc
    myFilterConfigProc,	    // configProc
    myFilterStringProc,		// stringProc
    NULL,					// startProc
    NULL,					// endProc    
	&myFilter_obj,			// script_obj
    myFilterFssProc,		// fssProc

};

///////////////////////////////////////////////////////////////////////////

extern "C" int __declspec(dllexport) __cdecl VirtualdubFilterModuleInit2(FilterModule *fm, const FilterFunctions *ff, int& vdfd_ver, int& vdfd_compat);
extern "C" void __declspec(dllexport) __cdecl VirtualdubFilterModuleDeinit(FilterModule *fm, const FilterFunctions *ff);

static FilterDefinition *fd_interlacedrgb;


int __declspec(dllexport) __cdecl VirtualdubFilterModuleInit2(FilterModule *fm, const FilterFunctions *ff, int& vdfd_ver, int& vdfd_compat) {
    if (!(fd_interlacedrgb = ff->addFilter(fm, &filterDef_interlacedrgb, sizeof(FilterDefinition))))
        return 1;

	vdfd_ver = VIRTUALDUB_FILTERDEF_VERSION;
	vdfd_compat = VIRTUALDUB_FILTERDEF_COMPATIBLE;

    return 0;
}

void __declspec(dllexport) __cdecl VirtualdubFilterModuleDeinit(FilterModule *fm, const FilterFunctions *ff) {
    ff->removeFilter(fd_interlacedrgb);
}

///////////////////////////////////////////////////////////////////////////


/*
int myFilterStartProc(FilterActivation *fa, const FilterFunctions *ff) {
    
	interlacedrgbData *mfd = (interlacedrgbData *)fa->filter_data;
   
	return 0;
}

int myFilterEndProc(FilterActivation *fa, const FilterFunctions *ff) {
    
	interlacedrgbData *mfd = (interlacedrgbData *)fa->filter_data;

	return 0;
}

*/


int myFilterRunProc(const FilterActivation *fa, const FilterFunctions *ff) {
    PixDim w, h, y;
	Pixel32 *pixo;
	Pixel32 *pixe;

	Pixel32 bo, be, po, pe;

	bool rgb2y1, rgb2y2;

	argb *rgb;
	unsigned long l;

	bool o1, o2, o3, o4, o5, o6, o7, o8;
	unsigned long m1, m2, m3, m4, m5, m6, m7, m8;
	unsigned long n1, n3, n4, n6, n7, n8;
	int linewidth = fa->src.pitch;

    interlacedrgbData *mfd = (interlacedrgbData *)fa->filter_data;

	//
	// Main algorithm (non-test)
	// 

	rgb2y1=mfd->rgb2y1;
	rgb2y2=mfd->rgb2y2;

	o1=mfd->o1;
	o2=mfd->o2;
	o3=mfd->o3;
	o4=mfd->o4;
	o5=mfd->o5;
	o6=mfd->o6;
	o7=mfd->o7;
	o8=mfd->o8;

	m1=mfd->m1;
	m2=mfd->m2;
	m3=mfd->m3;
	m4=mfd->m4;
	m5=mfd->m5;
	m6=mfd->m6;
	m7=mfd->m7;
	m8=mfd->m8;

	n1=mfd->n1;
	n3=mfd->n3;
	n4=mfd->n4;
	n6=mfd->n6;
	n7=mfd->n7;
	n8=mfd->n8;

	h = fa->src.h - 1; // the last line in frames with odd number of lines is not affected
						// is not supposed to be odd !!!
	for (y=0; y<h; y+=2) {
		pixe = (Pixel32 *)((char *)fa->src.data + (linewidth*y));
		pixo = (Pixel32 *)((char *)pixe + linewidth);

		pe = *pixe; // first previous equal to first one
		po = *pixo;

		w = fa->src.w;			
		do {

			if (o1) *pixo=(*pixo&m1)|(*pixe&n1); // clone even to odd

			if (o2) *pixe=*pixe&m2; // cancel even

			if (o3) { // blend odd to even
					be=*pixe&n3;
					bo=*pixo&n3;
					*pixe=(	((((be&0xff00ff)+(bo&0xff00ff))>>1)&0xff00ff) +
							((((be&0x00ff00)+(bo&0x00ff00))>>1)&0x00ff00) ) | (*pixe&m3);
			}

			if (o7) { // blur even
					be=*pixe&n7;
					bo=pe&n7;

					pe = *pixe; // save as previous (to blur)

					*pixe=(	((((be&0xff00ff)+(bo&0xff00ff))>>1)&0xff00ff) +
							((((be&0x00ff00)+(bo&0x00ff00))>>1)&0x00ff00) ) | (*pixe&m7);
			}
			
			if (o4) *pixe=(*pixe&m4)|(*pixo&n4); // clone odd to even

			if (o5) *pixo=*pixo&m5; // cancel odd

			if (o6) { // blend even to odd
					be=*pixe&n6;
					bo=*pixo&n6;
					*pixo=(	((((be&0xff00ff)+(bo&0xff00ff))>>1)&0xff00ff) +
							((((be&0x00ff00)+(bo&0x00ff00))>>1)&0x00ff00) ) | (*pixo&m6);
			}

			if (o8) { // blur odd
					bo=*pixo&n8;
					be=po&n8;

					po = *pixo; // save as previous (to blur)

					*pixo=(	((((be&0xff00ff)+(bo&0xff00ff))>>1)&0xff00ff) +
							((((be&0x00ff00)+(bo&0x00ff00))>>1)&0x00ff00) ) | (*pixo&m8);
			}
			

			if (rgb2y1) { // convert rgb2y on even lines
				rgb=(argb *)pixe;
				l=(((rgb->r*76)+(rgb->g*150)+(rgb->b*29))>>8)&0xff;
				rgb->v=(l<<16)|(l<<8)|l;
			}

			if (rgb2y2) { // convert rgb2y on odd lines
				rgb=(argb *)pixo;
				l=(((rgb->r*76)+(rgb->g*150)+(rgb->b*29))>>8)&0xff;
				rgb->v=(l<<16)|(l<<8)|l;
			}

			pixo++; // advance
			pixe++;

		} while (--w);
	}

	return 0;
}

long myFilterParamProc(FilterActivation *fa, const FilterFunctions *ff) {
	fa->dst.offset = fa->src.offset;

	return 0;
}


// compute configuration values
// this is here to avoid repeated computation in myFilterRunProc() for each frame
void myFilterConfigDigest(interlacedrgbData *mfd) {

	if (mfd->cc.r1||mfd->cc.g1||mfd->cc.b1) {
		mfd->o1=true;
		mfd->m1=0xffffff;
		if (mfd->cc.r1) mfd->m1&=0x00ffff;
		if (mfd->cc.g1) mfd->m1&=0xff00ff;
		if (mfd->cc.b1) mfd->m1&=0xffff00;
		mfd->n1=~mfd->m1&0xffffff;
	}
	else mfd->o1=false;

	if (mfd->cc.r2||mfd->cc.g2||mfd->cc.b2) {
		mfd->o2=true;
		mfd->m2=0xffffff;
		if (mfd->cc.r2) mfd->m2&=0x00ffff;
		if (mfd->cc.g2) mfd->m2&=0xff00ff;
		if (mfd->cc.b2) mfd->m2&=0xffff00;
	}
	else mfd->o2=false;

	if (mfd->cc.r3||mfd->cc.g3||mfd->cc.b3) {
		mfd->o3=true;
		mfd->m3=0xffffff;
		if (mfd->cc.r3) mfd->m3&=0x00ffff;
		if (mfd->cc.g3) mfd->m3&=0xff00ff;
		if (mfd->cc.b3) mfd->m3&=0xffff00;
		mfd->n3=~mfd->m3&0xffffff;
	}
	else mfd->o3=false;

	if (mfd->cc.r4||mfd->cc.g4||mfd->cc.b4) {
		mfd->o4=true;
		mfd->m4=0xffffff;
		if (mfd->cc.r4) mfd->m4&=0x00ffff;
		if (mfd->cc.g4) mfd->m4&=0xff00ff;
		if (mfd->cc.b4) mfd->m4&=0xffff00;
		mfd->n4=~mfd->m4&0xffffff;
	}
	else mfd->o4=false;

	if (mfd->cc.r5||mfd->cc.g5||mfd->cc.b5) {
		mfd->o5=true;
		mfd->m5=0xffffff;
		if (mfd->cc.r5) mfd->m5&=0x00ffff;
		if (mfd->cc.g5) mfd->m5&=0xff00ff;
		if (mfd->cc.b5) mfd->m5&=0xffff00;
	}
	else mfd->o5=false;

	if (mfd->cc.r6||mfd->cc.g6||mfd->cc.b6) {
		mfd->o6=true;
		mfd->m6=0xffffff;
		if (mfd->cc.r6) mfd->m6&=0x00ffff;
		if (mfd->cc.g6) mfd->m6&=0xff00ff;
		if (mfd->cc.b6) mfd->m6&=0xffff00;
		mfd->n6=~mfd->m6&0xffffff;
	}
	else mfd->o6=false;

	if (mfd->cc.r7||mfd->cc.g7||mfd->cc.b7) {
		mfd->o7=true;
		mfd->m7=0xffffff;
		if (mfd->cc.r7) mfd->m7&=0x00ffff;
		if (mfd->cc.g7) mfd->m7&=0xff00ff;
		if (mfd->cc.b7) mfd->m7&=0xffff00;
		mfd->n7=~mfd->m7&0xffffff;
	}
	else mfd->o7=false;

	if (mfd->cc.r8||mfd->cc.g8||mfd->cc.b8) {
		mfd->o8=true;
		mfd->m8=0xffffff;
		if (mfd->cc.r8) mfd->m8&=0x00ffff;
		if (mfd->cc.g8) mfd->m8&=0xff00ff;
		if (mfd->cc.b8) mfd->m8&=0xffff00;
		mfd->n8=~mfd->m8&0xffffff;
	}
	else mfd->o8=false;

}

// Creat config gui
BOOL CALLBACK interlacedrgbConfigDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    interlacedrgbData *mfd = (interlacedrgbData *)GetWindowLong(hdlg, DWL_USER);

    switch(msg) {
    case WM_INITDIALOG:
        SetWindowLong(hdlg, DWL_USER, lParam);
        mfd = (interlacedrgbData *)lParam;

		// Store current config
		mfd->oc = mfd->cc;
		mfd->orgb2y1 = mfd->rgb2y1;
		mfd->orgb2y2 = mfd->rgb2y2;

		CheckDlgButton(hdlg, IDC_CHECK1, mfd->cc.r1);
		CheckDlgButton(hdlg, IDC_CHECK2, mfd->cc.g1);
		CheckDlgButton(hdlg, IDC_CHECK3, mfd->cc.b1);

		CheckDlgButton(hdlg, IDC_CHECK4, mfd->cc.r2);
		CheckDlgButton(hdlg, IDC_CHECK5, mfd->cc.g2);
		CheckDlgButton(hdlg, IDC_CHECK6, mfd->cc.b2);

		CheckDlgButton(hdlg, IDC_CHECK7, mfd->cc.r3);
		CheckDlgButton(hdlg, IDC_CHECK8, mfd->cc.g3);
		CheckDlgButton(hdlg, IDC_CHECK9, mfd->cc.b3);

		CheckDlgButton(hdlg, IDC_CHECK10, mfd->cc.r4);
		CheckDlgButton(hdlg, IDC_CHECK11, mfd->cc.g4);
		CheckDlgButton(hdlg, IDC_CHECK12, mfd->cc.b4);

		CheckDlgButton(hdlg, IDC_CHECK13, mfd->cc.r5);
		CheckDlgButton(hdlg, IDC_CHECK14, mfd->cc.g5);
		CheckDlgButton(hdlg, IDC_CHECK15, mfd->cc.b5);

		CheckDlgButton(hdlg, IDC_CHECK16, mfd->cc.r6);
		CheckDlgButton(hdlg, IDC_CHECK17, mfd->cc.g6);
		CheckDlgButton(hdlg, IDC_CHECK18, mfd->cc.b6);

		CheckDlgButton(hdlg, IDC_CHECK19, mfd->cc.r7);
		CheckDlgButton(hdlg, IDC_CHECK20, mfd->cc.g7);
		CheckDlgButton(hdlg, IDC_CHECK21, mfd->cc.b7);

		CheckDlgButton(hdlg, IDC_CHECK22, mfd->cc.r8);
		CheckDlgButton(hdlg, IDC_CHECK23, mfd->cc.g8);
		CheckDlgButton(hdlg, IDC_CHECK24, mfd->cc.b8);

		CheckDlgButton(hdlg, IDC_CHECK25, mfd->rgb2y1);
		CheckDlgButton(hdlg, IDC_CHECK26, mfd->rgb2y2);

		// Preview button
		mfd->ifp->InitButton(GetDlgItem(hdlg, IDC_PREVIEW));

        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam)) {
        case IDOK:
			myFilterConfigDigest(mfd);
            EndDialog(hdlg, 0);
            return TRUE;
        case IDCANCEL:
			// Restore old backed up values
			mfd->cc = mfd->oc;
			mfd->rgb2y1 = mfd->orgb2y1;
			mfd->rgb2y2 = mfd->orgb2y2;
			myFilterConfigDigest(mfd);
			EndDialog(hdlg, 1);
            return FALSE;
		case IDC_PREVIEW:
			mfd->ifp->Toggle(GetDlgItem(hdlg, IDC_PREVIEW));
			break;
		case IDC_CHECK1:
			mfd->cc.r1 = !!IsDlgButtonChecked(hdlg, IDC_CHECK1);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK2:
			mfd->cc.g1 = !!IsDlgButtonChecked(hdlg, IDC_CHECK2);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK3:
			mfd->cc.b1 = !!IsDlgButtonChecked(hdlg, IDC_CHECK3);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK4:
			mfd->cc.r2 = !!IsDlgButtonChecked(hdlg, IDC_CHECK4);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK5:
			mfd->cc.g2 = !!IsDlgButtonChecked(hdlg, IDC_CHECK5);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK6:
			mfd->cc.b2 = !!IsDlgButtonChecked(hdlg, IDC_CHECK6);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK7:
			mfd->cc.r3 = !!IsDlgButtonChecked(hdlg, IDC_CHECK7);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK8:
			mfd->cc.g3 = !!IsDlgButtonChecked(hdlg, IDC_CHECK8);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK9:
			mfd->cc.b3 = !!IsDlgButtonChecked(hdlg, IDC_CHECK9);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK10:
			mfd->cc.r4 = !!IsDlgButtonChecked(hdlg, IDC_CHECK10);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK11:
			mfd->cc.g4 = !!IsDlgButtonChecked(hdlg, IDC_CHECK11);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK12:
			mfd->cc.b4 = !!IsDlgButtonChecked(hdlg, IDC_CHECK12);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK13:
			mfd->cc.r5 = !!IsDlgButtonChecked(hdlg, IDC_CHECK13);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK14:
			mfd->cc.g5 = !!IsDlgButtonChecked(hdlg, IDC_CHECK14);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK15:
			mfd->cc.b5 = !!IsDlgButtonChecked(hdlg, IDC_CHECK15);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK16:
			mfd->cc.r6 = !!IsDlgButtonChecked(hdlg, IDC_CHECK16);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK17:
			mfd->cc.g6 = !!IsDlgButtonChecked(hdlg, IDC_CHECK17);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK18:
			mfd->cc.b6 = !!IsDlgButtonChecked(hdlg, IDC_CHECK18);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK19:
			mfd->cc.r7 = !!IsDlgButtonChecked(hdlg, IDC_CHECK19);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK20:
			mfd->cc.g7 = !!IsDlgButtonChecked(hdlg, IDC_CHECK20);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK21:
			mfd->cc.b7 = !!IsDlgButtonChecked(hdlg, IDC_CHECK21);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK22:
			mfd->cc.r8 = !!IsDlgButtonChecked(hdlg, IDC_CHECK22);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK23:
			mfd->cc.g8 = !!IsDlgButtonChecked(hdlg, IDC_CHECK23);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK24:
			mfd->cc.b8 = !!IsDlgButtonChecked(hdlg, IDC_CHECK24);
			myFilterConfigDigest(mfd);
			mfd->ifp->RedoFrame();
			break;
		case IDC_CHECK25:
			mfd->rgb2y1 = !!IsDlgButtonChecked(hdlg, IDC_CHECK25);
			mfd->ifp->RedoFrame(); // no digest needed
			break;
		case IDC_CHECK26:
			mfd->rgb2y2 = !!IsDlgButtonChecked(hdlg, IDC_CHECK26);
			mfd->ifp->RedoFrame(); // no digest needed
			break;
		}
		break;
	}

    return FALSE;
}

int myFilterConfigProc(FilterActivation *fa, const FilterFunctions *ff, HWND hwnd) {
    interlacedrgbData *mfd = (interlacedrgbData *) fa->filter_data;

	mfd->ifp = fa->ifp;

	return DialogBoxParam(fa->filter->module->hInstModule,
            MAKEINTRESOURCE(IDD_FILTER_INTERLACEDRGB), hwnd,
            interlacedrgbConfigDlgProc, (LPARAM)fa->filter_data);
}

void myFilterStringProc(const FilterActivation *fa, const FilterFunctions *ff, char *str) {
    interlacedrgbData *mfd = (interlacedrgbData *)fa->filter_data;
sprintf(str, " %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d %d %d"
		, mfd->cc.r1, mfd->cc.g1, mfd->cc.b1
		, mfd->cc.r2, mfd->cc.g2, mfd->cc.b2
		, mfd->cc.r3, mfd->cc.g3, mfd->cc.b3
		, mfd->cc.r7, mfd->cc.g7, mfd->cc.b7
		, mfd->cc.r4, mfd->cc.g4, mfd->cc.b4
		, mfd->cc.r5, mfd->cc.g5, mfd->cc.b5
		, mfd->cc.r6, mfd->cc.g6, mfd->cc.b6
		, mfd->cc.r8, mfd->cc.g8, mfd->cc.b8
		, mfd->rgb2y1, mfd->rgb2y2);
}


// Batch job/script support

void myFilterScriptProc(IScriptInterpreter *isi, void *lpVoid, CScriptValue *argv, int argc) {
    FilterActivation *fa = (FilterActivation *)lpVoid;
    interlacedrgbData *mfd = (interlacedrgbData *)fa->filter_data;

    mfd->cc.r1 = 0!=argv[0].asInt();
    mfd->cc.g1 = 0!=argv[1].asInt();
    mfd->cc.b1 = 0!=argv[2].asInt();

    mfd->cc.r2 = 0!=argv[3].asInt();
    mfd->cc.g2 = 0!=argv[4].asInt();
    mfd->cc.b2 = 0!=argv[5].asInt();

    mfd->cc.r3 = 0!=argv[6].asInt();
    mfd->cc.g3 = 0!=argv[7].asInt();
    mfd->cc.b3 = 0!=argv[8].asInt();

    mfd->cc.r4 = 0!=argv[9].asInt();
    mfd->cc.g4 = 0!=argv[10].asInt();
    mfd->cc.b4 = 0!=argv[11].asInt();

    mfd->cc.r5 = 0!=argv[12].asInt();
    mfd->cc.g5 = 0!=argv[13].asInt();
    mfd->cc.b5 = 0!=argv[14].asInt();

    mfd->cc.r6 = 0!=argv[15].asInt();
    mfd->cc.g6 = 0!=argv[16].asInt();
    mfd->cc.b6 = 0!=argv[17].asInt();

    mfd->cc.r7 = 0!=argv[18].asInt();
    mfd->cc.g7 = 0!=argv[19].asInt();
    mfd->cc.b7 = 0!=argv[20].asInt();

    mfd->cc.r8 = 0!=argv[21].asInt();
    mfd->cc.g8 = 0!=argv[22].asInt();
    mfd->cc.b8 = 0!=argv[23].asInt();

    mfd->rgb2y1 = 0!=argv[24].asInt();
    mfd->rgb2y2 = 0!=argv[25].asInt();

	myFilterConfigDigest(mfd);

}

bool myFilterFssProc(FilterActivation *fa, const FilterFunctions *ff, char *buf, int buflen) {
    interlacedrgbData *mfd = (interlacedrgbData *)fa->filter_data;

    _snprintf(buf, buflen, "Config(%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d)"
		, mfd->cc.r1, mfd->cc.g1, mfd->cc.b1
		, mfd->cc.r2, mfd->cc.g2, mfd->cc.b2
		, mfd->cc.r3, mfd->cc.g3, mfd->cc.b3
		, mfd->cc.r7, mfd->cc.g7, mfd->cc.b7
		, mfd->cc.r4, mfd->cc.g4, mfd->cc.b4
		, mfd->cc.r5, mfd->cc.g5, mfd->cc.b5
		, mfd->cc.r6, mfd->cc.g6, mfd->cc.b6
		, mfd->cc.r8, mfd->cc.g8, mfd->cc.b8
		, mfd->rgb2y1, mfd->rgb2y2);

    return true;
}

