#ifndef QAUDIOPARAMETER_H
#define QAUDIOPARAMETER_H

#include "main.h"

#include <QString>


#if (_USE_QT_AUDIO == 1)

#include <QList>
#include <QAudioDeviceInfo>
#include <QAudioFormat>

#else

#include <QMutex>
#include <QString>
#include <vector>
#include <map>

struct PaDeviceInfo;
class QAudioParameter;
typedef std::map<int, QAudioParameter*> IdToParameterMap;

class QAudioInit
{
public:
    QAudioInit() ;
    ~QAudioInit();

    bool isValid();
private:
    int mError;
};

#endif

class QAudioParameter
{
public:
#if (_USE_QT_AUDIO == 1)
    enum AdditionalAudioFormats { SignedInt16 = QAudioFormat::SignedInt|0x04, SignedInt32 = QAudioFormat::SignedInt|0x08,
                                  UnSignedInt16 = QAudioFormat::UnSignedInt|0x04, UnSignedInt32 = QAudioFormat::UnSignedInt|0x08};
    QAudioParameter(bool bInput=true);
#else
    QAudioParameter();
#endif
    bool init(int aDeviceID, int aChannels);

    int getNumberOfDevices() const;

    QString getName() const;

    int getMaxInputChannels() const;
    int getMaxOutputChannels() const;
    int getMaxChannels() const;

    double getDefaultLowInputLatency() const;
    double getDefaultLowOutputLatency() const;
    double getDefaultHighInputLatency() const;
    double getDefaultHighOutputLatency() const;

    double getDefaultSampleRate() const;
    int    currentDeviceID() const  { return mDeviceID; }


#if (_USE_QT_AUDIO == 1)
    const QAudioDeviceInfo& getDeviceInfo(int aIndex) const { return mDevicesList[aIndex]; }
    QAudioParameter& operator= (const QAudioParameter& other)
    {
        mDevicesList = other.mDevicesList;
        mDeviceID = other.mDeviceID;
        mIsInput = other.mIsInput;
        return *this;
    }

    std::vector<double> getSupportedSampleFrequencies() const;
    QString getSupportedSampleFormatStrings() const;
    bool isInput() { return mIsInput; }
    static QString getStringFromSampleType(unsigned long aType);
private:
    QList<QAudioDeviceInfo> mDevicesList;
    int                     mDeviceID;
    bool                    mIsInput;
#else
    QString getSupportedSampleFormatStrings() const;
    unsigned long   getSupportedSampleFormats() const { return mSampleFormats; }
    std::vector<double> getSupportedSampleFrequencies() const { return mSupportedSampleFrequencies; }

    static const char* getStringFromPaType(unsigned long aType);
    static unsigned long getPaTypeFromString(QString& aName);

    static uint isFormatFloat();
    static uint isFormatInt32();
    static uint isFormatInt24();
    static uint isFormatInt16();
    static uint isFormatInt8();
    static uint isFormatUInt8();
    static uint isFormatCustom();
private:
    const PaDeviceInfo* mDevInfo;
    int                 mNumInputDevices;
    int                 mDeviceID;
    std::vector<double> mSupportedSampleFrequencies;
    unsigned long       mSampleFormats;
    static QMutex           mInstanceMutex;
    static IdToParameterMap mIdInstances;
#endif


};

#endif // QAUDIOPARAMETER_H
