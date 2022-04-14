#ifndef __HELPER_H__
#define __HELPER_H__
/*
 * some helper functions that are not available on all platforms
 *
 * (C) 1999 DResearch GmbH
 *
 * helper.h
 *
 * $Header: /DV/DVProcess/DResearch/helper.h 1     15.02.01 11:59 Uwe $
 */
#if defined(__TCS__)
/*
 * Purpose:
 *		strdup implementation
 */
char *strdup(const char *s);

/*
 * Purpose:
 *		stricmp implementation
 */
int strnicmp(const char *s, const char *p, size_t n);

/*
 * Purpose:
 *		stricmp implementation
 */
int stricmp(const char *s, const char *p);

/*
 * Purpose:
 *		(wrong) snprintf implementation
 */
int snprintf(char *buf, size_t buflen, const char *fmt, ...);

/* alternate name */
#define _snprintf	snprintf

#endif

#ifdef WIN32
/* on WIN32 use underscore */
#define snprintf	_snprintf

#endif

#endif	/* __HELPER_H__ */

