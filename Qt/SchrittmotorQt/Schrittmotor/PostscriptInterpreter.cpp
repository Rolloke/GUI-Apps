#include "PostscriptInterpreter.h"
#include "PlotterDC.h"
#include <QErrorMessage>
#include "commonFunctions.h"

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <QFile>
#include <boost/lexical_cast.hpp>

#define TRACE_NYI TRACE
//#define TRACE_NYI //

// FEATURE_A Binary Tokens, String Tokens, Homogeneous Number Arrays, Binary Object Sequences, Encoded Number Strings
// FEATURE_A implement dictionary first

// Units of length[edit]
// PostScript uses the point as its unit of length.
// PostScript uses exactly 72 points to the inch. 
// 127 / 360 mm = ca. 352.77777 micro meter. 
namespace
{
const FLOAT toTenthOf_mm = static_cast<FLOAT>(127.0*SCALE_FACTOR / (360.0)); // factor to calculate 1/10 mm
// 842 595 -> 297 mm * 210 mm -> DIN A4
}

PostscriptInterpreter::parameter::parameter(eNames aName) : mName(aName)
{
    switch (mName)
    {
    case procedure: case literal_array:
        mParameter = parameter_vector_ptr(new parameter_vector);
        break;
    default: break;
    }
    v.mNumber = 0; 
}

PostscriptInterpreter::parameter::parameter(const std::string& fLiteral, eNames aName) : mName(aName)
{ 
    using boost::lexical_cast;
    v.mNumber = 0;
    if (fLiteral.size())
    {
        if (aName == literal_string)
        {
            mLiteral = std_string_ptr(new std::string(fLiteral));
        }
        else if (isdigit(fLiteral[0]) || fLiteral[0] == '-' || fLiteral[0] == '+')
        {
            if (fLiteral.find(".") == std::string::npos)
            {
                v.mInteger = lexical_cast<int>(fLiteral);
                mName = integer;
            }
            else
            {
                v.mNumber = lexical_cast<FLOAT>(fLiteral);
                mName = number;
            }
        }
        else if (fLiteral == "false")
        {
            v.mBool = false;
            mName = boolean;
        }
        else if (fLiteral == "true")
        {
            v.mBool = true;
            mName = boolean;
        }
        else
        {
            mLiteral = std_string_ptr(new std::string(fLiteral));
            mName = literal_string;
        }
    }
}

PostscriptInterpreter::parameter::parameter(const boost::shared_ptr< std::vector<parameter> >& param, eNames aName): mName(aName)
{
    v.mNumber = 0;
    mParameter = param;
}

PostscriptInterpreter::parameter::parameter(FLOAT aNumber): mName(number)
{
    v.mNumber = aNumber;
}

PostscriptInterpreter::parameter::parameter(int aInt, eNames aName): mName(aName)
{
    v.mInteger = aInt;
}

PostscriptInterpreter::parameter::parameter(bool aBool): mName(boolean)
{
    v.mBool = aBool;
}

PostscriptInterpreter::parameter::parameter(const rke::matrix<FLOAT>& aMatrix)
{
    setMatrix(aMatrix);
}

const char* PostscriptInterpreter::parameter::getType() const
{
    switch (mName)
    {
    case literal_string:    return "stringtype";
    case number:            return "realtype";
    case integer:           return "integertype";
    case square_brace_open: return "marktype";
    case boolean:           return "booleantype";
    case procedure:         return "operatortype";
    case _array:            return "arraytype";
    case matrix:            return "matrix";
    default:                return "nametype";
    }
}

const char* PostscriptInterpreter::parameter::getText() const
{
    static char sOutput[64];

    switch (mName)
    {
    case literal_string: return mLiteral->c_str(); 
    case number:
        sprintf(sOutput, "# %f", v.mNumber);
        return sOutput;
    case integer:
        sprintf(sOutput, "# %d", v.mInteger);
        return sOutput;
    case boolean:        return v.mBool ? "true" : "false";
    case procedure:      return "procedure";
    case _array:         return "array";
    case matrix:         return "matrix";
    default:
        sprintf(sOutput, "# %d", mName);
        return sOutput;
    }
}

FLOAT PostscriptInterpreter::parameter::getNumber() const
{
    if (mName == integer) return static_cast<FLOAT>(v.mInteger); 
    return v.mNumber;  
}

int PostscriptInterpreter::parameter::getInteger() const 
{
    if (mName == number) return static_cast<int>(v.mNumber);
    return v.mInteger;
}

const char* PostscriptInterpreter::Error::what() const throw()
{
    static char szError[1024];
    switch (mE)
    {
    case wrong_argument:   strcpy(szError, "wrong argument"); break;
    case too_few_aruments: strcpy(szError, "too few aruments"); break;
    case not_implemented:  strcpy(szError, "not implemented"); break;
    case unmatchedmark:    strcpy(szError, "unmatched mark"); break;
    case rangecheck:       strcpy(szError, "index out of range"); break;
    case undefinedresult:  strcpy(szError, "undefined result"); break;
    default:               strcpy(szError, "unknown error"); break;
    }
    strcat(szError, " for ");
    strcat(szError, exception::what());
    return szError;
}

rke::matrix<FLOAT> PostscriptInterpreter::parameter::getMatrix() const 
{
    rke::matrix<FLOAT> fMatrix;
    if (isMatrix())
    {
        fMatrix.resize(3, 3, true);
        fMatrix[0][0] = getParameter()[0];
        fMatrix[1][0] = getParameter()[1];
        fMatrix[0][1] = getParameter()[2];
        fMatrix[1][1] = getParameter()[3];
        fMatrix[0][2] = getParameter()[4];
        fMatrix[1][2] = getParameter()[5];
    }
    return fMatrix;
}

bool PostscriptInterpreter::parameter::setMatrix(const rke::matrix<FLOAT>& aMatrix)
{
    //[[ a c tx ]
    // [ b d ty ]
    // [ 0 0 0  ]]
    // [ a b c d tx ty ]
    if (aMatrix.rows() == 3 && aMatrix.columns() == 3)
    {
        if (!mParameter.get())
        {
            mParameter.reset(new parameter_vector(matrixsize, static_cast<FLOAT>(0.0)));
        }
        mName = matrix;
        getParameter()[0] = aMatrix[0][0];
        getParameter()[1] = aMatrix[1][0];
        getParameter()[2] = aMatrix[0][1];
        getParameter()[3] = aMatrix[1][1];
        getParameter()[4] = aMatrix[0][2];
        getParameter()[5] = aMatrix[1][2];
        return true;
    }
    return false;
}
void  PostscriptInterpreter::parameter::transform(parameter& x, parameter& y) const
{
    if (isMatrix())
    {
        parameter temp = x * getProcedure()[0] + y * getProcedure()[2] + getProcedure()[4];
                     y = x * getProcedure()[1] + y * getProcedure()[3] + getProcedure()[5];
        x = temp;
    }
}

#define SET_MAP(map, value) map[#value] = value;

PostscriptInterpreter::PostscriptInterpreter() : mDC(0)
, mTranslateX(0)
, mTranslateY(0)
, mScaleX(1)
, mScaleY(1)
, mRotate(0)

#ifdef FORTH
, mpCurrentDictionary(0)
, mLine(0)
, mUsePath(0)
#endif
{
    SET_MAP(mNameMap, moveto);
    SET_MAP(mNameMap, rmoveto);
    SET_MAP(mNameMap, lineto);
    SET_MAP(mNameMap, rlineto);
    SET_MAP(mNameMap, rcurveto);
    SET_MAP(mNameMap, curveto);
    SET_MAP(mNameMap, arc);
    SET_MAP(mNameMap, arcn);
    SET_MAP(mNameMap, arct);
    SET_MAP(mNameMap, newpath);
    SET_MAP(mNameMap, closepath);
    SET_MAP(mNameMap, stroke);
    SET_MAP(mNameMap, rotate);
    SET_MAP(mNameMap, scale);
    SET_MAP(mNameMap, translate);
    SET_MAP(mNameMap, setpagesize);
    SET_MAP(mNameMap, currentmatrix);
    SET_MAP(mNameMap, setmatrix);
    SET_MAP(mNameMap, identmatrix);
    SET_MAP(mNameMap, defaultmatrix);
    SET_MAP(mNameMap, initmatrix);
    SET_MAP(mNameMap, fill); 
    SET_MAP(mNameMap, setlinewidth);
    SET_MAP(mNameMap, setdash);
    SET_MAP(mNameMap, setlinecap);
    SET_MAP(mNameMap, setlinejoin);
    SET_MAP(mNameMap, show);
    SET_MAP(mNameMap, ashow);
    SET_MAP(mNameMap, setfont);
    SET_MAP(mNameMap, dup);
    SET_MAP(mNameMap, exch);
    SET_MAP(mNameMap, pop);
    SET_MAP(mNameMap, copy);
    SET_MAP(mNameMap, roll);
    SET_MAP(mNameMap, index);
    SET_MAP(mNameMap, mark);
    SET_MAP(mNameMap, clear);
    SET_MAP(mNameMap, count);
    SET_MAP(mNameMap, counttomark);
    SET_MAP(mNameMap, cleartomark);
    SET_MAP(mNameMap, ceiling);
    SET_MAP(mNameMap, truncate);
    SET_MAP(mNameMap, add);
    SET_MAP(mNameMap, sub);
    SET_MAP(mNameMap, mul);
    SET_MAP(mNameMap, div);
    SET_MAP(mNameMap, idiv);
    SET_MAP(mNameMap, mod);
    SET_MAP(mNameMap, neg);
    SET_MAP(mNameMap, rectfill);
    SET_MAP(mNameMap, rrand);
    SET_MAP(mNameMap, bind);
    SET_MAP(mNameMap, def);
    SET_MAP(mNameMap, store);
    SET_MAP(mNameMap, astore);
    SET_MAP(mNameMap, aload);
    SET_MAP(mNameMap, setrgbcolor);
    SET_MAP(mNameMap, setcmykcolor);
    SET_MAP(mNameMap, sethsbcolor);
    SET_MAP(mNameMap, grestore);
    SET_MAP(mNameMap, grestoreall);
    SET_MAP(mNameMap, eq);
    SET_MAP(mNameMap, gt);
    SET_MAP(mNameMap, lt);
    SET_MAP(mNameMap, ge);
    SET_MAP(mNameMap, le);
    SET_MAP(mNameMap, _or);
    SET_MAP(mNameMap, _and);
    SET_MAP(mNameMap, _xor);
    SET_MAP(mNameMap, _not);
    SET_MAP(mNameMap, bitshift);    
    SET_MAP(mNameMap, type);
    SET_MAP(mNameMap, anchorsearch);
    SET_MAP(mNameMap, search);
    SET_MAP(mNameMap, token);
    SET_MAP(mNameMap, forall);
    SET_MAP(mNameMap, matrix);
    SET_MAP(mNameMap, gsave);
    SET_MAP(mNameMap, dict);
    SET_MAP(mNameMap, begin);
    SET_MAP(mNameMap, put);
    SET_MAP(mNameMap, get);
    SET_MAP(mNameMap, putinterval);
    SET_MAP(mNameMap, getinterval);
    SET_MAP(mNameMap, findfont);
    SET_MAP(mNameMap, makefont);
    SET_MAP(mNameMap, image);
    SET_MAP(mNameMap, pathbbox);
    SET_MAP(mNameMap, restore);
    SET_MAP(mNameMap, showpage);
    SET_MAP(mNameMap, setpagedevice);
    SET_MAP(mNameMap, length);    
    SET_MAP(mNameMap, ifelse);    
    SET_MAP(mNameMap, repeat);    
    SET_MAP(mNameMap, loop);    
    SET_MAP(mNameMap, exit);    
    SET_MAP(mNameMap, scalefont);    
    SET_MAP(mNameMap, concatmatrix);    
    SET_MAP(mNameMap, execstack);    
    SET_MAP(mNameMap, countexecstack);    
    SET_MAP(mNameMap, packedarray);    
    SET_MAP(mNameMap, transform);    
    SET_MAP(mNameMap, dtransform);    
    SET_MAP(mNameMap, itransform);    
    SET_MAP(mNameMap, idtransform);    
    SET_MAP(mNameMap, invertmatrix);    

    mNameMap["abs"]   = _abs;
    mNameMap["array"] = _array;
    mNameMap["floor"] = _floor;
    mNameMap["round"] = _round;
    mNameMap["exp"]   = _exp;
    mNameMap["ln"]    = _ln;
    mNameMap["log"]   = _log;
    mNameMap["rand"]  = _rand;
    mNameMap["srand"] = _srand;
    mNameMap["sin"]   = _sin;
    mNameMap["cos"]   = _cos;
    mNameMap["atan"]  = _atan;
    mNameMap["sqrt"]  = _sqrt;
    mNameMap["if"]    = _if;
    mNameMap["for"]   = _for;
}

PostscriptInterpreter::~PostscriptInterpreter()
{
}

void PostscriptInterpreter::readFile(const QString& sFileName)
{
    QFile file(sFileName);
    mLine=0;
    mProcedureMap.clear();
    mString.clear();
    mRotate = 0;
    mTranslateX = 0;
    mTranslateY = 0;
    mScaleX = 1;
    mScaleY = 1;
    mDC->clearDrawCommands();
    mDC->reset_transformation();
#ifdef FORTH
    mOperandStack.clear();
    mExecutionStack.clear();
    mErrors.clear();
    mGraphicsState.clear();
    mDefaultMatrix = parameter();
    mDictionaryStack.clear();
    mpCurrentDictionary = NULL;
#endif

    mParse = boost::bind(&PostscriptInterpreter::parseInitial, this, _1);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const qint64 linelength=1024;
        char fLine[linelength];
        int  freadlength;
        while ((freadlength = file.readLine(fLine, linelength)) != -1)
        {
            try
            {
                ++mLine;
                if (fLine[freadlength-1] == '\n')
                {
                    fLine[freadlength-1] = 0;
                }
                mParse(fLine);
            }
            catch(const std::exception& eErr)
            {
//                TRACE(Logger::info,"Error: %s %s in line %d\n", fLine, eErr.what(), mLine);
                char sz[256];
                sprintf(sz, "%s %s in line %d", fLine, eErr.what(), mLine);
#ifdef FORTH
                mErrors.push_back(sz);
#endif
            }
        }
    }

#ifdef FORTH
    if (mErrors.size())
    {
        QString fErrors;
        for (stringlist::const_iterator it=mErrors.begin(); it!=mErrors.end(); ++it)
        {
            fErrors += it->c_str();
            fErrors += "\n";
        }
//        QInputDialog fDlg;
//        fDlg.setInputMode(QInputDialog::TextInput);
//        fDlg.setTextValue(fErrors);
//        fDlg.exec();

//        QErrorMessage fDlg;
//        fDlg.showMessage(fErrors);
//        fDlg.exec();

    }
#endif
   mDC->setDeviderForRedSquare(100);
   mDC->concatenateDrawCommands();
   mDC->updateWindowMapping();
   mDC->redraw();
}

void PostscriptInterpreter::parseText(const QString &aText)
{
    mDC->MoveTo(0, 0);
    mDC->clearDrawCommands();
    mDC->reset_transformation();
    stringlist fStrings;
    splitString(aText.toStdString(), "\n", fStrings);

    try
    {
        foreach (std::string fLine, fStrings)
        {
            mParse(fLine);
        }
    }
    catch(const std::exception& eErr)
    {
//                TRACE(Logger::info,"Error: %s %s in line %d\n", fLine, eErr.what(), mLine);
        char sz[256];
        sprintf(sz, "%s in line %d", eErr.what(), mLine);
#ifdef FORTH
        mErrors.push_back(sz);
#endif
    }

    mDC->setDeviderForRedSquare(100);
    mDC->concatenateDrawCommands();
    mDC->updateWindowMapping();
    mDC->redraw();
}

void PostscriptInterpreter::parseInitial(const std::string& aLine)
{
    stringlist fStrings;
    size_t fNoOfStrings = splitString(aLine, " ", fStrings);
    if (fNoOfStrings > 0)
    {
        if (   fStrings[0] == "%%BoundingBox:"
            || fStrings[0] == "%%HiResBoundingBox:"
            || fStrings[0] == "%%PageBoundingBox:")
        {
            mBoundingBox.setLeft(round(atof(fStrings[1].c_str()) * toTenthOf_mm));
            mBoundingBox.setTop(round(atof(fStrings[2].c_str()) * toTenthOf_mm));
            mBoundingBox.setRight(round(atof(fStrings[3].c_str()) * toTenthOf_mm));
            mBoundingBox.setBottom(round(atof(fStrings[4].c_str()) * toTenthOf_mm));
            TRACE(Logger::info,"%s: (l:%d, t:%d, r:%d, b:%d)\n", fStrings[0].c_str(), mBoundingBox.left(), mBoundingBox.top(), mBoundingBox.right(), mBoundingBox.bottom());
        }
        else if (fStrings[0] == "%%BeginProlog")
        {
            TRACE(Logger::info,"BeginProlog\n");
            mParse = boost::bind(&PostscriptInterpreter::parseProlog, this, _1);
        }
    }
}

#ifndef FORTH

void PostscriptInterpreter::parseProlog(const std::string& aLine)
{
    stringlist fStrings;
    size_t fNoOfStrings = splitString(aLine, " ", fStrings);
    if (fNoOfStrings > 0)
    {
        if (fStrings[0] == "%%EndPageSetup")
        {
            mParse = boost::bind(&PostscriptInterpreter::parseDraw, this, _1);
            CPoint fCenter = mBoundingBox.CenterPoint();
           mDC->setWindowExtension(mBoundingBox.Width(), mBoundingBox.Height());
            if (mRotate != 0)
            {
                // Landscape format transformation
               mDC->translate(static_cast<FLOAT>(-fCenter.x), static_cast<FLOAT>(-fCenter.y));
               mDC->rotate(static_cast<FLOAT>(mRotate*deg2rad));
               mDC->translate(static_cast<FLOAT>(fCenter.x), static_cast<FLOAT>(fCenter.x));
            }
            
           mDC->translate(mTranslateX * toTenthOf_mm, mTranslateY * toTenthOf_mm);
           mDC->scale(mScaleX * toTenthOf_mm, mScaleY * toTenthOf_mm);
        }
        else
        {
            std::map< std::string, eNames >::const_iterator it = mNameMap.begin();
            for (; it != mNameMap.end(); ++it)
            {
                if (it->second == rotate)
                {
                    stringlist::iterator si = std::find(fStrings.begin(), fStrings.end(), it->first);
                    if (si != fStrings.end())
                    {
                        --si;
                        mRotate = static_cast<float>(atof(si->c_str()));
                    }
                    continue;
                }
                if (it->second == scale)
                {
                    stringlist::iterator si = std::find(fStrings.begin(), fStrings.end(), it->first);
                    if (si != fStrings.end())
                    {
                        --si;
                        mScaleY = static_cast<float>(atof(si->c_str()));
                        --si;
                        mScaleX = static_cast<float>(atof(si->c_str()));
                    }
                    continue;
                }
                if (it->second == translate)
                {
                    stringlist::iterator si = std::find(fStrings.begin(), fStrings.end(), it->first);
                    if (si != fStrings.end())
                    {
                        --si;
                        mTranslateY = static_cast<float>(atof(si->c_str()));
                        --si;
                        mTranslateX = static_cast<float>(atof(si->c_str()));
                    }
                    continue;
                }

                if (it->second == setpagesize)
                {
                    stringlist::iterator si = std::find(fStrings.begin(), fStrings.end(), it->first);
                    if (si != fStrings.end())
                    {
                        mBoundingBox.left = 0;
                        mBoundingBox.top = 0;
                        --si;
                        --si;
                        mBoundingBox.bottom = static_cast<LONG>(round(atof(si->c_str())*toTenthOf_mm));
                        --si;
                        mBoundingBox.right  = static_cast<LONG>(round(atof(si->c_str())*toTenthOf_mm));
                    }
                    continue;
                }

                size_t fPos = 0, fStart = 0;
                while ((fPos = aLine.find(it->first, fStart)) != std::string::npos)
                {
                    size_t fEnd = aLine.rfind("{", fPos);
                    if (fEnd == std::string::npos)
                    {
                        fEnd =  aLine.rfind("/", fPos);
                    }
                    if (fEnd != std::string::npos)
                    {
                        size_t fStart = aLine.rfind("/", fEnd-1);
                        if (fStart != std::string::npos)
                        {
                            ++fStart;
                            fEnd = fEnd - fStart;
                            if (fEnd)
                            {
                                mProcedureMap[aLine.substr(fStart, fEnd)] = parameter_vector_ptr(new parameter_vector(1, parameter(it->second)));
                            }
                        }
                    }
                    fStart = fPos + it->first.size();
                }
            }

        }
        // further prolog things?
    }
}

void PostscriptInterpreter::parseDraw(const std::string& aLine)
{
    stringlist fStrings;
    size_t fNoOfStrings = splitString(aLine, " ", fStrings);
    procedure_map::const_iterator itFound;
    while (fNoOfStrings > 0)
    {
         itFound = mProcedureMap.find(fStrings[fNoOfStrings-1]);
         if (itFound != mProcedureMap.end())
         {
             for (UINT i=0; i<itFound->second->size(); ++i)
             {
                 TRACE(Logger::info,"%s: ", getNameOf((*itFound->second)[i].getName()));
                 switch ((*itFound->second)[i].getName())
                 {
                 case moveto:
                 if (fNoOfStrings == 3)
                 {
                     std::vector<float> fNumbers;
                     decodeNumbers(fStrings, fNoOfStrings-1, fNumbers); 
                    mDC->MoveTo(fNumbers[0], fNumbers[1]);
                 }
                 fNoOfStrings = 0;
                 break;
                 case lineto:
                 if (fNoOfStrings == 3)
                 {
                     std::vector<float> fNumbers;
                     decodeNumbers(fStrings, fNoOfStrings-1, fNumbers); 
                    mDC->LineTo(fNumbers[0], fNumbers[1]);
                 }
                 fNoOfStrings = 0;
                 break;
                 case curveto:
                 if (fNoOfStrings == 7)
                 { 
                     std::vector<float> fNumbers;
                     decodeNumbers(fStrings, fNoOfStrings-1, fNumbers); 
                    mDC->CurveTo(fNumbers);
                     fNoOfStrings = 0;
                 }
                 fNoOfStrings = 0;
                 break;
                 case rcurveto:
                 if (fNoOfStrings == 7)
                 { 
                     std::vector<float> fNumbers;
                     float fX, fY;
                    mDC->getLastPoint(fX, fY);
                     decodeNumbers(fStrings, fNoOfStrings-1, fNumbers); 
                     for (std::vector<float>::iterator it = fNumbers.begin(); it != fNumbers.end();)
                     {
                        *it += fX;
                        ++it;
                        *it += fY;
                        ++it;
                     }
                    mDC->CurveTo(fNumbers);
                 }
                 fNoOfStrings = 0;
                 break;
                 case newpath:
                     // collect drawing commands
                 fNoOfStrings = 0;
                 break;
                 case closepath:
                     // stop collecting drawing commands
                 fNoOfStrings = 0;
                 break;
                 case rectfill:
                 if (fNoOfStrings == 5)
                 {
                     // •x y w h rectfill draw a solid rectangle of width w, height h, and lower left endpoint (x,y).
                     const int x=0, y=1, w=2, h=3;
                     std::vector<float> fCoord;
                     decodeNumbers(fStrings, fNoOfStrings-1, fCoord); 
                     if (mDC.hasVariablePenWidth())
                     {
                         // FEATURE fill rect
                     }
                     else
                     {
                        mDC->MoveTo(fCoord[x], fCoord[y]);
                        mDC->LineTo(fCoord[x]+ fCoord[w], fCoord[y]);
                        mDC->LineTo(fCoord[x]+ fCoord[w], fCoord[y]+fCoord[h]);
                        mDC->LineTo(fCoord[x],            fCoord[y]+fCoord[h]);
                        mDC->LineTo(fCoord[x], fCoord[y]);
                     }
                     
                 }
                 fNoOfStrings = 0;
                 break;
                 case fill:
                 if (fNoOfStrings == 5)
                 {
                 }
                 fNoOfStrings = 0;
                 break;

                 case stroke: 
                 if (fNoOfStrings == 5)
                 {
                     std::vector<float> fNumbers;
                     decodeNumbers(fStrings, fNoOfStrings-1, fNumbers); 
                    mDC->MoveTo(fNumbers[0], fNumbers[1]);
                    mDC->LineTo(fNumbers[0]+fNumbers[2], fNumbers[1]+fNumbers[3]);
                 }
                 fNoOfStrings = 0;
                 break;
                 case setdash: 
                 {
                     //std::vector<float> fNumbers;
                     //decodeNumbers(fStrings, fNoOfStrings-1, fNumbers);
                     fNoOfStrings -= 2;
                 } break;
                 case setlinecap: case setlinejoin:
                 {
                     //std::vector<float> fNumbers;
                     //decodeNumbers(fStrings, fNoOfStrings-1, fNumbers);
                     fNoOfStrings -= 2;
                 } break;
                 case show:
                 {
                     // FEATURE extract the text from postscript format (), <>
                    mDC->TextOut(aLine.substr(0, aLine.size()-fStrings[fNoOfStrings-1].size()-1));
                     fNoOfStrings = 0;
                 } break;
                 case setlinewidth: 
                 {
                     std::vector<float> fNumbers;
                     decodeNumbers(fStrings, fNoOfStrings-1, fNumbers);
                     if (fNumbers.size())
                     {
                        FLOAT fWidth =  fNumbers[fNumbers.size()-1] * toTenthOf_mm * mScaleX;
                        TRACE(Logger::info,"width: %f mm\n", fWidth / 10);
                       mDC->setDrawingWidth(fWidth);
                     }
                     fNoOfStrings -= 2;
                 }   break;
                 case arc: 
                 default:
                     TRACE(Logger::info,"Unhandled command: %s", itFound->first.c_str());
                     fNoOfStrings = 0;
                     break;
                 }
             }
         }
         else
         {
             --fNoOfStrings;
         }
    }
}

#else

void PostscriptInterpreter::buildParameterArray(std::string& fOperatorString, parameter_vector& fOperators, eNames aType)
{
    parseOperators(fOperatorString, fOperators);
    fOperatorString.clear();
    eNames fBrace = unknown;

    switch (aType)
    {
    case literal_array: fBrace = square_brace_open; break;
    case procedure:     fBrace = curly_brace_open;  break;
    default: break;
    }

    auto it = std::find_if(fOperators.rbegin(), fOperators.rend(), [fBrace]( parameter& fIt ){ return fIt.getName() == fBrace; });
    if (it != fOperators.rend())
    {
        parameter fProc(aType);
        fProc.getParameter().assign(it.base(), fOperators.end());
        fOperators.erase(it.base()-1, fOperators.end());
        fOperators.push_back(fProc);
    }
}

void PostscriptInterpreter::parseProlog(const std::string& aLine)
{
    std::string fProlog;
    if (aLine.size() > 2 && aLine.at(0) == '%' && aLine.at(1) != '%' )
    {
        // a comment
    }
    else
    {
        if (aLine == "%%EndProlog")
        {
            TRACE(Logger::info,"EndProlog\n");
            parseString(mString);
            mString.clear();
        }
        else if (aLine == "%%BeginPageSetup")
        {
            mParse = boost::bind(&PostscriptInterpreter::parsePageSetup, this, _1);
            TRACE(Logger::info,"BeginPageSetup\n");
        }
        else
        {
            mString += " ";
            mString += aLine;
        }
    }
}

void PostscriptInterpreter::parseString(const std::string& aString)
{
    std::string fOperatorString;
    int  fcbo=0;
    int  fsbo=0;
    size_t fBraceOpen, fBraceClose;
    parameter_vector fOperators;
    std::string::const_iterator it = aString.begin();
    for ( ;it != aString.end(); ++it)
    {
        bool bExecute = false;
        eNames fInsert = unknown;
        switch (*it)
        {
        case '{': 
            ++fcbo;
            fInsert = curly_brace_open;
            break;
        case '}':
        {
            --fcbo;
            buildParameterArray(fOperatorString, fOperators, procedure);
            if (fcbo == 0) bExecute = true;
        }    
        break;
        case '[': 
            ++fsbo;
            fInsert = square_brace_open;
            break;
        case ']':
        {
            --fsbo;
            buildParameterArray(fOperatorString, fOperators, literal_array);
            if (fsbo == 0 && fcbo == 0) bExecute = true;
        }
        break;
        case '<':fOperators.push_back(parameter(angle_brace_open)); break;
        case '>': // FEATURE Insert this content
            break;
        case '(':
            if (findCorrespondingBraces(&(*it), fBraceOpen, fBraceClose, "(", ")"))
            {
                const std::string& sText = &(*it);
                fOperators.push_back(parameter(sText.substr(fBraceOpen+1, fBraceClose - fBraceOpen-1), literal_string));
                it += (fBraceClose-fBraceOpen);
            }
            break;
        case ')':
            break;
        case '/':
        case ' ':
            if (fOperatorString.size())
            {
                parseOperators(fOperatorString, fOperators);
                fOperatorString.clear();
            }
            if (fcbo == 0 && fsbo == 0) bExecute = true;
            break;
        default: fOperatorString.push_back(*it); break;
        }
        if (fInsert != unknown)
        {
            parseOperators(fOperatorString, fOperators);
            fOperatorString.clear(); 
            fOperators.push_back(parameter(fInsert));  
        }
        if (bExecute)
        {
            try
            {
                execute(fOperators);
            }
            catch(const std::exception& eErr)
            {
                char sz[256];
                sprintf(sz, "%s in line %d", eErr.what(), mLine);
                mErrors.push_back(sz);
            }
            fOperators.clear();
        }
    }
    if (fOperatorString.size())
    {
        parseOperators(fOperatorString, fOperators);
        fOperatorString.clear(); 
        try
        {
            execute(fOperators);
        }
        catch(const std::exception& eErr)
        {
            char sz[256];
            sprintf(sz, "%s in line %d", eErr.what(), mLine);
            mErrors.push_back(sz);
        }
    }
}

bool PostscriptInterpreter::findCorrespondingBraces(const std::string& aString, size_t &aBraceOpen, size_t &aBraceClose, const char *aBO, const char *aBC)
{
    aBraceOpen = aString.find(aBO);
    if (aBraceOpen != std::string::npos)
    {
        size_t fStart1 = aBraceOpen+1;
        size_t fStart2 = fStart1;
        int fOpened = 1;
        do
        {
            aBraceClose = aString.find(aBC, fStart2);
            if (aBraceClose != std::string::npos) --fOpened;
            size_t fNextBraceOpen = aString.find(aBO, fStart1);
            if (aBO[0] != ' ' && fNextBraceOpen != std::string::npos && fNextBraceOpen < aBraceClose)
            {
                ++fOpened;
                fStart1 = fNextBraceOpen +1;
                fStart2 = aBraceClose + 1;
            }
        } while (fOpened && fStart1 < aString.size() && fStart2 < aString.size());
    }
    return (aBraceOpen != std::string::npos && aBraceClose != std::string::npos && aBraceOpen < aBraceClose);
}

void PostscriptInterpreter::parseOperators(const std::string& aObjects, parameter_vector& aProcedure)
{
    stringlist  fStrings;
    size_t      fNoOfStrings = 0;
    parameter   fParam;
    fNoOfStrings = splitString(aObjects, " ", fStrings);
    //TRACE(Logger::info," ");
    for (UINT i=0; i<fNoOfStrings; ++i)
    {
        if (fParam.getName() != unknown && fStrings[i].size() == 0)
        {
            aProcedure.push_back(fParam);
            continue;
        }
        name_map::iterator fItName = mNameMap.find(fStrings[i]);
        if (fItName != mNameMap.end())
        {
            //TRACE(Logger::info,"!");
            aProcedure.push_back(parameter(fItName->second));
        }
        else
        {
            aProcedure.push_back(parameter(fStrings[i]));
            //if (aProcedure.rbegin()->getName() == literal_string)
            //{
            //    TRACE_NYI(Logger::info,"Unknown ID: %s\n", fStrings[i].c_str());
            //}
        }
    }
}

void PostscriptInterpreter::parsePageSetup(const std::string& aLine)
{
    if (aLine == "%%EndPageSetup")
    {
        TRACE(Logger::info,"EndPageSetup\n");
        QPoint fCenter = mBoundingBox.center();
        mDC->setWindowExtension(mBoundingBox.width(), mBoundingBox.height());
        if (mRotate != 0)
        {
            // Landscape format transformation
            mDC->translate(static_cast<FLOAT>(-fCenter.x()), static_cast<FLOAT>(-fCenter.y()));
            mDC->rotate(static_cast<FLOAT>(mRotate*deg2rad));
            mDC->translate(static_cast<FLOAT>(fCenter.x()), static_cast<FLOAT>(fCenter.x()));
        }
        
        mDC->translate(mTranslateX * toTenthOf_mm, mTranslateY * toTenthOf_mm);
        mDC->scale(mScaleX * toTenthOf_mm, mScaleY * toTenthOf_mm);
        mDefaultMatrix.setMatrix(mDC->getTransformation());
        mParse = boost::bind(&PostscriptInterpreter::parseDraw, this, _1);
    }
    else
    {
        parseDraw(aLine);
    }
}

const PostscriptInterpreter::parameter& PostscriptInterpreter::findInProcedureMap(const parameter& aParam)
{
    if (aParam.getName() == literal_string)
    {
        procedure_map::iterator it = mProcedureMap.find(aParam.getLiteral());
        if (it != mProcedureMap.end())
        {
            return (*it->second)[0];
        }
    }
    return aParam;
}

void PostscriptInterpreter::execute(parameter_vector& anObject)
{
    size_t s;
    for (size_t iObj=0; iObj<anObject.size(); iObj++)
    {
        s = mOperandStack.size();
        //TRACE(Logger::info,"%s\n", getNameOf(anObject[iObj]));
        switch (anObject[iObj].getName())
        {
        case literal_string:
        {
            if (mpCurrentDictionary)
            {

            }
            procedure_map::iterator it = mProcedureMap.find(anObject[iObj].getLiteral());
            if (it != mProcedureMap.end())
            {
                switch ((*it->second)[0].getName())
                {
                case procedure:
                    anObject.insert(anObject.begin()+iObj+1, (*it->second)[0].getParameter().begin(), (*it->second)[0].getParameter().end());
                    break;
                case literal_string: case number: case integer: case boolean: case literal_array:
                    mOperandStack.push_back(anObject[iObj]);
                    break;
                default:
                    anObject.insert(anObject.end(), (*it->second).begin(), (*it->second).end());
                    break;
                }
            }
            else
            {
                mOperandStack.push_back(anObject[iObj]);
            }
        }   break;
        case number: case integer: case boolean:
        case procedure: case literal_array:
            mOperandStack.push_back(anObject[iObj]);
            break;
        case _array:
        case packedarray:   create_array(anObject[iObj], s); break;
        case length:        get_length(anObject[iObj], s); break;
        case put:           do_put(anObject[iObj], s); break;    // TEST put:
        case get:           do_get(anObject[iObj], s); break;    // TEST get:
        case getinterval:   do_getinterval(anObject[iObj], s); break;
        case putinterval:   do_putinterval(anObject[iObj], s); break;
        case anchorsearch:  do_anchorsearch(anObject[iObj], s); break;
        case search:        do_search(anObject[iObj], s); break;
        case aload:         do_aload(anObject[iObj], s); break;
        case astore:        do_astore(anObject[iObj], s); break;
        case gsave: 
            mGraphicsState.push_back(mDC->SaveDC());
            break;
        case grestore:
            if (mGraphicsState.size())
            {
                mDC->RestoreDC(*mGraphicsState.rbegin());
                mGraphicsState.pop_back();
            }
            break;
        case token: // FEATURE token ?
            TRACE_NYI(Logger::info,"token nyi\n");
            break;
        case save: // FEATURE save
            TRACE_NYI(Logger::info,"save nyi\n");
            break;
        case restore:  // FEATURE restore
            TRACE_NYI(Logger::info,"restore nyi\n");
            break;
        case dict:  // FEATURE_A creates an empty dictionary with an initial capacity
            TRACE_NYI(Logger::info,"dict nyi\n");
            break;
        case begin:  // FEATURE_A pushes dict on the dictionary stack, making it the current dictionary and installing
                     // it as the first of the dictionaries consulted during implicit name lookup
            TRACE_NYI(Logger::info,"begin nyi\n");
            do_pop(mOperandStack);
            break;
        case findfont:    // FEATURE key findfont -> font
            TRACE_NYI(Logger::info,"findfont nyi\n");
            break;
        case makefont:    // FEATURE font matrix makefont -> font'
            TRACE_NYI(Logger::info,"makefont nyi\n");
            do_pop(mOperandStack);
            break;
        case image:       // FEATURE width height bits/sample matrix datasrc image -> –
            TRACE_NYI(Logger::info,"image nyi\n");
            do_pop(mOperandStack, 5);
            break;
        case pathbbox:   // FEATURE bounding box of current path in mDC – pathbbox -> left bottom right top
            TRACE_NYI(Logger::info,"pathbbox nyi\n");
            break;
        case showpage:   // FEATURE transmits the contents of the current page to the current output device
            TRACE_NYI(Logger::info,"showpage nyi\n");
            break;  // start plott ?
        case setpagedevice: // FEATURE modifies the contents of the page device dictionary in the graphics state based on the contents of the dictionary operand.
            TRACE_NYI(Logger::info,"setpagedevice nyi\n");
            do_pop(mOperandStack);
            break;
        case def:
        case store: do_store(anObject[iObj], s);break;
        case bind:  do_bind(anObject[iObj], s); break;
        case type:
            if (s >= 1)
            {
                mOperandStack[s-1] = parameter(std::string(mOperandStack[s-1].getType()));
            }
            else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            break;
        case moveto:
        case rmoveto:   do_move_to(anObject[iObj], s); break;
        case lineto:
        case rlineto:   draw_line_to(anObject[iObj], s); break;
        case curveto:
        case rcurveto:  draw_curve_to(anObject[iObj], s); break;
        case rotate:    do_rotate(anObject[iObj], s); break;
        case scale:     do_scale(anObject[iObj], s); break;
        case translate: do_translate(anObject[iObj], s); break;
        case concat:
            // TEST matrix concat – ; Replace CTM by matrix x CTM
            //applies the transformation represented by matrix to the user coordinate space.
            //concat accomplishes this by concatenating matrix with the current transformation
            //matrix (CTM); that is, it replaces the CTM with the matrix product matrix * CTM
            //(see Section 4.3, “Coordinate Systems and Transformations”).
            TRACE_NYI(Logger::info,"concat test\n");
            if (s >= 1)     // matrix currentmatrix matrix
            {
                parameter& fMatrix = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-1]));
                if (fMatrix.isMatrix())
                {
                    mDC->setTransformation(mDC->getTransformation() * fMatrix.getMatrix());
                    do_pop(mOperandStack);
                }
                else throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            }
            else  throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            break;
        case currentmatrix: // TEST replaces the value of matrix with the current transformation matrix
            if (s >= 1)     // matrix currentmatrix matrix
            {
                parameter& fMatrix = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-1]));
                if (fMatrix.isMatrix()) fMatrix.setMatrix(mDC->getTransformation());
                else  throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            }
            else  throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            break;
        case setmatrix:     // TEST Replace CTM by matrix 
            if (s >= 1)     // matrix setmatrix - 
            {
                const parameter& fMatrix = findInProcedureMap(mOperandStack[s-1]);
                if (fMatrix.isMatrix())mDC->setTransformation(fMatrix.getMatrix());
                else  throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            }
            else  throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            break;
        case defaultmatrix: fill_defaultmatrix(anObject[iObj], s); break;
        case initmatrix: // TEST – initmatrix – ; Set CTM to device default
            TRACE_NYI(Logger::info,"initmatrix test\n");
           mDC->setTransformation(mDefaultMatrix.getMatrix());
            break;
        case concatmatrix: do_concatmatrix(anObject[iObj], s); break;
        case identmatrix: fill_identmatrix(anObject[iObj], s); break;
        case matrix: // TEST – matrix matrix ; Create identity matrix
        {
            rke::matrix<FLOAT> fMatrix;
            fMatrix.identity(3);
            mOperandStack.push_back(parameter(fMatrix));
        } break;
        case transform: do_transform(anObject[iObj], s); break;
        case dtransform:
        if (s >= 2)
        {
            TRACE_NYI(Logger::info,"dtransform nyi\n");
        //FEATURE! dx dy dtransform dx' dy' ; Transform distance (dx, dy) by CTM
        //FEATURE! dx dy matrix dtransform dx' dy' ; Transform distance (dx, dy) by matrix
        } break;
        case itransform:
        if (s >= 2)
        {
            TRACE_NYI(Logger::info,"itransform nyi\n");
        //FEATURE! x' y' itransform x y ; Perform inverse transform of (x', y') by CTM
        //FEATURE! x' y' matrix itransform x y ; Perform inverse transform of (x', y') by matrix
        } break;
        case idtransform:
        if (s >= 2)
        {
            TRACE_NYI(Logger::info,"idtransform nyi\n");
        //FEATURE! dx' dy' idtransform dx dy ; Perform inverse transform of distance (dx', dy') by CTM
        //FEATURE! dx' dy' matrix idtransform dx dy ; Perform inverse transform of distance (dx', dy') by matrix
        } break;
        case invertmatrix:
        if (s >= 2)
        {
            TRACE_NYI(Logger::info,"invertmatrix nyi\n");
        //FEATURE! matrix1 matrix2 invertmatrix matrix2 ; Fill matrix2 with inverse of matrix1
        }  break;
        case setpagesize:
            if (s >= 4)
            {
                mBoundingBox.setLeft(round(mOperandStack[s-4].getNumber()));
                mBoundingBox.setTop(round(mOperandStack[s-3].getNumber()));
                mBoundingBox.setRight(round(mOperandStack[s-2].getNumber()));
                mBoundingBox.setBottom(round(mOperandStack[s-1].getNumber()));
                do_pop(mOperandStack, 4);
            }
            else  throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            break;
        case newpath:         // FEATURE collect drawing commands in mDC
        if (mUsePath)
        {
           mDC->BeginPath();
            TRACE(Logger::info,"mDC.BeginPath()\n");
        } break;
        case closepath:       // FEATURE stop collecting drawing commands in mDC
        if (mUsePath)
        {
           mDC->EndPath();
            TRACE(Logger::info,"mDC.EndPath()\n");
        } break;
        case fill:            // FEATURE_A fill path in mDC
            TRACE_NYI(Logger::info,"fill test\n");
            // FEATURE_A! find out why begin and end path are confused in order
           mDC->StrokePath(true);
            // get path rects
        break;
        case stroke: // FEATURE stroke
            TRACE_NYI(Logger::info,"stroke test\n");
           mDC->StrokePath();
            /*
            paints a line centered on the current path, with sides parallel to the path segments.
            The line’s graphical properties are defined by various parameters of the graphics
            state. Its thickness is determined by the current line width parameter (see
            setlinewidth) and its color by the current color (see setcolor). The joints between
            connected path segments and the ends of open subpaths are painted with the current
            line join (see setlinejoin) and the current line cap (see setlinecap), respectively.
            The line is either solid or broken according to the dash pattern established by
            setdash. Uniform stroke width can be ensured by enabling automatic stroke adjustment
            (see setstrokeadjust). All of these graphics state parameters are consulted
            at the time stroke is executed; their values during the time the path is being
            constructed are irrelevant.
            If a subpath is degenerate (consists of a single-point closed path or of two or more
            points at the same coordinates), stroke paints it only if round line caps have been
            specified, producing a filled circle centered at the single point. If butt or projecting
            square line caps have been specified, stroke produces no output, because the
            orientation of the caps would be indeterminate. A subpath consisting of a singlepoint
            open path produces no output.
            After painting the current path, stroke clears it with an implicit newpath operation.
            */
        break;
        case rectfill: do_rectfill(anObject[iObj], s); break;
        case setrgbcolor:       // FEATURE_C setrgbcolor: red green blue (range 0.0 to 1.0)
            //TRACE_NYI(Logger::info,"setrgbcolor nyi\n");
            do_pop(mOperandStack, 3);
            break;
        case sethsbcolor:       // FEATURE_C sethsbcolor: hue saturation b (range 0.0 to 1.0)
            //TRACE_NYI(Logger::info,"sethsbcolor nyi\n");
            do_pop(mOperandStack, 3);
            break;
        case setcmykcolor:      // FEATURE_C setcmykcolor: cyan magenta yellow black (range 0.0 to 1.0)
            //TRACE_NYI(Logger::info,"setcmykcolor nyi\n");
            do_pop(mOperandStack, 4);
            break;
        case setdash:           // FEATURE_C setdash: parameter: array, offset
            TRACE_NYI(Logger::info,"setdash nyi\n");
            do_pop(mOperandStack, 2);
        break;
        case setlinejoin:
        if (s >= 1)
        {
           mDC->SetLineJoin(findInProcedureMap(mOperandStack[s-1]).getInteger());
            do_pop(mOperandStack);
        } break;
        case setlinecap:
        if (s >= 1)
        {
           mDC->SetLineCap(findInProcedureMap(mOperandStack[s-1]).getInteger());
            do_pop(mOperandStack);
        } break;
        break;
        case setlinewidth: 
           mDC->setDrawingWidth(fabs(findInProcedureMap(mOperandStack[s-1]).getNumber() * toTenthOf_mm * mScaleX));
            do_pop(mOperandStack);
        break;
        case setfont:
        if (s >= 1)
        {
            TRACE_NYI(Logger::info,"setfont nyi, %s\n", findInProcedureMap(mOperandStack[s-1]).getText());
            do_pop(mOperandStack);
        } break;
        case scalefont:
        if (s >= 1)
        {
            TRACE_NYI(Logger::info,"scalefont nyi, %s\n", findInProcedureMap(mOperandStack[s-1]).getText());
        } break;
        case ashow: 
        if (s >= 1)
        {
            // FEATURE_A ashow: use parameters ax ay
            const parameter& ax = findInProcedureMap(mOperandStack[s-3]);
            const parameter& ay = findInProcedureMap(mOperandStack[s-2]);
            const parameter& fstring = findInProcedureMap(mOperandStack[s-1]);
            if (fstring.getName() == literal_string)
            {
                TRACE(Logger::info,"ashow (%f, %f): %s\n", ax.getNumber(), ay.getNumber(), fstring.getLiteral().c_str());
               mDC->TextOut(fstring.getLiteral());
            }
            else throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            do_pop(mOperandStack, 3);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
        break;
        case show:
        if (s >= 1)
        {
            const parameter& op1 = findInProcedureMap(mOperandStack[s-1]);
            if (op1.getName() == literal_string)
            {
                TRACE(Logger::info,"show: %s\n", op1.getLiteral().c_str());
               mDC->TextOut(op1.getLiteral());
            }
            else throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
        break;
        /// Operand Stack Manipulation Operators
        case pop:           do_pop(mOperandStack);         break;
        case exch:          do_exch(mOperandStack);        break;
        case dup:           do_dup(mOperandStack);         break;
        case copy:          do_copy(mOperandStack);        break;
        case index:         do_index(mOperandStack);       break;
        case roll:          do_roll(mOperandStack);        break;
        case clear:         do_clear(mOperandStack);       break;
        case count:         do_count(mOperandStack);       break;
        case mark:          do_mark(mOperandStack);        break;
        case counttomark:   do_counttomark(mOperandStack); break;
        case cleartomark:   do_cleartomark(mOperandStack); break;
        /// Relational, Boolean, and Bitwise Operators
        case eq: 
        if (s >= 2)
        {
            mOperandStack[s-2] = findInProcedureMap(mOperandStack[s-2]) == findInProcedureMap(mOperandStack[s-1]);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
        break;
        case gt: 
        if (s >= 2)
        {
            mOperandStack[s-2] = findInProcedureMap(mOperandStack[s-2]) > findInProcedureMap(mOperandStack[s-1]);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
        break;
        case lt: 
        if (s >= 2)
        {
            mOperandStack[s-2] = findInProcedureMap(mOperandStack[s-2]) < findInProcedureMap(mOperandStack[s-1]);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
        break;
        case ge: 
        if (s >= 2)
        {
            mOperandStack[s-2] = findInProcedureMap(mOperandStack[s-2]) >= findInProcedureMap(mOperandStack[s-1]);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
        break;
        case le: 
        if (s >= 2)
        {
            mOperandStack[s-2] = findInProcedureMap(mOperandStack[s-2]) <= findInProcedureMap(mOperandStack[s-1]);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
        break;
        case _or:
        if (s >= 2)
        {
            mOperandStack[s-2] = findInProcedureMap(mOperandStack[s-2]) || findInProcedureMap(mOperandStack[s-1]);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
        break;
        case _and:
        if (s >= 2)
        {
            mOperandStack[s-2] = findInProcedureMap(mOperandStack[s-2]) && findInProcedureMap(mOperandStack[s-1]);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _xor:
        if (s >= 2)
        {
            mOperandStack[s-2] = findInProcedureMap(mOperandStack[s-2]) ^ findInProcedureMap(mOperandStack[s-1]);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case bitshift: 
        if (s >= 2)
        {
            mOperandStack[s-2] = findInProcedureMap(mOperandStack[s-2]) >> findInProcedureMap(mOperandStack[s-1]);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _not:
        if (s >= 1)
        {
            mOperandStack[s-1] = !findInProcedureMap(mOperandStack[s-1]);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _if:
            if (s >= 2)
            {
                const parameter& op1 = findInProcedureMap(mOperandStack[s-2]);
                const parameter& op2 = findInProcedureMap(mOperandStack[s-1]);
                if (op1.getName() == boolean && op2.getName() == procedure)
                {
                    if (op1.getBool())
                    {
                        anObject.insert(anObject.begin()+iObj+1, op2.getProcedure().begin(), op2.getProcedure().end());
                    }
                }
                else throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
                do_pop(mOperandStack, 2);
            }
            else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
            break;
        case ifelse:
            if (s >= 3)
            {
                const parameter& op1 = findInProcedureMap(mOperandStack[s-3]);
                const parameter& op2 = findInProcedureMap(mOperandStack[s-2]);
                const parameter& op3 = findInProcedureMap(mOperandStack[s-1]);
                if (op1.getName() == boolean && op2.getName() == procedure && op3.getName() == procedure)
                {
                    if (op1.getBool())
                    {
                        anObject.insert(anObject.begin()+iObj+1, op3.getProcedure().begin(), op3.getProcedure().end());
                    }
                    else
                    {
                        anObject.insert(anObject.begin()+iObj+1, op2.getProcedure().begin(), op2.getProcedure().end());
                    }
                }
                else throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
                do_pop(mOperandStack, 3);
            }
            else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            break;
        case _for:  
            if (s >= 4)
            {
                const parameter& initial   = findInProcedureMap(mOperandStack[s-4]);
                const parameter& increment = findInProcedureMap(mOperandStack[s-3]);
                const parameter& limit     = findInProcedureMap(mOperandStack[s-2]);
                const parameter& proc      = findInProcedureMap(mOperandStack[s-1]);
                if (initial.isNumber() && increment.isNumber() && limit.isNumber() && proc.getName() == procedure)
                {
                    mExecutionStack.push_back(initial);
                    mExecutionStack.push_back(increment);
                    mExecutionStack.push_back(limit);
                    mExecutionStack.push_back(anObject[iObj]);
                    parameter for_loop(static_cast<int>(iObj), lookup_execstack);
                    anObject.insert(anObject.begin()+ iObj+1, for_loop);
                    anObject.insert(anObject.begin()+ iObj+1, proc.getProcedure().begin(), proc.getProcedure().end());
                }
                else throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
                do_pop(mOperandStack, 3);
            }
            else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            break;
        case repeat: // TEST int proc repeat –
            if (s >= 2)
            {
                const parameter& count     = findInProcedureMap(mOperandStack[s-2]);
                const parameter& proc      = findInProcedureMap(mOperandStack[s-1]);
                if (count.isNumber() && proc.getName() == procedure)
                {
                    mExecutionStack.push_back(count);
                    mExecutionStack.push_back(anObject[iObj]);
                    parameter repeat_loop(static_cast<int>(iObj), lookup_execstack);
                    anObject.insert(anObject.begin()+ iObj+1, repeat_loop);
                    anObject.insert(anObject.begin()+ iObj+1, proc.getProcedure().begin(), proc.getProcedure().end());
                }
                else throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
                do_pop(mOperandStack, 2);
            }
            else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            break;
        case forall: 
            if (s >= 2)
            {
                const parameter& anarray = findInProcedureMap(mOperandStack[s-2]);
                const parameter& proc    = findInProcedureMap(mOperandStack[s-1]);
                if (proc.getName() == procedure)
                {
                    mExecutionStack.push_back(parameter(0));
                    mExecutionStack.push_back(anarray);
                    mExecutionStack.push_back(anObject[iObj]);
                    parameter for_loop(static_cast<int>(iObj), lookup_execstack);
                    anObject.insert(anObject.begin()+ iObj+1, for_loop);
                    anObject.insert(anObject.begin()+ iObj+1, proc.getProcedure().begin(), proc.getProcedure().end());
                    while (iObj < anObject.size() && anObject[iObj].getName() != lookup_execstack) ++iObj;
                    --iObj;
                }
                else throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
                do_pop(mOperandStack, 2);
            }
            else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            break;
        case loop: // TEST proc loop –  -> until exit is called
            if (s >= 1)
            {
                const parameter& proc      = findInProcedureMap(mOperandStack[s-1]);
                if (proc.getName() == procedure)
                {
                    mExecutionStack.push_back(anObject[iObj]);
                    parameter for_loop(static_cast<int>(iObj), lookup_execstack);
                    anObject.insert(anObject.begin()+ iObj+1, for_loop);
                    anObject.insert(anObject.begin()+ iObj+1, proc.getProcedure().begin(), proc.getProcedure().end());
                }
                else throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
                do_pop(mOperandStack, 2);
            }
            else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);
            do_pop(mOperandStack);
            break;
        case lookup_execstack: lookupExecstack(anObject, iObj); break; // look up exec stack
        case countexecstack:  // TEST countexecstack
            mOperandStack.push_back(parameter(static_cast<int>(mExecutionStack.size())));
            break;
        case execstack: // TEST execstack
            if (mExecutionStack.size())
            {
                parameter_vector_ptr fObjectParameter(new parameter_vector);
                fObjectParameter.get()->insert(fObjectParameter.get()->begin(), mExecutionStack.begin(), mExecutionStack.end());
                mOperandStack.push_back(parameter(fObjectParameter, literal_array));
            }
            break;
        case exit: do_exit(anObject, iObj); break; // TEST exit loops: for, repeat, loop, forall
        /// Arithmetic and Math Operators
        case add: 
        if (s >= 2)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-2]);
            const parameter op2 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber() || !op2.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-2] = op1 + op2;
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case div:
        if (s >= 2)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-2]);
            const parameter op2 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber() || !op2.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            if (op2.getNumber() == 0)  throw Error(getNameOf(anObject[iObj]), Error::undefinedresult);
            mOperandStack[s-2] = op1 / op2;
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case idiv:
        if (s >= 2)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-2]);
            const parameter op2 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber() || !op2.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            if (op2.getInteger() == 0)  throw Error(getNameOf(anObject[iObj]), Error::undefinedresult);
            mOperandStack[s-2] = parameter(op1.getInteger() / op2.getInteger());
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case mod: 
        if (s >= 2)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-2]);
            const parameter op2 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber() || !op2.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            if (op2.getNumber() == 0)  throw Error(getNameOf(anObject[iObj]), Error::undefinedresult);
            mOperandStack[s-2] = parameter(static_cast<FLOAT>(fmod(op1.getNumber(), op2.getNumber())));
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case mul: 
        if (s >= 2)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-2]);
            const parameter op2 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber() || !op2.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-2] = op1 * op2;
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case sub:  
        if (s >= 2)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-2]);
            const parameter op2 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber() || !op2.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-2] = op1 - op2;
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _abs: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-1] = parameter(static_cast<FLOAT>(fabs(op1.getNumber())));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case neg: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-1] = parameter(-(op1.getNumber()));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case ceiling: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-1] = parameter(static_cast<FLOAT>(ceil(op1.getNumber())));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _floor: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-1] = parameter(static_cast<FLOAT>(floor(op1.getNumber())));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _round: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-1] = parameter(round(op1.getNumber()));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case truncate: // test truncate
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            if (op1.getNumber() >= 0)
            {
                mOperandStack[s-1] = parameter(static_cast<FLOAT>(floor(op1.getNumber())));
            }
            else
            {
                mOperandStack[s-1] = parameter(static_cast<FLOAT>(ceil(op1.getNumber())));
            }
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _sqrt: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            if (op1.getNumber() < 0)  throw Error(getNameOf(anObject[iObj]), Error::rangecheck);
            mOperandStack[s-1] = parameter(static_cast<FLOAT>(sqrt(op1.getNumber())));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _atan: 
        if (s >= 2)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-2]);
            const parameter op2 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber() || !op2.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            if (op1.getNumber() == 0 && op2.getNumber() == 0)  throw Error(getNameOf(anObject[iObj]), Error::undefinedresult);
            mOperandStack[s-2] = parameter(static_cast<FLOAT>(180.0*atan2(op1.getNumber(), op2.getNumber())/M_PI));
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _cos: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-1] = parameter(static_cast<FLOAT>(cos(op1.getNumber())));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _sin: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-1] = parameter(static_cast<FLOAT>(sin(op1.getNumber())));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _exp: // FEATURE_A errorhandling
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            mOperandStack[s-1] = parameter(static_cast<FLOAT>(exp(op1.getNumber())));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _ln: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            if (op1.getNumber() <= 0)  throw Error(getNameOf(anObject[iObj]), Error::rangecheck);
            mOperandStack[s-1] = parameter(static_cast<FLOAT>(log(op1.getNumber())));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case _log: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            if (op1.getNumber() <= 0)  throw Error(getNameOf(anObject[iObj]), Error::rangecheck);
            mOperandStack[s-1] = parameter(static_cast<FLOAT>(log10(op1.getNumber())));
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        /// random functions
        case _rand: mOperandStack.push_back(parameter(rand())); break;
        case _srand: 
        if (s >= 1)
        {
            const parameter op1 = findInProcedureMap(mOperandStack[s-1]);
            if (!op1.isNumber()) throw Error(getNameOf(anObject[iObj]), Error::wrong_argument);
            srand(op1.getInteger());
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject[iObj]), Error::too_few_aruments);;
        break;
        case rrand:    // FEATURE rrand
            TRACE_NYI(Logger::info,"rrand nyi\n");
         break;
        case arc:      // FEATURE_C draw arc to mDC
            TRACE_NYI(Logger::info,"arc nyi\n");
            do_pop(mOperandStack, 5);
         break;
        case unknown:
            break;
        default:
            TRACE(Logger::info,"unknown: %s\n", getNameOf(anObject[iObj]));
         break;
        }
#if _EXT_TRACE
        s = mOperandStack.size();
        if (s > 0)
        {
            TRACE(Logger::info,"stack (%d): %s", s, getNameOf(mOperandStack[s-1]));
            if (s > 1)
            {
                TRACE(Logger::info,", %s", getNameOf(mOperandStack[s-2]));
                if (s > 2)
                {
                    TRACE(Logger::info,", %s", getNameOf(mOperandStack[s-3]));
                    if (s > 3)
                    {
                        TRACE(Logger::info,", %s", getNameOf(mOperandStack[s-4]));
                    }
                }
            }
            TRACE(Logger::info,"\n");
        }
#endif
    }
}

void PostscriptInterpreter::lookupExecstack(parameter_vector &anObject, size_t &aObj)
{
    if (mExecutionStack.size())
    {
        size_t se = mExecutionStack.size();
        const parameter& proc = mExecutionStack[se-1];
        switch (proc.getName())
        {
        case _for:
            if (se >= 4)
            {
                parameter&       initial   = mExecutionStack[se-4];
                const parameter& increment = mExecutionStack[se-3];
                const parameter& limit     = mExecutionStack[se-2];
                initial = initial + increment;
                bool belowLimit = false;
                if (increment.getNumber() > 0)
                {
                    belowLimit = initial.getNumber() <= limit.getNumber();
                }
                else
                {
                    belowLimit = initial.getNumber() >= limit.getNumber();
                }
                if (belowLimit)
                {
                    mOperandStack.push_back(initial);
                    aObj = anObject[aObj].getInteger();
                }
                else
                {
                    do_pop(mExecutionStack, 4);
                }
            }
            else throw Error("for stack", Error::too_few_aruments);
            break;
        case repeat: 
            if (se >= 2)
            {
                parameter& count = mExecutionStack[se-2];
                if (count.getInteger() > 1)
                {
                    aObj = anObject[aObj].getInteger();
                }
                else
                {
                    do_pop(mExecutionStack, 2);
                }
                count = count - parameter(1);
            }
            else throw Error("repeat stack", Error::too_few_aruments);
            break;
        case loop:// TEST lookup_execstack loop
            if (se >= 1)
            {
                aObj = anObject[aObj].getInteger();
            }
            else throw Error("loop stack", Error::too_few_aruments);
            break;
        case forall: // TEST lookup_execstack forall
            if (se >= 3)
            {
                parameter&       count   = mExecutionStack[se-3];
                const parameter& anarray = mExecutionStack[se-2];
                int fsize = static_cast<int>(anarray.getName() == literal_string ? anarray.getLiteral().size() : anarray.getProcedure().size());
                if (count.getInteger() < fsize)
                {
                    if (anarray.getName() == literal_array)
                    {
                        mOperandStack.push_back(anarray.getProcedure()[count.getInteger()]);
                    }
                    else
                    {
                        mOperandStack.push_back(parameter(static_cast<int>(anarray.getLiteral()[count.getInteger()])));
                    }
                    aObj = anObject[aObj].getInteger();
                }
                else
                {
                    do_pop(mExecutionStack, 3);
                }
                count = count + parameter(1);
            }
            else throw Error("forall stack", Error::too_few_aruments);
            break;
        default: break;
        }
    }
}

void PostscriptInterpreter::do_exit(parameter_vector&anObject, size_t& aObj)
{
    if (mExecutionStack.size())
    {
        size_t se = mExecutionStack.size();
        while (aObj < anObject.size() && anObject[aObj].getName() != lookup_execstack) ++aObj;
        const parameter& proc = mExecutionStack[se-1];
        switch (proc.getName())
        {
        case _for:   do_pop(mExecutionStack, 4); break;
        case repeat: do_pop(mExecutionStack, 2); break;
        case loop:   do_pop(mExecutionStack);    break;
        case forall: do_pop(mExecutionStack, 3); break;
        default: break;
        }
    }
    else throw Error("exit", Error::too_few_aruments);
}
/// duplicates an object.
void PostscriptInterpreter::do_dup(parameter_vector&aVec)
{
    if (!aVec.empty()) aVec.push_back(*aVec.rbegin());
}

/// exchanges the top two elements of the stack.
void PostscriptInterpreter::do_exch(parameter_vector&aVec)
{
    size_t s = aVec.size();
    if (s > 1) std::swap(aVec[s-1], aVec[s-2]);
    else throw Error("exch", Error::too_few_aruments);
}

/// removes the top element from the stack.
void PostscriptInterpreter::do_pop(parameter_vector&aVec, size_t aNum)
{
    while(aNum-- && !aVec.empty()) aVec.pop_back();
}

/// duplicates portions of the operand stack
void PostscriptInterpreter::do_copy(parameter_vector&aVec)
{
    // TEST do_copy
    size_t s = aVec.size();
    if (s > 1)
    {
        if (aVec[s-1].isNumber())
        {
            size_t elements = static_cast<size_t>(aVec[s-1].getNumber());
            do_pop(aVec); --s;
            if (elements <= s)
            {
                aVec.insert(aVec.end(), aVec.end()-s, aVec.end());
            }
            else throw Error("copy", Error::too_few_aruments);;
        }
        else if (aVec[s-1].getName() == literal_array && aVec[s-2].getName() == literal_array)
        {
            // TEST copy for array 
            aVec[s-2].getParameter().insert(aVec[s-2].getParameter().begin(), aVec[s-1].getParameter().begin(), aVec[s-1].getParameter().end());
            do_pop(aVec);
        }
        else if (aVec[s-1].getName() == literal_string && aVec[s-2].getName() == literal_string)
        {
            // TEST copy for string 
            aVec[s-2].getString() = aVec[s-2].getLiteral() + aVec[s-1].getLiteral();
            do_pop(aVec);
        }
        else throw Error("copy", Error::wrong_argument);
    }
    else throw Error("copy", Error::too_few_aruments);;
}
/// treats a portion of the stack as a circular queue.
void PostscriptInterpreter::do_roll(parameter_vector&aVec)
{
    int s = static_cast<int>(aVec.size())-2;
    if (s > 0)
    {
        if (aVec[s].isNumber() && aVec[s+1].isNumber())
        {
            int j = static_cast<int>(aVec[s+1].getNumber());
            int n = static_cast<int>(aVec[s  ].getNumber());
            do_pop(aVec, 2);
            if (abs(n) <= s)
            {
                if (j > 0)
                {
                    aVec.insert(aVec.end()-s, *(aVec.end()-1));
                    do_pop(aVec);
                }
                else if (j < 0)
                {
                    aVec.push_back(*(aVec.end()-s));
                    aVec.erase(aVec.end()-s-1);
                }
                else throw Error("roll", Error::wrong_argument);
            }
            else throw Error("roll", Error::too_few_aruments);;
        }
        else throw Error("roll", Error::wrong_argument);
    }
    else throw Error("roll", Error::too_few_aruments);;
}

/// accesses the stack as if it were an indexable array.
void PostscriptInterpreter::do_index(parameter_vector&aVec)
{
    int s = static_cast<int>(aVec.size())-1;
    if (s > 0)
    {
        if (aVec[s].isNumber())
        {
            int element = aVec[s].getInteger();
            do_pop(aVec);
            if (element < 0) throw Error("index", Error::wrong_argument);
            if (element < s)
            {
                aVec.push_back(*(aVec.end()-element-1));
            }
            else throw Error("index", Error::too_few_aruments);;
        }
        else throw Error("index", Error::wrong_argument);
    }
    else throw Error("index", Error::too_few_aruments);;
}

/// clears the stack.
void PostscriptInterpreter::do_clear(parameter_vector&aVec)
{
    aVec.clear();
}

/// counts the number of elements on the stack.
void PostscriptInterpreter::do_count(parameter_vector&aVec)
{
    aVec.push_back(parameter(static_cast<FLOAT>(aVec.size())));
}

// marks a position on the stack.
void PostscriptInterpreter::do_mark(parameter_vector&aVec)
{
    aVec.push_back(parameter(square_brace_open));
}

// counts the elements above the highest mark. This is used primarily for array construction (described later), but has other applications as well.
void PostscriptInterpreter::do_counttomark(parameter_vector&aVec)
{
    bool bFound = false;
    parameter_vector::reverse_iterator it = aVec.rbegin();
    int count = 0;
    for (; it != aVec.rend(); ++it)
    {
        if (it->getName() == square_brace_open)
        {
            bFound = true;
            break;
        }
        ++count;
    }
    aVec.push_back(parameter(count));
    if (!bFound) throw Error("cleartomark", Error::unmatchedmark);
}

// removes all elements above the highest mark and then removes the mark itself.
void PostscriptInterpreter::do_cleartomark(parameter_vector&aVec)
{
    bool bFound = false;
    while (aVec.rbegin() != aVec.rend())
    {
        if (aVec.rbegin()->getName() == square_brace_open)
        {
            bFound = true;
            aVec.pop_back();
            break;
        }
        aVec.pop_back();
    };
    if (!bFound) throw Error("cleartomark", Error::unmatchedmark);
}

void PostscriptInterpreter::parseDraw(const std::string& aLine)
{
    parseString(aLine);
    //parameter_vector fOperators;
    //parseOperators(aLine, fOperators);
    //execute(fOperators);
}

void PostscriptInterpreter::do_anchorsearch(const parameter& anObject, size_t s)
{
    // TEST anchorsearch: string seek anchorsearch (true, false)
    if (s >= 2)
    {
        const parameter& op1 = findInProcedureMap(mOperandStack[s-1]);
        parameter& op2 = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
        if (op1.getName() == literal_string && op2.getName() == literal_string)
        {
            op2 = parameter(op2.getLiteral().compare(0, op1.getLiteral().size(), op1.getLiteral()) == 0);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::do_search(const parameter& anObject, size_t s)
{
    // TEST search: string seek seach (true, false)
    if (s >= 2)
    {
        const parameter& op1 = findInProcedureMap(mOperandStack[s-1]);
        parameter& op2 = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
        if (op1.getName() == literal_string && op2.getName() == literal_string)
        {
            op2 = parameter(op2.getLiteral().find(op1.getLiteral()) != std::string::npos);
            do_pop(mOperandStack);
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::get_length(const parameter& anObject, size_t s)
{
    if (s >= 1)
    {
        int fLenght = 0;
        const parameter &op1 = findInProcedureMap(mOperandStack[s-1]);
        switch (op1.getName())
        {
        case literal_array:  fLenght = static_cast<int>(op1.getParameter().size()); break;
        case procedure:      fLenght = static_cast<int>(op1.getProcedure().size()); break;
        case literal_string: fLenght = static_cast<int>(op1.getLiteral().size());   break;
        default: fLenght=0; break;
        // FEATURE length of a dict
        }
        do_pop(mOperandStack);
        mOperandStack.push_back(parameter(fLenght));
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::create_array(const parameter& anObject, size_t s)
{   // TEST packedarray
    if (s >= 1)
    {
        const parameter &op1 = findInProcedureMap(mOperandStack[s-1]);
        if (op1.isNumber())
        {
            size_t nObjects = op1.getInteger();
            boost::shared_ptr< std::vector<parameter> > fObjectParameter( new parameter_vector );
            do_pop(mOperandStack);
            --s;
            if (s >= nObjects)
            {
                for (size_t i=0; i<nObjects; ++i)
                {
                    fObjectParameter.get()->push_back(mOperandStack[s-i-1]);
                }
            }
            else throw Error(getNameOf(anObject), Error::too_few_aruments);
            mOperandStack.push_back(parameter(fObjectParameter, literal_array));
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::do_astore(const parameter& anObject, size_t s)
{   // TEST astore
    if (s > 1)
    {
        const parameter &op1 = findInProcedureMap(mOperandStack[s-1]);
        if (op1.getName() == literal_array)
        {
            parameter_vector& anArray = op1.getParameter();
            const UINT nparam = static_cast<UINT>(anArray.size());
            if (s > nparam)
            {
                for (UINT sp=static_cast<int>(s)-1-nparam, ap=0; ap<nparam; ++ap, ++sp)
                {
                    anArray[ap] = mOperandStack[sp];
                }
                mOperandStack.erase(mOperandStack.end()-nparam-1, mOperandStack.end()-1);
            }
        }
        else Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);;
}

void PostscriptInterpreter::do_aload(const parameter& anObject, size_t s)
{   // TEST aload
    if (s > 1)
    {
        const parameter &op1 = findInProcedureMap(mOperandStack[s-1]);
        if (op1.getName() == literal_array)
        {
            const parameter_vector& anArray = op1.getProcedure();
            const UINT nparam = static_cast<UINT>(anArray.size());
            for (UINT ap=0; ap<nparam; ++ap)
            {
                mOperandStack.insert(mOperandStack.end()-1, anArray[ap]);
            }
        }
        else Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);;
}

void PostscriptInterpreter::do_get(const parameter& anObject, size_t s)
{
    if (s >= 2)
    {
        const parameter& op1 = findInProcedureMap(mOperandStack[s-1]);
        if (op1.isNumber())
        {
            int index = op1.getInteger();
            parameter& op2 = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
            switch(op2.getName())
            {
            case literal_array:
            if (index < static_cast<int>(op2.getParameter().size()))
            {
                op2 = op2.getParameter().at(index);
                do_pop(mOperandStack);
            }
            else throw Error(getNameOf(anObject), Error::rangecheck);
            break;
            case literal_string:
            if (index < static_cast<int>(op2.getLiteral().size()))
            {
                op2 = parameter(static_cast<int>(mOperandStack[s-2].getLiteral().at(index)));
                do_pop(mOperandStack);
            } 
            else throw Error(getNameOf(anObject), Error::rangecheck);
            break;
            default:
                throw Error(getNameOf(anObject), Error::wrong_argument);
                break;
            }
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::do_put(const parameter& anObject, size_t s)
{
    if (s >= 3)
    {
        const parameter& op1 = findInProcedureMap(mOperandStack[s-1]);
        const parameter& op2 = findInProcedureMap(mOperandStack[s-2]);
        parameter& op3 = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-3]));
        if (op2.isNumber())
        {
            int index = op2.getInteger();
            switch(op3.getName())
            {
            case literal_array:
            if (index < static_cast<int>(op3.getParameter().size()))
            {
                op3.getParameter()[index] = op1;
                do_pop(mOperandStack, 3);
            }
            else throw Error(getNameOf(anObject), Error::rangecheck);
            break;
            case literal_string:
            if (index < static_cast<int>(op3.getLiteral().size()))
            {
                op3.getString()[index] = static_cast<char>(op1.getInteger());
                do_pop(mOperandStack, 3);
            } 
            else throw Error(getNameOf(anObject), Error::rangecheck);
            break;
            default:
                throw Error(getNameOf(anObject), Error::wrong_argument);
                break;
            }
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void  PostscriptInterpreter::do_getinterval(const parameter& anObject, size_t s)
{
    // TEST  array index count getinterval -> subarray
    if (s >= 3)
    {
        if (mOperandStack[s-2].isNumber() && mOperandStack[s-1].isNumber())
        {
            int count = mOperandStack[s-1].getInteger();
            int index = mOperandStack[s-2].getInteger();
            switch(mOperandStack[s-3].getName())
            {
            case literal_array:
            if (index + count < static_cast<int>(mOperandStack[s-3].getParameter().size()))
            {
                boost::shared_ptr< std::vector<parameter> > fObjectParameter( new parameter_vector(count));
                for (int i=0; i<count; ++i)
                {
                    (*fObjectParameter)[i] = mOperandStack[s-3].getParameter()[i+index];
                }
                mOperandStack.push_back(parameter(fObjectParameter, literal_array));
                do_pop(mOperandStack, 3);
            }
            else throw Error(getNameOf(anObject), Error::rangecheck);
            break;
            case literal_string:
            if (index + count < static_cast<int>(mOperandStack[s-3].getLiteral().size()))
            {
                std::string fsubstring = mOperandStack[s-3].getLiteral().substr(index, count);
                do_pop(mOperandStack, 3);
                mOperandStack.push_back(parameter(fsubstring));
            } 
            else throw Error(getNameOf(anObject), Error::rangecheck);
            break;
            default:
                throw Error(getNameOf(anObject), Error::wrong_argument);
                break;
            }
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::do_putinterval(const parameter& anObject, size_t s)
{
    // TEST: array1 index array2 putinterval -> - replace in array1 from index with array2
    if (s >= 3)
    {
        if (mOperandStack[s-2].isNumber() && mOperandStack[s-1].getName() == mOperandStack[s-3].getName())
        {
            int index = mOperandStack[s-2].getInteger();
            switch(mOperandStack[s-3].getName())
            {
            case literal_array:
            if (index + mOperandStack[s-1].getParameter().size() < mOperandStack[s-3].getParameter().size())
            {
                for (int i=0; i<static_cast<int>(mOperandStack[s-1].getParameter().size()); ++i)
                {
                    mOperandStack[s-3].getParameter()[i+index] = mOperandStack[s-1].getParameter()[i];
                }
                do_pop(mOperandStack, 2);
            }
            else throw Error(getNameOf(anObject), Error::rangecheck);
            break;
            case literal_string:
            if (index + mOperandStack[s-1].getLiteral().size() < mOperandStack[s-3].getLiteral().size())
            {
                for (int i=0; i<static_cast<int>(mOperandStack[s-1].getLiteral().size()); ++i)
                {
                    mOperandStack[s-3].getString()[i+index] = mOperandStack[s-1].getLiteral()[i];
                }
                do_pop(mOperandStack, 2);
            } 
            else throw Error(getNameOf(anObject), Error::rangecheck);
            break;
            default:
                throw Error(getNameOf(anObject), Error::wrong_argument);
                break;
            }
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::do_store(const parameter& anObject, size_t s)
{
    // TEST store
    if (s >= 2)
    {
        if (mOperandStack[s-2].getName() == literal_string)
        {
            parameter_vector_ptr fObjectParameter(new parameter_vector);
            fObjectParameter->push_back(mOperandStack[s-1]);
            mProcedureMap[mOperandStack[s-2].getLiteral()] = fObjectParameter;
        }
        else if (mOperandStack[s-1].getName() == literal_string)
        {
            parameter_vector_ptr fObjectParameter(new parameter_vector);
            fObjectParameter->push_back(mOperandStack[s-2]);
            mProcedureMap[mOperandStack[s-1].getLiteral()] = fObjectParameter;
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
        do_pop(mOperandStack, 2);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);;
}

void  PostscriptInterpreter::do_bind(const parameter& anObject, size_t s)
{
    if (s >= 1)
    {
        if (mOperandStack[s-1].getName() == literal_string)
        {
            name_map::iterator it = mNameMap.find(mOperandStack[s-1].getLiteral());
            if (it != mNameMap.end())
            {
                mOperandStack[s-1] = parameter(it->first);
            }
            else
            {
                TRACE_NYI(Logger::info,"do_bind not found: %s\n", mOperandStack[s-1].getLiteral().c_str());
            }
        }
        else if (mOperandStack[s-1].getName() == procedure)
        {
            parameter_vector::iterator param = mOperandStack[s-1].getParameter().begin();
            for ( ; param != mOperandStack[s-1].getParameter().end(); ++param)
            {
                if (param->getName() == literal_string)
                {
                    name_map::iterator it = mNameMap.find(param->getLiteral());
                    if (it != mNameMap.end())
                    {
                        *param = parameter(it->first);
                    }
                    else
                    {
                        TRACE_NYI(Logger::info,"do_bind not found p:%s\n", param->getLiteral().c_str());
                    }
                }
            }
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::do_move_to(const parameter& anObject, size_t s)
{
    if (s >= 2)
    {
        FLOAT x(0), y(0);
        if (anObject.getName() == rmoveto)
        {
            mDC->getCurrentPoint(x, y);
        }
        mDC->MoveTo(x+mOperandStack[s-2].getNumber(), y+mOperandStack[s-1].getNumber());
        do_pop(mOperandStack, 2);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::draw_line_to(const parameter& anObject, size_t s)
{
    if (s >= 2)
    {
        FLOAT x(0), y(0);
        if (anObject.getName() == rlineto)
        {
            mDC->getCurrentPoint(x, y);
        }
        mDC->LineTo(x+mOperandStack[s-2].getNumber(), y+mOperandStack[s-1].getNumber());
        do_pop(mOperandStack, 2);
    } 
    else  throw Error(getNameOf(anObject), Error::too_few_aruments);
}


void PostscriptInterpreter::draw_curve_to(const parameter& anObject, size_t s)
{
    if (s >= 6)
    { 
        FLOAT x(0), y(0);
        if (anObject.getName() == rcurveto)
        {
            mDC->getCurrentPoint(x, y);
        }
        std::vector<float> fNumbers;
        for (int i=6; i>=1; i-=2)
        {
            fNumbers.push_back(x+findInProcedureMap(mOperandStack[s-i  ]).getNumber());
            fNumbers.push_back(y+findInProcedureMap(mOperandStack[s-i+1]).getNumber());
        }
        do_pop(mOperandStack, 6);
        mDC->CurveTo(fNumbers);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::do_scale(const parameter& anObject, size_t s)
{
    if (s >= 2)
    {
        parameter& fParam  = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-1]));
        parameter& fParam1 = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
        if (fParam.isMatrix() && s>= 3)
        {
            parameter& fScaleX = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-3]));
            parameter& fScaleY = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
            if (fScaleX.isNumber() && fScaleY.isNumber() )
            {
                // TEST! sx sy matrix scale matrix ; Define scaling by sx and sy
                rke::matrix<FLOAT> fMatrix = fParam.getMatrix();
                std::vector<FLOAT> fv(2);
                fv[0] = fScaleX.getNumber();
                fv[1] = fScaleY.getNumber();
                fMatrix.scale(fv);
                fParam.setMatrix(fMatrix);
                TRACE_NYI(Logger::info,"scale matix\n");
            }
            else throw Error(getNameOf(anObject), Error::wrong_argument);
        }
        else if (fParam.isNumber() && fParam1.isNumber())
        {
            // TEST! sx sy scale – ; Scale user space by sx and sy
            if (mDefaultMatrix.isMatrix())  
            {
               mDC->scale(fParam1.getNumber(), fParam.getNumber());
                TRACE_NYI(Logger::info,"scale user space\n");
            }
            else
            {
                mScaleX = fParam1.getNumber();
                mScaleY = fParam.getNumber();
            }
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
        do_pop(mOperandStack, 2);
    } 
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void   PostscriptInterpreter::do_rotate(const parameter& anObject, size_t s)
{
    if (s >= 1)
    {
        parameter& fParam = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-1]));
        // TEST! angle matrix rotate matrix ; Define rotation by angle degrees
        if (fParam.isMatrix() && s >= 2)
        {
            parameter& fRotate = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
            if (fRotate.isNumber())
            {
                rke::matrix<FLOAT> fMatrix = fParam.getMatrix();
                fMatrix.rotate_z(static_cast<FLOAT>(fRotate.getNumber()*deg2rad));
                fParam.setMatrix(fMatrix);
                TRACE_NYI(Logger::info,"rotate matix\n");
            }
            else throw Error(getNameOf(anObject), Error::wrong_argument);
        }
        else if (fParam.isNumber())
        {
            // TEST! angle rotate – ; Rotate user space by angle degrees
            if (mDefaultMatrix.isMatrix())
            {
                TRACE_NYI(Logger::info,"rotate user space\n");
               mDC->rotate(fParam.getNumber()*deg2rad);
            }
            else 
            {
                mRotate = fParam.getNumber();
            }
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
        do_pop(mOperandStack);
    }
    else throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::do_translate(const parameter& anObject, size_t s)
{
    if (s >= 2)
    {
        parameter& fParam  = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-1]));
        parameter& fParam1 = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
        if (fParam.isMatrix() && s>= 3)
        {
            parameter& fScaleX = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-3]));
            parameter& fScaleY = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
            if (fScaleX.isNumber() && fScaleY.isNumber() )
            {
                // TEST! sx sy matrix translate matrix ; Define translate by sx and sy
                rke::matrix<FLOAT> fMatrix = fParam.getMatrix();
                std::vector<FLOAT> fv(2);
                fv[0] = fScaleX.getNumber();
                fv[1] = fScaleY.getNumber();
                fMatrix.translate(fv);
                fParam.setMatrix(fMatrix);
                TRACE_NYI(Logger::info,"translate matix\n");
            }
            else throw Error(getNameOf(anObject), Error::wrong_argument);
        }
        else if (fParam.isNumber() && fParam1.isNumber())
        {
            // TEST! sx sy translate – ; Translate user space by sx and sy
            if (mDefaultMatrix.isMatrix())  
            {
               mDC->translate(fParam1.getNumber(), fParam.getNumber());
                TRACE_NYI(Logger::info,"translate user space\n");
            }
            else
            {
                mTranslateX = fParam1.getNumber();
                mTranslateY = fParam.getNumber();
            }
        }
        else throw Error(getNameOf(anObject), Error::wrong_argument);
        do_pop(mOperandStack, 2);
    }
}

// TEST matrix1 matrix2 matrix3 concatmatrix matrix3 ; Fill matrix3 with matrix1 * matrix2
void PostscriptInterpreter::do_concatmatrix(const parameter& anObject, size_t s)
{
    if (s >= 3)
    {
        TRACE_NYI(Logger::info,"concatmatrix test\n");
        parameter& fMatrix1 = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-3]));
        parameter& fMatrix2 = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
        parameter& fMatrix3 = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-1]));
        if (fMatrix1.isMatrix() && fMatrix2.isMatrix() && fMatrix3.isMatrix())
        {
            TRACE_NYI(Logger::info,"concatmatrix successful\n");
            fMatrix1.setMatrix(fMatrix1.getMatrix()*fMatrix2.getMatrix());
            do_pop(mOperandStack, 2);
        }
        else  throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else  throw Error(getNameOf(anObject), Error::too_few_aruments);
}

// TEST matrix defaultmatrix matrix ; Fill matrix with device default matrix
void PostscriptInterpreter::fill_defaultmatrix(const parameter& anObject, size_t s)
{
    if (s >= 1)     // matrix defaultmatrix matrix
    {
        parameter& fMatrix = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-1]));
        if (fMatrix.isMatrix() && mDefaultMatrix.isMatrix())
        {
            fMatrix.getParameter() = mDefaultMatrix.getParameter();
        }
        else  throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else  throw Error(getNameOf(anObject), Error::too_few_aruments);
}

 // TEST draw rectfill to mDC
void PostscriptInterpreter::do_rectfill(const parameter& anObject, size_t s)
{
    if (s >= 1)
    {
        TRACE_NYI(Logger::info,"rectfill nyi\n");
        const parameter& op1 = findInProcedureMap(mOperandStack[s-1]);
        float x(0),y(0), width(0), height(0); 
        if (op1.getName() == literal_array)
        {
            x      = op1.getParameter()[0].getNumber();
            y      = op1.getParameter()[1].getNumber();
            width  = op1.getParameter()[2].getNumber();
            height = op1.getParameter()[3].getNumber();
            do_pop(mOperandStack);
        }
        else if (op1.getName() == literal_string)
        {
            sscanf(op1.getLiteral().c_str(), "%f %f %f %f", &x, &y, &width, &height);
        }
        else if (s >= 4 && op1.isNumber())
        {
            const parameter& op2 = findInProcedureMap(mOperandStack[s-2]);
            const parameter& op3 = findInProcedureMap(mOperandStack[s-3]);
            const parameter& op4 = findInProcedureMap(mOperandStack[s-4]);
            if (op2.isNumber() && op3.isNumber() && op4.isNumber())
            {
                x      = op1.getNumber();
                y      = op2.getNumber();
                width  = op3.getNumber();
                height = op4.getNumber();
            }
            else throw Error(getNameOf(anObject), Error::wrong_argument);
            do_pop(mOperandStack, 4);
        }
        else  throw Error(getNameOf(anObject), Error::wrong_argument);
       mDC->RectFill(x, y, width, height);
    }
}

// TEST matrix identmatrix matrix ; Fill matrix with identity transform
void  PostscriptInterpreter::fill_identmatrix(const parameter& anObject, size_t s)
{
    if (s >= 1)
    {
        parameter& fMatrix = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-1]));
        if (fMatrix.isMatrix())
        {
            std::fill(fMatrix.getParameter().begin(), fMatrix.getParameter().end(), static_cast<FLOAT>(0.0));
            fMatrix.getParameter().at(0) = static_cast<FLOAT>(1.0);
            fMatrix.getParameter().at(3) = static_cast<FLOAT>(1.0);
        }
        else  throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else  throw Error(getNameOf(anObject), Error::too_few_aruments);
}

void PostscriptInterpreter::do_transform(const parameter& anObject, size_t s)
{
    if (s >= 2)
    {
        const parameter& fMatrix = findInProcedureMap(mOperandStack[s-1]);
        if (fMatrix.isMatrix())
        {
            //TEST! x y matrix transform x' y' ; Transform (x, y) by matrix
            parameter& fX = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-3]));
            parameter& fY = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
            fMatrix.transform(fX, fY);
        }
        else if (fMatrix.isNumber())
        {
            //TEST! x y transform x' y' ; Transform (x, y) by CTM
            parameter& fX = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-2]));
            parameter& fY = const_cast<parameter&>(findInProcedureMap(mOperandStack[s-1]));
            FLOAT x = fX;
            FLOAT y = fY;
            mDC->transform_point(x, y);
            fX = x;
            fY = y;
        }
        else  throw Error(getNameOf(anObject), Error::wrong_argument);
    }
    else  throw Error(getNameOf(anObject), Error::too_few_aruments);
}

#endif

const char* PostscriptInterpreter::getNameOf(const parameter& aParam)
{
    auto found = std::find_if(mNameMap.begin(), mNameMap.end(), [&aParam](name_map::const_reference& fIt) { return fIt.second == aParam.getName(); });
    if (found != mNameMap.end()) return found->first.c_str();
    return aParam.getText();
}

void PostscriptInterpreter::decodeNumbers(const stringlist& aStrings, size_t aCount, std::vector<float>& aNumbers)
{
    aNumbers.resize(aCount);
    for (size_t i=0; i<aCount; ++i)
    {
        aNumbers[i] = static_cast<float>(atof(aStrings[i].c_str()));
    }
}


/*


Array, Packed Array, Dictionary, and String Operators
A number of operators are polymorphic: they may be applied to operands of several
different types and their precise functions depend on the types of the operands.
Except where indicated otherwise, the operators listed below apply to any of
the following types of composite objects: arrays, packed arrays, dictionaries, and
strings.
• get takes a composite object and an index (or a key, in the case of a dictionary) and returns a single element of the object.
• put stores a single element in an array, dictionary, or string. This operator does not apply to packed array objects, because they always have read-only (or even
  more restrictive) access.
• copy copies the value of a composite object to another composite object of the same type, replacing the second object’s former value. This is different from
  merely copying the object. See Section 3.3.1, “Simple and Composite Objects” for a discussion of copying objects.
• length returns the number of elements in a composite object.
• forall accesses all of the elements of a composite object in sequence, calling a procedure for each one.
• getinterval creates a new object that shares a subinterval of an array, a packed array, or a string. This operator does not apply to dictionary objects.
• putinterval overwrites a subinterval of one array or string with the contents of another. This operator does not apply to dictionary or packed array objects, although
  it can overwrite a subinterval of an array with the contents of a packed array.

In addition to the polymorphic operators, there are operators that apply to only
one of the array, packed array, dictionary, and string types. For each type, there is
an operator (array, packedarray, dict, string) that creates a new object of that
type and a specified length. These four operators explicitly create new composite
object values, consuming virtual memory (VM) resources (see Section 3.7.1,
“Virtual Memory”). Most other operators read and write the values of composite
objects but do not create new ones. Operators that return composite results usually
require an operand that is the composite object into which the result values
are to be stored. The operators are organized this way to give programmers maximum
control over consumption of VM.
Array, packed array, and string objects have a fixed length that is specified when
the object is created. In LanguageLevel 1, dictionary objects also have this property.
In LanguageLevels 2 and 3, a dictionary’s capacity can grow beyond its initial
allocation.

The following operators apply only to arrays and (sometimes) packed arrays:
• aload and astore transfer all the elements of an array to or from the operand
stack in a single operation. aload may also be applied to a packed array.
• The array construction operators [ and ] combine to produce a new array object
whose elements are the objects appearing between the brackets. The [ operator,
which is a synonym for mark, pushes a mark object on the operand stack. Execution
of the program fragment between the [ and the ] causes zero or more objects
to be pushed on the operand stack. Finally, the ] operator counts the
number of objects above the mark on the stack, creates an array of that length,
stores the elements from the stack in the array, removes the mark from the
stack, and pushes the array on the stack.
• setpacking and currentpacking (both LanguageLevel 2) control a mode setting
that determines the type of procedure objects the scanner generates when it encounters
a sequence of tokens enclosed in { and }. If the array packing mode is
true, the scanner produces packed arrays; if the mode is false, it produces ordinary
arrays. The default value is false.
• Packed array objects always have read-only (or even more restricted) access, so
the put, putinterval, and astore operations are not allowed on them. Accessing
arbitrary elements of a packed array object can be quite slow; however, accessing
the elements sequentially, as the PostScript interpreter and the forall operator
do, is efficient.

The following operators apply only to dictionaries:
• begin and end push new dictionaries on the dictionary stack and pop them off.
• def and store associate keys with values in dictionaries on the dictionary stack; load and where search for keys there.
• countdictstack, cleardictstack, and dictstack operate on the dictionary stack.
• known queries whether a key is present in a specific dictionary.
• maxlength obtains a dictionary’s maximum capacity.
• undef (LanguageLevel 2) removes an individual key from a dictionary
• << and >> (LanguageLevel 2) construct a dictionary consisting of the bracketed objects interpreted as key-value pairs.

The following operators apply only to strings:
• search and anchorsearch perform textual string searching and matching.
• token scans the characters of a string according to the PostScript language syntax
rules, without executing the resulting objects.
There are many additional operators that use array, dictionary, or string operands
for special purposes—for instance, as transformation matrices, font dictionaries,
or text.

Relational, Boolean, and Bitwise Operators
The relational operators compare two operands and produce a boolean result indicating
whether the relation holds. Any two objects may be compared for equality
(eq and ne—equal and not equal); numbers and strings may be compared by
the inequality operators (gt, ge, lt, and le—greater than, greater than or equal to,
less than, and less than or equal to).
The boolean and bitwise operators (and, or, xor, true, false, and not) compute
logical combinations of boolean operands or bitwise combinations of integer operands.
The bitwise shift operator bitshift applies only to integers.

3.6.5 Control Operators
The control operators modify the interpreter’s usual sequential execution of objects.
Most of them take a procedure operand that they execute conditionally or repeatedly.

• if and ifelse execute a procedure conditionally depending on the value of a boolean operand. (ifelse is introduced in Section 3.5, “Execution.”)
• exec executes an arbitrary object unconditionally. for, repeat, loop, and forall execute a procedure repeatedly. Several specialized
  graphics and font operators, such as pathforall and kshow, behave similarly.
• exit transfers control out of the scope of any of these looping operators.
• countexecstack and execstack are used to read the execution stack.

A PostScript program may terminate prematurely by executing the stop operator.
This occurs most commonly as a result of an error; the default error handlers (in
errordict) all execute stop.
The stopped operator establishes an execution environment that encapsulates
the effect of a stop. That is, stopped executes a procedure given as an operand,
just the same as exec. If the interpreter executes stop during that procedure, it
terminates the procedure and resumes execution at the object immediately after
the stopped operator.

Type, Attribute, and Conversion Operators
These operators deal with the details of PostScript types, attributes, and values,
introduced in Section 3.3, “Data Types and Objects.”



Text sample in postscript:
/Times-Roman findfont
12 scalefont
setfont
newpath
100 200 moveto
(Example 3) show


PostScript
  



What is PostScript and how do I create it? PostScript is a stack-based language that is used by many printers. Printers have built-in computers that translate the PostScript language into printed output. A PostScript program is a text file (just like a C++ program). You will write a C++ program that creates a PostScript program! Use cout statements to print the PostScript program to the screen. Then use redirection to output it to a file, e.g., "htree > picture.ps". 

Are there any good references for PostScript? Yes, check out A First Guide to PostScript. For those of you who would like to have more details, the PostScript Language Reference Manual provides near complete documentation. 

How do I view and print a PostScript file? To print a PostScript file named picture.ps, use the Unix command "lpr picture.ps". The printer will realize that it is a PostScript program, not just a regular text file. Before printing, make sure that your PostScript program works properly by viewing. The Unix command "gs picture.ps" will "compile" the program and set up an X-window for you to view it. You can't do this over a telnet session. 

How do I view and print a PostScript file on a PC or Mac? You can download AFPL Ghostscript and the PostScript viewer GSview via the http://www.cs.wisc.edu/~ghost 

How do I put comments into my PostScript program? The % in PostScript is analogous to // in C++. Upon seeing a %, the rest of the line is ignored. There is one exception - the first line of a PostScript program must begin with %!. 

Could you describe PostScript commands for drawing graphics? Here are some of the most common ones. 
•%! every PostScript program starts with these two symbols 
•x y moveto moves turtle to (x,y) without drawing anything - this starts a new path. 
•u v rmoveto if current point is (x,y), change current point to (x+u,y+v) without drawing anything - this starts a new subpath. 
•x y lineto moves turtle from current point to (x,y), adding the straight line segment to the current path. 
•u v rlineto if current point is (x,y), move turtle to (x+u, y+v) and add the straight line segment to the path. 
•closepath adds a straight line segment connecting the current point to the starting point of the current path (typically the point most recently specified by moveto) thereby "closing" the current path. The following PostScript fragment builds a path in the shape of a triangle. 
256 0 moveto 512 512 lineto 0 512 lineto closepath


•stroke draws a line (of some thickness) around the current path. Note that stroke destroys the current path, so to start drawing a new path after stroke you need to use a command like moveto to establish a new current path. The following PostScript fragment draws a triangle. 
256 0 moveto 512 512 lineto 0 512 lineto closepath stroke


•fill paints the entire region enclosed by the current path (using the current color). Like stroke, fill eats up the current path. The following PostScript fragment draws a filled diamond. 
256   0 moveto
512 256 lineto
256 512 lineto
  0 256 lineto
256   0 lineto fill


•showpage ejects page from printer. 
•x y w h rectstroke draw a rectangle of width w, height h, and lower left endpoint (x,y). 
•x y w h rectfill draw a solid rectangle of width w, height h, and lower left endpoint (x,y). 
•x setlinewidth change the default line thickness used by stroke to x. 
•x setgray changes pen color to shade of gray x, where x is between 0 and 1 (0 = black, 1 = white). 
•d rotate changes the orientation of the turtle d degrees counterclockwise. Warning: whatever drawing commands (including moveto) are now with respect to the new orientation. The following PostScript fragment draws a pentagon. 
128 128 moveto
256 0 rlineto 72 rotate
256 0 rlineto 72 rotate
256 0 rlineto 72 rotate
256 0 rlineto 72 rotate
256 0 rlineto 72 rotate
stroke

The last "72 rotate" does not affect the drawing of the pentagon, but it ensures that the turtle orientation after drawing the pentagon is identical to when it started (a net change of 360 degrees). 
•a b scale scales (multiplies) all x-coordinates by a, and all y-coordinates by b. It's OK to have a or b negative. 
•x y translate change origin to (x,y) - 50 50 translate is often used to prevent anything from being drawn in the very lower left margin of the page 
•x1 y1 x2 y2 x3 y3 curveto appends a Bezier curve to the current path from the current point, say (x,y), to (x3,y3) using (x1,y1) and (x2,y2) as "Bezier cubic control points" 
•x y r ang1 ang2 arc appends an arc of a circle to the current path - the arc has radius r, is centered at (x,y), and goes counterclockwise from ang1 degrees to ang2 degrees. The following PostScript fragment draws a disc of radius 100, centered at (256, 256). 
256 256 100 0 360 arc fill


•N { . . . } repeat repeats the statements delimited by the braces N times 
•/anyname { . . . } defines anyname as a synonym for the statements delimited within the braces. The following defines a function pt so that x y r pt will subsequently draw a solid circle (in the current color) centered at (x,y) of radius r. 
/pt { 0 360 arc fill } def



What is the unit of measure for the PostScript coordinate system? By default, one unit is 1/72 of an inch. On an 8.5 x 11 page, the lower left corner is (0,0) and the upper right is (612,792). 

What are the standard colors in rgb format? Red, green, and blue, are (1,0,0), (0,1,0), and (0,0,1), respectively. Black is (0,0,0), white is (1,1,1). Cyan, magenta, and yellow are (0,1,1), (1,0,1), and (1,1,0), respectively. Here's a color palette in PostScript. 

How do I draw a single path that has several colors? You can't. Each path in PostScript must be drawn the same color. So you need to break up your path into pieces, and color each one individually. This can be done by individual moveto and lineto commands. If you are using relative coordinates (e.g., with rlineto) then you may not know (or want to compute) where to move to to begin the next path. In this case, use the command currentpoint to push the current point onto the stack and use it to start the next path. The following draws an equilateral triangle in red, green, and blue. 

0 440 moveto
1 0 0 setrgbcolor 512 0 rlineto currentpoint stroke moveto -120 rotate
0 1 0 setrgbcolor 512 0 rlineto currentpoint stroke moveto -120 rotate 
0 0 1 setrgbcolor 512 0 rlineto stroke

*/
