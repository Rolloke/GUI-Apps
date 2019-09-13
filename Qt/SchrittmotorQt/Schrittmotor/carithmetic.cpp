#include "carithmetic.h"

//#include "math.h"
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <stdio.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
const char *Arithmetic::gm_UnaryFkt[] =
{
    "sin" ,  "cos" ,  "tan" ,           // 101, 102, 103
    "asin" , "acos" , "atan" ,          // 104, 105, 106
    "sinh",  "cosh",  "tanh",           // 107, 108, 109
    "asinh", "acosh", "atanh",          // 110, 111, 112
    "abs"  ,"floor" , "ceil" , "round", // 113, 114, 115, 116
    "ln"   ,"log"   , "sqrt" ,          // 117, 118, 119
    "phase", "real", "imag",            // 120,121, 122
    NULL
};

const char *Arithmetic::gm_BinaryFkt[] =
{
    "logb",                            // 150
    "hypot",                           // 151
    "bess1",                           // 152
    "bess2",                           // 153
    "atan2",                           // 154
    "roundto",                         // 155
    "complex",                         // 156
    "complexAP",                       // 157
    "min",                             // 158
    "max",                             // 159
    NULL
};

const char Arithmetic::gConstID     = '_';
const char Arithmetic::gConstNull[] = "_0";
using namespace std;


struct SArithmaNode
{
    SArithmaNode();
    SArithmaNode(const SArithmaNode& pin);
    unsigned int order()
    {
        return static_cast<unsigned int>(brace) << 8 | static_cast<unsigned int>(level);
    }
    char                 level;
    char                 brace;
    unsigned int         coperator;
    const char          *pszname;
    shared_ptr<complex<double>> pvalue;
    tArithmaNode pleft;
    tArithmaNode pright;
};


#define _BESSEL_FUNCTIONS

#ifndef M_E
#define M_E 2.71828182845904523536
#endif


SArithmaNode::SArithmaNode() :
    level(0)
  ,   brace(0)
  ,   coperator(0)
  ,   pszname(0)
{

}

SArithmaNode::SArithmaNode(const SArithmaNode& pin) :
    level(pin.level)
  ,   brace(pin.brace)
  ,   coperator(pin.coperator)
  ,   pszname(pin.pszname)
  ,   pvalue(pin.pvalue)
  ,   pleft(pin.pleft)
  ,   pright(pin.pright)
{

}

const sConstants Arithmetic::gm_constants[] =
{
    {"e"    , "Eulersche Konstante"                 , M_E},
    {"pi"   , "Zahl PI"                             , M_PI},
    {"µ0"   , "Permeabilität des Vakuums"           , 12.566370614e-7},
    {"e0"   , "Dielektrizitätskonstante des Vakuums", 8.854187817e-12},
    {"h"    , "Plancksches Wirkungsquantum"         , 6.626070040E-34 },
    {"k"    , "Boltzmannsche Konstante"             , 1.3806485279e-23  },
    {"ec"   , "Elementarladung"                     , 1.6021766208e-19  },
    {"NA"   , "Avogadrosche Zahl"                   , 6.022140857e23  },
    {"c"    , "Lichtgeschwindigkeit"                , 2.99792458e8  },
    {"alpha", "Feinstrukturkonstante"               , 7.2973525664e-3 },
    {"me"   , "Elektronenruhemasse"                 , 9.1093835611e-31 },
    {"mp"   , "Protonenruhemasse"                   , 1.6726189821e-27 },
    {"mn"   , "Neutronenruhemasse"                  , 1.674927471214e-27 },
    {"µB"   , "Bohrsches Magneton"                  , 9.274009994e-24 },
    {"µN"   , "Kernmagneton"                        , 5.05078369931e-27 },
    {"ge"   , "g-Faktor des freien Elektrons"       , 2.002319304386},
    {"µe"   , "magnetisches Moment des Elektrons"   ,-9.28476462057e-24 },
    {"µp"   , "magnetisches Moment des Protons"     , 1.410606787397e-26 },
    {"amu"  , "Einheit der Atommasse"               , 1.66053904020e-27 },
    {"a0"   , "Bohrscher Radius"                    , 5.291772109217e-11 },
    {"re"   , "Elektronenradius"                    , 2.81837551647665e-15 },
    {"R"    , "Gaskonstante"                        , 8.314459848 },
    {"Vmol" , "Molvolumen"                          , 22.41396213e-3},
    {"F"    , "Faradaysche Konstante"               , 9.648533289e4},
    {"G"    , "Gravitationskonstante"               , 6.6740831e-11 },
    {"g"    , "Erdbeschleunigung"                   , 9.80665 },
    {NULL, "", 0}
};

double Arithmetic::gm_AngleTo   = 1.0;
double Arithmetic::gm_AngleFrom = 1.0;
const double Arithmetic::gm_d2PI = 2.0*M_PI;

Arithmetic::Arithmetic():
    mError(IDE_AR_NOEQUATION)
{
    char szDecimal_t[8] = ".";
    sprintf(szDecimal_t, "%.1f", 1.1);
    mDecimalChar[0] = szDecimal_t[1];
    mDecimalChar[1] = 0;
    mDecimalChar[2] = (mDecimalChar[0] == '.') ? ',' : ';';
    mDecimalChar[3] = 0;
}

Arithmetic::~Arithmetic()
{
    deleteAll();
}

void Arithmetic::resetError()
{
    mError = 0;
}

int Arithmetic::getError()
{
    return mError;
}

int  Arithmetic::getNumVar() const
{
    return mVarMap ? mVarMap->size() : 0;
}

char Arithmetic::getDecimal() const
{
    return mDecimalChar[0];
}

char Arithmetic::getSeparator() const
{
    return mDecimalChar[2];
}

std::complex<double> Arithmetic::calculate()
{
    std::complex<double> fResult = 0;
    if (mRoot)
    {
        mError = 0;
        try
        {
            fResult = calc(mRoot);
        }
        catch (int nError)
        {
            mError = nError;
        }
    }
    else
    {
        mError  = IDE_AR_NOEQUATION;
    }

    return fResult;
}

void Arithmetic::deleteAll()
{
    mRoot.reset();
    mVarMap.reset();
    mError  = IDE_AR_NOEQUATION;
}

Arithmetic::tVarMap& Arithmetic::getVarMap()
{
    if (not mVarMap)
    {
        mVarMap = std::make_shared<tVarMap>();
    }
    return *mVarMap;
}

void Arithmetic::setVarMap(const std::shared_ptr<Arithmetic::tVarMap>& aMap)
{
    mVarMap = aMap;
}

sError Arithmetic::setExpression(const char *aExpression)
{
    int     fGs   = 0,                                      // Gleichungsstringindex
            fV    = 0,                                      // Variablen-, Funktions- oder Zahlenstringindex
            fBinaryFunction = 0,                            // Funktion mit zwei Argumenten
            fChar = 0, fK, fLastChar = 0;                   // Einzelbuchstabe des Gleichungsstrings
    std::complex<double>     fTemp  = 0;
    char    fLevel,                                         // Operatorpriorität
            fStr[128] = "";                                 // Variablen-, Funktions- oder Zahlenstring

    tArithmaNode fNumber, fConstantNull,                    // Einfügeknoten für Zahlen, Konstanten, Variablen
            fBinaryOperator, fUnaryOperator;                // und Operatoren

    if ((!aExpression) || (!strlen(aExpression)))               // Prüfen des Eingabegleichungsstrings
    {
        mError = IDE_AR_NOEQUATION;
        return sError();
    }

    fNumber = make_shared<SArithmaNode>();                  // Vorbelegung für Zahl
    fNumber->level         = IDL_AR_NUMBER;                 // eine Zahl hat immer den Zahlen Level

    fBinaryOperator = make_shared<SArithmaNode>();          // Vorbelegung für Operator
    fBinaryOperator->brace = 0;                             // Operatoren haben immer die aktuelle Klammerebene

    fUnaryOperator = make_shared<SArithmaNode>();           // Vorbelegung für Funktion
    fUnaryOperator->level  = IDL_AR_FUNCTION;

    fConstantNull = make_shared<SArithmaNode>();            // Vorbelegung für die Konstante 0
    fConstantNull->level      = IDL_AR_NUMBER;
    fConstantNull->coperator  = IDO_AR_CONST;               // 0 ist eine Konstante
    getVarMap()[gConstNull]  = make_shared<complex<double>>(fTemp);
    fConstantNull->pvalue     = getVarMap()[gConstNull];
    fConstantNull->pszname    = gConstNull;

    do
    {
        fLastChar = fChar;
        fChar = aExpression[fGs++];
        fLevel = 0;
        switch(fChar)
        {
        case '\"': case '?':  case ';': case ':': case '#': case gConstID:
            mError = IDE_AR_INVALIDCHARACTER;
            return sError(fGs-1, fGs);
        case '\n':
        case '\r':
        case ' ' : break;
        case ')' : fBinaryOperator->brace--;
            break;
        case '(' : fBinaryOperator->brace++;
        case '^' :            fLevel++;                     // Level = 5
        case '/' : case '%' : fLevel++;                     // Level = 4
        case '*' : case '&' : fLevel++;                     // Level = 3
        case '-' :
            if (   fChar == '-'
                && (fLastChar=='e' || fLastChar == 'E'))
            {
                fStr[fV++] = fChar;
                break;
            }
            else
            {
                fLevel++;                                   // Level = 2   // Achtung nachträglich geändert !
            }
        case '+' : case '|' : case '!' :                    // Level = 1   // Testen
        case '>' : case '<' : case '=' :
            if (   fChar == '+'
                && (fLastChar=='e' || fLastChar == 'E'))
            {
                fStr[fV++] = fChar;
                break;
            }
            else
            {
                fLevel++;                                   // Level = 2   // Achtung nachträglich geändert !
            }
        case ',' :
            if (fChar == ',')
            {
                if (fBinaryFunction == 0)
                {
                    mError = IDE_AR_DECIMAL_POINT;
                    return sError(fGs-1, fGs);
                }
            }
        case  0  :
            if (fV)                                         // Wenn Variablen-, Funktions- oder Zahlenstring gefüllt ist
            {
                for (fK=0; fK<fV; fK++)                     // Zeichenkette analysieren
                {
                    if (!isdigit(fStr[fK]) && fStr[fK] != '.' && fStr[fK] != 'e'&& fStr[fK] != '-' && fStr[fK] != '+')
                    {
                        break;
                    }
                }
                if (fK==fV)                                 // ist es eine Zahl ?
                {
                    char *ptr = strstr(fStr, ".");          // Dezimalzeichen korrigieren
                    if (ptr) ptr[0]  = mDecimalChar[0];
                    fNumber->pvalue    = make_shared<complex<double>>(); // Speicher allocieren
                    *fNumber->pvalue = atof(fStr);          // String in Zahlenwert umwandeln
                    fNumber->coperator = IDO_AR_NUM;        // als numerische Konstante deklarieren
                    fNumber->pszname   = NULL;              // einen Namen hat sie nicht
                }
                else if (fChar == '(')                      // ist eine Klammer hinter der Zeichenkette
                {
                    fUnaryOperator->coperator = getFuncIndex(fStr); // Ist es eine Funktion
                    if (!fUnaryOperator->coperator) {mError = IDE_AR_NOFUNCTION; return sError(fGs-fV-1,fGs-1);}
                    if (fUnaryOperator->coperator < IDO_AR_LOGB)
                    {
                        fConstantNull->brace = fBinaryOperator->brace;   // die 0 befindet sich in der Funktionsklammer
                        mRoot = insert(mRoot, fConstantNull); // Einfügen der Konstante 0

                        // die Klammerebene der Funktion ist die vor der Funktionsklammer
                        fUnaryOperator->brace = fBinaryOperator->brace - 1;
                        mRoot = insert(mRoot, fUnaryOperator);// Einfügen des unären Operators
                    }
                    else
                    {
                        fBinaryFunction = fUnaryOperator->coperator;
                    }
                    fV = 0;
                    fStr[fV] = 0;
                    break;                                  // danach nächsten Ausdruck suchen
                }
                else                                        // sonst ist es eine Variable oder eine Konstante
                {
                    fTemp = std::complex<double>(getConstValue(fStr), 0);
                    if (fTemp.real() == 0)                  // Nein, als Variable deklarieren
                    {
                        fNumber->coperator = IDO_AR_VAR;
                    }
                    else                                    // Ja
                    {
                        fNumber->coperator = IDO_AR_CONST;  // als Konstante deklarieren
                        for (fK=fV; fK>0; fK--) fStr[fK] = fStr[fK-1];
                        fStr[0] = gConstID;                 // das Paragraphenzeichen als Indikator für Konstanten
                    }
                    if (not getVarMap().count(fStr))        // anlegen, wenn noch nicht vorhanden
                    {
                        getVarMap()[fStr] = make_shared<std::complex<double>>(fTemp);
                    }
                    fNumber->pvalue    = getVarMap()[fStr]; // Zeiger auf Variable übergeben
                    fNumber->pszname   = fStr;              // Zeiger auf Variablennamen übergeben
                }
                fV = 0;                                     // nach Wandlung auf 0 setzen
                fStr[fV] = 0;

                if (fNumber->brace < fBinaryOperator->brace)// wenn die Zahl eigentlich schon in der Klammer ist
                {
                    fNumber->brace = fBinaryOperator->brace;// Klammerebene für die Zahl setzen
                }
                mRoot = insert(mRoot, fNumber);               // Einfügen der Zahl oder Variable bei binären Operatoren

                fNumber->brace = fBinaryOperator->brace;    // nach dem Einfügen der Zahl Klammerebene aktualisieren

                if (fChar == 0) break;                      // ist es das letzte Zeichen : Ende
                if (fChar == ',')
                {
                    fChar = fBinaryFunction;
                    fBinaryFunction = 0;
                }

                fBinaryOperator->level     = fLevel;        // Struktur aktualisieren
                fBinaryOperator->coperator = fChar;
                mRoot = insert(mRoot, fBinaryOperator);       // Einfügen des binären Operators
            }
            else if ((fChar == '-') || (fChar == '!'))      // Ein Minuszeichen hinter einem Operator ist eine Negation
            {
                fConstantNull->brace = fBinaryOperator->brace;// Klammerebene aktualisieren
                mRoot = insert(mRoot, fConstantNull);         // Einfügen der Zahl 0
                fUnaryOperator->coperator = fChar;          // Struktur aktualisieren
                fUnaryOperator->brace     = fBinaryOperator->brace;
                mRoot = insert(mRoot, fUnaryOperator);        // Einfügen des unären Operators '-'
            }
            else if (fChar == '(')                          // Klammern sind erlaubt
            {
            }
            else                                            // sonst Fehler
            {
                mError = IDE_AR_OPERATOR; return sError(fGs-1, fGs);
            }
            break;
        default :
            fStr[fV++] = fChar;
            fStr[fV]   = 0;
            break;
        }
    }
    while (fChar != 0);

    if (fBinaryOperator->brace)
    {
        mError = IDE_AR_BRACE;
        return sError(fGs, fGs);
    }
    mExpression = aExpression;
    mError = IDE_AR_OK;
    return sError();
}
const std::string& Arithmetic::getExpression()
{
    return mExpression;
}

tArithmaNode Arithmetic::newnode(const tArithmaNode& aPin)
{
    return make_shared<SArithmaNode>(*aPin);
}

tArithmaNode Arithmetic::insert(const tArithmaNode& aP, const tArithmaNode& aPin)
{
    if (!aP)                                                     // Fall (1)
    {
        return newnode(aPin);                                    // neues Element eingefügen
    }
    else if (aPin->coperator<=IDO_AR_CONST)                      // ist es ein numerischer Wert
    {
        if      (!aP->pleft)                                     // Fall (2)
        {
            aP->pleft = insert(aP->pleft, aPin);                 // linkes Blatt zuerst belegen
        }
        else if (!aP->pright)                                    // Fall (3)
        {
            aP->pright = insert(aP->pright, aPin);               // dann rechtes Blatt
        }
        else                                                     // Fall (4)
        {
            aP->pright = insert(aP->pright, aPin);               // weitersuchen
        }
    }
    else                                                         // wenn es ein Operator ist
    {
        tArithmaNode fQ;
        if (aPin->order() <= aP->order())                        // Fall (5)
        {                                                        // wenn der Rang niedriger als der Rang im Knoten oder gleich ist
            fQ = newnode(aPin);                                  // Knoten dazwischensetzen
            fQ->pleft = aP;
            return fQ;
        }
        else                                                     // Fall (6)
        {
            aP->pright=insert(aP->pright, aPin);                 // weitersuchen
        }
    }
    return aP;
}

std::complex<double> Arithmetic::calc(const tArithmaNode& aNode)
{
    std::complex<double> fA, fB;
    bool    fAisComplex = false, fBisComplex = false;

    if (/*(p->coperator>=IDO_AR_NUM)&& "((DWORD) >= 0)"*/       // ist es ein numerischer Wert
            (aNode->coperator<=IDO_AR_CONST))
    {
        return *aNode->pvalue;
    }

    fA = calc(aNode->pleft);
    if (fA.imag() != 0) fAisComplex = true;
    fB = calc(aNode->pright);
    if (fB.imag() != 0) fBisComplex = true;

    if (fAisComplex || fBisComplex)                             // ein operator Komplex
    {
        switch(aNode->coperator)
        {
        case '+': return fA + fB;                               // binäre Operatoren
        case '-': return fA - fB;
        case '*': return fA * fB;
        case '/':
            if (fBisComplex)
            {
                if (hypot(fB.real(), fB.imag())  !=0) return fA / fB;
            }
            else
            {
                if (fB.real() != 0)                   return fA / fB.real();
            }
            throw (int)IDE_AR_ZERO;
            break;
        case  IDO_AR_ABS:                                     // unäre und binäre Funktionen
            if (fBisComplex)    return std::complex<double>(hypot(fB.real(), fB.imag()), 0);
            else                return std::complex<double>(fabs(fB.real()), 0);
        case  IDO_AR_SQRT:      return sqrt(fB);
        case '^':
            if (fBisComplex)    return pow(fA, fB);
            else                return pow(fA, fB.real());
        case IDO_AR_LOG:        return Logn(fB);
        case IDO_AR_LOG10:      return Logb(fB, 10);
        case IDO_AR_LOGB:       return Logb(fA, fB);
        case IDO_AR_COMPLEX:    return std::complex<double>(fA.real(), fB.real());
        case IDO_AR_COMPLEX_AP: return ComplexAP(fA.real(), fB.real()*gm_AngleFrom);
        case IDO_AR_HYPOT:      return Hypot(fA, fB);
        case IDO_AR_PHASE:      return arg(fB)*gm_AngleFrom;
        case IDO_AR_REAL:       return std::complex<double>(fB.real(), 0);
        case IDO_AR_IMAG:       return std::complex<double>(fB.imag(), 0);
        case IDO_AR_ASINH:      return Asinh(fB);
        case IDO_AR_ACOSH:      return Acosh(fB);
        case IDO_AR_ATANH:      if (fB!=std::complex<double>(1, 0)) return Atanh(fB);
            throw (int)IDE_AR_OUTOFRANGE;
        case IDO_AR_TAN:        if ((-355 <= fB.imag()) && (fB.imag() <= 355)) return tan(fB);
        case IDO_AR_TANH:       if ((-355 <= fB.real()) && (fB.real() <= 355)) return tanh(fB);
            throw (int)IDE_AR_OUTOFRANGE;
        case IDO_AR_SIN:        if ((-710 <= fB.imag()) && (fB.imag() <= 710)) return sin(fB);
        case IDO_AR_COS:        if ((-710 <= fB.imag()) && (fB.imag() <= 710)) return cos(fB);
        case IDO_AR_SINH:       if ((-710 <= fB.real()) && (fB.real() <= 710)) return sinh(fB);
        case IDO_AR_COSH:       if ((-710 <= fB.real()) && (fB.real() <= 710)) return cosh(fB);
            throw (int)IDE_AR_OUTOFRANGE;

#ifdef ARCUS_COMPLEX
        case  IDO_AR_ATAN  : return std::complex<double>(0.5*Logn((1.0+b) / (1.0-b))*gm_AngleFrom, 0);
        case  IDO_AR_ASIN  : if ((-1 <= b.real()) && (b.real() <= 1)) return casin(b)*gm_AngleFrom;
        case  IDO_AR_ACOS  : if ((-1 <= b.real()) && (b.real() <= 1)) return cacos(b)*gm_AngleFrom;
            throw (int)IDE_AR_OUTOFRANGE;
            break;
            throw (int)IDE_AR_OUTOFRANGE;
#endif
        default:
            throw (int)IDE_AR_NO_COMPLEX_IMPL;
        }
    }
    else
    {
        switch(aNode->coperator)
        {
        case '+': return std::complex<double>(fA.real()+fB.real(), 0);                                    // binäre Operatoren
        case '-': return std::complex<double>(fA.real()-fB.real(), 0);
        case '*': return std::complex<double>(fA.real()*fB.real(), 0);
        case '&': return std::complex<double>(((fA.real() != 0) && (fB.real() != 0)) ? 1 : 0, 0);
        case '|': return std::complex<double>(((fA.real() != 0) || (fB.real() != 0)) ? 1 : 0, 0);
        case '<': return std::complex<double>((fA.real() <  fB.real()) ? 1 : 0, 0);
        case '>': return std::complex<double>((fA.real() >  fB.real()) ? 1 : 0, 0);
        case '=': return std::complex<double>((fA.real() == fB.real()) ? 1 : 0, 0);
        case '/': if (fB.real() != 0) return std::complex<double>(fA.real() / fB.real(), 0);
        case '%': if (fB.real() != 0) return std::complex<double>(fmod(fA.real(), fB.real()), 0);
            throw (int)IDE_AR_ZERO;
            break;
        case '^':
            if ((fA.real() < 0) && (fmod(fB.real(), 1) != 0)) return pow(fA, fB.real());
            else                                            return std::complex<double>(pow(fA.real(), fB.real()), 0);
        case '!':            return std::complex<double>((fB.real() != 0) ? 0 : 1, 0);// unäre und binäre Funktionen
        case  IDO_AR_SIN   : return std::complex<double>(sinEx(fB.real()*gm_AngleTo), 0);
        case  IDO_AR_COS   : return std::complex<double>(cos(fB.real()*gm_AngleTo), 0);
        case  IDO_AR_TAN   : return std::complex<double>(tan(fB.real()*gm_AngleTo), 0);
        case  IDO_AR_HYPOT : return std::complex<double>(hypot(fA.real(), fB.real()), 0);

        case  IDO_AR_ATAN  : return std::complex<double>(atan(fB.real())*gm_AngleFrom, 0);
        case  IDO_AR_ASIN  : if ((-1 <= fB.real()) && (fB.real() <= 1)) return std::complex<double>(asin(fB.real())*gm_AngleFrom, 0);
        case  IDO_AR_ACOS  : if ((-1 <= fB.real()) && (fB.real() <= 1)) return std::complex<double>(acos(fB.real())*gm_AngleFrom, 0);
            throw (int)IDE_AR_OUTOFRANGE;
        case  IDO_AR_TANH  : return std::complex<double>(tanh(fB.real()), 0);
        case  IDO_AR_SINH  : if ((-710 <= fB.real()) && (fB.real() <= 710)) return std::complex<double>(sinh(fB.real()), 0);
        case  IDO_AR_COSH  : if ((-710 <= fB.real()) && (fB.real() <= 710)) return std::complex<double>(cosh(fB.real()), 0);
            throw (int)IDE_AR_OUTOFRANGE;
        case  IDO_AR_ATANH : return std::complex<double>(atanh(fB.real()), 0); break;
        case  IDO_AR_ASINH : return std::complex<double>(asinh(fB.real()), 0); break;
        case  IDO_AR_ACOSH : return std::complex<double>(acosh(fB.real()), 0); break;

        case  IDO_AR_ABS   : return std::complex<double>(fabs(fB.real()), 0);
        case  IDO_AR_FLOOR : return std::complex<double>(floor(fB.real()), 0);
        case  IDO_AR_CEIL  : return std::complex<double>(ceil(fB.real()), 0);
        case  IDO_AR_ROUND : return std::complex<double>(round(fB.real()), 0);

        case  IDO_AR_SQRT  : if      (fB.real() >= 0) return std::complex<double>(sqrt(fB.real()), 0);
            else                    return std::complex<double>(0, sqrt(-fB.real()));
        case  IDO_AR_LOG   : if      (fB.real() >  0) return std::complex<double>(log(fB.real()), 0);
            else if (fB.real() <  0) return Logn(fB);
        case  IDO_AR_LOG10 : if      (fB.real() >  0) return std::complex<double>(log10(fB.real()), 0);
            else if (fB.real() <  0) return Logb(complex<double>(10, 0), fB);
            throw (int)IDE_AR_OUTOFRANGE;
        case  IDO_AR_LOGB  : return Logb(fA, fB);
        case  IDO_AR_ATAN2 : return std::complex<double>(atan2(fA.real(),fB.real())*gm_AngleFrom, 0);
#ifdef _BESSEL_FUNCTIONS
        case  IDO_AR_BESS1 :
        {
            int n = (int)round(fA.real());
            if      (n==0) return std::complex<double>(j0(fB.real()), 0);
            else if (n==1) return std::complex<double>(j1(fB.real()), 0);
            else if (n>=2) return std::complex<double>(jn(n, fB.real()), 0);
            throw (int)IDE_AR_OUTOFRANGE;
        }
        case  IDO_AR_BESS2 :
        {
            int n = (int)round(fA.real());
            if      (n==0) return std::complex<double>(y0(fB.real()), 0);
            else if (n==1) return std::complex<double>(y1(fB.real()), 0);
            else if (n>=2) return std::complex<double>(yn(n, fB.real()), 0);
            throw (int)IDE_AR_OUTOFRANGE;
        }
#endif
        case IDO_AR_ROUNDTO:    return std::complex<double>(Roundto(fA.real(), fB.real()), 0);
        case IDO_AR_COMPLEX:    return std::complex<double>(fA.real(), fB.real());
        case IDO_AR_COMPLEX_AP: return ComplexAP(fA.real(), fB.real()*gm_AngleTo);
        case IDO_AR_PHASE:      case IDO_AR_IMAG: return std::complex<double>(0, 0);
        case IDO_AR_MIN:        return std::complex<double>(std::min(fA.real(), fB.real()), 0);
        case IDO_AR_MAX:        return std::complex<double>(std::max(fA.real(), fB.real()), 0);
        }
    }
    return 1;
}

std::complex<double> ComplexAP(double aAmplitude, double aPhase)
{
    return std::complex<double> (aAmplitude * cos(aPhase), aAmplitude * sin(aPhase));;
}

void Arithmetic::show() const
{
    show(mRoot, 0);
}

void Arithmetic::show(const tArithmaNode& aP, int aLevel) const
{
    if (!aP) return;
    for (int i=0; i<aLevel; ++i)
    {
        std::cout << "--- ";
    }
    switch (aP->coperator)
    {
    case  IDO_AR_NUM :
        std::cout << *aP->pvalue << std::endl;
        break;
    case  IDO_AR_VAR : case  IDO_AR_CONST :
        std::cout << aP->pszname << std::endl;
        break;
    case '+': case '-': case '*': case '&': case '|': case '<':
    case '>': case '=': case '/': case '%': case '^': case '!':
        std::cout << "Operator:" << static_cast<char>(aP->coperator) << std::endl;
        break;
    default:
        if      ((aP->coperator >= IDO_AR_SIN) && (aP->coperator <= IDO_AR_IMAG))
        {
            std::cout << gm_UnaryFkt[aP->coperator - IDO_AR_SIN] << std::endl;
        }
        else if ((aP->coperator >= IDO_AR_LOGB) && (aP->coperator <= IDO_AR_MAX))
        {
            std::cout << gm_BinaryFkt[aP->coperator - IDO_AR_LOGB] << std::endl;
        }
    }

    show(aP->pleft, aLevel+1);
    show(aP->pright, aLevel+1);
}


const char * Arithmetic::getVarName(int aIndex) const
{
    if (mVarMap)
    {
        auto& fMap  = *mVarMap;
        auto fIter = fMap.begin();
        for (int i=0; i<aIndex && fIter != fMap.end(); ++i, ++fIter);
        if (fIter != fMap.end())
        {
            return fIter->first.c_str();
        }
    }
    return "";
}


std::complex<double> * Arithmetic::getVarAdress(const char *aName) const
{
    if (mVarMap)
    {
        auto& fMap  = *mVarMap;
        auto fIt = fMap.find(aName);
        if (fIt != fMap.end())
        {
            return fIt->second.get();
        }
    }
    return nullptr;
}

int Arithmetic::getFuncIndex(const char *aName)
{
    int i;
    if (!aName)         return 0;
    if (!strlen(aName)) return 0;
    for (i=0; gm_UnaryFkt[i]  != NULL; i++) if (!strcmp(gm_UnaryFkt[i] , aName)) return i+IDO_AR_SIN;
    for (i=0; gm_BinaryFkt[i] != NULL; i++) if (!strcmp(gm_BinaryFkt[i], aName)) return i+IDO_AR_LOGB;
    return 0;
}

double Arithmetic::getConstValue(const char *aName)
{
    int i;
    if (!aName)         return 0;
    if (!strlen(aName)) return 0;
    for (i=0; gm_constants[i].name != NULL; i++) if (!strcmp(gm_constants[i].name, aName)) return gm_constants[i].value;
    return 0;
}

std::complex<double> Arithmetic::Logn(const std::complex<double>& aValue)
{
    double fBetrag = hypot(aValue.real(), aValue.imag());
    double fPhase  = arg(aValue);
    return ComplexAP(log(fBetrag), fPhase);
}

// selbsdefinierte Funktionen
std::complex<double> Arithmetic::Logb(const std::complex<double>& aValue, const std::complex<double>& aBase)
{
    return Logn(aBase) / Logn(aValue);
}

std::complex<double> Arithmetic::Asinh(const std::complex<double>& aValue)
{
    return Logn(aValue + sqrt(aValue * aValue + std::complex<double>(1, 0)));
}

std::complex<double> Arithmetic::Acosh(const std::complex<double>& aValue)
{
    return Logn(aValue + sqrt(aValue * aValue - std::complex<double>(1, 0)));
}

std::complex<double> Arithmetic::Atanh(const std::complex<double>& aValue)
{
    return 0.5 * Logn((std::complex<double>(1, 0) + aValue) / (std::complex<double>(1, 0) - aValue));
}

double Arithmetic::Roundto(double aRounding, double aValue)
{
    return round(aRounding/aValue)*aValue;
}

double Arithmetic::sinEx(double aValue)
{
    if (aValue > gm_d2PI*2)
    {
        double fDiv2pi = floor(aValue/gm_d2PI);
        aValue -= (fDiv2pi * gm_d2PI);
    }
    return sin(aValue);
}

std::complex<double> Arithmetic::Hypot(std::complex<double>&aA, std::complex<double>&aB)
{
    return sqrt(aA*aA + aB*aB);
}

std::complex<double> Arithmetic::ComplexAP(double aAmplitude, double aPhase)
{
    return std::complex<double> (aAmplitude * cos(aPhase), aAmplitude * sin(aPhase));
}
