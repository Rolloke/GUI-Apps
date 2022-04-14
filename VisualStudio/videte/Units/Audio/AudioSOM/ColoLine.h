#pragma once


// CColorLine

class CColorLine : public CStatic
{
	DECLARE_DYNAMIC(CColorLine)

public:
	CColorLine(CPen*);
	virtual ~CColorLine();

protected:
   afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
   CPen *m_pPen;
};


