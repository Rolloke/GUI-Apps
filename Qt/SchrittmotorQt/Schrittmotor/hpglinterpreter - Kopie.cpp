#include "hpglinterpreter.h"
#include "PlotterDC.h"
#include "commonFunctions.h"
#include <boost/lexical_cast.hpp>

#include <QFile>

namespace
{
const FLOAT toTenthOf_mm = static_cast<FLOAT>(SCALE_FACTOR / 40.0); // factor to calculate 1/10 mm
const FLOAT toPen_mm = static_cast<FLOAT>(SCALE_FACTOR * 3000.0 / 254.0 ); // factor to calculate 1/10 mm
const FLOAT deg2rad = static_cast<FLOAT>(M_PI/180.0);
}

HPGLInterpreter::HPGLInterpreter():
  mDC(0)
, mPenDown(false)
, mScaleX(0)
, mScaleY(0)
{

    mNameMap["DF"]   = default_instruction;
    mNameMap["IN"]   = initialize_instructon;

    mNameMap["IP"]   = scaling_point;
    mNameMap["SC"]   = scale;
    mNameMap["IW"]   = input_window;
    mNameMap["RO"]   = rotate;
    mNameMap["PG"]   = page_output;

    mNameMap["PU"]   = pen_up;
    mNameMap["PD"]   = pen_down;
    mNameMap["PA"]   = plot_absolute;
    mNameMap["PR"]   = pen_move_relative;
    mNameMap["AA"]   = absolute_arc_plot;
    mNameMap["AR"]   = relative_arc_plot;
    mNameMap["CI"]   = circle;

    mNameMap["FT"]   = fill_type;
    mNameMap["LT"]   = line_type;
    mNameMap["PW"]   = pen_width;
    mNameMap["SM"]   = symbol_mode;
    mNameMap["SP"]   = select_pen;
    mNameMap["TL"]   = tick_length;
    mNameMap["XT"]   = x_tick;
    mNameMap["YT"]   = y_tick;
    mNameMap["PT"]   = pen_thickness;

    mNameMap["EA"]   = edge_rectangle_absolute;
    mNameMap["ER"]   = edge_rectangle_relative;


}

size_t splitStringOnNum2Char(const std::string& aString, stringlist& aList)
{
    size_t fPos, fStart = 0, fInserted = 0;

    for (fPos = 1; fPos < aString.size(); ++fPos )
    {
        if (isdigit(aString[fPos-1]) && isalpha(aString[fPos]))
        {
            aList.push_back(aString.substr(fStart, fPos-fStart));
            fStart = fPos;
            ++fInserted;
        }
    }
    if (fStart<aString.size())
    {
        aList.push_back(aString.substr(fStart));
    }
    return aList.size();
}

void HPGLInterpreter::readFile(const QString& sFileName)
{
    QFile file(sFileName);
    int aLine=0;
    mDC->clearDrawCommands();
    mDC->reset_transformation();
    mScaleX = 0;
    mScaleY = 0;

    mParse = boost::bind(&HPGLInterpreter::parseHPGL, this, _1);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::string fLine;
        int linelength = 64*1024;
        fLine.reserve(linelength);
        fLine.resize(linelength, 0);
        int  freadlength;
        while ((freadlength = file.readLine(&fLine[0], linelength)) != -1)
        {
            try
            {
                ++aLine;
                fLine.resize(freadlength);
                if (fLine[freadlength-1] == '\n')
                {
                    fLine[freadlength-1] = 0;
                }
                stringlist fStrings, fStringsColonSeparated;
                splitString(fLine, ";", fStringsColonSeparated);
                for (unsigned int i=0; i<fStringsColonSeparated.size(); ++i)
                {
                    splitStringOnNum2Char(fStringsColonSeparated[i], fStrings);
                }
                for (unsigned int i=0; i<fStrings.size(); ++i)
                {
                    trim_left(fStrings[i]);
                    if (fStrings[i].size()>1)
                    {
                        mParse(fStrings[i]);
                    }
                }
            }
            catch(const std::exception& eErr)
            {
                TRACE("Error: %s %s in line %d\n", fLine.c_str(), eErr.what(), aLine);
            }
        }
        fLine.resize(linelength);
    }

    if (mScaleX == 0 && mScaleY == 0)
    {
        mDC->scale(toTenthOf_mm, -toTenthOf_mm);
    }

    mDC->setDeviderForRedSquare(40);
    mDC->concatenateDrawCommands();
    mDC->updateWindowMapping();
    mDC->redraw();
}

void HPGLInterpreter::parseHPGL(const std::string& aLine)
{
    std::vector<FLOAT> fNumbers;
    size_t fCount;
    bool doNextPosition = false;
    std::string fNext;
    const std::string *pLine = &aLine;
    do
    {
        name_map::iterator fItName = mNameMap.find(pLine->substr(0, 2));
        fCount = 0;
        fNumbers.clear();
        doNextPosition = false;
        if (pLine->size() > 2)
        {
            fNext = pLine->substr(2);
            trim_left(fNext);
//            if (fNext.size() >= 2 && isalpha(fNext[0]) && isalpha(fNext[1]))
//            {
//                doNextPosition = true;
//                pLine = &fNext;
//            }
//            else
            {
                fCount = decodeNumbers(fNext, ",", fNumbers);
            }
        }

        if (fItName != mNameMap.end())
        {
            switch (fItName->second)
            {
    //        case default_instruction: break;
    //        case initialize_instructon: break;

            case scaling_point:
            if (fCount == 4)
            {
                mScaleX = (fNumbers[2]-fNumbers[0]);
                mScaleY = (fNumbers[3]-fNumbers[1]);
            }
            else if( fCount == 2)
            {
                mScaleX = fNumbers[0];
                mScaleY = fNumbers[1];
            }   break;
            case scale:
            if (fCount == 4 && mScaleX != 0 && mScaleY != 0)
            {
                mDC->scale((fNumbers[1]-fNumbers[0]) / mScaleX * toTenthOf_mm , -(fNumbers[3]-fNumbers[2]) / mScaleY * toTenthOf_mm);
            }
            else if (fCount == 0)
            {
                mScaleX = 0;
                mScaleY = 0;
            }
            break;
    //        case input_window: break;
            case rotate:
            if (fCount == 1)
            {
                mDC->rotate(fNumbers[0]*deg2rad);
            } break;
    //        case page_output: break;

            case pen_up: mPenDown = false; break;
            case pen_down: mPenDown = true; break;

            case plot_absolute:
            if (fCount == 2)
            {
                if (mPenDown) mDC->LineTo(fNumbers[0], fNumbers[1]);
                else          mDC->MoveTo(fNumbers[0], fNumbers[1]);
            } break;
            case pen_move_relative: break;
            if (fCount == 2)
            {
                FLOAT x, y;
                mDC->getLastPoint(x, y);
                if (mPenDown) mDC->LineTo(x + fNumbers[0], y + fNumbers[1]);
                else          mDC->MoveTo(x + fNumbers[0], y + fNumbers[1]);
            } break;
            case absolute_arc_plot:
            if (fCount >= 3)
            {
                mDC->ArcTo(fNumbers[0], fNumbers[1], fNumbers[2]);
            }   break;
            case relative_arc_plot:
            if (fCount >= 3)
            {
                FLOAT x, y;
                mDC->getLastPoint(x, y);
                mDC->ArcTo(x + fNumbers[0], y + fNumbers[1], fNumbers[2]);
            }   break;
            case circle:
            if (fCount >= 2)
            {
                mDC->ArcTo(fNumbers[0], 0, fNumbers[1], true);
            }   break;
            case edge_rectangle_absolute:
            if (fCount >= 2)
            {
                QRectF rc;
                FLOAT x, y;
                mDC->getLastPoint(x, y);
                rc.setTopLeft(QPointF(x, y));
                rc.setBottomRight(QPointF(fNumbers[0], fNumbers[1]));
                mDC->drawRectangle(rc);
            }   break;
            case edge_rectangle_relative:
            if (fCount >= 2)
            {
                QRectF rc;
                FLOAT x, y;
                mDC->getLastPoint(x, y);
                rc.setTopLeft(QPointF(x, y));
                rc.setBottomRight(QPointF(x + fNumbers[0], y + fNumbers[1]));
                mDC->drawRectangle(rc);
            }   break;

    //        case fill_type: break;
    //        case line_type: break;
            case pen_width:
            if (fCount == 1)
            {
                mDC->setDrawingWidth(fNumbers[0]*toPen_mm);
            } break;

    //        case symbol_mode: break;
    //        case select_pen: break;
    //        case tick_length: break;
    //        case x_tick: break;
    //        case y_tick: break;
            case pen_thickness:
            if (fCount == 1)
            {
                TRACE("desired pen thickness: %f\n", fNumbers[0]);
                //mDC->setDrawingWidth(fNumbers[0]);
            } break;
            default:
                TRACE("unhandled cmd %s with %d args\n", fItName->first.c_str(), fCount);
                break;
            }
        }
    } while (doNextPosition);
}

size_t HPGLInterpreter::decodeNumbers(const std::string& aString,const std::string&aSeparator, std::vector<FLOAT>& aNumbers)
{
    stringlist fStrings;
    splitString(aString, aSeparator, fStrings);
    aNumbers.resize(fStrings.size());
    for (size_t i=0; i<fStrings.size(); ++i)
    {
        aNumbers[i] = static_cast<float>(atof(fStrings[i].c_str()));
    }
    return aNumbers.size();
}
