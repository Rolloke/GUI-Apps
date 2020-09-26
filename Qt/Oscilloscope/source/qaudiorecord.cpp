#include "qaudiorecord.h"
#include "xml_functions.h"
#include "main.h"

#include "boost/bind.hpp"


#if (_USE_QT_AUDIO == 1)
#include <QAudio>
#include <QAudioDeviceInfo>
#include <qendian.h>
#else
#include "portaudio/include/portaudio.h"
#endif

#include "sse_helper.h"

#include <cmath>
#include <map>

namespace{

#if (_USE_QT_AUDIO == 1)
    template <class Type>
    void convertBuffer(size_t aStartPos, size_t aSize, std::uint32_t aStartChannel, std::uint32_t aChannels, const Channel* aCP, const void *aInputBuffer, std::vector< std::vector<double> >&aOutputBuffer)
    {
        static const double fTypeFactor = typeid(float) == typeid(Type) ? 1.0 : 1.0 / static_cast<float>( 1 << (sizeof(Type) * 8 - 1) );
        std::uint32_t fItems = aSize*aChannels;
        std::uint32_t fStopChannel = aStartChannel + aChannels;
        const unsigned char *fPtr = reinterpret_cast<const unsigned char *>(aInputBuffer);
        std::vector<double> fFactor;
        fFactor.resize(fStopChannel);
        for (std::uint32_t fC=aStartChannel; fC<fStopChannel; ++fC)
        {
            fFactor[fC] = aCP[fC].getAudioInputScale() * aCP[fC].getAudioScale();
        }
        for (std::uint32_t i=0; i<fItems; ++aStartPos)
        {
            for (std::uint32_t fC=aStartChannel; fC<fStopChannel; ++fC, ++i)
            {
                aOutputBuffer[fC][aStartPos] = (aCP[fC].getAudioInputOffset() + qFromLittleEndian<Type>(fPtr) * fTypeFactor) * fFactor[fC];
                fPtr += sizeof(Type);
            }
        }
    }
#else
    template <class Type>
    void convertBuffer(size_t aStartPos, size_t aSize, std::uint32_t aStartChannel, std::uint32_t aChannels, const Channel* aCP, const void *aInputBuffer, std::vector< std::vector<double> >&aOutputBuffer)
    {
        std::uint32_t fItems = aSize*aChannels;
        std::uint32_t fStopChannel = aStartChannel + aChannels;
        const Type*fBuffer = static_cast<const Type*>(aInputBuffer);
        std::vector<double> fFactor;
        fFactor.resize(fStopChannel);
        for (std::uint32_t fC=aStartChannel; fC<fStopChannel; ++fC)
        {
            fFactor[fC] = aCP[fC].getAudioInputScale() * aCP[fC].getAudioScale();
        }
        for (std::uint32_t i=0; i<fItems; ++aStartPos)
        {
            for (std::uint32_t fC=aStartChannel; fC<fStopChannel; ++fC, ++i)
            {
                aOutputBuffer[fC][aStartPos] = (aCP[fC].getAudioInputOffset() + static_cast<double>(fBuffer[i])) * fFactor[fC];
            }
        }
    }
#endif

#if (_USE_QT_AUDIO != 1)
    void convertBufferInt24(size_t aStartPos, size_t aSize, std::uint32_t aStartChannel, std::uint32_t aChannels, const Channel* aCP, const void *aInputBuffer, std::vector< std::vector<double> >&aOutputBuffer)
    {
        struct int24 {
            int32_t value:24;
        };
        int fItems       = aSize*aChannels;
        int fStopChannel = aStartChannel + aChannels;
        std::vector<double> fFactor;
        fFactor.resize(fStopChannel);
        for (int fC=aStartChannel; fC<fStopChannel; ++fC)
        {
            fFactor[fC] = aCP[fC].getAudioInputScale() * aCP[fC].getAudioScale();
        }
        const char*fBuffer = static_cast<const char*>(aInputBuffer);
        for (int i=0; i<fItems; ++aStartPos)
        {
            for (int fC=aStartChannel; fC<fStopChannel; ++fC, ++i)
            {
                aOutputBuffer[fC][aStartPos] = (aCP[fC].getAudioInputOffset() + reinterpret_cast<const int24*>(fBuffer)->value) * fFactor[fC];
                fBuffer += 3;
            }
        }
    }
#endif
}

QAudioRecord::eType getValue(const QDomNode& aValue, QAudioRecord::eType aDefault)
{
    QAudioRecord::eType fType = aDefault;
    if (aValue.isElement())
    {
        QString s = aValue.toElement().text();
        fType = QAudioRecord::getTypeFromString(s);
        if (fType == QAudioRecord::eInvalid) fType = aDefault;
    }
    return fType;
}

QAudioRecord::QAudioRecord(QObject *parent) : QThread(parent)
#if (_USE_QT_AUDIO == 1)
  , mType(eQtAudio)
  #ifdef __linux__
    , mSampleFormat(QAudioFormat::Float)
  #else
    , mSampleFormat(QAudioParameter::SignedInt16)
  #endif
  , mTempBufferPos(0)
#else
  , mType(ePortAudio)
  , mSampleFormat(paFloat32)
#endif
  , mStarting(false)
  , mChannels(2)
  , mSampleFrequency(44100)
  , mDeviceID(0)
  , mSuggestedLatency(0)
  , mBufferSize(mSampleFrequency/10)
  , mBuffers(50)
  , mCurrentPosition(0)
  , mChannelParameter(0)
  , mBufferPoolSize(0)
  , mConvert(0)
  , mCallbackConvert(0)
  , mCallbackChannels(0)
  , mHold(false)
{
}

QAudioRecord::~QAudioRecord()
{
    stopDevice();
}

void QAudioRecord::stopDevice()
{
    eType fOld = mType;
    mType = eStopDevice;
    mBufferSemaphoreIM.release();
    resume();
    wait();

#if (_USE_QT_AUDIO == 1)
    if (mAudioInput)
    {
        mAudioInput->stop();
    }
    if (mAudioInputDevice)
    {
        mAudioInputDevice->close();
    }

    mAudioInput.clear();
    mAudioInputDevice.clear();
#endif
    mType = fOld;
}


int QAudioRecord::getBitsPerSample() const
{
    switch (mSampleFormat)
    {
#if (_USE_QT_AUDIO == 1)
    case QAudioFormat::Float:            return 32; break;
    case QAudioFormat::SignedInt:        return 8; break;
    case QAudioFormat::UnSignedInt:      return 8; break;
    case QAudioParameter::UnSignedInt16: return 16; break;
    case QAudioParameter::SignedInt16:   return 16; break;
    case QAudioParameter::UnSignedInt32: return 32; break;
    case QAudioParameter::SignedInt32:   return 32; break;
    default:                             return  0;
#else
    case paFloat32:      return 32; break;
    case paInt32:        return 32; break;
    case paInt24:        return 24; break;
    case paInt16:        return 16; break;
    case paInt8:         return 8; break;
    case paUInt8:        return 8; break;
    case paCustomFormat: return 0; break;
#endif
    }
    return 0;
}
/**
 * @brief sets Sample Parameter for the audio input
 * @param aSampleFrequency sample frequency of the input channel
 * @param aBufferLength length of the buffer in seconds
 * @param aSampleLength length of a recorded audio sample
 */
void QAudioRecord::setSampleParameter(double aSampleFrequency, double aBufferLength, double aSampleLength)
{
    mSampleFrequency = aSampleFrequency;
    mBuffers = static_cast<std::uint32_t>(aBufferLength / aSampleLength + 0.5);
    mBufferSize = static_cast<std::uint32_t>(mSampleFrequency * aSampleLength);
    TRACE(Logger::info, "setSampleParameter(%f, %f, %f\n",aSampleFrequency, aBufferLength, aSampleLength);
    TRACE(Logger::info, "result: %f, %d, %d\n", mSampleFrequency, mBuffers, mBufferSize);

}

void QAudioRecord::setType(eType aType)
{
    mType = aType;
}

const char* QAudioRecord::getStringFromType(QAudioRecord::eType aType)
{
    switch (aType)
    {
    case eStopDevice:       return "Stop";
    case eSimulation: return "Simulation";
#if (_USE_QT_AUDIO == 1)
    case eQtAudio:    return "QtAudio";
#else
    case ePortAudio:  return "PortAudio";
#endif
    default: case eInvalid: return "Invalid";
    }
}

const QAudioParameter& QAudioRecord::getAudioParameters()
{
    mAudioParameter.init(mDeviceID, mChannels);
    return mAudioParameter;
}

void QAudioRecord::clearLastError()
{
    mLastError.clear();
}

QAudioRecord::eType QAudioRecord::getTypeFromString(QString& aName)
{
    static std::map<QString, eType> typemap;
    if (typemap.empty())
    {
        for (int i=eInvalid; i<eLast; ++i)
        {
            typemap[getStringFromType(static_cast<eType>(i))] = static_cast<eType>(i);
        }
    }
    std::map<QString, eType>::iterator it = typemap.find(aName);
    if (it != typemap.end())
    {
        return it->second;
    }
    return eInvalid;
}

void QAudioRecord::setBufferSize(int aSize)
{
    mBufferSize = aSize;
}

void QAudioRecord::setBuffers(int aBuffers)
{
    mBuffers = aBuffers;
}

void QAudioRecord::setChannels(int aChannels)
{
    mChannels = aChannels;
}

void QAudioRecord::setSampleFrequency(double aSF)
{
    mSampleFrequency = aSF;
}

void QAudioRecord::setSampleFormat(unsigned long aSF)
{
    mSampleFormat = aSF;
}

void QAudioRecord::setSuggestedLatency(double aSL)
{
    mSuggestedLatency = aSL;
}

void QAudioRecord::suspend()
{
    mHold = true;
    TRACE(Logger::notice, "Audio input suspended");
#if (_USE_QT_AUDIO == 1)
    mAudioInput->stop();
#endif
}

void QAudioRecord::resume()
{
    if (mHold)
    {
        mHold = false;
        mHoldSeamphore.release();
        TRACE(Logger::notice, "Audio input resumed");
#if (_USE_QT_AUDIO == 1)
        mAudioInput->start(mAudioInputDevice.data());
#endif
    }
}

void QAudioRecord::hold()
{
    if (mHold)
    {
        mHoldSeamphore.acquire();
    }
}


#if (_USE_QT_AUDIO == 1)

void QAudioRecord::start(Priority aPriority)
{
    initQtAudio();
    QThread::start(aPriority);
}

void QAudioRecord::run ()
{
    switch (mType)
    {
    case eSimulation: runSimulation(); break;
    case eQtAudio:
        runQtAudio();
        break;
    case eStopDevice: break;
    default : break;
    }
}

void QAudioRecord::initQtAudio()
{
    mCurrentPosition = 0;
    mStarting = true;

    QMutexLocker fLock(&mBufferMutex);
    mBuffer.clear();
    mBuffer.resize(mChannels+mCallbackChannels, std::vector<double>(mBufferSize*mBuffers, 0));
    int fSampleBytes = getBitsPerSample() * mChannels / 8;
    mTempBuffer.resize(mBufferSize * fSampleBytes);
    mTempBufferPos = 0;

    if (mBufferPoolSize)
    {
        mBufferPoolIM.initialize(mBufferPoolSize, mBufferSize * fSampleBytes);
    }
    fLock.unlock();

    if (mAudioParameter.getNumberOfDevices())
    {
        QAudioDeviceInfo fInfo = mAudioParameter.getDeviceInfo(mDeviceID);

        QAudioFormat fAudioFormat;
        fAudioFormat.setSampleRate(static_cast<int>(mSampleFrequency));
        fAudioFormat.setChannelCount(mChannels);
        fAudioFormat.setSampleType(static_cast<QAudioFormat::SampleType>(mSampleFormat & QAudioFormat::Float));
        QStringList fCodec = fInfo.supportedCodecs();
        if (fCodec.size())
        {
            fAudioFormat.setCodec(fCodec[0]);
        }

    #ifdef __AVX__
        bool bUseSSEFloat = ((mChannels & 1) == 0) && (mBufferSize % (sizeof(__m256)/sizeof(float))) == 0;
    #elif __SSE2__
        bool bUseSSEFloat = ((mChannels & 1) == 0) && (mBufferSize % (sizeof(__m128)/sizeof(float))) == 0;
    #else
        bool bUseSSEFloat = false;
    #endif
        bUseSSEFloat = false;
        switch (mSampleFormat)
        {
        case QAudioFormat::Float:
            if (bUseSSEFloat)
            {
    #ifdef __AVX__
                mConvert = &sse::convertBufferFloat256bit;
    #elif __SSE2__
                mConvert = &sse::convertBufferFloat128bit;
    #endif
            }
            else
            {
                mConvert = &convertBuffer<float>;
            }
            fAudioFormat.setSampleSize(32);
            break;
        case QAudioFormat::SignedInt:
            fAudioFormat.setSampleSize(8);
            mConvert = &convertBuffer<qint8>;
            break;
        case QAudioParameter::SignedInt16:
            fAudioFormat.setSampleSize(16);
            mConvert = &convertBuffer<qint16>;
            break;
        case QAudioParameter::SignedInt32:
            fAudioFormat.setSampleSize(32);
            mConvert = &convertBuffer<qint32>;
            break;
        case QAudioFormat::UnSignedInt:
            fAudioFormat.setSampleSize(8);
            mConvert = &convertBuffer<quint8>;
            break;
        case QAudioParameter::UnSignedInt16:
            fAudioFormat.setSampleSize(16);
            mConvert = &convertBuffer<quint16>;
            break;
        case QAudioParameter::UnSignedInt32:
            fAudioFormat.setSampleSize(32);
            mConvert = &convertBuffer<quint32>;
            break;
        }

        if (!fInfo.isFormatSupported(fAudioFormat))
        {
            TRACE(Logger::notice, "Unsupported format: %d Hz, %d Bits, %s, %d c, %s, %d", fAudioFormat.sampleRate(), fAudioFormat.sampleSize()
                  , QAudioParameter::getStringFromSampleType(fAudioFormat.sampleType()).toStdString().c_str()
                  , fAudioFormat.channelCount(), fAudioFormat.codec().toStdString().c_str(), fAudioFormat.byteOrder());
            fAudioFormat = fInfo.nearestFormat(fAudioFormat);
            TRACE(Logger::warning, "Changing Audio Format to nearest supported");
        }
        TRACE(Logger::notice, "Input audio format: %d Hz, %d Bits, %s, %d c, %s, %d", fAudioFormat.sampleRate(), fAudioFormat.sampleSize()
              , QAudioParameter::getStringFromSampleType(fAudioFormat.sampleType()).toStdString().c_str()
              , fAudioFormat.channelCount(), fAudioFormat.codec().toStdString().c_str(), fAudioFormat.byteOrder());

        mAudioInputDevice = new DirectIODevice(this);
        mAudioInput = new QAudioInput(fInfo, fAudioFormat);

        if (mAudioInput->error() == QAudio::NoError)
        {
            QObject::connect(mAudioInputDevice.data(), SIGNAL(write_data(const char*,qint64,qint64&)), this, SLOT(on_write_audio_data(const char*,qint64,qint64&)));

            int fInfterval_ms = static_cast<int>(0.5 + mBufferSize * 1000 / mSampleFrequency);
            mAudioInput->setNotifyInterval(fInfterval_ms);

            if (mAudioInputDevice->open(QIODevice::WriteOnly))
            {
                mAudioInput->start(mAudioInputDevice.data());
                TRACE(Logger::notice, "Audio input started");
            }
        }
    }
}

void QAudioRecord::runQtAudio()
{
    while ( mAudioInput &&  mType != eStopDevice)// && fError == QAudio::NoError)
    {
        processIntermediateBuffer();

        switch (mAudioInput->error())
        {
        case QAudio::NoError:
            if (mStarting)
            {
                mLastError = "Audio Input Device Initialized";
                mStarting = false;
            }
            break;
        case QAudio::OpenError:
            mLastError = "Could not open Audio Input Device";
            mType = eStopDevice;
            break;
        case QAudio::IOError:
            mLastError = "IO-Error occurred in Audio Input Device";
            mType = eStopDevice;
            break;
        case QAudio::UnderrunError:
            mLastError = "Underrun-Error occurred in Audio Input Device";
            break;
        case QAudio::FatalError:
            mLastError = "Fatal-Error occurred in Audio Input Device";
            mType = eStopDevice;
            break;
        default:
            mLastError = "Unknown-Error occurred in Audio Input Device";
            TRACE(Logger::warning, mLastError.toStdString().c_str());
            break;
        }
        if (mType == eStopDevice && mLastError.size())
        {
            TRACE(Logger::error, mLastError.toStdString().c_str());
        }
    };

    if (!mStarting)
    {
        mLastError = "Audio Input Device Closed properly";
        TRACE(Logger::error, mLastError.toStdString().c_str());
    }
}

void QAudioRecord::on_write_audio_data(const char* aData, qint64 aBytes, qint64& aBytesWritten)
{
    if (mHold)
    {
        aBytesWritten = aBytes;
    }
    else
    {
        if(aBytes == static_cast<qint64>(mTempBuffer.size()))
        {
            convertQAudioSamples(aData, aBytes);
            aBytesWritten = aBytes;
        }
        else
        {
            qint64 fSize = mTempBuffer.size() - mTempBufferPos;

            aBytesWritten = std::min(fSize, aBytes);

            memcpy(&mTempBuffer[mTempBufferPos], aData, aBytesWritten);
            mTempBufferPos += aBytesWritten;
            if (mTempBufferPos == mTempBuffer.size())
            {
                convertQAudioSamples(&mTempBuffer[0], mBufferSize);
                mTempBufferPos = 0;
                Logger::printDebug(Logger::debug, "delivering full buffer, rest %d, written %d, total %d", fSize, aBytesWritten, aBytes);
            }
        }
    }
}

#else

void QAudioRecord::start(Priority aPriority)
{
    QThread::start(aPriority);
}

void QAudioRecord::run ()
{
    mCurrentPosition = 0;
    mStarting = true;

    QMutexLocker fLock(&mBufferMutex);
    mBuffer.clear();
    mBuffer.resize(mChannels+mCallbackChannels, std::vector<double>(mBufferSize*mBuffers, 0));
    if (mBufferPoolSize)
    {
        mBufferPoolIM.initialize(mBufferPoolSize, mBufferSize*getBitsPerSample()*mChannels/8);
    }
    fLock.unlock();

    switch (mType)
    {
    case eSimulation: runSimulation(); break;
    case ePortAudio:  runPortAudio();  break;
    case eStopDevice: break;
    default : break;
    }
}

void QAudioRecord::runPortAudio()
{
    PaError fError = Pa_Initialize( );

    PaStream *fAudioStream = 0;

    if (fError == paNoError)
    {
        if (mDeviceID == paNoDevice)
        {
            mDeviceID = Pa_GetDefaultHostApi ();
        }

        getAudioParameters(); // initialize, if not already done

        if (mSuggestedLatency == 0)
        {
            mSuggestedLatency = getAudioParameters().getDefaultLowInputLatency();
        }

        PaStreamParameters  fInputParameters;
        fInputParameters.channelCount     = mChannels;
        fInputParameters.device           = mDeviceID;
        fInputParameters.suggestedLatency = mSuggestedLatency;
        fInputParameters.sampleFormat     = mSampleFormat;
        fInputParameters.hostApiSpecificStreamInfo = NULL;

        if (Pa_IsFormatSupported(&fInputParameters, NULL, mSampleFrequency) != paFormatIsSupported)
        {
            mSampleFrequency = getAudioParameters().getDefaultSampleRate();
        }
#ifdef __AVX__
        bool bUseSSEFloat = ((mChannels & 1) == 0) && (mBufferSize % (sizeof(__m256)/sizeof(float))) == 0;
#elif __SSE2__
        bool bUseSSEFloat = ((mChannels & 1) == 0) && (mBufferSize % (sizeof(__m128)/sizeof(float))) == 0;
#else
        bool bUseSSEFloat = false;
#endif
        switch (mSampleFormat)
        {
        case paFloat32:
            if (bUseSSEFloat)
            {
#ifdef __AVX__
                mConvert = &convertBufferSSEFloat256bit;
#elif __SSE2__
                mConvert = &sse::convertBufferFloat128bit;
#endif
            }
            else
            {
                mConvert = &convertBuffer<float>;
            }   break;
        case paInt32:   mConvert = &convertBuffer<int32_t>; break;
        case paInt24:   mConvert = &convertBufferInt24;     break;
        case paInt16:   mConvert = &convertBuffer<int16_t>; break;
        case paInt8:    mConvert = &convertBuffer<int8_t>;  break;
        case paUInt8:   mConvert = &convertBuffer<uint8_t>; break;
        }

        unsigned long  framesPerBuffer = mBufferSize;
        // we won't output out of range samples so don't bother clipping them
        // paNoFlag, paClipOff, paDitherOff, paPlatformSpecificFlags, paNeverDropInput
        PaStreamFlags  fStreamFlags = paClipOff|paDitherOff;
        fError = Pa_OpenStream( &fAudioStream, &fInputParameters,
                  NULL,                  // no outputParameters
                  mSampleFrequency, framesPerBuffer, fStreamFlags,
                  QAudioRecord::PortAudioCallback, static_cast<void*>(this));

        if (fError == paNoError)
        {
            fError = Pa_StartStream (fAudioStream);
        }
        TRACE(Logger::notice, "Audio input stream opened\n");

        if (fError == paNoError)
        {
            mLastError = "Audio Input Device Initialized: ";
            mLastError += Pa_GetErrorText(fError);
            mStarting = false;
            while ( mType != eStopDevice )
            {
                processIntermediateBuffer();
            };

            if (fAudioStream)
            {
                Pa_AbortStream (fAudioStream);
                TRACE(Logger::notice, "Audio input stream aborted\n");
                Pa_CloseStream(fAudioStream);
                TRACE(Logger::notice, "Audio input stream closed\n");
            }
            QThread::msleep(100);
        }
    }
    mStarting = false;

    Pa_Terminate();
    TRACE(Logger::notice, "Audio input device terminated\n");

    if (mType != eStopDevice)
    {
        mType = eStopDevice;
        mLastError = "Audio Device Error: ";
        mLastError += Pa_GetErrorText(fError);
    }
}

void QAudioRecord::on_write_audio_data(const char*, qint64, qint64& )
{
}

#endif


bool QAudioRecord::isValid()
{
    return mBuffer.size() != 0 && mType != eStopDevice && mType != eInvalid;
}

void QAudioRecord::setBufferPoolSize(int aSize)
{
    mBufferPoolSize = aSize;
}

AudioCallback  QAudioRecord::getAudioCallback()
{
#if (_USE_QT_AUDIO == 1)
   return boost::bind(&QAudioRecord::convertQAudioSamples, this, _1, _2);
#else
    return boost::bind(&QAudioRecord::PortAudioCallbackM, this, _1, _2, _3, _4, _5);
#endif
}

void QAudioRecord::setCallbackChannels(int aCannels)
{
    mCallbackChannels = aCannels;
    mCallbackConvert  = &convertBuffer<float>;
}

#if (_USE_QT_AUDIO == 1)
int QAudioRecord::convertQAudioSamples( const void *aInputBuffer, unsigned long aFramesPerBuffer)
{
    int fFinished(0);
    void *aOutputBuffer = 0;
#else
int QAudioRecord::PortAudioCallback( const void *aInputBuffer, void * /* aOutputBuffer */, unsigned long aFramesPerBuffer,
                           const PaStreamCallbackTimeInfo* aTimeInfo, PaStreamCallbackFlags aStatusFlags, void *aUserData )
{
    QAudioRecord* pThis = static_cast<QAudioRecord*>(aUserData);
    return pThis->PortAudioCallbackM(aInputBuffer, NULL, aFramesPerBuffer, aTimeInfo, aStatusFlags);
}


int QAudioRecord::PortAudioCallbackM( const void *aInputBuffer, void* aOutputBuffer, unsigned long  aFramesPerBuffer,
                                     const PaStreamCallbackTimeInfo* /* timeInfo */, PaStreamCallbackFlags /* statusFlags */)
{
    int fFinished(paContinue);
#endif
    if (mType != eStopDevice)
    {
        if (aFramesPerBuffer != mBufferSize)
        {
            TRACE(Logger::warning, "callback: %d != %d\n", aFramesPerBuffer, mBufferSize);
        }
        if (mBufferPoolIM.isInitialized())
        {
            void* fBuffer = mBufferPoolIM.getBuffer();
            if (fBuffer)
            {
                memcpy(fBuffer, aInputBuffer, mBufferPoolIM.getBufferSize());
                mBufferMutexIM.lock();
                mBufferListIM.push_back(fBuffer);
                mBufferMutexIM.unlock();
                mBufferSemaphoreIM.release();
                TRACE(Logger::debug, "free pool buffers: %d of %d", mBufferPoolIM.getFreePoolItems(), mBufferListIM.count());
            }
            else
            {
                mBufferMutexIM.lock();
                mBufferPoolIM.releaseBuffer(mBufferListIM.front());
                mBufferListIM.pop_front();
                mBufferMutexIM.unlock();
                TRACE(Logger::warning, "no more pool buffers: %d, %d", mBufferPoolIM.getFreePoolItems(), mBufferListIM.count());
            }
        }
        else
        {
            convertIntermediateBuffer(aInputBuffer, aOutputBuffer);
        }
        hold();
    }
    else
    {
#if (_USE_QT_AUDIO != 1)
        fFinished = paAbort;
#endif
    }
    return fFinished;
}

void QAudioRecord::processIntermediateBuffer()
{
    if (mBufferPoolIM.isInitialized())
    {
        mBufferSemaphoreIM.acquire();
        QMutexLocker fLock(&mBufferMutexIM);
        if (mBufferListIM.count())
        {
            void * fBuffer = mBufferListIM.front();
            mBufferListIM.pop_front();
            fLock.unlock();
            convertIntermediateBuffer(fBuffer, NULL);
            mBufferPoolIM.releaseBuffer(fBuffer);
        }
        else
        {
            TRACE(Logger::warning, "no buffer in list: %d", mBufferListIM.count());
        }
    }
    else
    {
        QThread::msleep(5);
    }
}

void QAudioRecord::convertIntermediateBuffer(const void *aInputBuffer, void* aOutputBuffer)
{
    QMutexLocker fLock(&mBufferMutex);

    TRACE(Logger::debug, "converting buffer");

    if (mType == eSimulation)
    {
        if (mCurrentPosition >= mBuffer[0].size())
        {
            mCurrentPosition = 0;
        }
        mConvert(mCurrentPosition, mBufferSize, 0, mChannels, mChannelParameter, aOutputBuffer, mBuffer);
        mPostBufferChanged(mCurrentPosition);
        mCurrentPosition += mBufferSize;
        fLock.unlock();
    }
    else if (aInputBuffer)
    {
        if (mCurrentPosition >= mBuffer[0].size())
        {
            mCurrentPosition = 0;
        }
        mConvert(mCurrentPosition, mBufferSize, 0, mChannels, mChannelParameter, aInputBuffer, mBuffer);
        mPostBufferChanged(mCurrentPosition);
        mCurrentPosition += mBufferSize;
        fLock.unlock();
    }
    else if (aOutputBuffer)
    {
        if (mHold)
        {
            if (mCurrentPosition >= mBuffer[0].size())
            {
                mCurrentPosition = 0;
            }
        }

        mCallbackConvert(mCurrentPosition, mBufferSize, mChannels, mCallbackChannels, mChannelParameter, aOutputBuffer, mBuffer);

        if (mHold)
        {
            mPostBufferChanged(mCurrentPosition);
            mCurrentPosition += mBufferSize;
        }
    }
}

void QAudioRecord::runSimulation()
{
    mConvert = &convertBuffer<float>;
    mStarting = false;
    while ( mType != eStopDevice )
    {
        sleep(1);
    }
}




