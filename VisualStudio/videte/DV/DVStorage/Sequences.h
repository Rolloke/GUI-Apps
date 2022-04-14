// Sequences.h: interface for the CSequences class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCES_H__241E3CA9_9B2A_4576_A030_4512B7667E57__INCLUDED_)
#define AFX_SEQUENCES_H__241E3CA9_9B2A_4576_A030_4512B7667E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tape.h"

class CSequences : public CTapeArrayCS  
{
	// construction / destruction
public:
	CSequences();
	virtual ~CSequences();

	// attributes
public:
		   int			GetNrTapes();
	// operations
public:
	void Sort(BOOL& bCancel);

};

#endif // !defined(AFX_SEQUENCES_H__241E3CA9_9B2A_4576_A030_4512B7667E57__INCLUDED_)
