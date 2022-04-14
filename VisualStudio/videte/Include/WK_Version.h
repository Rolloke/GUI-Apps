
#ifndef _WK_Version_H
#define _WK_Version_H

#ifndef WK_LANG_BLOCK
#define WK_LANG_BLOCK "040704b0"
#endif

#ifndef WK_LANG_BLOCK_NUM
#define WK_LANG_BLOCK_NUM 0x407
#endif

// NEW 120507 unused szProductVersion, szVersion
// HD 120398 dropped "CompanyName", "ProductName", "ProductVersion"
// HD 120398 universal copyright string
// GF 080103 dropped WK_LANG_BLOCK and WK_LANG_BLOCK_NUM, not used anyway, made problems with SaVicUnit project
#define WK_VERSION_MACRO(va,vb,vc,vd, szFileVersion, szExeName, szProduct, szProductDescription, szProductVersion, szVersion, szCompany, szCopyright) \
VS_VERSION_INFO VERSIONINFO \
 FILEVERSION va,vb,vc,vd \
 PRODUCTVERSION 1,0,0,1 \
 FILEFLAGSMASK 0x3fL \
 FILEFLAGS 0x0L \
 FILEOS 0x4L \
 FILETYPE 0x1L \
 FILESUBTYPE 0x0L \
BEGIN \
    BLOCK "StringFileInfo" \
    BEGIN \
        BLOCK WK_LANG_BLOCK \
        BEGIN\
            VALUE "FileVersion", szFileVersion \
            VALUE "FileDescription", szProductDescription \
            VALUE "LegalCopyright", "Copyright © 1997 - 2004\0" \
            VALUE "OriginalFilename", szExeName \
        END \
    END \
    BLOCK "VarFileInfo" \
    BEGIN \
        VALUE "Translation", WK_LANG_BLOCK_NUM, 1200 \
    END \
END \

#define WK_VERSION(va,vb,vc,vd,szFileVersion,szExeName,szProduct,szProductDescription,szProductVersion,szVersion) \
	    WK_VERSION_MACRO(va,vb,vc,vd,szFileVersion,szExeName,szProduct,szProductDescription,szProductVersion,szVersion, \
				         "videte IT® AG\0", "Copyright  videte IT AG ©2004\0" )

// erzeuct LTEXT "...", IDC_STATIC
// Zum Beispiel im .rc2 in dem AboutDialog
//	RC_TEXT_VERSION(27, 2, 13, 27, "27/2/97,13:27\0", // @AUTO_UPDATE
//			)
//			60,40,190,8
#define RC_TEXT_VERSION(va,vb,vc,vd,szFileVersion,dummy) LTEXT  szFileVersion, IDC_STATIC
// erzeugt nur den versionString
#define PLAIN_TEXT_VERSION(va,vb,vc,vd,szFileVersion,dummy) szFileVersion

//
// das perlScript superReplace kann mit -updateVersion aufgerufen werden
// dann sucht es nur in .rc .rc2 und ersetzt zwichen
// "VERSION....@AUTO_UPDATE" das aktuelle datum
//

/*<><><><> START OF SAMPLE CODE

#include "WK_Version.h"

#ifdef _GARNY
GARNY_VERSION(19, 8, 11, 18, "19/8/96\0", // @AUTO_UPDATE
		   "sec3.exe\0",	// exe name
		   "Video-Save Server\0",	// product name
		   "Video-Save Server application\0",	// product description
		   "3.0\0",		// UNUSED now hardcoded for all general product version
		   "3.001\0"	// UNUSED now hardcoded for all real version
		   )
#else
WK_VERSION(19, 8, 11, 18, "19/8/96\0", // @AUTO_UPDATE
		   "sec3.exe\0",	// exe name
		   "idip! Security server\0",	// product name
		   "Security Server application\0",	// product description
		   "3.0\0",		// UNUSED now hardcoded for all general product version
		   "3.001\0"	// UNUSED now hardcoded for all real version
		   )
#endif

<><><><> END OF SAMPLE CODE <><><><> */


#endif
