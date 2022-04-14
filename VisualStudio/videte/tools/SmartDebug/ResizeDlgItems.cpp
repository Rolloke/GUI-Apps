#include "StdAfx.h"
#include "ResizeDlgItems.h"


CResizeDlgItems::positions::positions()
{
}

long CResizeDlgItems::positions::get(int pos)
{
   std::map<int, long>::const_iterator it(find(pos));
   if (it != end()) return it->second;
   else             return 0;
}

CResizeDlgItems::SItems::SItems() {};
CResizeDlgItems::SItems::SItems(int aFlag) : mFlags(aFlag) {};


CResizeDlgItems::CResizeDlgItems() : mParent(NULL)
{
    mSize.cx = 0;
    mSize.cy = 0;
}

/*!
\brief transformes screen coordinates to client coordinates
\param lpPoint pointer to points array
\param nPoints length of the array
*/
void CResizeDlgItems::ScreenToClient(LPPOINT lpPoint, int nPoints)
{
   for (int i=0; i<nPoints; ++i)
   {
      ::ScreenToClient(mParent, &lpPoint[i]);
   }
}

/*!
\brief moves a window to desired rectangle coordinates
\param hWnd window handle
\param prc rectangle coordinates
\param bRepaint repaint window after resize
\return (TRUE, FALSE)
*/
BOOL CResizeDlgItems::MoveWindow(HWND hWnd, RECT&prc, BOOL bRepaint)
{
   return ::MoveWindow(hWnd, prc.left, prc.top, prc.right-prc.left, prc.bottom-prc.top, bRepaint);
}

/*!
\brief adds a child window item by its ID
\param aID ID of the child window item
\param aFlags flags for resize option 
*/
void CResizeDlgItems::AddItem(UINT aID, int aFlags)
{
    SItems item(aFlags);
    mItems[aID] = item;
}

/*!
\brief initializes the sizes of the added child window items
\param aWnd parent window handle 
*/
void CResizeDlgItems::InitializeSizes(HWND aWnd)
{
    RECT fDlgRect, fItemRect;
    mParent = aWnd;
    GetClientRect(mParent, &fDlgRect);
    ItemMap::iterator it;
    mSize.cx = fDlgRect.right;
    mSize.cy = fDlgRect.bottom;
    for (it = mItems.begin(); it != mItems.end(); ++it)
    {
        GetWindowRect(GetDlgItem(mParent, it->first), &fItemRect);
        ScreenToClient((LPPOINT)&fItemRect, 2);
        int flags       = it->second.mFlags;
        positions&point = it->second.mPoints;
        if (flags & rs::left  ) point[rs::left  ] = fItemRect.left;
        if (flags & rs::top   ) point[rs::top   ] = fItemRect.top;
        if (flags & rs::right ) point[rs::right ] = fItemRect.right - fDlgRect.right;
        if (flags & rs::bottom) point[rs::bottom] = fItemRect.bottom - fDlgRect.bottom;

        if (flags & rs::left_rel  ) point[rs::left_rel  ] = fItemRect.left;
        if (flags & rs::top_rel   ) point[rs::top_rel   ] = fItemRect.top;
        if (flags & rs::right_rel ) point[rs::right_rel ] = fItemRect.right;
        if (flags & rs::bottom_rel) point[rs::bottom_rel] = fItemRect.bottom;

        if (flags & rs::hcenter) point[rs::hcenter] = (fItemRect.left+fItemRect.right)/2;
        if (flags & rs::vcenter) point[rs::vcenter] = (fItemRect.top+fItemRect.bottom)/2;

        if (flags & rs::fixedheight) point[rs::fixedheight] = fItemRect.bottom - fItemRect.top;
        if (flags & rs::fixedwidth)  point[rs::fixedwidth ] = fItemRect.right - fItemRect.left;
    }
}

/*!
\brief moves the position of the child window item edges according the flags
\param aSize new size of the parent window
\param bUpdate redraw items after moving
*/
void CResizeDlgItems::Resize(SIZE aSize, BOOL bUpdate)
{
    RECT fItemRect;
    ItemMap::iterator it;
    for (it = mItems.begin(); it != mItems.end(); ++it)
    {
        int flags       = it->second.mFlags;
        positions&point = it->second.mPoints;
        if (flags & rs::hcenter)
        {
            int pos  = MulDiv(point.get(rs::hcenter), aSize.cx, mSize.cx);
            int sizehalf = point.get(rs::fixedwidth)/2;
            fItemRect.left  =  pos - sizehalf;
            fItemRect.right =  pos + sizehalf;
        }
        else
        {
            if (flags & rs::left)
            {
                fItemRect.left = point.get(rs::left);
                if (flags & rs::fixedwidth)
                    fItemRect.right = fItemRect.left + point.get(rs::fixedwidth);
            }
            if (flags & rs::right)
            {
                fItemRect.right = aSize.cx + point.get(rs::right);
                if (flags & rs::fixedwidth)
                    fItemRect.left = fItemRect.right - point.get(rs::fixedwidth);
            }
            if (flags & rs::left_rel)
            {
                fItemRect.left =MulDiv(point.get(rs::left_rel), aSize.cx, mSize.cx);
                if (flags & rs::fixedwidth)
                    fItemRect.right = fItemRect.left + point.get(rs::fixedwidth);
            }
            if (flags & rs::right_rel)
            {
                fItemRect.right =MulDiv(point.get(rs::right_rel), aSize.cx, mSize.cx);
                if (flags & rs::fixedwidth)
                    fItemRect.left = fItemRect.right - point.get(rs::fixedwidth);
            }
        }
        if (flags & rs::vcenter)
        {
            int pos  = MulDiv(point.get(rs::vcenter), aSize.cy, mSize.cy);
            int sizehalf = point.get(rs::fixedheight)/2;
            fItemRect.top    =  pos - sizehalf;
            fItemRect.bottom =  pos + sizehalf;
        }
        else
        {
            if (flags & rs::top)
            {
                fItemRect.top = point.get(rs::top);
                if (flags & rs::fixedheight )
                    fItemRect.bottom = fItemRect.top + point.get(rs::fixedheight);
            }
            if (flags & rs::bottom)
            {
                fItemRect.bottom = aSize.cy + point.get(rs::bottom);
                if (flags & rs::fixedheight )
                    fItemRect.top = fItemRect.bottom - point.get(rs::fixedheight);
            }
            if (flags & rs::top_rel)
            {
                fItemRect.top =MulDiv(point.get(rs::top_rel), aSize.cy, mSize.cy);
                if (flags & rs::fixedheight )
                    fItemRect.bottom = fItemRect.top + point.get(rs::fixedheight);
            }
            if (flags & rs::bottom_rel)
            {
                fItemRect.bottom = MulDiv(point.get(rs::bottom_rel), aSize.cy, mSize.cy);
                if (flags & rs::fixedheight )
                    fItemRect.top = fItemRect.bottom - point.get(rs::fixedheight);
            }
        }
        MoveWindow(GetDlgItem(mParent, it->first), fItemRect, bUpdate);
    }
}

void CResizeDlgItems::ResizeFont(SIZE aSize)
{
    ItemMap::iterator it;
    HFONT hF = (HFONT)::SendMessage(mParent, WM_GETFONT, 0, 0);
    LOGFONT lf;
    ::GetObject(hF, sizeof(LOGFONT), &lf);
    lf.lfHeight = MulDiv(lf.lfHeight, aSize.cy, mSize.cy);
    DeleteObject(hF);
    hF = CreateFontIndirect(&lf);
    ::SendMessage(mParent, WM_SETFONT, (WPARAM)hF, 0);
    for (it = mItems.begin(); it != mItems.end(); ++it)
    {
        ::SendMessage(GetDlgItem(mParent, it->first), WM_SETFONT, (WPARAM)hF, 0);
    }
}
