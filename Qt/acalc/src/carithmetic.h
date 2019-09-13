#ifndef CARITHMETIC_H
#define CARITHMETIC_H


#include <cmath>
#include <complex>
#include <memory>
#include <map>

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
