/*******************************************************************************
 *
 * $Source$
 * $Revision$
 *
 * Hawk-35 program to demonstrate interrupt connections.
 *
 * $Log$
 *
 ******************************************************************************/

#include <defBF535.h>
#include "m21535.h"
#include "basicmon.h"
//#include "dev_inttst.h"

#if defined(_HAWK_35_) // [ _HAWK_35_ 
//------------------------------------------------------------------------------
	.extern start_super;
	.extern start;
		
	.section jump_user_program;
	.align 2;
//	jump start_super;
	jump start;
#else              // ] [ !_HAWK_35_ 
	#error "Board not defined"
#endif             // ] _HAWK_35_ 