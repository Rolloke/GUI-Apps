// SimpleStringArray.cpp: implementation of the SimpleStringArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "SimpleStringArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CSimpleStringArray destructor

CSimpleStringArray::~CSimpleStringArray ()
{
	if (true == m_bOwnData)
	{
		// Iterate through the vector and kill the strings.
		//
		for (int i = 0; i < (int)(vect.size()); i ++)
		{
			LPSTR psString = vect[i];

			delete [] psString;
		}
	}
}

/*********************************************************************************************
 Class		   : CSimpleStringArray
 Function      : Add 
 Description   : Add a string to the CSimpleStringArray vector

 Parameters:  
  psString (I): the striing to add to the vector (LPSTR)

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE Add >
 <GROUP CSimpleStringArray>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CSimpleStringArray, Add>
*********************************************************************************************/
void CSimpleStringArray::Add (LPSTR psString)
{
	vect.insert (vect.end (), psString);
}

/*********************************************************************************************
 Class		   : CSimpleStringArray
 Function      : Clear 
 Description   : Clears the CSimpleStringArray vector

 Parameters:  --

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE Clear >
 <GROUP CSimpleStringArray>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CSimpleStringArray, Clear>
*********************************************************************************************/
void CSimpleStringArray::Clear ()
{
	vect.clear();

}

/*********************************************************************************************
 Class		   : CSimpleStringArray
 Function      : GetSize 
 Description   : Returns the CSimpleStringArray vector size

 Parameters:  --

 Result type:  the vector size (int)

 Author: TKR
 created: August, 02 2002
 <TITLE GetSize >
 <GROUP CSimpleStringArray>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CSimpleStringArray, GetSize>
*********************************************************************************************/
int CSimpleStringArray::GetSize() const
{
  return vect.size();
}
