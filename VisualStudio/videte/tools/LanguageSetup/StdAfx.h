// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AF34A793_7F6A_47C7_8FB8_FD7E3AE2A97D__INCLUDED_)
#define AFX_STDAFX_H__AF34A793_7F6A_47C7_8FB8_FD7E3AE2A97D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define HKCU_DESKTOP				"Control Panel\\Desktop\\"
#define HKU_DEFAULT					".DEFAULT"
#define MUILANGUAGEPENDING			"MUILanguagePending"
#define MULTIUILANGUAGEID			"MultiUILanguageId"

#define HKCU_KEYBORD_LAYOUT			"Keyboard Layout\\Preload\\"
#define LANG_ID_ENU					_T("00000409")

#define HKCU_KEYBORD_TOGGLE			"Keyboard Layout\\Toggle\\"
#define HOTKEY						"Hotkey"
#define LANGUAGE_HOTKEY				"Language Hotkey"
#define LAYOUT_HOTKEY				"Layout Hotkey"

#define HKLM_KEYBORD_LAYOUTS		"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\"
#define LAYOUT_TEXT					"Layout Text"

#define HKLM_MUILANGUAGES			"SYSTEM\\CurrentControlSet\\Control\\Nls\\MUILanguages"

#define REG_CLOSE_KEY(hKey) {if (hKey) {RegCloseKey(hKey); hKey = NULL;}}

#endif // !defined(AFX_STDAFX_H__AF34A793_7F6A_47C7_8FB8_FD7E3AE2A97D__INCLUDED_)
