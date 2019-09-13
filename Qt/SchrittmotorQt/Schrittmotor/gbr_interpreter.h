#ifndef GbrInterpreter_H
#define GbrInterpreter_H

#include "defines.h"
#include "matrix.h"
#include "PlotterDC.h"
#include "carithmetic.h"

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <QRect>



class CPlotterDC;
class PointF;

class GbrInterpreter
{

public:
    GbrInterpreter();
    void setPlotterDC(CPlotterDC*aDC) { mDC = aDC; }
    void readFile(const QString& sFileName);
    void parseText(const QString& aText);
    void setAutoDetermineDrillpoints(bool aAuto);

private:
    enum class Command
    {
        NotDefined=-1,
        FormatSpec,
        ModeMM,
        ModeInch,
        ApertureDefine,
        ApertureMacro,
        ApertureMacroRead,
        ApertureBlock,
        InterpolateOperation,
        MoveOperation,
        FlashOperation,
        IntLinear,
        IntCwCircular,
        IntCounterCwCircular,
        QuadrantModeSingle,
        QuadrantModeMulti,
        PolarityDark,
        PolarityClear,
        LoadMirror,
        LoadRotation,
        LoadScale,
        RegionStart,
        RegionEnd,
        StepAndRepeat,
        Comment,
        AttributeFile,
        AttributeAperture,
        AttributeObject,
        AttributeDelete,
        EndOfFile,
        SetCurrentApertureDefine,
        CreateDrawCmd,
        // deprecated
        ImageName,
        ImagePolarityPos,
        ImagePolarityNeg,
        ImageRotation,
        LevelName,
        MirrorImage,
        Offset,
        ScaleFactor
    };

    struct CombineID
    {
        enum id
        {
            Start=1, DrillPointOffset=100000
        };
    };

    class ApertureDefine
    {
    public:
        enum class Type : char
        {
            undefined   = 'U',
            circular    = 'C',
            rectangular = 'R',
            obround     = 'O',
            polygon     = 'P',
            macro       = 'M'
        };
        static const FLOAT InvalidAngle;

        class Transformation
        {
        public:
              Transformation(CPlotterDC& aDC, const PointF& aTranslation, FLOAT aRotation);
              ~Transformation();
        private:
              CPlotterDC &mDC;
              std::unique_ptr<rke::matrix<FLOAT>> mTransformation;
        };

        ApertureDefine();

        virtual void draw(CPlotterDC* aDC, FLOAT anAngle=InvalidAngle) const;
        std::string nameOf() const;

        Type mType;
        std::vector<FLOAT> mArguments;

        FLOAT width()              const;
        FLOAT height()             const;

    private:
        void drawCircle(CPlotterDC* aDC, FLOAT anAngle=InvalidAngle) const;
        void drawRectangle(CPlotterDC* aDC) const;
        void drawObround(CPlotterDC* aDC)   const;
        void drawPolygon(CPlotterDC* aDC)   const;
    };
    typedef ApertureDefine::Transformation tTransformation;

    class ApertureMacro : public ApertureDefine
    {
    public:
        ApertureMacro();
        void insertContent(const std::string& aContent);
        virtual void draw(CPlotterDC* aDC, FLOAT anAngle=InvalidAngle) const;

        typedef std::map<std::string, std::shared_ptr<Arithmetic>> tVariableExpressions;
        class Primitive
        {
        public:

            Primitive( ApertureMacro& aAM);

            enum class Code
            {
                Comment = 0,
                Circle  = 1,
                VectorLine = 20,
                CenterLine = 21,
                Outline    = 4,
                Polygone   = 5,
                Moire      = 6,
                Thermal    = 7
            };

            void setContent(const stringlist& aContent);
            void draw(CPlotterDC* aDC) const;
            std::string name() const;

        private:
            void drawCircle(CPlotterDC* aDC) const;
            void drawVectorLine(CPlotterDC* aDC) const;
            void drawCenterLine(CPlotterDC* aDC) const;
            void drawPolygon(CPlotterDC* aDC)   const;
            void drawMoire(CPlotterDC* aDC)   const;
            void drawThermal(CPlotterDC* aDC)   const;
            void drawOutline(CPlotterDC* aDC)   const;


            Code mCode;
            bool mExposureOn;
            std::vector< std::shared_ptr<Arithmetic> > mComponents;
            ApertureMacro& mParent;
        };

    private:
        static void modifyEquation(std::string& aEquation);
        std::shared_ptr<Arithmetic::tVarMap>  mVariableMap;
        tVariableExpressions                  mVariableExpression;
        std::vector<Primitive>                mPrimitives;
    };
    typedef ApertureMacro::Primitive tPrimitive;


    struct CoordinateFormat
    {
        struct Digit
        {
            void modifyNumberString(std::string& aNumber);
            std::uint16_t total();

            std::uint16_t mInteger;
            std::uint16_t mDecimal;
        };

        void setFormatSpec(const std::string& aLine);

        bool mOmitLeadingZeros;
        bool mAbsoluteCoordinates;
        Digit mX;
        Digit mY;
    };

    typedef std::map< std::string, std::shared_ptr<ApertureDefine>> aperture_define_map;
    typedef std::map< std::string, std::shared_ptr<ApertureDefine>> aperture_macro_map;
    typedef std::map< std::string, Command> cmd_map;

    void        clear();
    QString     parseGbr(const std::string& aLine);

    Command     getCommand(const std::string& aText, int &aBeginning);
    std::string nameOf(Command);

    bool        getFloat(const std::string& aText, FLOAT& aValue, int &anEnd);
    int         getCoordinates(const std::string& aLine, FLOAT& aX, FLOAT& aY);

    bool        createDrawCmd(const std::string& aLine);

    void        createApertureDefine(const std::string& aLine);
    void        startApertureMacro(const std::string& aLine);
    void        readApertureMacroContent(const std::string& aLine);
    void        createApertureBlock(const std::string& aLine);

    void        moveOperation(const std::string& aLine);
    void        interpolateOperation(const std::string& aLine);
    void        flashOperation(const std::string& aLine);

    void        updateView();

    CPlotterDC*         mDC;
    ApertureDefine*     mCurentApperture;
    FLOAT               mScale;
    cmd_map             mCommandMap;
    PointF              mCurrentCoordinate;
    aperture_define_map mApertureDefineMap;
    aperture_macro_map  mApertureMacroMap;
    CoordinateFormat    mFormatSpec;
    Command             mInterpolation;
    Command             mQuadrandMode;
    Command             mPolarity;
    Command             mRegionMode;
    PointF              mRegionPt;
    std::string         mApertureMacroName;
    unsigned long       mCombineID;
    bool                mAutoDetermineDrillpoints;
    int                 mCurrentLine;
    std::shared_ptr<ApertureDefine> mDeprecatedDefaultAperture;
};

#endif // GbrInterpreter_H
