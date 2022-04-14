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

#if defined(_HAWK_35_) // [ _HAWK_35_ 
//------------------------------------------------------------------------------
	.extern start;
		
	.section jump_user_program;
	.align 2;

	jump start;
#else              // ] [ !_HAWK_35_ 
	#error "Board not defined"
#endif             // ] _HAWK_35_ 