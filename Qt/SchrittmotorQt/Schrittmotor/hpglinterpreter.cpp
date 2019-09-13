#include "hpglinterpreter.h"
#include "PlotterDC.h"
#include "commonFunctions.h"
#include <boost/lexical_cast.hpp>
#include <algorithm>

#include <QFile>

namespace
{
const FLOAT toTenthOf_mm = static_cast<FLOAT>(SCALE_FACTOR / 40.0); // factor to calculate 1/10 mm
const FLOAT toPen_mm     = static_cast<FLOAT>(SCALE_FACTOR * 3000.0 / 254.0 ); // factor to calculate 1/10 mm
const FLOAT toFontHeight = static_cast<FLOAT>(72 / 2.54); // from cm to 72 points per inch
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
    mNameMap["EW"]   = edge_wedge;
    mNameMap["RA"]   = fill_absolute_rectangle;
    mNameMap["RR"]   = fill_relative_rectangle;
    mNameMap["WG"]   = fill_wedge;

    mNameMap["LB"]   = label;
    mNameMap["SS"]   = select_standard_font;
    mNameMap["SR"]   = relative_character_size;
    mNameMap["SI"]   = absolute_character_size;
    mNameMap["SD"]   = standard_font_definition;
    mNameMap["SB"]   = scalable_or_bitmap_fonts;
    mNameMap["SA"]   = select_alternate_font;
    mNameMap["AD"]   = alternate_font_definition;
//    mNameMap["SD"]   = ;
// TODO ! erweitern


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
        qint64 fSize = file.size();
        std::string fLine;
        fLine.resize(fSize);
        int  freadlength;
        while ((freadlength = file.readLine(&fLine[0], fSize)) != -1)
        {
            try
            {
                ++aLine;
                if (fLine[freadlength-1] == '\n')
                {
                    fLine[freadlength-1] = 0;
                }
                fLine.resize(freadlength);
                stringlist fStrings;
                splitString(fLine, ";", fStrings);
                for (UINT i=0; i<fStrings.size(); ++i)
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
                TRACE(Logger::info,"Error: %s %s in line %d\n", fLine.c_str(), eErr.what(), aLine);
            }
            fLine.resize(fSize);
        }
    }

    if (mScaleX == 0 && mScaleY == 0)
    {
        mDC->scale(toTenthOf_mm, -toTenthOf_mm);
    }

    mDC->setDeviderForRedSquare(40);
    //mDC->concatenateDrawCommands();
    mDC->updateWindowMapping();
    mDC->redraw();
}

void HPGLInterpreter::parseText(const QString &aText)
{
    mDC->MoveTo(0, 0);
    mDC->clearDrawCommands();
    mDC->reset_transformation();
    stringlist fStrings;
    splitString(aText.toStdString(), "\n", fStrings);
    int aLine = 0;
    try
    {
        foreach (std::string fLine, fStrings)
        {
            ++aLine;
            mParse(fLine);
        }

        if (mScaleX == 0 && mScaleY == 0)
        {
            mDC->scale(toTenthOf_mm, -toTenthOf_mm);
        }

        mDC->setDeviderForRedSquare(40);
    //    mDC->concatenateDrawCommands();
        mDC->updateWindowMapping();
        mDC->redraw();
    }
    catch(const std::exception& eErr)
    {
        TRACE(Logger::info,"Error: %s in line %d\n", eErr.what(), aLine);
    }

}

void HPGLInterpreter::parseHPGL(const std::string& aLine)
{
    for (UINT i=0; i<aLine.size()-1; ++i)
    {
        std::vector<FLOAT> fNumbers;
        size_t fCount = 0;
        name_map::iterator fItName = mNameMap.find(aLine.substr(i, 2));
        if (fItName != mNameMap.end())
        {
            fNumbers.clear();
            if (aLine.size() > 2)
            {
                size_t count = 0;
                for (unsigned j=i+2; j<aLine.size(); ++j, ++count)
                {
                    if (!isdigit(aLine[j]) && !isspace(aLine[j]) && aLine[j] != ',' && aLine[j] != '-' && aLine[j] != '.') break;
                }
                std::string sNumbers = aLine.substr(2+i, count);
                size_t trimmed = trim_left(sNumbers);
                fCount = decodeNumbers(sNumbers, ",", fNumbers);
                i += count+trimmed;
            }

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
            case pen_up:
            {
                mPenDown = false;
                if (fCount)
                {
                    mDC->MoveTo(fNumbers[0], fNumbers[1]);
                }
            } break;
            case pen_down:
            {
                mPenDown = true;
                for (size_t i=0; i<fCount; i+=2)
                {
                    mDC->LineTo(fNumbers[i], fNumbers[i+1]);
                }
            } break;

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
                mDC->getCurrentPoint(x, y);
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
                mDC->getCurrentPoint(x, y);
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
                mDC->getCurrentPoint(x, y);
                rc.setTopLeft(QPointF(x, y));
                rc.setBottomRight(QPointF(fNumbers[0], fNumbers[1]));
                mDC->drawRectangle(rc);
            }   break;
            case edge_rectangle_relative:
            if (fCount >= 2)
            {
                QRectF rc;
                FLOAT x, y;
                mDC->getCurrentPoint(x, y);
                rc.setTopLeft(QPointF(x, y));
                rc.setBottomRight(QPointF(x + fNumbers[0], y + fNumbers[1]));
                mDC->drawRectangle(rc);
            }   break;
            case edge_wedge:
            if (fCount >= 3)
            {
                mDC->Wedge(fNumbers[0], fNumbers[1], fNumbers[2]);
            }  break;
    //        case fill_absolute_rectangle: break;
    //        case fill_relative_rectangle: break;
    //        case fill_wedge: break;

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
                TRACE(Logger::info,"desired pen thickness: %f\n", fNumbers[0]);
                // TODO!
                //mDC->setDrawingWidth(fNumbers[0]);
            } break;
            case label:
            {
                // TODO label
                mDC->TextOut(aLine.substr(2));
                TRACE(Logger::info,"TextOut: %s\n", aLine.substr(2).c_str());
            } break;
            case select_standard_font:
            case select_alternate_font:
            {
                // TODO!
                std::map<int, float> fFD;
//                fFD[FD::Height] = fNumbers[0];
                mDC->setFontDefinition(fFD, fItName->second == alternate_font_definition);
            } break;
            case absolute_character_size:
            {
                mDC->setCharSize(fNumbers[0]*toFontHeight, fNumbers[1]*toFontHeight);
            } break;
            case standard_font_definition:
            case alternate_font_definition:
            {
                std::map<int, float> fFD;
                for (size_t i=0; i<fCount; i+=2)
                {
                    fFD[static_cast<int>(fNumbers[i])] = fNumbers[i+1];
                }
                mDC->setFontDefinition(fFD, fItName->second == alternate_font_definition);
            } break;
            case scalable_or_bitmap_fonts:
            {
                // TODO scalable_or_bitmap_fonts

            } break;

            default:
                TRACE(Logger::info,"unhandled cmd %s with %lu args\n", fItName->first.c_str(), fCount);
                break;
            }
            TRACE(Logger::info,"cmd %s with %lu args\n", fItName->first.c_str(), fCount);
        }
    }
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
