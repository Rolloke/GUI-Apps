/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaSys
// FILE:		$Workfile: PERF.H $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaSys/PERF.H $
// CHECKIN:		$Date: 25.03.04 11:26 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 25.03.04 10:15 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

///======================================================================
// 
// Perf.h
//
// Copyright (C) 1998 Mark Russinovich
//
// Fill in this header file with definitions for the performance
// counters you want to use.
//
///======================================================================

#ifndef _PERF_H_
#define _PERF_H_

//**********************************************************************
//
// Modify this section for your counters
//

//
// Define this to "1" turn on the cycle performance counters, "0"
// to disable them
//
#define PERFORMANCE 1

//
// This is the array of counter index definitions.
// Note that when a new entry is added here that the name array
// in perf.c must be updated as well. The convention is that
// the index name should match the function name identically, with
// the PERF_ prefix.
//
#define PERF_REPLACETHIS          1

// make one higher than highest value above
#define NUM_PERF_COUNTERS         2

//
// End of user-modified portion
// 
//**********************************************************************

typedef struct {
   KSPIN_LOCK      Lock;
   LONG            Count;
   LARGE_INTEGER   TotalCycles;
} PERF_COUNTER, *PPERF_COUNTER;


#if PERFORMANCE

extern PERF_COUNTER PerfCounter[];

//
// Definition for raw bytes that generate RDTSC instruction
//
#define RDTSC(_VAR)                \
   _asm {                          \
       _asm push eax               \
       _asm push edx               \
       _asm _emit 0Fh              \
       _asm _emit 31h              \
       _asm mov _VAR.LowPart, eax  \
       _asm mov _VAR.HighPart, edx \
       _asm pop edx                \
       _asm pop eax                \
   } 

//
// Definitions for performance counters that execute
// at DISPATCH_LEVEL (e.g. in DPCs) and lower IRQLs
//
#define PERF_ENTRY(_INDEX)                  \
   LARGE_INTEGER _INDEX##perfStart;         \
   LARGE_INTEGER _INDEX##perfEnd;           \
   RDTSC(_INDEX##perfStart);                \
   InterlockedIncrement( &PerfCounter[_INDEX].Count );

#define PERF_EXIT(_INDEX)                   \
   RDTSC(_INDEX##perfEnd);                  \
   _INDEX##perfEnd.QuadPart -= _INDEX##perfStart.QuadPart;          \
   ExInterlockedAddLargeInteger( &PerfCounter[_INDEX].TotalCycles,  \
         _INDEX##perfEnd,                   \
		 &PerfCounter[_INDEX].Lock )

//
// Definitions for performance counters that execute
// in ISRs, and hence need no locking
//
#define PERF_ISR_ENTRY(_INDEX)  PERF_ENTRY(_INDEX)

#define PERF_ISR_EXIT(_INDEX)               \
   RDTSC(_INDEX##perfEnd);                  \
   _INDEX##perfEnd.QuadPart -= _INDEX##perfStart.QuadPart;              \
   PerfCounter[_INDEX].TotalCycles.QuadPart += _INDEX##perfEnd.QuadPart

#else // PERFORMANCE

#define PERF_ENTRY(_INDEX)    

#define PERF_EXIT(_INDEX) 

#endif PERFORMANCE


//
// Externs for perf.c
//
VOID InitPerfCounters();
VOID DumpPerfCounters();
#endif _PERF_H_


