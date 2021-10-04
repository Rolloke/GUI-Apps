#ifndef CARITHMETIC_H
#define CARITHMETIC_H


#include <math.h>

#include <complex>
#include <memory>
#include <map>

#define ARCUS_COMPLEX

#if __has_cpp_attribute(fallthrough)
  #define CPP17_FALLTHROUGH        [[fallthrough]];
#else
  #define CPP17_FALLTHROUGH
#endif

#define  IDE_AR_OK               0 // Gleichung ist ok
#define  IDE_AR_BRACE            1 // Klammerfehler
#define  IDE_AR_OUTOFRANGE       2 // Zahlenbereich verlassen
#define  IDE_AR_OPERATOR         3 // aufeinanderfolgende Operatoren sind selten erlaubt
#define  IDE_AR_NOEQUATION       4 // Keine Gleichung vorhanden
#define  IDE_AR_ZERO             5 // durch 0 geteilt
#define  IDE_AR_NONUMBER         6 // Zahl kann nicht konvertiert werden
#define  IDE_AR_NOFUNCTION       7 // Funktion kann nicht konvertiert werden
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

#define  IDO_AR_SIN        127     // unäre Operatoren (Funktionen)
#define  IDO_AR_COS        128
#define  IDO_AR_TAN        129

#define  IDO_AR_ASIN       130
#define  IDO_AR_ACOS       131
#define  IDO_AR_ATAN       132

#define  IDO_AR_SINH       133
#define  IDO_AR_COSH       134
#define  IDO_AR_TANH       135

#define  IDO_AR_ASINH      136
#define  IDO_AR_ACOSH      137
#define  IDO_AR_ATANH      138

#define  IDO_AR_ABS        139
#define  IDO_AR_FLOOR      140
#define  IDO_AR_CEIL       141
#define  IDO_AR_ROUND      142

#define  IDO_AR_LOG        143
#define  IDO_AR_LOG10      144
#define  IDO_AR_SQRT       145
#define  IDO_AR_PHASE      146
#define  IDO_AR_REAL       147
#define  IDO_AR_IMAG       148
#define  IDO_AR_ERF        149
#define  IDO_AR_ERFC       150
#define  IDO_AR_GAMMA      151

#define  IDO_AR_LOGB       160      // binäre Funktionen
#define  IDO_AR_HYPOT      161
#define  IDO_AR_BESS1      162
#define  IDO_AR_BESS2      163
#define  IDO_AR_ATAN2      164
#define  IDO_AR_ROUNDTO    165
#define  IDO_AR_COMPLEX    166
#define  IDO_AR_COMPLEX_AP 167
#define  IDO_AR_MIN        168
#define  IDO_AR_MAX        169

struct SArithmaNode;
struct SVarNode;

struct sConstants
{
    const char* name;
    const char* Description;
    double value;
};

typedef std::shared_ptr<SArithmaNode> tArithmaNode;

struct sError
{
    sError(int aStart=-1, int aStop=-1): mStart(aStart), mStop(aStop) {}
    bool valid() { return mStart >= 0 || mStop >= 0; }
    int mStart;
    int mStop;
};

class CArithmetic
{
public:
    CArithmetic();
    ~CArithmetic();

    typedef std::map<std::string, std::shared_ptr<std::complex<double>> > tVarMap;

    sError         setEquation(const char *);
    std::complex<double>        calculate();
    void           deleteAll();
    void           show();
    void           resetError();
    int            getError();
    int            getNumVar();
    const char*    getVarName(int);
    std::complex<double>* getVarAdress(const char *);
    char           getDecimal();
    char           getSeparator();

    void           setVarMap(const std::shared_ptr<tVarMap>& aMap);
    tVarMap&       getVarMap();

    const static char  * gm_UnaryFkt[];
    const static char  * gm_BinaryFkt[];
    const static sConstants gm_constants[];

private:

    tArithmaNode   insert(const tArithmaNode&, const tArithmaNode&);
    tArithmaNode   newnode(const tArithmaNode& );
    std::complex<double>  calc(const tArithmaNode&);
    void           show(const tArithmaNode&, int aLevel);
    double         getConstValue(const char *);
    int            getFuncIndex(const char *);

    int            m_nError;
    int            m_nNumVar;
    std::shared_ptr<tVarMap> mVarMap;
    tArithmaNode   mRoot;
    char           mDecimalChar[4];
public:

public:
    static std::complex<double> ComplexAP(double dAmplitude, double dPhase);
    static std::complex<double> Logn(const std::complex<double>& wert);
    static std::complex<double> Asinh(const std::complex<double>& wert);
    static std::complex<double> Acosh(const std::complex<double>& wert);
    static std::complex<double> Atanh(const std::complex<double>& wert);
    static std::complex<double> Logb(const std::complex<double>&, const std::complex<double>&);
    static double Roundto(double, double);
    static double sinEx(double);
    static std::complex<double> Hypot(std::complex<double>&a, std::complex<double>&b);

    const static double  gm_d2PI;
    static double        gm_AngleTo;
    static double        gm_AngleFrom;
    static const char    gConstID;
    static const char    gConstNull[];
};

#endif // CARITHMETIC_H
