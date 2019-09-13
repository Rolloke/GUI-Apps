#pragma once

#include <vector>
#include <string>
#include <exception>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <QRect>

#include "defines.h"
#include "matrix.h"


class CPlotterDC;



class PostscriptInterpreter
{
public:
    PostscriptInterpreter();
    ~PostscriptInterpreter();
    // TODO_A implement more commands
    // get, null, cvi, cvs, string, copy
    // where
    //• xcheck, rcheck, and wcheck query the literal/executable and access attributes of an object.
    //• cvlit and cvx change the literal/executable attribute of an object.
    //• readonly, executeonly, and noaccess reduce an object’s access attribute. Access can only be reduced, never increased.
    //• cvi and cvr convert between integer and real types, and interpret a numeric string as an integer or real number.
    //• cvn converts a string to a name object defined by the characters of the string.
    //• cvs and cvrs convert objects of several types to a printable string representation.
    // currentmatrix, setfont


    // enums
    enum eNames { unknown=-1, literal_string, number, integer, boolean, literal_array, procedure, find_procedure,
        moveto, rmoveto, lineto, rlineto, curveto, rcurveto, arc, arcn, arct, show, ashow,
        dup, exch, pop, copy, roll, index, mark, clear, count, counttomark, cleartomark, length,
        bind, def, astore, store, aload, grestore, grestoreall, _array, packedarray, type, anchorsearch, search, token,
        add, sub, mul, div, idiv, mod,
        eq, gt, lt, ge, le, _or, _and, _not, _xor, bitshift, forall, _if, _for, ifelse, repeat, loop, exit, lookup_execstack, countexecstack, execstack,
        _abs, neg, ceiling, _floor, _round, truncate, _sqrt, _exp, _ln, _log, _sin, _cos, _atan, _rand, _srand, rrand,
        newpath, closepath, stroke, fill, rectfill,        
        gsave, dict, begin, put, get, putinterval, getinterval, findfont, makefont, image, pathbbox, save, restore, showpage, setpagedevice,
        rotate, scale, translate, setpagesize, currentmatrix, setmatrix, concat, concatmatrix, matrix, identmatrix, defaultmatrix, initmatrix,
        transform, dtransform, itransform, idtransform, invertmatrix,
        setdash, setlinecap, setlinejoin, setlinewidth, setfont, scalefont,
        curly_brace_open, square_brace_open, round_brace_open, angle_brace_open,
        setrgbcolor, setcmykcolor, sethsbcolor
    };
    
    struct LineCap { enum eLineCap { butt_cap=1, round_cap, projecting_squar_cap }; };
    struct LineJoin{ enum eLineJoin{ miter_join=1, round_join, bevel_join }; };

    class Error : public std::exception
    {
    public:
        enum eError{ wrong_argument, too_few_aruments, not_implemented, unmatchedmark, rangecheck, undefinedresult };
        Error(const char* awhat, eError aE):  mWhat(awhat), mE(aE)
        {
        };
        virtual const char* what() const throw();
    private:
        const char*mWhat;
        eError mE;
        eNames mN;
        PostscriptInterpreter*mParent;
    };

    class parameter
    {
        enum { matrixsize = 6 };
    public:
        parameter(eNames aName=unknown);
        parameter(const std::string& fLiteral, eNames aName=unknown);
        parameter(const boost::shared_ptr< std::vector<parameter> >& param, eNames aName=procedure);
        parameter(FLOAT aNumber);
        parameter(int aInt, eNames aName=integer);
        parameter(bool aBool);
        parameter(const rke::matrix<FLOAT>& aMatrix);

        bool                          isNumber() const     { return mName == number || mName == integer; }
        bool                          isMatrix() const     { return mName == matrix && mParameter->size() == matrixsize; }
        eNames                        getName() const      { return mName;      }
        FLOAT                         getNumber() const;
        int                           getInteger() const;
        bool                          getBool() const      { return v.mBool;    }
        const std::string&            getLiteral() const   { return *mLiteral;   }
        std::string&                  getString()          { return *mLiteral;   }
        const std::vector<parameter>& getProcedure() const { return *mParameter; }
        std::vector<parameter>&       getParameter() const { return *mParameter; }
        const char*                   getText() const;
        const char*                   getType() const;
        rke::matrix<FLOAT>            getMatrix() const;
        bool                          setMatrix(const rke::matrix<FLOAT>& aMatrix);
        void                          transform(parameter& x, parameter& y) const;

        operator FLOAT() { return getNumber(); }
        void operator=(const FLOAT aValue) { mName = number; v.mNumber = aValue; }

        operator int() { return getInteger(); }
        void operator=(const int aValue) { mName = integer; v.mInteger = aValue; }

        operator bool() { return getBool(); }
        void operator=(const bool aValue) { mName = boolean; v.mBool = aValue; }

        friend parameter operator+(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == integer && a2.mName == integer) return parameter(a1.getInteger() + a2.getInteger());
            else                                            return parameter(a1.getNumber()  + a2.getNumber());
        };
        friend parameter operator-(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == integer && a2.mName == integer) return parameter(a1.getInteger() - a2.getInteger());
            else                                            return parameter(a1.getNumber()  - a2.getNumber());
        };
        friend parameter operator*(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == integer && a2.mName == integer) return parameter(a1.getInteger() * a2.getInteger());
            else                                            return parameter(a1.getNumber()  * a2.getNumber());
        };
        friend parameter operator/(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == integer && a2.mName == integer) return parameter(a1.getInteger() / a2.getInteger());
            else                                            return parameter(a1.getNumber()  / a2.getNumber());
        };
        friend parameter operator&&(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == integer && a2.mName == integer) return parameter(a1.getInteger() & a2.getInteger());
            else if (a1.mName == boolean && a2.mName == boolean) return parameter(a1.getBool() && a2.getBool());
            else throw Error("", Error::wrong_argument);
        };
        friend parameter operator||(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == integer && a2.mName == integer) return parameter(a1.getInteger() | a2.getInteger());
            else if (a1.mName == boolean && a2.mName == boolean) return parameter(a1.getBool() || a2.getBool());
            else throw Error("", Error::wrong_argument);
        };
        friend parameter operator>>(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == integer && a2.mName == integer)
            {
                if (a2.getInteger()>0) return parameter(a1.getInteger() << a2.getInteger());
                else                   return parameter(a1.getInteger() >> a2.getInteger());
            }
            else throw Error("", Error::wrong_argument);
        };
        friend parameter operator^(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == integer && a2.mName == integer) return parameter(a1.getInteger() ^ a2.getInteger());
            else if (a1.mName == boolean && a2.mName == boolean) return parameter(a1.getBool() ^ a2.getBool());
            else throw Error("", Error::wrong_argument);
        };
        friend parameter operator!(const parameter& a1)
        {
            if (a1.mName == integer) return parameter(static_cast<int>(!a1.getInteger()));
            else if (a1.mName == boolean) return parameter(!a1.getBool());
            else throw Error("", Error::wrong_argument);
        };
        friend parameter operator<(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == literal_string && a2.mName == literal_string) return parameter(a1.getLiteral() < a2.getLiteral());
            else if (a1.isNumber() && a2.isNumber()) return parameter(a1.getNumber() < a2.getNumber());
            else throw Error("", Error::wrong_argument);
        };
        friend parameter operator<=(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == literal_string && a2.mName == literal_string) return parameter(a1.getLiteral() <= a2.getLiteral());
            else if (a1.isNumber() && a2.isNumber()) return parameter(a1.getNumber() <= a2.getNumber());
            else throw Error("", Error::wrong_argument);
        };
        friend parameter operator>(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == literal_string && a2.mName == literal_string) return parameter(a1.getLiteral() > a2.getLiteral());
            else if (a1.isNumber() && a2.isNumber()) return parameter(a1.getNumber() > a2.getNumber());
            else throw Error("", Error::wrong_argument);
        };
        friend parameter operator>=(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == literal_string && a2.mName == literal_string) return parameter(a1.getLiteral() >= a2.getLiteral());
            else if (a1.isNumber() && a2.isNumber()) return parameter(a1.getNumber() >= a2.getNumber());
            else throw Error("", Error::wrong_argument);
        };
        friend parameter operator==(const parameter& a1, const parameter& a2)
        {
            if (a1.mName == literal_string && a2.mName == literal_string) return parameter(a1.getLiteral() == a2.getLiteral());
            else if (a1.isNumber() && a2.isNumber()) return parameter(a1.getNumber() == a2.getNumber());
            else throw Error("", Error::wrong_argument);
        };
    private:
        eNames      mName;
        boost::shared_ptr< std::string > mLiteral;
        union {
        FLOAT mNumber;
        int   mInteger;
        bool  mBool;
        } v;
        boost::shared_ptr< std::vector<parameter> > mParameter;
    };


    typedef std::vector<parameter>              parameter_vector;
    typedef std::map< std::string, eNames>      name_map;
    typedef boost::shared_ptr<parameter_vector> parameter_vector_ptr;
    typedef std::map< std::string, parameter_vector_ptr> procedure_map;
    typedef boost::shared_ptr< std::string >    std_string_ptr;

    void setPlotterDC(CPlotterDC*aDC) { mDC = aDC; }
    void usePath(bool aUse) { mUsePath = aUse; }
    // worker
    void readFile(const QString& sFileName);
    void parseText(const QString& aText);
    void parseInitial(const std::string& aLine);
    void parseProlog(const std::string& aLine);
    void parsePageSetup(const std::string& aLine);
    void parseDraw(const std::string& aLine);

#ifdef FORTH
    void   parseOperators(const std::string& aObjects, parameter_vector& aProcedure);
    bool   findCorrespondingBraces(const std::string&aString, size_t &aBraceOpen, size_t &aBraceClose, const char *aBO="{", const char *aBC="}");
    void   execute(parameter_vector& anObject);
    void   buildParameterArray(std::string& fOperatorString, parameter_vector& fOperators, eNames aType);
    void   parseString(const std::string& aString);

    void   do_dup(parameter_vector&aVec);
    void   do_exch(parameter_vector&aVec);
    void   do_pop(parameter_vector&aVec, size_t aNum=1);
    void   do_copy(parameter_vector&aVec);
    void   do_roll(parameter_vector&aVec);
    void   do_index(parameter_vector&aVec);
    void   do_mark(parameter_vector&aVec);
    void   do_clear(parameter_vector&aVec);
    void   do_count(parameter_vector&aVec);
    void   do_counttomark(parameter_vector&aVec);
    void   do_cleartomark(parameter_vector&aVec);
    void   do_exit(parameter_vector &anObject, size_t &aObj);
    void   create_array(const parameter& anObject, size_t s);
    void   get_length(const parameter& anObject, size_t s);
    void   do_anchorsearch(const parameter& anObject, size_t s);
    void   do_search(const parameter& anObject, size_t s);
    void   do_aload(const parameter& anObject, size_t s);
    void   do_astore(const parameter& anObject, size_t s);
    void   do_get(const parameter& anObject, size_t s);
    void   do_put(const parameter& anObject, size_t s);
    void   do_getinterval(const parameter& anObject, size_t s);
    void   do_putinterval(const parameter& anObject, size_t s);
    void   do_store(const parameter& anObject, size_t s);
    void   do_bind(const parameter& anObject, size_t s);
    void   do_move_to(const parameter& anObject, size_t s);
    void   draw_line_to(const parameter& anObject, size_t s);
    void   draw_curve_to(const parameter& anObject, size_t s);
    void   do_rotate(const parameter& anObject, size_t s);
    void   do_transform(const parameter& anObject, size_t s);
    void   do_scale(const parameter& anObject, size_t s);
    void   do_translate(const parameter& anObject, size_t s);
    void   do_concatmatrix(const parameter& anObject, size_t s);
    void   fill_defaultmatrix(const parameter& anObject, size_t s);
    void   fill_identmatrix(const parameter& anObject, size_t s);
    void   do_rectfill(const parameter& anObject, size_t s);
    void   _if_(const parameter& anObject, size_t s);
    void   lookupExecstack(parameter_vector &anObject, size_t &aObj);
#endif

    const parameter &findInProcedureMap(const parameter& aParam);
    static void decodeNumbers(const stringlist& aStrings, size_t aCount, std::vector<float>& aNumbers);
    const char* getNameOf(const parameter& aParam);

private:
    name_map mNameMap;
    procedure_map mProcedureMap;
    std::string mString;
    CPlotterDC *mDC;
    
    boost::function< void (const std::string& ) > mParse;

    QRect mBoundingBox;
    float mTranslateX;
    float mTranslateY;
    float mScaleX;
    float mScaleY;
    float mRotate;

private:
#ifdef FORTH
    parameter_vector mOperandStack;     // stack for operands 
    parameter_vector mExecutionStack;   // stack executing loops
    stringlist       mErrors;
    std::vector<int> mGraphicsState;
    parameter        mDefaultMatrix;
    std::vector<name_map> mDictionaryStack;
    name_map*        mpCurrentDictionary;
    int              mLine;
public:
    bool  mUsePath;
#endif

};
