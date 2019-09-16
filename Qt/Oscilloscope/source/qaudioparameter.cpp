#include "qaudioparameter.h"
#include "main.h"

#include <QMutexLocker>

#include <map>
#include <boost/lexical_cast.hpp>


#if (_USE_QT_AUDIO == 1)

QAudioParameter::QAudioParameter(bool bInput):
   mDeviceID(-1), mIsInput(bInput)
{
     mDevicesList = QAudioDeviceInfo::availableDevices(bInput ? QAudio::AudioInput : QAudio::AudioOutput);
}

bool QAudioParameter::init(int aDeviceID, int )
{
    if (aDeviceID < getNumberOfDevices())
    {
        mDeviceID = aDeviceID;
        return true;
    }
    return false;
}

int QAudioParameter::getNumberOfDevices()  const
{
    return mDevicesList.size();
}

int QAudioParameter::getMaxInputChannels() const
{
    if (mDeviceID != -1 && mIsInput)
    {
        QList<int> fChannels = mDevicesList[mDeviceID].supportedChannelCounts();
        int fSize = fChannels.size();
        return fSize > 0 ? fChannels[fSize-1] : 0;
    }
    return 0;
}

int QAudioParameter::getMaxChannels() const
{
    if (mDeviceID != -1)
    {
        QList<int> fChannels = mDevicesList[mDeviceID].supportedChannelCounts();
        int fSize = fChannels.size();
        return fSize > 0 ? fChannels[fSize-1] : 0;
    }
    return 0;
}

std::vector<double> QAudioParameter::getSupportedSampleFrequencies() const
{
    std::vector<double> fdSF;
    if (mDeviceID != -1)
    {
        QList<int> fSF = mDevicesList[mDeviceID].supportedSampleRates();
        for (int fS : fSF)
        {
            fdSF.push_back(fS);
        }
    }
    return fdSF;
}

int QAudioParameter::getMaxOutputChannels() const
{
    if (mDeviceID != -1 && !mIsInput)
    {
        QList<int> fChannels = mDevicesList[mDeviceID].supportedChannelCounts();
        int fSize = fChannels.size();
        return fSize > 0 ? fChannels[fSize-1] : 0;
    }
    return  0;
}


double QAudioParameter::getDefaultLowInputLatency() const
{
    return  0;
}

double QAudioParameter::getDefaultLowOutputLatency() const
{
    return 0;
}

double QAudioParameter::getDefaultHighInputLatency() const
{
    return  0;
}

double QAudioParameter::getDefaultHighOutputLatency() const
{
    return  0;
}

double QAudioParameter::getDefaultSampleRate() const
{
    if (mDeviceID != -1)
    {
        return mDevicesList[mDeviceID].preferredFormat().sampleRate();
    }
    return  44100;
}

QString QAudioParameter::getName() const
{
    if (mDeviceID != -1)
    {
        return mDevicesList[mDeviceID].deviceName();
    }
    return  "";
}

QString QAudioParameter::getSupportedSampleFormatStrings() const
{
    return   getStringFromSampleType(QAudioFormat::Float)
    + ", " + getStringFromSampleType(QAudioFormat::SignedInt)
    + ", " + getStringFromSampleType(QAudioFormat::UnSignedInt)
    + ", " + getStringFromSampleType(QAudioParameter::SignedInt16)
    + ", " + getStringFromSampleType(QAudioParameter::UnSignedInt16)
    + ", " + getStringFromSampleType(QAudioParameter::SignedInt32)
    + ", " + getStringFromSampleType(QAudioParameter::UnSignedInt32);
}

QString QAudioParameter::getStringFromSampleType(unsigned long aType)
{
    switch (aType)
    {

    case QAudioFormat::Float:            return "Float";
    case QAudioFormat::SignedInt:        return "Signed 8";
    case QAudioFormat::UnSignedInt:      return "Unsigned 8";
    case QAudioParameter::SignedInt16:   return "Signed 16";
    case QAudioParameter::UnSignedInt16: return "Unsigned 16";
    case QAudioParameter::SignedInt32:   return "Signed 32";
    case QAudioParameter::UnSignedInt32: return "Unsigned 32";
    default: return "Unknown";
    }
}

#else

#include "portaudio/include/portaudio.h"

IdToParameterMap QAudioParameter::mIdInstances;
QMutex QAudioParameter::mInstanceMutex;

QAudioInit::QAudioInit()
{
    mError = Pa_Initialize( );
}

QAudioInit::~QAudioInit()
{
    if (isValid())
    {
        Pa_Terminate();
    }
}

bool QAudioInit::isValid()
{
    return mError == paNoError;
}

QAudioParameter::QAudioParameter():
    mDevInfo(0)
  , mNumInputDevices(0)
  , mDeviceID(-1)
  , mSampleFormats(0)
{
}

bool QAudioParameter::init(int aDeviceID, int aChannels)
{
    if (aDeviceID != mDeviceID)
    {
        QMutexLocker fLock(&mInstanceMutex);

        mNumInputDevices = Pa_GetHostApiCount();
        mDevInfo         = Pa_GetDeviceInfo ( aDeviceID );
        mDeviceID        = aDeviceID;
        mSampleFormats   = 0;
        IdToParameterMap::iterator fIt = mIdInstances.find(mDeviceID);
        if (fIt != mIdInstances.end())
        {
            mSupportedSampleFrequencies = fIt->second->mSupportedSampleFrequencies;
            mSampleFormats              = fIt->second->mSampleFormats;
        }
        else
        {
            mIdInstances[mDeviceID] = this;
            PaStreamParameters  fInputParameters;
            fInputParameters.channelCount     = aChannels;
            fInputParameters.device           = mDeviceID;
            fInputParameters.suggestedLatency = getDefaultLowInputLatency();
            fInputParameters.hostApiSpecificStreamInfo = NULL;

            const double fDefaultSampleRate   = getDefaultSampleRate();
            for (unsigned int mask=1; (mask & 0x3f) != 0; mask <<= 1)
            {
                fInputParameters.sampleFormat = mask;
                if (Pa_IsFormatSupported(&fInputParameters, NULL, fDefaultSampleRate) == paFormatIsSupported)
                {
                    mSampleFormats |= mask;
                }
            }

            fInputParameters.sampleFormat = paFloat32;
            mSupportedSampleFrequencies.clear();
            const double fSampleFrq[] = { 4000, 8000, 11025, 22050, 24000, 32000, 40000, 44100, 48000, 56000, 64000, 72000, 80000, 88200, 96000, 192000 };
            for (double fFS: fSampleFrq)
            {
                if (Pa_IsFormatSupported(&fInputParameters, NULL, fFS) == paFormatIsSupported)
                {
                    mSupportedSampleFrequencies.push_back(fFS);
                }
            }
        }
        fLock.unlock();
    }
    return mDevInfo != 0;
}

int QAudioParameter::getNumberOfDevices()  const
{
    return mNumInputDevices;
}

int QAudioParameter::getMaxInputChannels() const
{
    return mDevInfo != 0 ? mDevInfo->maxInputChannels : 2;
}

int QAudioParameter::getMaxOutputChannels() const
{
    return mDevInfo != 0 ? mDevInfo->maxOutputChannels: 2;
}

int QAudioParameter::getMaxChannels() const
{
    return mDevInfo != 0 ? std::max(mDevInfo->maxOutputChannels, mDevInfo->maxInputChannels) : 2;
}

double QAudioParameter::getDefaultLowInputLatency() const
{
    return mDevInfo != 0 ? mDevInfo->defaultLowInputLatency : 0;
}

double QAudioParameter::getDefaultLowOutputLatency() const
{
    return mDevInfo != 0 ? mDevInfo->defaultLowOutputLatency : 0;
}

double QAudioParameter::getDefaultHighInputLatency() const
{
    return mDevInfo != 0 ? mDevInfo->defaultHighInputLatency : 0;
}

double QAudioParameter::getDefaultHighOutputLatency() const
{
    return mDevInfo != 0 ? mDevInfo->defaultHighOutputLatency : 0;
}

double QAudioParameter::getDefaultSampleRate() const
{
    return mDevInfo != 0 ? mDevInfo->defaultSampleRate : 44100;
}

QString QAudioParameter::getName() const
{
    return mDevInfo != 0 ? mDevInfo->name : "";
}

const char* QAudioParameter::getStringFromPaType(unsigned long aType)
{
    switch (aType)
    {
    case paFloat32:      return "Float32";
    case paInt16:        return "Int16";
    case paInt32:        return "Int32";
    case paInt24:        return "Int24";
    case paInt8:         return "Int8";
    case paUInt8:        return "UInt8";
    case paCustomFormat: return "CustomFormat";
    }
    return "";
}

unsigned long QAudioParameter::getPaTypeFromString(QString& aName)
{
    static std::map<QString, PaSampleFormat> typemap;
    if (typemap.empty())
    {
        size_t n = sizeof(PaSampleFormat) * 4;
        for (unsigned long i=0, mask=1; i<n; ++i, mask <<= 1)
        {
            typemap[getStringFromPaType(mask)] = static_cast<PaSampleFormat>(mask);
        }
    }
    std::map<QString, PaSampleFormat>::iterator it = typemap.find(aName);
    if (it != typemap.end())
    {
        return it->second;
    }
    return 0;
}

uint QAudioParameter::isFormatFloat()
{
    return paFloat32;
}
uint QAudioParameter::isFormatInt32()
{
    return paInt32;
}
uint QAudioParameter::isFormatInt24()
{
    return paInt24;
}
uint QAudioParameter::isFormatInt16()
{
    return paInt16;
}
uint QAudioParameter::isFormatInt8()
{
    return paInt8;
}
uint QAudioParameter::isFormatUInt8()
{
    return paUInt8;
}
uint QAudioParameter::isFormatCustom()
{
    return paCustomFormat;
}

QString QAudioParameter::getSupportedSampleFormatStrings() const
{
    QString  fName;
    unsigned long fSF = getSupportedSampleFormats();
    for (unsigned long mask = 1; mask != 0; mask <<= 1 )
    {
        if (fSF & mask)
        {
            fName += QAudioParameter::getStringFromPaType(mask);
            fName += ": ";
            fName += boost::lexical_cast<std::string, int>(mask).c_str();
            fName += ", ";
        }
    }
    return fName;
}

#endif
