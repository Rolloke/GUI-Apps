#include "gcodeinterpreter.h"
#include "PlotterDC.h"
#include "commonFunctions.h"
#include <boost/lexical_cast.hpp>
#include <string>

#include <QFile>
namespace
{
const FLOAT scale_mm   = static_cast<FLOAT>(10.0);
const FLOAT scale_inch = static_cast<FLOAT>(254.0);
const FLOAT factor_y    = -1;
}

GCodeInterpreter::GCodeInterpreter(): mDC(0)
{
}

void GCodeInterpreter::readFile(const QString &sFileName)
{

    QFile file(sFileName);
    int aLine=0;
    mDC->clearDrawCommands();
    mDC->reset_transformation();
    mDC->scale(scale_mm, scale_mm * factor_y);  // mm is default

    mParse = boost::bind(&GCodeInterpreter::parseGCode, this, _1);
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
                        removeComment(fStrings[i]);
                        QString sError = mParse(fStrings[i]);
                        if (sError.size() > 0)
                        {
                            TRACE(Logger::error,"Error: %s in line %d\n", sError.toStdString().c_str(), aLine);
                        }

                    }
                }
            }
            catch(const std::exception& eErr)
            {
                TRACE(Logger::error,"Error: %s %s in line %d\n", fLine.c_str(), eErr.what(), aLine);
            }
            fLine.resize(fSize);
        }
    }


    mDC->setDeviderForRedSquare(40);
    mDC->concatenateDrawCommands();
    mDC->updateWindowMapping();
    mDC->redraw();

}
void GCodeInterpreter::removeComment(std::string& aLine)
{
    while (1)
    {
        size_t fStartPos = aLine.find("(");
        if (fStartPos != std::string::npos)
        {
            size_t fEndPos = aLine.find(")", fStartPos);
            if (fEndPos != std::string::npos)
            {
                aLine = aLine.substr(0, fStartPos) + aLine.substr(fEndPos+1);
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    };

}

void GCodeInterpreter::parseText(const QString &aText)
{
    mDC->MoveTo(0, 0);
    mDC->clearDrawCommands();
    mDC->reset_transformation();
    mDC->scale(scale_mm, scale_mm * factor_y);  // mm is default
    stringlist fStrings;
    int aLine = 0;
    splitString(aText.toStdString(), "\n", fStrings);
    try
    {
        foreach (std::string fLine, fStrings)
        {
            ++aLine;
            removeComment(fLine);
            parseGCode(fLine);
        }
        mDC->setDeviderForRedSquare(40);
        mDC->concatenateDrawCommands();
        mDC->updateWindowMapping();
        mDC->redraw();
    }
    catch(const std::exception& eErr)
    {
        TRACE(Logger::error,"Error: %s in line %d\n", eErr.what(), aLine);
    }
}

size_t GCodeInterpreter::decodeNumbers(const std::string &aString, const std::string &aSeparator, float_id_map& aItems)
{
    stringlist fStrings;
    splitString(aString, aSeparator, fStrings);
    int fSize = 1; // TODO! count characters at start
    for (size_t i=0; i<fStrings.size(); ++i)
    {
        aItems[fStrings[i].substr(0, fSize)] = static_cast<float>(atof(&fStrings[i][fSize]));
    }
    return aItems.size();
}

FLOAT GCodeInterpreter::getItem(const float_id_map& aMap, const std::string& aID) const
{
    float_id_map::const_iterator fFound = aMap.find(aID);
    if (fFound != aMap.end())
    {
        return fFound->second;
    }
    return NAN;
}

bool  GCodeInterpreter::moveCurrentPoint(const float_id_map& aItems, PointF& aPoint)
{
    bool fReturn = false;
    FLOAT X = getItem(aItems, "X");
    if (X != 0)
    {
        aPoint.x += X;
        fReturn = true;
    }
    FLOAT Y = getItem(aItems, "Y");
    if (Y != 0)
    {
        aPoint.y += Y;
        fReturn = true;
    }
    return fReturn;
}

QString GCodeInterpreter::parseGCode(const std::string &aLine)
{
    if (aLine.size())
    {
        float_id_map fItems;
        decodeNumbers(aLine, " ", fItems);
        int fGnumber = static_cast<int>(getItem(fItems, "G"));
        FLOAT fX = getItem(fItems, "X");
        FLOAT fY = getItem(fItems, "Y");
        FLOAT fZ = getItem(fItems, "Z");
        switch (fGnumber)
        {
        case G0:    // Im Eilgang eine Position mit den Vorschub Achsen anfahren
        {
            if (fX != NAN && fY != NAN)
            {
                mDC->MoveTo(fX, fY);
            }
            else if (fZ != NAN)
            {
                return QString("Z is not yet implemented: ") + aLine.c_str();
            }
            else
            {
                return QString("No Coordinates for G0: ") + aLine.c_str();
            }
            break;
        }
        case G1:    // In normaler Geschwindigkeit eine Position anfahren (lineare Interpolation)
        {
            if (fX != NAN && fY != NAN)
            {
                mDC->LineTo(fX, fY);
            }
            else if (fZ != NAN)
            {
                return QString("Z is not yet implemented: ") + aLine.c_str();
            }
            else
            {
                return QString("No Coordinates for G1: ") + aLine.c_str();
            }
            break;
        }
        case G2:    // G02: Vorschub mit Kreisbogen, im Uhrzeigersinn (Kreis Interpolation)
        case G3:    // G03: Vorschub mit Kreisbogen, gegen den Uhrzeigersinn
            return createCircle(static_cast<eNames>(fGnumber), fItems);
            break;
        case G20:   // G20: Frei definierbare Ebene wählen
            mDC->reset_transformation();
            mDC->scale(scale_inch, scale_inch * factor_y);
            break;
        case G21:
            mDC->reset_transformation();
            mDC->scale(scale_mm, scale_mm * factor_y);
            break;
        case G28: // G28: HOME Position anfahren
            mDC->MoveTo(0, 0);
            break;
        default:
        {
            int fMnumber = static_cast<int>(getItem(fItems, "M"));
            if (fMnumber >= 0)
            {
                switch (fMnumber)
                {
                case M0: break; //  Stop or Unconditional stop
                case M5: break; //  Spindle Off (CNC specific)

                }
            }
            else
            {
                return QString("Unknown G-Code command: ") + aLine.c_str();
            }

        }
        }
    }
    return "";
}
/*
 Testcode
N10 G01 X0 Y0 Z0
N20 G01 Z-10
N30 G02 X0 Y10 I0 J5
N40 G01 X10 Y10
N50 G02 X10 Y0 I0 J-5
N60 G01 X0 Y0
N70 G01 Z0
 */
QString GCodeInterpreter::createCircle(eNames aName, const float_id_map& aItems)
{
    PointF fCurrent;
    mDC->getCurrentPoint(fCurrent.x, fCurrent.y);
    FLOAT fRadius = getItem(aItems, "R");
    if (fRadius != NAN && fRadius != 0)
    {
        moveCurrentPoint(aItems, fCurrent);
        FLOAT Left   = fCurrent.x - fRadius;
        FLOAT Right  = fCurrent.x + fRadius;
        FLOAT Top    = fCurrent.y + fRadius * factor_y;
        FLOAT Bottom = fCurrent.y - fRadius * factor_y;
        mDC->MoveTo(Right, (Top+Bottom) / 2);
        mDC->ArcTo(Left, Top, Right, Bottom, 0, aName == G2 ? -360 : 360);
    }
    else
    {
        FLOAT X = getItem(aItems, "X");
        FLOAT Y = getItem(aItems, "Y");
        FLOAT I = getItem(aItems, "I");
        FLOAT J = getItem(aItems, "J");
        if (X != NAN && Y != NAN && I != NAN && J != NAN)
        {
            PointF fCenterDirection(I, J);
            PointF fDestinationPt(X, Y);

            PointF fCenter    = fCurrent + fCenterDirection;
            fCenterDirection  = fCurrent - fCenter;
            FLOAT fStartAngle = atan2(fCenterDirection.y, fCenterDirection.x) * rad2deg;
            if (fStartAngle < 0)
            {
                fStartAngle += 360;
            }

            fCenterDirection  = fDestinationPt - fCenter;
            FLOAT fStopAngle  = atan2(fCenterDirection.y, fCenterDirection.x) * rad2deg;
            FLOAT fAngle      = fStopAngle - fStartAngle ;
            if (fabs(fAngle) < 0.01)
            {
                fAngle = 360;
            }
            if (aName == G3)
            {
                fAngle = -fAngle;
            }

            FLOAT fRadius = distance(fCenter, fCurrent);
            if (fRadius != distance(fCenter, fDestinationPt))
            {
                Logger::printDebug(Logger::warning, "Radius not consistent %f != %f", fRadius, distance(fCenter, fDestinationPt));
            }
            mDC->ArcTo(fCenter.x - fRadius, fCenter.y + fRadius, fCenter.x + fRadius, fCenter.y - fRadius, fStartAngle, fAngle);
        }
        else
        {
            return "Not enaugh circle coordinates";
        }
    }
    return "";
}

/*
G04: Verweilzeit
G05: Spline Definition
G06: Spline Interpolation
G09: Genauhalt
G14: Polarkoordinatensystem, absolut
G15: Polarkoordinatensystem, relativ
G17: X-Y Ebene wählen
G18: Z-X Ebene wählen
G19: Y-Z Ebene wählen


G33: Gewindeschneiden mit konstanter Steigung
G34: Gewindeschneiden mit zunehmender Steigung
G35: Gewindeschneiden mit abnehmender Steigung
G40: Aufheben aller Werkzeugbahnkorrekturen
G41: Werkzeugbahnkorrektur in Vorschubrichtung links
G42: Werkzeugbahnkorrektur in Vorschubrichtung rechts
G43: Werkzeugkorrektur positiv
G44: Werkzeugkorrektur negativ
G53: Löschen der Nullpunktverschiebung
G54-G59: Nullpunktverschiebungen 1-6
G80: Zyklen Lölschen
G81-G89: Bohrzyklen
G90: Absolute Koordinatenangaben im aktuellen Koordinatensystem
G91: Relative Koordinatenangaben im aktuellen Koordinatensystem
G92: Bezugspunktverschiebung – Speicher setzen
G94: Vorschub in mm/min
G95: Vorschub in mm/U
G96: Konstante Schnittgeschwindigkeit (Bitte G92 Spindeldrehzahlbegrenzung setzen)
G97: Konstante Spindeldrehzahl in U/min


M00: Programmhalt
M02: Programmende
M03: Spindel ein: Im Uhrzeigersinn
M04: Spindel ein: Gegen den Uhrzeigersinn
M05: Spindel aus
M06: Werkzeugwechsel (Werkzeug muss vorher eingemessen worden sein)
M08: Kühlmittel ein
M09: Kühlmittel aus

A 	Absolut oder Inkrementposition um die X-Achse (Drehen um X)
B 	Absolut oder Inkrementposition um die Y-Achse (Drehen um Y)
C 	Absolut oder Inkrementposition um die Z-Achse (Drehen um Z)
D 	Werkzeugkorrekturfaktor
E 	Zweiter Vorschub
F 	Vorschub
G 	Verfahrbefehle (G0)
H 	Werkzeuglängenkorrektur
I 	Definiert den Kreisbogenmittelpunkt auf der x-Achse bei den Befehlen G02 und G03
J 	Definiert den Kreisbogenmittelpunkt auf der y-Achse bei den Befehlen G02 und G03
K 	Definiert den Kreisbogenmittelpunkt auf der z-Achse bei den Befehlen G02 und G03
L 	Fixed Cycle Loop Count
M 	Zusatzfunktionen (Miscellaneous) (z.B. Kühlmittel an/aus, Motorspindel an/aus… )
N 	Satznummer
O 	Programmname
P 	Zusätzlicher Parameter für manche G und M Befehle
Q 	Zusätzlicher Parameter für manche G Befehlen
R 	Parameter, z.B. Radius bei Kreisbahn (Radius)
S 	Spindeldrehzahl
T 	Werkzeugauswahl
U 	Inkrement X
V 	Inkrement Y
W 	Inkrement Z
X 	Absolut oder Inkrementposition in Richtung der X-Achse
Y 	Absolut oder Inkrementposition in Richtung der Y-Achse
Z 	Absolut oder Inkrementposition in Richtung der Z-Achse
*/
