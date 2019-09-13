#ifndef PINTYPE_H
#define PINTYPE_H

#include <QString>
#include <QStringList>

class PinType
{
public:
    PinType();
    PinType(const QString& aName, int aNo, bool aInput, bool aOutput, bool aAnalog, bool aPWM, bool aTone, bool aBinary, bool aInterrupt);

    const QString& getName() const;
    int            getPinNo() const;
    bool           isInput() const;
    bool           isOutput() const;
    bool           isAnalog() const;
    bool           isPWM() const;
    bool           isTone() const;
    bool           isBinary() const;
    bool           isInterrupt() const;
    const QStringList& getTypes(bool aOutput) const;

    //template <class STREAM>
    friend std::ostream& operator << (std::ostream& os, const PinType& aPin)
    {
        QString fName = aPin.mName;
        fName.replace(" ", "--");
        os << aPin.mPinNumber << " " << aPin.mInputPin << " " << aPin.mOutputPin << " ";
        os << aPin.mAnalog << " " << aPin.mPWM << " " << aPin.mTone << " " << aPin.mBinary << " ";
        os << aPin.mInterrupt << " " << fName.toStdString();
        return os;
    }
    friend std::istream& operator >> (std::istream& os, PinType& aPin)
    {
        os >> aPin.mPinNumber;
        os >> aPin.mInputPin;
        os >> aPin.mOutputPin;
        os >> aPin.mAnalog;
        os >> aPin.mPWM;
        os >> aPin.mTone;
        os >> aPin.mBinary;
        os >> aPin.mInterrupt;
        std::string fName;
        os >> fName;
        aPin.mName = fName.c_str();
        aPin.mName.replace("--", " ");
        aPin.init_types();
        return os;
    }
private:
    void init_types();

    QString mName;
    int  mPinNumber;
    bool mInputPin;
    bool mOutputPin;
    bool mAnalog;
    bool mPWM;
    bool mTone;
    bool mBinary;
    bool mInterrupt;
    QStringList mInputTypes;
    QStringList mOutputTypes;
};

#endif // PINTYPE_H
