#ifndef GCODEINTERPRETER_H
#define GCODEINTERPRETER_H

#include "defines.h"
#include "matrix.h"

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/container/flat_map.hpp>
#include <QRect>



class CPlotterDC;
class PointF;

class GCodeInterpreter
{
    typedef boost::container::flat_map<std::string, FLOAT> float_id_map;
public:
    GCodeInterpreter();
    void setPlotterDC(CPlotterDC*aDC) { mDC = aDC; }
    void readFile(const QString& sFileName);
    void parseText(const QString& aText);

private:
    enum eNames  { unknown=-1, G0, G1, G2, G3, G4, G5, G20=20, G21=21, G28=28 };
    enum eMcodes {  M0=0, M5 = 5};

    void removeComment(std::string& aLine);

    QString parseGCode(const std::string& aLine);
    FLOAT getItem(const float_id_map& aMap, const std::string& aID)const;
    bool moveCurrentPoint(const float_id_map& aItems, PointF& aPoint);

    size_t decodeNumbers(const std::string& aString,const std::string&aSeparator, float_id_map& aItems);

    QString createCircle(eNames aName, const float_id_map& aItems);

    CPlotterDC *mDC;
    boost::function< QString (const std::string& ) > mParse;
};

#endif // GCODEINTERPRETER_H
