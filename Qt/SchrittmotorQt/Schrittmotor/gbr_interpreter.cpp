#include "gbr_interpreter.h"
#include "carithmetic.h"
#include "commonFunctions.h"

#include <string>
#include <iostream>
#include <complex>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <QFile>

namespace
{
const FLOAT scale_mm   = static_cast<FLOAT>(SCALE_FACTOR);
const FLOAT scale_inch = static_cast<FLOAT>(25.4 * SCALE_FACTOR);
const FLOAT factor_y    = -1;
const FLOAT HALF_CIRCLE = 180;
}

GbrInterpreter::GbrInterpreter():
    mDC(0)
  , mCurentApperture(0)
  , mScale(1)
  , mInterpolation(Command::IntLinear)
  , mQuadrandMode(Command::QuadrantModeSingle)
  , mPolarity(Command::PolarityDark)
  , mRegionMode(Command::RegionEnd)
  , mCombineID(static_cast<unsigned long>(CombineID::Start))
  , mAutoDetermineDrillpoints(false)
  , mCurrentLine(0)
{
    mCommandMap["FS"]  = Command::FormatSpec;
    mCommandMap["MOMM"]= Command::ModeMM;
    mCommandMap["MOIN"]= Command::ModeInch;
    mCommandMap["AD"]  = Command::ApertureDefine;
    mCommandMap["AM"]  = Command::ApertureMacro;
    mCommandMap["AB"]  = Command::ApertureBlock;
    mCommandMap["D01"] = Command::InterpolateOperation;
    mCommandMap["D02"] = Command::MoveOperation;
    mCommandMap["D03"] = Command::FlashOperation;
    mCommandMap["G01"] = Command::IntLinear;
    mCommandMap["G02"] = Command::IntCwCircular;
    mCommandMap["G03"] = Command::IntCounterCwCircular;
    mCommandMap["G74"] = Command::QuadrantModeSingle;
    mCommandMap["G75"] = Command::QuadrantModeMulti;
    mCommandMap["LPD"] = Command::PolarityDark;
    mCommandMap["LPC"] = Command::PolarityClear;
    mCommandMap["LM"]  = Command::LoadMirror;
    mCommandMap["LR"]  = Command::LoadRotation;
    mCommandMap["LS"]  = Command::LoadScale;
    mCommandMap["G36"] = Command::RegionStart;
    mCommandMap["G37"] = Command::RegionEnd;
    mCommandMap["SR"]  = Command::StepAndRepeat;
    mCommandMap["G04"] = Command::Comment;
    mCommandMap["TF"]  = Command::AttributeFile;
    mCommandMap["TA"]  = Command::AttributeAperture;
    mCommandMap["TO"]  = Command::AttributeObject;
    mCommandMap["TD"]  = Command::AttributeDelete;
    mCommandMap["M02"] = Command::EndOfFile;
    mCommandMap["SAD"] = Command::SetCurrentApertureDefine;

    mCommandMap["IPP"] = Command::ImagePolarityPos;
    mCommandMap["IPN"] = Command::ImagePolarityNeg;
    mCommandMap["IN"]  = Command::ImageName;
    mCommandMap["IR"]  = Command::ImageRotation;
    mCommandMap["LN"]  = Command::LevelName;
    mCommandMap["MI"]  = Command::MirrorImage;
    mCommandMap["OF"]  = Command::Offset;
    mCommandMap["SF"]  = Command::ScaleFactor;
}
void GbrInterpreter::clear()
{
    TRACE(Logger::to_info, ""); // delete info content
    mApertureDefineMap.clear();
    mApertureMacroMap.clear();
    mApertureMacroName.clear();
    mScale             = 0;
    mInterpolation     = Command::IntLinear;
    mQuadrandMode      = Command::QuadrantModeSingle;
    mPolarity          = Command::PolarityDark;
    mRegionMode        = Command::RegionEnd;
    mCombineID         = CombineID::Start;
    mCurrentCoordinate = PointF();
    mDC->EndPath();
    mDC->MoveTo(0, 0);
    mDC->clearDrawCommands();
    mDC->reset_transformation();
    mCurentApperture = 0;
    mCurrentLine = 0;
    mDeprecatedDefaultAperture.reset();
}

void GbrInterpreter::readFile(const QString &sFileName)
{
    QFile file(sFileName);
    clear();

    TRACE(Logger::to_info, "readFile(%s)", sFileName.toStdString().c_str());

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qint64 fSize = file.size();
        std::string fText;
        fText.resize(fSize + 4);
        fSize = file.read(&fText[0], fSize);
        fText[fSize] = 0;
        parseText(QString(fText.c_str()));
    }
    TRACE(Logger::to_info, "%d lines read", mCurrentLine);

    updateView();
}

void GbrInterpreter::parseText(const QString &aText)
{
    stringlist fStrings;
    splitString(aText.toStdString(), "\n", fStrings);

    clear();
    try
    {
        for (const std::string& fLine : fStrings)
        {
            ++mCurrentLine;
            parseGbr(fLine);
        }
    }
    catch(const std::exception& eErr)
    {
        TRACE(Logger::to_info, "Error: %s in line %d", eErr.what(), mCurrentLine);
    }

    updateView();
}

void GbrInterpreter::updateView()
{
    mDC->setDeviderForRedSquare(40);
    mDC->concatenateMoveToAndPolyLine();
    mDC->combineDrawCommands(CombineID::DrillPointOffset);

    TRACE(Logger::to_info, "%d draw commands created", mDC->getDrawCommands().size());
    mDC->updateWindowMapping();
    mDC->redraw();
}

void GbrInterpreter::setAutoDetermineDrillpoints(bool aAuto)
{
    mAutoDetermineDrillpoints = aAuto;
}

QString GbrInterpreter::parseGbr(const std::string &aLine)
{
    if (aLine.size())
    {
        int fBeginning = 0;
        Command fCmd = getCommand(aLine, fBeginning);
        bool fHandled    = true;
        bool fSuppressed = false;

        if (fCmd >= Command::ImageName && not mDeprecatedDefaultAperture)
        {
            mDeprecatedDefaultAperture = std::make_shared<ApertureDefine>();
            mDeprecatedDefaultAperture->mType = ApertureDefine::Type::circular;
            mDeprecatedDefaultAperture->mArguments.push_back(mDC->getPenWidth());
            TRACE(Logger::to_info, "File contains deprecated commands");
        }

        switch (fCmd)
        {
        case Command::ModeMM:
            mScale = scale_mm ;
            mDC->scale(mScale, mScale * factor_y);
            break;
        case Command::ModeInch:
            mScale = scale_inch;
            mDC->scale(mScale, mScale * factor_y);
            break;
        case Command::FormatSpec:           mFormatSpec.setFormatSpec(aLine.substr(fBeginning)); break;
        case Command::ApertureDefine:       createApertureDefine(aLine.substr(fBeginning));      break;
        case Command::ApertureMacro:        startApertureMacro(aLine); break;
        case Command::ApertureMacroRead:    readApertureMacroContent(aLine); break;
        case Command::ApertureBlock:        createApertureBlock(aLine); break;
        case Command::InterpolateOperation:
            interpolateOperation(aLine.substr(fBeginning));
            fSuppressed = true;
            break;
        case Command::MoveOperation:
            moveOperation(aLine.substr(fBeginning));
            break;
        case Command::FlashOperation:
            flashOperation(aLine.substr(fBeginning));
            fSuppressed = true;
            break;
        case Command::IntLinear: case Command::IntCwCircular: case Command::IntCounterCwCircular:
            // G01 Command chapter 4.9.1
            mInterpolation = fCmd;
            break;
        case Command::QuadrantModeSingle: case Command::QuadrantModeMulti:
            mQuadrandMode = fCmd; // TODO! Use QuadrantMode
            break;
        case Command::PolarityClear: case Command::PolarityDark:
            mPolarity = fCmd; // TODO! Use Polarity
            break;
        case Command::LoadMirror:   fHandled = false; break;
        case Command::LoadRotation: fHandled = false; break;
        case Command::LoadScale:    fHandled = false; break;

        case Command::RegionStart:mRegionMode = fCmd; break;
        case Command::RegionEnd:
            mRegionMode = fCmd;
            if (mDC->isPathActive())
            {
                mDC->LineTo(mRegionPt.x, mRegionPt.y);
                mDC->EndPath();
                mDC->StrokePath();
                // TODO! Fill contours created
                // TODO! handle Polarity
            }
            break;
        case Command::ImageName: TRACE(Logger::to_info, "Image name: %s", aLine.substr(fBeginning).c_str()); break;
        case Command::LevelName: TRACE(Logger::to_info, "Level name: %s", aLine.substr(fBeginning).c_str()); break;
        case Command::Comment:   TRACE(Logger::to_info, "Comment: %s", aLine.substr(fBeginning).c_str()); break;
        case Command::ImageRotation: mDC->rotate(atoi(aLine.substr(fBeginning).c_str()) * deg2rad); break;
        case Command::ImagePolarityPos: mPolarity = Command::PolarityDark; break;
        case Command::ImagePolarityNeg: mPolarity = Command::PolarityClear; break;

        case Command::MirrorImage:       fHandled = false; break;
        case Command::Offset:            fHandled = false; break;
        case Command::ScaleFactor:       fHandled = false; break;
        case Command::StepAndRepeat:     fHandled = false; break;
        case Command::AttributeFile:     fHandled = false; break;
        case Command::AttributeAperture: fHandled = false; break;
        case Command::AttributeObject:   fHandled = false; break;
        case Command::AttributeDelete:   fHandled = false; break;
        case Command::EndOfFile: break;
        case Command::SetCurrentApertureDefine: break;
        default:
            fHandled = createDrawCmd(aLine);
            fCmd = Command::CreateDrawCmd;
            fSuppressed = fHandled;
            break;
        }
        if (not fSuppressed)
        {
            if (fHandled)
            {
                TRACE(Logger::debug, "%s: %s", aLine.c_str(), nameOf(fCmd).c_str());
            }
            else
            {
                TRACE(Logger::to_info, "not handled(%d): %s", mCurrentLine, aLine.c_str());
            }
        }
    }
    return "";
}

void GbrInterpreter::createApertureDefine(const std::string& aLine)
{
    using namespace boost;
    regex fApertureExp{"(D\\d{1,3})([a-zA-Z_.$][a-zA-Z_.0-9]{0,})"};
    smatch fWhat;
    if (    regex_search(aLine, fWhat, fApertureExp)
         && fWhat.size() > 2
         && fWhat[1].matched
         && fWhat[2].matched)
    {
        std::string  fDCode      = fWhat[1];
        std::string  fType       = fWhat[2];
        UINT fStart      = fDCode.size() + fType.size();
        if (aLine.size() > fStart) ++fStart;
        std::string fMoreValues  = aLine.substr(fStart);
        FLOAT       fValue       = 0;
        int         fLength      = 0;
        std::shared_ptr<ApertureDefine> fApertureCommon;
        if (fType.size() == 1)
        {
            std::shared_ptr<ApertureDefine> fAperture = std::make_shared<ApertureDefine>();
            fApertureCommon = fAperture;
            fAperture->mType = static_cast<ApertureDefine::Type>(fType[0]);
            switch (fAperture->mType)
            {
            case ApertureDefine::Type::circular: case ApertureDefine::Type::rectangular:
            case ApertureDefine::Type::polygon:case ApertureDefine::Type::obround: break;
            default: throw std::invalid_argument("defineAperture() unknown type"); break;
            }
            mApertureDefineMap[fDCode] = fAperture;
        }
        else
        {
            auto fMacro = mApertureMacroMap.find(fType);
            if (fMacro != mApertureMacroMap.end())
            {
                mApertureDefineMap[fDCode] = fMacro->second;
                fApertureCommon = fMacro->second;
            }
            else
            {
                throw std::invalid_argument("ApertureMacro " + fType + " not found");
            }
        }
        while (getFloat(fMoreValues, fValue, fLength))
        {
            fApertureCommon->mArguments.push_back(fValue);
            fMoreValues = fMoreValues.substr(fLength + 1);
        };
    }
}

void GbrInterpreter::startApertureMacro(const std::string& aLine)
{
    using namespace boost;
    regex fApertureExp{"%AM([a-zA-Z_\\.$][a-zA-Z_\\.0-9]+)"};
    smatch fWhat;
    if (    regex_search(aLine, fWhat, fApertureExp)
         && fWhat.size() > 1
         && fWhat[1].matched)
    {
        mApertureMacroName = fWhat[1];
        std::shared_ptr<ApertureMacro> fAM = std::make_shared<ApertureMacro>();
        mApertureMacroMap[mApertureMacroName] = fAM;
    }
    else
    {
        throw std::invalid_argument("startApertureMacro wrong syntax" + aLine);
    }
}

void GbrInterpreter::readApertureMacroContent(const std::string& aLine)
{
    using namespace boost;
    regex fApertureExp{"([ =$a-zA-Z_\\.\\+\\-\\(\\)/,0-9]+)\\*(%)?"};
    smatch fWhat;
    if (    regex_search(aLine, fWhat, fApertureExp)
         && fWhat.size() > 1
         && fWhat[1].matched)
    {
        std::string fContent = fWhat[1];
        auto fMacro = mApertureMacroMap.find(mApertureMacroName);
        if (fMacro != mApertureMacroMap.end())
        {
            ApertureMacro* fAM = dynamic_cast<ApertureMacro*>(fMacro->second.get());
            fAM->insertContent(fContent);
        }

        if (fWhat[2].matched)
        {
            mApertureMacroName.clear();
        }
    }
    else
    {
        throw std::invalid_argument("readApertureMacroContent wrong syntax" + aLine);
    }
}


void GbrInterpreter::createApertureBlock(const std::string& aLine)
{
    TRACE(Logger::to_info, "NYI!: createApertureBlock(%d): %d", mCurrentLine, aLine);
    //throw std::range_error("createApertureBlock nyi" + aLine);
    // TODO: createApertureBlock
}


bool GbrInterpreter::createDrawCmd(const std::string& aLine)
{
    using namespace boost;

    QString fString = ".*(D[\\d]{2})";
    regex fNumberExp{fString.toStdString().c_str()};
    smatch fExtracted;

    if (    regex_search(aLine, fExtracted, fNumberExp)
         && fExtracted.size() > 1
         && fExtracted[1].matched)
    {
        std::string fCmdText(fExtracted[1]);
        std::string fNumbers(fExtracted[0].first, fExtracted[1].first);

        parseGbr(fCmdText + fNumbers);
        return true;
    }
    return false;
}


/// brief moveOperation chapter 4.8.3 D02 Command
void GbrInterpreter::moveOperation(const std::string& aLine)
{
    getCoordinates(aLine, mCurrentCoordinate.x, mCurrentCoordinate.y);
    if (mRegionMode == Command::RegionStart)
    {
        if (mDC->isPathActive())
        {
            mDC->LineTo(mRegionPt.x, mRegionPt.y);
            mDC->EndPath();
            mDC->StrokePath();
            // TODO! store region
            TRACE(Logger::to_info, "(%d): region behaviour for moveOperation 2 not correct", mCurrentLine);
        }
        else
        {
            mDC->BeginPath();
            mDC->MoveTo(mCurrentCoordinate.x, mCurrentCoordinate.y);
        }
        mRegionPt = mCurrentCoordinate;
    }
    else
    {
        mDC->MoveTo(mCurrentCoordinate.x, mCurrentCoordinate.y);
    }

   TRACE(Logger::debug, "Move To %f, %f", mCurrentCoordinate.x, mCurrentCoordinate.y);
}

/// brief flashOperation chapter 4.8.4 D03 Command
void GbrInterpreter::flashOperation(const std::string& aLine)
{
    getCoordinates(aLine, mCurrentCoordinate.x, mCurrentCoordinate.y);
    mDC->MoveTo(mCurrentCoordinate.x, mCurrentCoordinate.y);
    TRACE(Logger::debug, "FlashOperation at %f, %f", mCurrentCoordinate.x , mCurrentCoordinate.y );

    if (mCurentApperture)
    {
        mDC->setCombineID(boost::optional<unsigned long>(CombineID::DrillPointOffset+mCombineID));
        mCurentApperture->draw(mDC);
        mDC->setCombineID(boost::optional<unsigned long>());
        ++mCombineID;
    }
    else
    {
        std::out_of_range("flashOperation(" +  aLine + "): wrong Aperture");
    }
}

bool GbrInterpreter::getFloat(const std::string& aText, FLOAT& aValue, int &anEnd)
{
    using namespace boost;
    bool fResult = false;
    regex fNumberExp{"([-\\d\\.]{1,10})"};
    smatch fExtracted;
    if (    regex_search(aText, fExtracted, fNumberExp)
         && fExtracted[1].matched)
    {
        std::string fNumber(fExtracted[1].first, fExtracted[1].second);
        try
        {
            aValue = lexical_cast<FLOAT>(fNumber);
            anEnd = fNumber.size();
            fResult = true;
        }
        catch (const bad_lexical_cast & fLC)
        {
            throw std::invalid_argument("getFloat(" + aText + "): " + fLC.what() );
        }
    }
    return fResult;
}

/// brief getCoordinates chapter 4.8.1
/// param aLine input text
/// param aValueX extracted x coordinate
/// param aValueY extracted y coordinate
/// return length of used text for extraction
int GbrInterpreter::getCoordinates(const std::string& aLine, FLOAT& aValueX, FLOAT& aValueY)
{
    using namespace boost;
    enum eComponent { all, xi, xiNum, yj, yjNum };
    int fLength = 0;
    QString fString = "([XI]{1})?([-\\d]{1," + QString::number(mFormatSpec.mX.total() + 1) + "})?"
                      "([YJ]{1})?([-\\d]{1," + QString::number(mFormatSpec.mY.total() + 1) + "})?";
    /// hint: add one digit for sign
    regex fNumberExp{fString.toStdString().c_str()};
    smatch fExtracted;

    if (    regex_search(aLine, fExtracted, fNumberExp)
         && fExtracted.size() > xiNum)
    {
        std::string fNumber(fExtracted[all]);
        try
        {
            if (fExtracted[xiNum].matched)
            {
                std::string fNumberX(fExtracted[xiNum]);
                mFormatSpec.mX.modifyNumberString(fNumberX);
                aValueX = lexical_cast<FLOAT>(fNumberX);
            }
            if (fExtracted[yjNum].matched)
            {
                std::string fNumberY(fExtracted[yjNum]);
                mFormatSpec.mY.modifyNumberString(fNumberY);
                aValueY = lexical_cast<FLOAT>(fNumberY);
            }
        }
        catch (const bad_lexical_cast & fLC)
        {
            throw std::invalid_argument("getCoordinates(" + aLine + "): " + fLC.what() );
        }
        fLength = fNumber.size();
    }
    return fLength;
}

GbrInterpreter::Command GbrInterpreter::getCommand(const std::string& aText, int &aBeginning)
{
    int fStart  = (aText[0] == '%') ? 1 : 0;
    if (mApertureMacroName.size())
    {
        return Command::ApertureMacroRead;
    }
    else
    {
        for (int fLength = 4; fLength >= 1; --fLength)
        {
            auto fAperture = mApertureDefineMap.find(aText.substr(fStart, fLength));
            if (fAperture != mApertureDefineMap.end())
            {
                mCurentApperture = fAperture->second.get();
                return Command::SetCurrentApertureDefine;
            }
            auto fCommand = mCommandMap.find(aText.substr(fStart, fLength));
            if (fCommand != mCommandMap.end())
            {
                aBeginning = fStart + fLength;
                return fCommand->second;
            }
        }
    }
    return Command::NotDefined;
}

std::string GbrInterpreter::nameOf(GbrInterpreter::Command aCmd)
{
    switch (aCmd)
    {
    case Command::ModeMM:         return "Mode mm";
    case Command::ModeInch:       return "Mode inch";
    case Command::ApertureDefine: return "ApertureDefine";
    case Command::ApertureMacro:  return "ApertureMacro";
    case Command::FormatSpec:     return "FormatSpec";
    case Command::CreateDrawCmd:  return "CreateDrawCmd";
    case Command::RegionStart:    return "Start Region";
    case Command::RegionEnd:      return "End Region";
    case Command::InterpolateOperation: return "Interpolate";
    case Command::MoveOperation:        return "Move";
    case Command::FlashOperation:       return "Flash";
    case Command::IntLinear:            return "Interpolate Linear";
    case Command::IntCwCircular:        return "Interpolate Cw Circular";
    case Command::IntCounterCwCircular: return "Interpolate CounterCw Circular";
    case Command::PolarityDark:         return "Polarity Dark";
    case Command::PolarityClear:        return "Polarity Clear";
    case Command::QuadrantModeSingle:   return "QuadrantMode Single";
    case Command::QuadrantModeMulti:    return "QuadrantMode Multi";
    case Command::SetCurrentApertureDefine: return "Set Current Apperture";
    default:
        for (auto fCmd : mCommandMap)
        {
            if (fCmd.second == aCmd)
            {
                return fCmd.first;
            }
        }
        break;
    }

    return "unknown";
}


int getQuadrand(const PointF& aCenterOffset)
{
    if (aCenterOffset.y >= 0)
    {
        if (aCenterOffset.x >= 0)   // Q I
        {
            return 1;
        }
        else                        // Q II
        {
            return 2;
        }
    }
    else
    {
        if (aCenterOffset.x >= 0)   // Q IIII
        {
            return 4;
        }
        else                        // Q III
        {
            return 3;
        }
    }
    return 0;
}

/// brief interpolateOperation chapter 4.8.2 D01 Command
void GbrInterpreter::interpolateOperation(const std::string& aLine)
{
    ApertureDefine *fCurrentApperture = mCurentApperture;

    if (not fCurrentApperture)
    {
        if (mDeprecatedDefaultAperture)
        {
            fCurrentApperture = mDeprecatedDefaultAperture.get();
        }
        else
        {
            throw std::invalid_argument("interpolateOperation(" +  aLine + "): no aperture available");
        }
    }

    const ApertureDefine::Type fType = fCurrentApperture->mType;
    const FLOAT fPenWidth   = mDC->getPenWidth();
    const FLOAT fHeight     = fCurrentApperture->height();
    const FLOAT fWidth      = fCurrentApperture->width();
    const FLOAT fHalfWidth  = fWidth * 0.5;
    const PointF fCurrentPoint = mDC->getCurrentPoint();

    TRACE(Logger::debug, "%s to %f, %f", nameOf(mInterpolation).c_str(), mCurrentCoordinate.x , mCurrentCoordinate.y );

    int fBegin = getCoordinates(aLine, mCurrentCoordinate.x, mCurrentCoordinate.y);

    const bool fRegionMode = mRegionMode == Command::RegionStart;
    if (fRegionMode)
    {
        if (not mDC->isPathActive())
        {
            mDC->BeginPath();
        }
    }

    if (fWidth > 0)
    {
        if (mInterpolation == Command::IntLinear)
        {
            if (fRegionMode)
            {
                mDC->LineTo(mCurrentCoordinate);
            }
            else
            {
                PointF fNormal = norm(cross_product(mCurrentCoordinate, fCurrentPoint));

                PointF fDifference = mCurrentCoordinate - fCurrentPoint;
                FLOAT fLength      = abs(fDifference);
                FLOAT fDrawAngle = 0;
                fDifference = norm(fDifference);
                FLOAT fDistance = fHalfWidth;

                if (fType == ApertureDefine::Type::rectangular)
                {
                    QRectF fRect(fCurrentPoint.x - fWidth/2, fCurrentPoint.y - fHeight/2, fWidth, fHeight);
                    rke::vectorX<FLOAT> fP(fCurrentPoint.x, fCurrentPoint.y, 0), fQ(fRect.top(), fRect.left(), 0), fU (fNormal.x, fNormal.y, 0);
                    fDistance = (cross_product(fU, fQ - fP)).absolute_value() / fU.absolute_value();
                }
                else if (fType == ApertureDefine::Type::circular)
                {
                    fDrawAngle = atan2(fDifference.x, fDifference.y) * rad2deg;
                    TRACE(Logger::debug, "with angle: %f deg", fDrawAngle);
                }


                int fLines = ceil(fDistance / fPenWidth);
                if (fLines == 0) fLines = 1;

                fDistance -= fPenWidth * 0.5;
                if (   mAutoDetermineDrillpoints
                    && fType == ApertureDefine::Type::circular
                    && fWidth * 2 >= fLength)
                {
                    mDC->setCombineID(mCombineID + CombineID::DrillPointOffset);
                }
                else
                {
                    mDC->setCombineID(mCombineID);
                }

                fCurrentApperture->draw(mDC, fDrawAngle);
                for (int fLine=0; fLine < fLines; ++fLine)
                {
                    if (fDistance * 2 < fPenWidth)
                    {
                        mDC->LineTo(fCurrentPoint     );
                        mDC->LineTo(mCurrentCoordinate);
                    }
                    else
                    {
                        PointF fAcross = fNormal   * fDistance;
                        mDC->LineTo(fCurrentPoint      + fAcross);
                        mDC->LineTo(mCurrentCoordinate + fAcross);
                        mDC->LineTo(mCurrentCoordinate - fAcross);
                        mDC->LineTo(fCurrentPoint      - fAcross);
                    }
                    fDistance -= fPenWidth;
                }
                mDC->MoveTo(mCurrentCoordinate);
                fCurrentApperture->draw(mDC, fDrawAngle + HALF_CIRCLE);

                ++mCombineID;
                mDC->setCombineID(boost::optional<unsigned long>());
            }
        }
        else
        {
            PointF fOffset;
            getCoordinates(aLine.substr(fBegin), fOffset.x, fOffset.y);
            PointF fCenter      = fCurrentPoint + fOffset;
            FLOAT  fRadius      = abs(fOffset);
            FLOAT  fStartAngle  = 0;
            FLOAT  fAngleRange  = 0;
            FLOAT  fStopAngle   = 0;
            PointF fStartOffset = fCurrentPoint - fCenter;
            PointF fEndOffset   = mCurrentCoordinate - fCenter;

            if (mInterpolation == Command::IntCwCircular)
            {
                TRACE(Logger::to_info, "Incomplete: Interpolation Clockwise circular");

                if (mQuadrandMode == Command::QuadrantModeSingle)
                {
                    // TODO: IntCwCircular QuadrantModeSingle
                    fStartAngle = atan2(fStartOffset.y, fStartOffset.x) * rad2deg;
                    fStopAngle  = atan2(fEndOffset.y, fEndOffset.x) * rad2deg;
                    fAngleRange = fStopAngle - fStartAngle;
                    fStartAngle = -fStartAngle;
                }
                else if (mQuadrandMode == Command::QuadrantModeMulti)
                {
                    // verify more quadrant variations !
                    fStartAngle = atan2(fStartOffset.y, fStartOffset.x) * rad2deg;
                    fStopAngle  = atan2(fEndOffset.y, fEndOffset.x) * rad2deg;
                    if (getQuadrand(fEndOffset) <= 2) fStopAngle -= 360;
                    fAngleRange = fStopAngle - fStartAngle;
                    fStartAngle = -fStartAngle;
                }
            }
            else if (mInterpolation == Command::IntCounterCwCircular)
            {
                TRACE(Logger::to_info, "Incomplete: Interpolation Counter Clockwise circular");
                if (mQuadrandMode == Command::QuadrantModeSingle)
                {
                    // TODO: IntCounterCwCircular QuadrantModeSingle
                    fStartAngle = atan2(fStartOffset.y, fStartOffset.x) * rad2deg;
                    fStopAngle  = atan2(fEndOffset.y, fEndOffset.x) * rad2deg;
                    fAngleRange = fStartAngle - fStopAngle;
                    fStartAngle = -fStartAngle;
                }
                else if (mQuadrandMode == Command::QuadrantModeMulti)
                {
                    // verify more quadrant variations !
                    fStartAngle = atan2(fStartOffset.y, fStartOffset.x) * rad2deg;
                    fStopAngle  = atan2(fEndOffset.y, fEndOffset.x) * rad2deg;
                    if (getQuadrand(fEndOffset) > 2) fStopAngle += 360;
                    fAngleRange = fStartAngle - fStopAngle;
                    fStartAngle = -fStartAngle;
                }
            }
            else
            {
                throw std::out_of_range("interpolateOperation(" +  aLine + "): wrong Interpolation");
            }

            mDC->setCombineID(mCombineID);
            fCurrentApperture->draw(mDC);

            FLOAT fDistance = fHalfWidth;

            int fLines = ceil(fDistance / fPenWidth);
            if (fLines == 0) fLines = 1;
            fOffset = norm(fOffset);
            fDistance -= fPenWidth * 0.5;

            for (int fLine=0; fLine < fLines; ++fLine)
            {
                if (fDistance < 0)
                {
                    fDistance = 0;
                }
                FLOAT  fRadiusD  = fRadius + fDistance;
                FLOAT  fDiameter = fRadiusD * 2;
                QRectF fRect(fCenter.x - fRadiusD, fCenter.y - fRadiusD, fDiameter, fDiameter);
                mDC->MoveTo(fCurrentPoint + fOffset * -fDistance);
                mDC->ArcTo(fRect, fStartAngle, fAngleRange);
                if (fDistance != 0)
                {
                    fRadiusD  = fRadius + fDistance;
                    fDiameter = fRadiusD * 2;
                    fRect.setRect(fCenter.x - fRadiusD, fCenter.y - fRadiusD, fDiameter, fDiameter);
                    mDC->MoveTo(fCurrentPoint + fOffset * fDistance);
                    mDC->ArcTo(fRect, fStartAngle, fAngleRange);
                }
                fRadius -= fPenWidth;
                fDistance -= fPenWidth;
            }
            mDC->MoveTo(mCurrentCoordinate);

            fCurrentApperture->draw(mDC);
            mDC->setCombineID(boost::optional<unsigned long>());
            ++mCombineID;
        }
    }
    else
    {
        mDC->MoveTo(mCurrentCoordinate);
    }
}

const FLOAT GbrInterpreter::ApertureDefine::InvalidAngle = 1000;

GbrInterpreter::ApertureDefine::ApertureDefine(): mType(Type::undefined)
{
}

void GbrInterpreter::ApertureDefine::draw(CPlotterDC* aDC, FLOAT anAngle) const
{
    switch (mType)
    {
    case Type::circular:    drawCircle(aDC, anAngle); break;
    case Type::obround:     drawObround(aDC);   break;
    case Type::polygon:     drawPolygon(aDC);   break;
    case Type::rectangular: drawRectangle(aDC); break;
    default: throw std::invalid_argument("wrong type: " + nameOf());
    }
}

void GbrInterpreter::ApertureDefine::drawCircle(CPlotterDC* aDC, FLOAT anAngle) const
{
    enum components
    {
        outerDiameter,
        holeDiameter
    };
    if (outerDiameter < mArguments.size())
    {
        const FLOAT  fPenWidth  = aDC->getPenWidth();
        const PointF fCurrentPt = aDC->getCurrentPoint();
        const FLOAT  fHoleRadius = holeDiameter < mArguments.size() ?
                    (mArguments[holeDiameter] - fPenWidth) * 0.5 : 0;

        FLOAT fDiameter  = mArguments[outerDiameter];
        FLOAT fRadius    = fDiameter * 0.5;
        int   fLines     = ceil(fRadius / fPenWidth);
        if (fLines == 0) fLines = 1;

        fRadius      -= fPenWidth * 0.5;
        while (fLines)
        {
            fDiameter = fRadius * 2;
            if (   fRadius   < fHoleRadius
                || fDiameter < fPenWidth)
            {
                break;
            }

            QRectF fRect(fCurrentPt.x - fRadius, fCurrentPt.y - fRadius, fDiameter, fDiameter);
            if (anAngle != InvalidAngle)
            {
                aDC->ArcTo(fRect, anAngle, HALF_CIRCLE);
            }
            else
            {
                aDC->MoveTo(fCurrentPt + PointF(fRadius, 0));
                aDC->addElipse(fRect);
            }
            fRadius -= fPenWidth;
            --fLines;
        }

        TRACE(Logger::debug, "drawCircle %f diameter in %f, %f, hole %f", mArguments[outerDiameter], fCurrentPt.x, fCurrentPt.y, fHoleRadius);

        aDC->MoveTo(fCurrentPt);
    }
    else
    {
        throw std::out_of_range("Apperture::drawCircle: to few arguments");
    }
}

void GbrInterpreter::ApertureDefine::drawObround(CPlotterDC* aDC) const
{
    enum components
    {
        width,
        height,
        holeDiameter
    };
    if (height < mArguments.size())
    {
        const FLOAT  fPenWidth  = aDC->getPenWidth();
        const FLOAT  f2PenWidth = fPenWidth * 2;
        const PointF fCurrentPt = aDC->getCurrentPoint();
        FLOAT  fWidth  = mArguments[width];
        FLOAT  fHeight = mArguments[height];
        FLOAT& fSmaller = fWidth < fHeight ? fWidth : fHeight;

        fWidth  -= fPenWidth;
        fHeight -= fPenWidth;

        int fLines = ceil((fSmaller / 2) / fPenWidth);
        if (fLines == 0) fLines = 1;
        while (fLines)
        {
            if (fSmaller <= 0)
            {
                fSmaller = fPenWidth;
            }

            if (fWidth > fHeight)
            {
                FLOAT fRadius = fHeight / 2;
                FLOAT fCenterDistance = (fWidth - fHeight) * 0.5;
                QRectF fRect(fCurrentPt.x - fRadius + fCenterDistance, fCurrentPt.y - fRadius, fHeight, fHeight);
                aDC->MoveTo(fCurrentPt + PointF(fCenterDistance, fRadius));
                aDC->ArcTo(fRect, -90, 180);
                aDC->LineTo(fCurrentPt - PointF(fCenterDistance, fRadius));
                fRect.translate(-fCenterDistance*2, 0);
                aDC->ArcTo(fRect, 90, 180);
                aDC->LineTo(fCurrentPt + PointF(fCenterDistance, fRadius));
            }
            else
            {
                FLOAT fRadius = fWidth / 2;
                FLOAT fCenterDistance = (fHeight - fWidth) * 0.5;
                QRectF fRect(fCurrentPt.x - fRadius ,fCurrentPt.y - fRadius - fCenterDistance, fWidth, fWidth);
                aDC->MoveTo(fCurrentPt + PointF(fRadius, -fCenterDistance));
                aDC->ArcTo(fRect, 0, 180);
                aDC->LineTo(fCurrentPt + PointF(-fRadius, fCenterDistance));
                fRect.translate(0, fCenterDistance*2);
                aDC->ArcTo(fRect, 180, 180);
                aDC->LineTo(fCurrentPt + PointF(fRadius, -fCenterDistance));
            }

            fWidth  -= f2PenWidth;
            fHeight -= f2PenWidth;
            --fLines;
        }

        TRACE(Logger::debug, "drawObround %f, %f in %f, %f", mArguments[width], mArguments[height], fCurrentPt.x, fCurrentPt.y);
        if (holeDiameter < mArguments.size())
        {
            // TODO: regard hole diameter?
            TRACE(Logger::to_info, "NYI: draw hole Circle %f diameter in %f, %f", mArguments[holeDiameter], fCurrentPt.x, fCurrentPt.y);
        }

        aDC->MoveTo(fCurrentPt);
    }
}

void GbrInterpreter::ApertureDefine::drawPolygon(CPlotterDC* aDC) const
{
    enum components
    {
        outerDiameter,
        vertices,
        rotation,
        holeDiameter
    };
    if (vertices < mArguments.size())
    {
        const FLOAT  fPenWidth   = aDC->getPenWidth();
        const PointF fCurrentPt  = aDC->getCurrentPoint();
        const int    fVertices   = mArguments[vertices];
        const FLOAT  fAngleStep  = 2 * M_PI / (fVertices);
        const FLOAT  fHoleRadius = holeDiameter < mArguments.size() ?
                    (mArguments[holeDiameter] - fPenWidth) * 0.5 : 0;

        //const FLOAT  fRotation   = rotation < mArguments.size() ? mArguments[rotation] : 0;
        //Transformation fTransformation(*aDC, PointF(), fRotation);
        // TODO: rotation
        if (rotation < mArguments.size())
        {
            TRACE(Logger::debug, "NYI: Rotation of ApertureDefine::drawPolygon");
        }
        std::vector<PointF> fPolygon;
        double fSin, fCos;
        for (int i=0; i<fVertices; ++i)
        {
            sincos(i*fAngleStep, &fSin, &fCos);
            fPolygon.push_back(PointF(fSin, fCos));
        }

        FLOAT  fDiameter  = mArguments[outerDiameter];
        int fLines = ceil((fDiameter / 2) / fPenWidth);
        if (fLines == 0) fLines = 1;

        FLOAT fRadius = fDiameter * 0.5;
        fRadius      -= fPenWidth * 0.5;
        while (fLines)
        {
            if (   fRadius     < fHoleRadius
                || fRadius * 2 < fPenWidth)
            {
                break;
            }

            aDC->MoveTo(fPolygon[0] * fRadius + fCurrentPt);
            for (int i=1; i<fVertices; ++i)
            {
                aDC->LineTo(fPolygon[i] * fRadius + fCurrentPt);
            }
            aDC->LineTo(fPolygon[0] * fRadius + fCurrentPt);

            fRadius -= fPenWidth;
            --fLines;
        }

        TRACE(Logger::debug, "drawPolygon %f, %f, in %f, %f", mArguments[0], mArguments[1], fCurrentPt.x, fCurrentPt.y);

        aDC->MoveTo(fCurrentPt);
    }
}

void GbrInterpreter::ApertureDefine::drawRectangle(CPlotterDC* aDC) const
{
    enum components
    {
        width,
        height,
        holeDiameter
    };
    if (height < mArguments.size())
    {
        const PointF fCurrentPt = aDC->getCurrentPoint();
        const FLOAT  fPenWidth  = aDC->getPenWidth();

        FLOAT  fWidth   = mArguments[width];
        FLOAT  fHeight  = mArguments[height];
        FLOAT& fSmaller = fWidth < fHeight ? fWidth : fHeight;

        fWidth  -= fPenWidth;
        fHeight -= fPenWidth;

        int fLines = ceil((fSmaller / 2) / fPenWidth);
        if (fLines == 0) fLines = 1;
        while (fLines)
        {
            if (fSmaller <= 0)
            {
                fSmaller = fPenWidth;
            }

            QRectF fRect(fCurrentPt.x - fWidth * 0.5, fCurrentPt.y - fHeight * 0.5, fWidth, fHeight);
            aDC->drawRectangle(fRect);

            fWidth  -= fPenWidth * 2;
            fHeight -= fPenWidth * 2;
            --fLines;
        }
        TRACE(Logger::debug, "drawRectangle %f, %f, in %f, %f", mArguments[width], mArguments[height], fCurrentPt.x, fCurrentPt.y);
        if (holeDiameter < mArguments.size())
        {
            // TODO: regard hole diameter?
            TRACE(Logger::to_info, "NYI: draw hole Circle %f diameter in, %f, %f", mArguments[holeDiameter], fCurrentPt.x, fCurrentPt.y);
        }
        aDC->MoveTo(fCurrentPt);
    }
}

std::string GbrInterpreter::ApertureDefine::nameOf() const
{
    QString fText = QString::number(static_cast<std::uint32_t>(mArguments.size()));
    switch (mType)
    {
    case Type::circular:    fText = "circular: "    + fText; break;
    case Type::obround:     fText = "obround: "     + fText; break;
    case Type::polygon:     fText = "polygon: "     + fText; break;
    case Type::rectangular: fText = "rectangular: " + fText; break;
    case Type::macro:       fText = "macro: "       + fText; break;
    case Type::undefined:   fText = "undefined";  break;
    }
    return fText.toStdString();
}

FLOAT GbrInterpreter::ApertureDefine::width() const
{
    enum components
    {
        width_and_diameter = 0
    };
    FLOAT fWidth = 0;
    if (mArguments.size())
    {
        switch (mType)
        {
        case Type::circular: case Type::rectangular: case Type::polygon: case Type::obround:
            fWidth = mArguments[width_and_diameter];
            break;
        default: break;
        }
    }
    return fWidth;
}

FLOAT GbrInterpreter::ApertureDefine::height() const
{
    enum components
    {
        height = 1,
        diameter = 0
    };
    FLOAT fHeight = 0;
    if (mArguments.size())
    {
        switch (mType)
        {
        case Type::circular:    case Type::polygon: fHeight = mArguments[diameter]; break;
        case Type::rectangular: case Type::obround: fHeight = mArguments[height];  break;
        default: break;
        }
    }
    return fHeight;
}

GbrInterpreter::ApertureMacro::ApertureMacro()
{
    mVariableMap = std::make_shared<Arithmetic::tVarMap>();
}

void GbrInterpreter::ApertureMacro::modifyEquation(std::string &aEquation)
{
    for (auto& aChar: aEquation)
    {
        if (aChar == 'x' || aChar == 'X') aChar = '*';
    }
}

void GbrInterpreter::ApertureMacro::insertContent(const std::string &aContent)
{
    TRACE(Logger::debug, "ApertureMacro::insertContent(%s)", aContent.c_str());
    if (aContent.size())
    {
        if (aContent[0] == '$') // variable
        {
            stringlist fList;
            splitString(aContent, "=", fList);
            (*mVariableMap)[fList[0]] = std::make_shared< std::complex<double> >();
            if (fList.size() > 1)
            {
                std::shared_ptr<Arithmetic> fAritmetic = std::make_shared<Arithmetic>();
                fAritmetic->setVarMap(mVariableMap);
                modifyEquation(fList[1]);
                sError fError = fAritmetic->setExpression(fList[1].c_str());
                if (fAritmetic->getError() == IDE_AR_OK)
                {
                    mVariableExpression[fList[0]] = fAritmetic;
                }
                else
                {
                    QString fText = "aritmetic expression wrong: " + QString::number(fAritmetic->getError())
                            + ", " + QString::number(fError.mStart) + ", " + QString::number(fError.mStop);
                    throw std::invalid_argument(fText.toStdString());
                }
            }
        }
        else                    // Primitive
        {
            stringlist fList;
            splitString(aContent, ",", fList);
            if (fList.size()>2)
            {
                tPrimitive fP(*this);
                fP.setContent(fList);
                mPrimitives.push_back(fP);
                TRACE(Logger::debug, "Primitive %d: %s", mPrimitives.size(), aContent.c_str());
            }
        }
    }
}

void GbrInterpreter::ApertureMacro::draw(CPlotterDC *aDC, FLOAT ) const
{
    auto &fMap = *mVariableMap;
    for (std::uint32_t fI = 0; fI < mArguments.size(); ++fI)
    {
        std::stringstream fVar;
        fVar << "$" << (fI+1);
        auto fVariable = fMap.find(fVar.str());
        if (fVariable  != fMap.end())
        {
            *fVariable->second = mArguments[fI];
            TRACE(Logger::debug, "Variable %s; Argument %d: %f", fVar.str().c_str(), fI, mArguments[fI]);
        }
    }

    for (auto& fExpression : mVariableExpression)
    {
        auto fVariable = fMap.find(fExpression.first);
        if (fVariable  != fMap.end())
        {
            *fVariable->second = fExpression.second->calculate();
            TRACE(Logger::debug, "Variable %s; Expression %s: %f", fExpression.first.c_str(), fExpression.second->getExpression().c_str(), fVariable->second->real());
        }
    }

    for (auto fPrimitive : mPrimitives)
    {
        fPrimitive.draw(aDC);
    }
}

void GbrInterpreter::tPrimitive::setContent(const stringlist& aList)
{
    mCode       = static_cast<tPrimitive::Code>(atoi(aList[0].c_str()));
    mExposureOn = atoi(aList[1].c_str()) != 0;
    if (mExposureOn == false)
    {
        TRACE(Logger::to_info, "Primitive %s of Apperture %s has Exposure off", name().c_str(), mParent.nameOf().c_str());
    }
    if (mCode != tPrimitive::Code::Comment)
    {
        for (std::uint32_t i = 2; i < aList.size(); ++i)
        {
            std::shared_ptr<Arithmetic> fAritmetic = std::make_shared<Arithmetic>();
            fAritmetic->setVarMap(mParent.mVariableMap);
            std::string fEquation = aList[i];
            modifyEquation(fEquation);
            sError fError = fAritmetic->setExpression(fEquation.c_str());
            if (fAritmetic->getError() == IDE_AR_OK)
            {
                mComponents.push_back(fAritmetic);
                TRACE(Logger::debug, "Component:%s[%d]: %s", name().c_str(), i-1, fEquation.c_str());
            }
            else
            {
                QString fText = "arithmetic expression wrong: " + QString::number(fAritmetic->getError())
                        + ", " + QString::number(fError.mStart) + ", " + QString::number(fError.mStop);
                throw std::range_error(fText.toStdString());
            }
        }
    }
}

GbrInterpreter::tPrimitive::Primitive(ApertureMacro &aAM) : mParent(aAM)
{

}

void GbrInterpreter::tPrimitive::draw(CPlotterDC *aDC) const
{
    switch (mCode)
    {
    case Code::Circle:     drawCircle(aDC);     break;
    case Code::CenterLine: drawCenterLine(aDC); break;
    case Code::VectorLine: drawVectorLine(aDC); break;
    case Code::Polygone:   drawPolygon(aDC);    break;
    case Code::Outline:    drawOutline(aDC);    break;
    case Code::Thermal:    drawThermal(aDC);    break;
    case Code::Moire:      drawMoire(aDC);      break;
    case Code::Comment:                         break;
    default:
        throw std::invalid_argument("Unknown aperture macro primitive type");
        break;
    }
}

void GbrInterpreter::tPrimitive::drawCircle(CPlotterDC *aDC) const
{
    enum components
    {
        diameter, centerX, centerY, rotation
    };
    if (centerY < mComponents.size())
    {
        const FLOAT  fDiameterOrg = mComponents[diameter]->calculate().real();
        const PointF fCenter(mComponents[centerX]->calculate().real(), mComponents[centerY]->calculate().real());
        FLOAT  fRotation = 0;
        if (rotation < mComponents.size())
        {
            fRotation = mComponents[rotation]->calculate().real();
        }

        const FLOAT  fPenWidth  = aDC->getPenWidth();
        const PointF fCurrentPt = aDC->getCurrentPoint();

        Transformation fTransform(*aDC, fCurrentPt, fRotation);

        FLOAT fRadius    = fDiameterOrg * 0.5;
        int   fLines     = ceil(fRadius / fPenWidth);
        if (fLines == 0) fLines = 1;

        fRadius      -= fPenWidth * 0.5;
        while (fLines)
        {
            FLOAT fDiameter = fRadius * 2;
            if (fDiameter < fPenWidth)
            {
                break;
            }

            QRectF fRect(fCenter.x - fRadius, fCenter.y - fRadius, fDiameter, fDiameter);

            aDC->MoveTo(fCenter + PointF(fRadius, 0));
            aDC->addElipse(fRect);

            fRadius -= fPenWidth;
            --fLines;
        }

        TRACE(Logger::debug, "drawCircle %f diameter in %f, %f", fDiameterOrg, fCenter.x, fCenter.y);

        aDC->MoveTo(fCurrentPt);
    }
    else
    {
        throw std::out_of_range("ApertureMacro::Primitive::drawCircle: to few arguments");
    }
}

void GbrInterpreter::tPrimitive::drawVectorLine(CPlotterDC *aDC) const
{
    enum components
    {
        width, startX, startY, endX, endY, rotation
    };
    if (rotation < mComponents.size())
    {
        const FLOAT  fWidth = mComponents[width]->calculate().real();
        const PointF fStart(mComponents[startX]->calculate().real(), mComponents[startY]->calculate().real());
        const PointF fEnd(mComponents[endX]->calculate().real(), mComponents[endY]->calculate().real());
        const FLOAT  fRotation = mComponents[rotation]->calculate().real();
        const FLOAT  fPenWidth  = aDC->getPenWidth();
        const PointF fCurrentPt = aDC->getCurrentPoint();

        Transformation fTransform(*aDC, fCurrentPt, fRotation);

        FLOAT fHalfWidth = fWidth/2;
        int   fLines     = ceil(fHalfWidth / fPenWidth);
        if (fLines == 0) fLines = 1;

        fHalfWidth      -= fPenWidth * 0.5;
        while (fLines)
        {
            if (fHalfWidth < fPenWidth)
            {
                break;
            }

            aDC->MoveTo(fStart);
            aDC->LineTo(fEnd);

            fHalfWidth -= fPenWidth;
            --fLines;
        }

        aDC->MoveTo(fCurrentPt);
    }
    else
    {
        throw std::out_of_range("ApertureMacro::Primitive::drawVectorLine: to few arguments");
    }
}

void GbrInterpreter::tPrimitive::drawCenterLine(CPlotterDC *aDC) const
{
    enum components
    {
        width, height, centerX, centerY, rotation
    };
    if (rotation < mComponents.size())
    {
        const PointF fCenter(mComponents[centerX]->calculate().real(), mComponents[centerY]->calculate().real());
        const FLOAT  fRotation = mComponents[rotation]->calculate().real();
        const PointF fCurrentPt = aDC->getCurrentPoint();
        const FLOAT  fPenWidth  = aDC->getPenWidth();

        FLOAT  fWidth  = mComponents[width]->calculate().real();
        FLOAT  fHeight = mComponents[height]->calculate().real();
        FLOAT& fSmaller = fWidth < fHeight ? fWidth : fHeight;

        Transformation fTransform(*aDC, fCurrentPt, fRotation);

        TRACE(Logger::debug, "drawRectangle %f, %f, in %f, %f", fWidth, fHeight, fCenter.x, fCenter.y);

        fWidth  -= fPenWidth;
        fHeight -= fPenWidth;

        int fLines = ceil((fSmaller / 2) / fPenWidth);
        if (fLines == 0) fLines = 1;
        while (fLines)
        {
            if (fSmaller <= 0)
            {
                fSmaller = fPenWidth;
            }

            QRectF fRect(fCenter.x - fWidth * 0.5, fCenter.y - fHeight * 0.5, fWidth, fHeight);
            aDC->drawRectangle(fRect);

            fWidth  -= fPenWidth * 2;
            fHeight -= fPenWidth * 2;
            --fLines;
        }
        aDC->MoveTo(fCurrentPt);
    }
    else
    {
        throw std::out_of_range("ApertureMacro::Primitive::drawCenterLine: to few arguments");
    }
}

void GbrInterpreter::tPrimitive::drawPolygon(CPlotterDC *aDC) const
{
    enum components
    {
        vertices, centerX, centerY, diameter, rotation
    };
    if (rotation < mComponents.size())
    {
        const std::uint32_t fVertices = mComponents[vertices]->calculate().real();
        const FLOAT  fDiameter = mComponents[diameter]->calculate().real();
        const PointF fCenter(mComponents[centerX]->calculate().real(), mComponents[centerY]->calculate().real());
        const FLOAT  fRotation = mComponents[rotation]->calculate().real();

        const FLOAT  fPenWidth   = aDC->getPenWidth();
        const PointF fCurrentPt  = aDC->getCurrentPoint();
        const FLOAT  fAngleStep  = 2 * M_PI / (fVertices);

        Transformation fTransform(*aDC, fCurrentPt, fRotation);

        std::vector<PointF> fPolygon;
        double fSin, fCos;
        for (std::uint32_t i=0; i<fVertices; ++i)
        {
            sincos(i*fAngleStep, &fSin, &fCos);
            fPolygon.push_back(PointF(fSin, fCos));
        }

        int fLines = ceil((fDiameter / 2) / fPenWidth);
        if (fLines == 0) fLines = 1;

        FLOAT fRadius = fDiameter * 0.5;
        fRadius      -= fPenWidth * 0.5;
        while (fLines)
        {
            if (fRadius * 2 < fPenWidth)
            {
                break;
            }

            aDC->MoveTo(fPolygon[0] * fRadius + fCenter);
            for (std::uint32_t  i=1; i<fVertices; ++i)
            {
                aDC->LineTo(fPolygon[i] * fRadius + fCenter);
            }
            aDC->LineTo(fPolygon[0] * fRadius + fCenter);

            fRadius -= fPenWidth;
            --fLines;
        }
        TRACE(Logger::debug, "drawPolygon %f, %d, in %f, %f", fDiameter, fVertices, fCenter.x, fCenter.y);

        aDC->MoveTo(fCurrentPt);
    }
    else
    {
        throw std::out_of_range("ApertureMacro::Primitive::drawPolygon: to few arguments");
    }
}

void GbrInterpreter::tPrimitive::drawMoire(CPlotterDC * /* aDC */) const
{
    TRACE(Logger::to_info, "NYI!: ApertureMacro::Primitive::drawMoire");
    // TODO! drawMoire
}

void GbrInterpreter::tPrimitive::drawThermal(CPlotterDC * /* aDC */) const
{
    TRACE(Logger::to_info, "NYI!: ApertureMacro::Primitive::drawThermal");
    // TODO! drawThermal
}

void GbrInterpreter::tPrimitive::drawOutline(CPlotterDC * aDC) const
{
    enum components
    {
        vertices, startX, startY, points
    };
    if (points < mComponents.size())
    {
        const std::uint32_t fVertices = mComponents[vertices]->calculate().real();
        const PointF fStart(mComponents[startX]->calculate().real(), mComponents[startY]->calculate().real());
        std::vector<PointF> fPoints;
        std::uint32_t i, j;
        for (i=0, j=points; i<fVertices; ++i, j+=2)
        {
            fPoints.push_back(PointF(mComponents[points + j]->calculate().real(), mComponents[points + j + 1]->calculate().real()));
        }
        const FLOAT fRotation = mComponents[j]->calculate().real();
        const PointF fCurrentPt = aDC->getCurrentPoint();

        Transformation fTransform(*aDC, fCurrentPt, fRotation);

        aDC->MoveTo(fStart.x, fStart.y);
        for (auto &fPoint : fPoints)
        {
            aDC->LineTo(fPoint.x, fPoint.y);
        }

        aDC->MoveTo(fCurrentPt);
    }
    else
    {
        throw std::out_of_range("ApertureMacro::Primitive::drawOutline: to few arguments");
    }
}

std::string GbrInterpreter::tPrimitive::name() const
{
    switch (mCode)
    {
    case Code::Circle: return "Circle";
    case Code::CenterLine: return "CenterLine";
    case Code::VectorLine: return "VectorLine";
    case Code::Comment: return "Comment";
    case Code::Moire: return "Moire";
    case Code::Outline: return "Outline";
    case Code::Polygone: return "Polygone";
    case Code::Thermal: return "Thermal";
    default: return "Undefined";
    }
}

GbrInterpreter::tTransformation::Transformation(CPlotterDC &aDC, const PointF &aTranslation, FLOAT aRotation)
    : mDC(aDC)
{
    if (aTranslation.x != 0 || aTranslation.y != 0 || aRotation != 0)
    {
        mTransformation.reset(new rke::matrix<FLOAT>());
        *mTransformation = aDC.getTransformation();
        aDC.translate(aTranslation.x, aTranslation.y);
        if (aRotation != 0)
        {
            aDC.rotate(aRotation*deg2rad);
        }
    }
}

GbrInterpreter::tTransformation::~Transformation()
{
    if (mTransformation)
    {
        mDC.setTransformation(*mTransformation);
    }
}

void GbrInterpreter::CoordinateFormat::Digit::modifyNumberString(std::string& aNumber)
{
    if (aNumber.size() > mDecimal)
    {
        aNumber.insert(aNumber.size() - mDecimal, ".");
    }
    else
    {
        int fPos = 0;
        if (aNumber[fPos] == '-')
        {
            fPos = 1;
        }
        while (aNumber.size() < mDecimal)
        {
            aNumber.insert(fPos, "0");
        }
        aNumber.insert(fPos, ".");
        aNumber.insert(fPos, "0");
    }
}

std::uint16_t GbrInterpreter::CoordinateFormat::Digit::total()
{
    return mDecimal + mInteger;
}

void GbrInterpreter::CoordinateFormat::setFormatSpec(const std::string& aLine)
{
    using namespace boost;
    enum components { all, flags, intX, ptX, decX, intY, ptY, decY };
    regex fNumberExp{"([A-Z]{2})X(\\d)(\\.)?(\\d)Y(\\d)(\\.)?(\\d)"};
    smatch fExtracted;
    if (    regex_search(aLine, fExtracted, fNumberExp)
         && fExtracted[all].matched)
    {
        mOmitLeadingZeros    = std::string(fExtracted[flags]).at(0) == 'L';
        mAbsoluteCoordinates = std::string(fExtracted[flags]).at(1) == 'A';

        mX.mInteger = static_cast<int>(std::string(fExtracted[intX]).at(0) - '0');
        mX.mDecimal = static_cast<int>(std::string(fExtracted[decX]).at(0) - '0');

        mY.mInteger = static_cast<int>(std::string(fExtracted[intY]).at(0) - '0');
        mY.mDecimal = static_cast<int>(std::string(fExtracted[decY]).at(0) - '0');
    }
    else
    {
        throw std::length_error("setFormatSpec() string wrong");
    }
}

