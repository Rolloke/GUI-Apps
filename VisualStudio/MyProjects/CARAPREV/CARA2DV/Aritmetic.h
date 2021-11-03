// Aritmetic.h: Schnittstelle für die Klasse CArithmetic.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARITMETIC_H__588E3E6E_0809_11D1_9DB9_B051418EA04A__INCLUDED_)
#define AFX_ARITMETIC_H__588E3E6E_0809_11D1_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <cmath>
#include "Calcmplx.h"

#define  IDE_AR_OK               0 // Gleichung ist ok
#define  IDE_AR_BRACE            1 // Klammerfehler
#define  IDE_AR_OUTOFRANGE       2 // Zahlenbereich verlassen
#define  IDE_AR_OPERATOR         3 // aufeinanderfolgende Operatoren sind selten erlaubt
#define  IDE_AR_NOEQUATION       4 // Keine Gleichung vorhanden
#define  IDE_AR_ZERO             5 // durch 0 geteilt
#define  IDE_AR_NONUMBER         6 // Zahl kann nicht konvertiert werden
#define  IDE_AR_NOFUNCTION       7 // Zahl kann nicht konvertiert werden
#define  IDE_AR_INVALIDCHARACTER 8 // ungültiges Zeichen
#define  IDE_AR_NO_COMPLEX_IMPL  9 //
#define  IDE_AR_DECIMAL_POINT   10 // ungültiges Zeichen

#define  IDA_AR_DEG         0      // Winkel in Grad
#define  IDA_AR_RAD         1      // Winkel in Rad
#define  IDA_AR_GRAD        2      // Winkel in Neugrad

#define  IDL_AR_FUNCTION    5      // Funktionen Level
#define  IDL_AR_NUMBER      6      // Zahlen Level

#define  IDO_AR_NUM          0     // Numerische Konstante wird im Gleichungsknoten gelöscht
#define  IDO_AR_VAR          1     // Variable darf nicht im Gleichungsknoten gelöscht werden, sondern im Variablenknoten
#define  IDO_AR_CONST        2     // Konstante darf nicht im Gleichungsknoten gelöscht werden, sondern im Variablenknoten

#define  IDO_AR_SIN        101     // unäre Operatoren (Funktionen)
#define  IDO_AR_COS        102
#define  IDO_AR_TAN        103

#define  IDO_AR_ASIN       104
#define  IDO_AR_ACOS       105
#define  IDO_AR_ATAN       106

#define  IDO_AR_SINH       107
#define  IDO_AR_COSH       108
#define  IDO_AR_TANH       109

#define  IDO_AR_ASINH      110
#define  IDO_AR_ACOSH      111
#define  IDO_AR_ATANH      112

#define  IDO_AR_ABS        113
#define  IDO_AR_FLOOR      114
#define  IDO_AR_CEIL       115
#define  IDO_AR_ROUND      116

#define  IDO_AR_LOG        117
#define  IDO_AR_LOG10      118
#define  IDO_AR_SQRT       119
#define  IDO_AR_PHASE      120
#define  IDO_AR_REAL       121
#define  IDO_AR_IMAG       122

#define  IDO_AR_LOGB       150      // binäre Funktionen
#define  IDO_AR_HYPOT      151
#define  IDO_AR_BESS1      152
#define  IDO_AR_BESS2      153
#define  IDO_AR_ATAN2      154
#define  IDO_AR_ROUNDTO    155
#define  IDO_AR_COMPLEX    156
#define  IDO_AR_COMPLEX_AP 157
#define  IDO_AR_MIN        158
#define  IDO_AR_MAX        159

struct SArithmaNode
{
    char                 level;
    char                 brace;
    DWORD                coperator;
    char                *pszname;
    Complex             *pvalue;
    struct SArithmaNode *pleft;
    struct SArithmaNode *pright;
};

struct SVarNode
{
    char            *pszname;
    Complex         *pvalue;
    struct SVarNode *pleft;
    struct SVarNode *pright;
};

struct sConstants
{
    char* name;
    UINT  idDescription;
    double value;
};

class CArithmetic  
{
public:
    CArithmetic();
    ~CArithmetic();

    DWORD          SetEquation(const char *);
    Complex        Calculate();
    void           Delete();
    void           Show(CTreeCtrl *);
    void           ResetError(){m_nError = 0;};
    int            GetError()  {return m_nError;};
    void           ClearError()  { m_nError = 0;};
    int            GetNumVar() {return m_nNumVar;};
    char         * GetVarName(int);
    Complex      * GetVarAdress(const char *);
    char           GetDecimal()   { return m_szDecimal[0]; }
    char           GetSeparator() { return m_szDecimal[2]; }

    const static char  * gm_UnaryFkt[];
    const static char  * gm_BinaryFkt[];
    const static sConstants gm_constants[];

private:
    SArithmaNode  *insert(SArithmaNode *, SArithmaNode *);
    SArithmaNode  *newnode(SArithmaNode *);
    SVarNode      *insertvar(SVarNode *, SVarNode *);
    SVarNode      *deletevariables(SVarNode*);
    SArithmaNode  *deletearithmnodes(SArithmaNode *);
    Complex        calc(SArithmaNode *);
    char          *getvarname(SVarNode *, int&);
    Complex       *getvaradress(SVarNode *, const char *);
    void           show(SArithmaNode *, TV_INSERTSTRUCT *, CTreeCtrl *);
    double         getConstValue(const char *);
    int            getFuncIndex(const char *);

    int            m_nError;
    int            m_nNumVar;
    SVarNode      *m_pVar;
    SArithmaNode  *m_pP;
    char           m_szDecimal[4];
public:

public:
    static double round(double);
    static double atanh(double);
    static double acotanh(double);
    static double acosh(double);
    static double asinh(double);
    static double logb(double, double);
    static double roundto(double, double);
    static double sinEx(double);
    static Complex Hypot(Complex&a, Complex&b);

    const static double  gm_d2PI;
    static double        gm_AngleTo;
    static double        gm_AngleFrom;
};

#endif // !defined(AFX_ARITMETIC_H__588E3E6E_0809_11D1_9DB9_B051418EA04A__INCLUDED_)
