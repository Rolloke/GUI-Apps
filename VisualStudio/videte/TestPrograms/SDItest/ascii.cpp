/////////////////////////////////////////////////////////////////////////////
//	Project:		Common
//
//	File:			$Workfile: ascii.cpp $ : implementation file
//
//	Start:			01.06.95
//
//  Last change:	$Modtime: 19.12.05 16:44 $
//
//  by Author	:	$Author: Uwe.freiwald $
//
//  Checked in:		$Date: 19.12.05 17:18 $
//
//  Version:		$Revision: 4 $
//
//	Authors:		Georg Feddern
//
//	Company:		w+k Video Communication GmbH & Co.KG
//
//$Nokeywords:$
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ascii.h"

/////////////////////////////////////////////////////////////////////////////
BOOL GetAsciiSymbol(BYTE byAscii, CString& sSymbol, BOOL bCRLFAsString /*=TRUE*/)
{
	BOOL bReturn = TRUE;
	sSymbol.Empty();
	
	if ( ((byAscii > 0x1f) && (byAscii < 0x7f))
		 || ((byAscii > 0x7f) && (byAscii < 0xff)) )
	{
		// Alle druckbaren Zeichen
		sSymbol = (char)byAscii;	
	}
	else if ( byAscii == ASCII_CR ) {	 // CR
		if ( bCRLFAsString ) {
			sSymbol = _T("<CR>");	
		}
		else {
			sSymbol = _T("\r");	
		}
	}
	else if ( byAscii == ASCII_LF ) {	 // LF
		if ( bCRLFAsString ) {
			sSymbol = _T("<LF>");	
		}
		else {
			sSymbol = _T("\n");	
		}
	}
	else if ( byAscii == ASCII_NUL ) {
		sSymbol = _T("<NUL>");	
	}
	else if ( byAscii == ASCII_SOH ) {
		sSymbol = _T("<SOH>");	
	}
	else if ( byAscii == ASCII_STX ) {
		sSymbol = _T("<STX>");	
	}
	else if ( byAscii == ASCII_ETX ) {
		sSymbol = _T("<ETX>");	
	}
	else if ( byAscii == ASCII_EOT ) {
		sSymbol = _T("<EOT>");	
	}
	else if ( byAscii == ASCII_ENQ ) {
		sSymbol = _T("<ENQ>");	
	}
	else if ( byAscii == ASCII_ACK ) {
		sSymbol = _T("<ACK>");	
	}
	else if ( byAscii == ASCII_BEL ) {
		sSymbol = _T("<BEL>");	
	}
	else if ( byAscii == ASCII_BS ) {
		sSymbol = _T("<BS>");	
	}
	else if ( byAscii == ASCII_HT ) {
		sSymbol = _T("<HT>");	
	}
	else if ( byAscii == ASCII_VT ) {
		sSymbol = _T("<VT>");	
	}
	else if ( byAscii == ASCII_FF ) {
		sSymbol = _T("<FF>");	
	}
	else if ( byAscii == ASCII_SO ) {
		sSymbol = _T("<SO>");	
	}
	else if ( byAscii == ASCII_SI ) {
		sSymbol = _T("<SI>");	
	}
	else if ( byAscii == ASCII_SLE ) {
		sSymbol = _T("<SLE>");	
	}
	else if ( byAscii == ASCII_CS1 ) {
		sSymbol = _T("<CS1>");	
	}
	else if ( byAscii == ASCII_DC2 ) {
		sSymbol = _T("<DC2>");	
	}
	else if ( byAscii == ASCII_DC3 ) {
		sSymbol = _T("<DC3>");	
	}
	else if ( byAscii == ASCII_DC4 ) {
		sSymbol = _T("<DC4>");	
	}
	else if ( byAscii == ASCII_NAK ) {
		sSymbol = _T("<NAK>");	
	}
	else if ( byAscii == ASCII_SYN ) {
		sSymbol = _T("<SYN>");	
	}
	else if ( byAscii == ASCII_ETB ) {
		sSymbol = _T("<ETB>");	
	}
	else if ( byAscii == ASCII_CAN ) {
		sSymbol = _T("<CAN>");	
	}
	else if ( byAscii == ASCII_EM ) {
		sSymbol = _T("<EM>");	
	}
	else if ( byAscii == ASCII_SIB ) {
		sSymbol = _T("<SIB>");	
	}
	else if ( byAscii == ASCII_ESC ) {
		sSymbol = _T("<ESC>");	
	}
	else if ( byAscii == ASCII_FS ) {
		sSymbol = _T("<FS>");	
	}
	else if ( byAscii == ASCII_GS ) {
		sSymbol = _T("<GS>");	
	}
	else if ( byAscii == ASCII_RS ) {
		sSymbol = _T("<RS>");	
	}
	else if ( byAscii == ASCII_US ) {
		sSymbol = _T("<US>");	
	}
	else if ( byAscii == ASCII_DEL ) {
		sSymbol = _T("<DEL>");	
	}
	else if ( byAscii == ASCII_255 ) {
		sSymbol = _T("<255>");	
	}
	else {
		// kein Ascii-Zeichen
		bReturn = FALSE;
	}
	
	return bReturn;
}
////////////////////////////////////////////////////////////////////////////////
// end of $Workfile: ascii.cpp $

