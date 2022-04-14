#include "StdAfx.h"
#include "PostscriptInterpreter.h"
#include "PlotterDC.h"
#define _USE_MATH_DEFINES
#include "math.h"


// Units of length[edit]
// PostScript uses the point as its unit of length.
// PostScript uses exactly 72 points to the inch. 
// 127 / 360 mm = ca. 352.77777 micro meter. 
const double gPoint = 127.0 / (360.0 * 1000); // factor to calculate mm
// 842 595 -> 29.7 cm * 21.0 cm -> DIN A4

PostscriptInterpreter::PostscriptInterpreter(CPlotterDC&aDC) : mDC(aDC), mParse(&PostscriptInterpreter::parseInitial)
{
    mNameMap["moveto"]    = moveto;
    mNameMap["lineto"]    = lineto;
    mNameMap["curveto"]   = curveto;
    mNameMap["arc"]       = arc;
    mNameMap["newpath"]   = newpath;
    mNameMap["closepath"] = closepath;
    mNameMap["stroke"]    = stroke;
    mNameMap["rotate"]    = rotate;
    mNameMap["scale"]     = scale;
}

PostscriptInterpreter::~PostscriptInterpreter()
{
}

void PostscriptInterpreter::readPostscript(LPCTSTR sFileName)
{
    CFile file;
    mText.clear();
    mShortNameMap.clear();
    mRotate = 0;
    mScaleX = 1;
    mScaleY = 1;
    mDC.clearDrawCommands();
    if (file.Open(sFileName, CFile::modeRead))
    {
        CArchive ar(&file, CArchive::load);
        CString  string;
        while (ar.ReadString(string))
        {
            mText.push_back(LPCTSTR(string));
        }
    }
    mParse = &PostscriptInterpreter::parseInitial;
    parsePostscript();
}

void PostscriptInterpreter::parsePostscript()
{
    std::vector< std::string >::const_iterator it;;
    for (it=mText.begin(); it != mText.end(); ++it)
    {
        (*this.*mParse)(*it);
    }

    // TODO! Parse postscript
    // TODO! Test postscript parser with GDI Functions
    // BeginProlog
    // BeginResource
    // EndResource
    // EndProlog

    // PageBoundingBox
    // BeginPageSetup
    // EndPageSetup
}

size_t PostscriptInterpreter::splitString(const std::string& aString,const std::string&aSeparator, stringlist& aList)
{
    size_t fPos, fStart = 0;
    while ((fPos = aString.find(aSeparator, fStart)) != -1)
    {
        aList.push_back(aString.substr(fStart, fPos-fStart));
        fStart = fPos+aSeparator.size();
    }
    aList.push_back(aString.substr(fStart));
    return aList.size();
}

void PostscriptInterpreter::decodeNumbers(const stringlist& aStrings, size_t aCount, std::vector<float>& aNumbers)
{
    aNumbers.resize(aCount);
    for (size_t i=0; i<aCount; ++i)
    {
        aNumbers[i] = static_cast<float>(atof(aStrings[i].c_str()));
    }
}

void PostscriptInterpreter::parseInitial(const std::string& aLine)
{
    stringlist fStrings;
    size_t fNoOfStrings = splitString(aLine, " ", fStrings);
    if (fNoOfStrings > 0)
    {
        if (fStrings[0] == "%%BoundingBox:")
        {
            mBoundingBox.left   = atoi(fStrings[1].c_str());
            mBoundingBox.top    = atoi(fStrings[2].c_str());
            mBoundingBox.right  = atoi(fStrings[3].c_str());
            mBoundingBox.bottom = atoi(fStrings[4].c_str());
        }
        else if (fStrings[0] == "%%BeginProlog")
        {
            mParse = &PostscriptInterpreter::parseProlog;
        }
    }
}

void PostscriptInterpreter::parseProlog(const std::string& aLine)
{
    stringlist fStrings;
    size_t fNoOfStrings = splitString(aLine, " ", fStrings);
    if (fNoOfStrings > 0)
    {
        if (fStrings[0] == "%%EndPageSetup")
        {
            CRect rcClient;
            mParse = &PostscriptInterpreter::parseDraw;
            mDC.SetMapMode(MM_ISOTROPIC);
            mDC.GetWindow()->GetClientRect(&rcClient);
            mDC.SetWindowExt(mBoundingBox.Width(), mBoundingBox.Height());
            mDC.SetViewportExt(rcClient.Width(), rcClient.Height());
            CPoint fCenter = mBoundingBox.CenterPoint();
            if (mRotate != 0)
            {
                // Landscape format transformation
                //mDC.translate(-fCenter.x, fCenter.y);
                //mDC.rotate(mRotate*M_PI/180);
                //mDC.translate(fCenter.x, -fCenter.y);
            }
            mDC.scale(mScaleX, -mScaleY);
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

                size_t fPos = 0, fStart = 0;
                while ((fPos = aLine.find(it->first, fStart)) != -1)
                {
                    size_t fEnd = aLine.rfind("{", fPos);
                    if (fEnd == -1)
                    {
                        fEnd =  aLine.rfind("/", fPos);
                    }
                    if (fEnd != -1)
                    {
                        size_t fStart = aLine.rfind("/", fEnd-1);
                        if (fStart != -1)
                        {
                            ++fStart;
                            fEnd = fEnd - fStart;
                            if (fEnd)
                            {
                                mShortNameMap[aLine.substr(fStart, fEnd)] = it->second;
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
    std::map< std::string, eNames >::const_iterator itFound;
    if (fNoOfStrings > 0)
    {
         itFound = mShortNameMap.find(fStrings[fNoOfStrings-1]);
         if (itFound != mShortNameMap.end())
         {
             switch (itFound->second)
             {
             case moveto:
             if (fNoOfStrings == 3)
             {
                 std::vector<float> fNumbers;
                 decodeNumbers(fStrings, fNoOfStrings-1, fNumbers); 
                 mDC.MoveTo(fNumbers[0], fNumbers[1]);
             }break;
             case lineto:
             if (fNoOfStrings == 3)
             {
                 std::vector<float> fNumbers;
                 decodeNumbers(fStrings, fNoOfStrings-1, fNumbers); 
                 mDC.LineTo(fNumbers[0], fNumbers[1]);
             }break;
             case curveto:
             if (fNoOfStrings == 7)
             {
                 std::vector<float> fNumbers;
                 decodeNumbers(fStrings, fNoOfStrings-1, fNumbers); 
                 mDC.LineTo(fNumbers[0], fNumbers[1]);
                 mDC.LineTo(fNumbers[2], fNumbers[3]);
                 mDC.LineTo(fNumbers[4], fNumbers[5]);
             }break;
             case arc: 
                 break;
             case newpath: 
                 break;
             case closepath:
                 break;
             case stroke: 
             if (fNoOfStrings == 5)
             {
                 std::vector<float> fNumbers;
                 decodeNumbers(fStrings, fNoOfStrings-1, fNumbers); 
                 mDC.MoveTo(fNumbers[0], fNumbers[1]);
                 mDC.LineTo(fNumbers[2], fNumbers[3]);
             }break;
             case setlinewidth:
                 mPen.CreatePen(PS_SOLID, 60, RGB(0,0,0));
                 mDC.SelectObject(&mPen);
                 break;
             }
         }
    }
}

