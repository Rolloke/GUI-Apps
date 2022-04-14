#pragma once


// CSplitterWndCommon

class CSplitterWndCommon : public CSplitterWnd
{
	DECLARE_DYNAMIC(CSplitterWndCommon)

public:
	CSplitterWndCommon();
	virtual ~CSplitterWndCommon();
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);

	virtual UINT GetSubstCmdID() { return  0;};
	virtual CImageList*GetSubstImageList(int) { return  NULL;};

protected:
	DECLARE_MESSAGE_MAP()
};


