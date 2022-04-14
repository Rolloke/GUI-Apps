#pragma once

#include <vector>
#include <map>

/// resize flags for the function AddItem
/// at least two flags per direction are needed
struct rs { enum 
{
    /// fixes the left edge of the item relative to the left edge
    left=1,
    /// fixes the top edge of the item relative to the top edge
    top=2,
    /// fixes the right edge of the item relative to the right edge
    right=4,
    /// fixes the bottom edge of the item relative to the bottom edge
    bottom=8,
    /// fixes the height of the item
    fixedheight=0x10,
    /// fixes the width of the item
    fixedwidth=0x20, 
    /// moves the horizontal center of the item relative to the parent window size
    hcenter=0x40, 
    /// moves the vertical center of the item relative to the parent window size
    vcenter=0x80,
    /// moves the left edge of the item relative to the parent window size
    left_rel=0x100,
    /// moves the top edge of the item relative to the parent window size
    top_rel=0x200,
    /// moves the right edge of the item relative to the parent window size
    right_rel=0x400,
    /// moves the bottom edge of the item relative to the parent window size
    bottom_rel=0x800,
    /// stores original fontsize
    font_size = 0x1000
};};

class CResizeDlgItems
{
    class positions : public std::map<int, long>
    {
    public:
       positions();
       long get(int pos);
    };
public:
    struct SItems
    {
        SItems();
        SItems(int aFlag);
        int mFlags;
        positions mPoints;
    };
    typedef std::map<UINT, SItems> ItemMap;
    
    CResizeDlgItems();

    void AddItem(UINT aID, int aFlags);
    void InitializeSizes(HWND aWnd);
    void Resize(SIZE aSize, BOOL bUpdate);
    void ResizeFont(SIZE aSize);

protected:
    void ScreenToClient(LPPOINT lpPoint, int nPoints);
    BOOL MoveWindow(HWND hWnd, RECT&prc, BOOL bRepaint);

private:
    HWND    mParent;
    ItemMap mItems;
    SIZE    mSize;
};
