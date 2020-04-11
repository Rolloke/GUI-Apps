#include "qaudiooutstream.h"

#if (_USE_QT_AUDIO == 1)

#include <QAudio>
#include <QAudioDeviceInfo>
#include <qendian.h>
#include <string.h>
#include <cmath>

namespace
{
    template <class Type>
    bool convertBuffer(std::vector<std::uint32_t>& aBufferIndex, const std::vector< std::vector<float> >& aBuffer, qint32& aCycles, char* aData, qint64 aBytes)
    {
        std::uint32_t fChannels = aBuffer.size();
        std::uint32_t i, fFramesPerBuffer = aBytes / sizeof(Type) / fChannels;
        unsigned char *fPtr = reinterpret_cast<unsigned char *>(aData);
        unsigned char *fEnd = reinterpret_cast<unsigned char *>(aData + aBytes);
        const float fFactor = static_cast<float>( 1 << (sizeof(Type) * 8 - 1) ) * 0.99f;
        for( i=0; i<fFramesPerBuffer; i++ )
        {
            for (std::uint32_t j=0; j<fChannels; ++j)
            {
                Type fValue = static_cast<Type>(round(aBuffer[j][aBufferIndex[j]] * fFactor));
                qToLittleEndian<Type>(fValue, fPtr);
                ++aBufferIndex[j];
                if (aBufferIndex[j] >= aBuffer[j].size())
                {
                    aBufferIndex[j] = 0;
                }
                fPtr += sizeof(Type);
            }
            if (aCycles != QAudioOutStream::Infinite)
            {
                if (--aCycles == 0)
                {
                    std::fill(fPtr, fEnd, 0);
                    return false;
                }
            }
        }
        return true;
    }
}

#else

#include "portaudio/include/portaudio.h"

#endif

#include "main.h"

#include <cmath>


QAudioOutStream::QAudioOutStream(QObject *parent) : QThread(parent)
  , mChannels(2)
  , mSampleFrequency(44100)
  , mCommand(eInvalid)
  , mBufferIndex(0)
  , mBufferCycles(Infinite)
  , mDeviceID(0)
#if (_USE_QT_AUDIO == 1)
#ifdef __linux__
  , mSampleFormat(QAudioFormat::Float)
#else
  , mSampleFormat(QAudioParameter::SignedInt16)
#endif
  , mAudioParameter(false)
#else
  , mSampleFormat(paFloat32)
#endif
  , mSuggestedLatency(0)
  , mFramesPerBuffer(64)
{
    setChannels(2);
}

QAudioOutStream::~QAudioOutStream()
{
    stopDevice();
}

void QAudioOutStream::stopDevice()
{
    mCommand = eStopThread;
    mCommandSeamphore.release();
    wait();
    mCommand = eInvalid;
}

void QAudioOutStream::setAudioCallback(const AudioCallback& aFunction)
{
    mAudioCallback = aFunction;
}

const QAudioParameter& QAudioOutStream::getAudioParameters()
{
    mAudioParameter.init(mDeviceID, mChannels);
    return mAudioParameter;
}


#if (_USE_QT_AUDIO == 1)

void QAudioOutStream::start(Priority aPriority)
{
    initQtAudio();
    QThread::start(aPriority);
}

void QAudioOutStream::run ()
{
    runQtAudio();
}

void QAudioOutStream::initQtAudio()
{
    const QAudioDeviceInfo& fInfo = mAudioParameter.getDeviceInfo(mDeviceID);

    QAudioFormat fAudioFormat;
    fAudioFormat.setSampleRate(static_cast<int>(mSampleFrequency));
    fAudioFormat.setChannelCount(mChannels);
    fAudioFormat.setSampleType(static_cast<QAudioFormat::SampleType>(mSampleFormat & QAudioFormat::Float));
    QStringList fCodec = fInfo.supportedCodecs();
    if (fCodec.size())
    {
        fAudioFormat.setCodec(fCodec[0]);
    }

    mAudioOutputDevice = new DirectIODevice();
    switch (mSampleFormat)
    {
    case QAudioFormat::Float:
        QObject::connect(mAudioOutputDevice.data(), SIGNAL(read_data(char*,qint64,qint64&)), this, SLOT(read_audio_dataFloat(char*,qint64,qint64&)));
        fAudioFormat.setSampleSize(32);
        break;
    case QAudioFormat::UnSignedInt:
        fAudioFormat.setSampleSize(8);
        mConvert = &convertBuffer<quint8>;
        QObject::connect(mAudioOutputDevice.data(), SIGNAL(read_data(char*,qint64,qint64&)), this, SLOT(read_audio_data(char*,qint64,qint64&)));
        break;
    case QAudioFormat::SignedInt:
        fAudioFormat.setSampleSize(8);
        mConvert = &convertBuffer<qint8>;
        QObject::connect(mAudioOutputDevice.data(), SIGNAL(read_data(char*,qint64,qint64&)), this, SLOT(read_audio_data(char*,qint64,qint64&)));
        break;
    case QAudioParameter::UnSignedInt16:
        fAudioFormat.setSampleSize(16);
        mConvert = &convertBuffer<quint16>;
        QObject::connect(mAudioOutputDevice.data(), SIGNAL(read_data(char*,qint64,qint64&)), this, SLOT(read_audio_data(char*,qint64,qint64&)));
        break;
    case QAudioParameter::SignedInt16:
        fAudioFormat.setSampleSize(16);
        mConvert = &convertBuffer<qint16>;
        QObject::connect(mAudioOutputDevice.data(), SIGNAL(read_data(char*,qint64,qint64&)), this, SLOT(read_audio_data(char*,qint64,qint64&)));
        break;
    case QAudioParameter::UnSignedInt32:
        fAudioFormat.setSampleSize(32);
        mConvert = &convertBuffer<quint32>;
        QObject::connect(mAudioOutputDevice.data(), SIGNAL(read_data(char*,qint64,qint64&)), this, SLOT(read_audio_data(char*,qint64,qint64&)));
        break;
    case QAudioParameter::SignedInt32:
        fAudioFormat.setSampleSize(32);
        mConvert = &convertBuffer<qint32>;
        QObject::connect(mAudioOutputDevice.data(), SIGNAL(read_data(char*,qint64,qint64&)), this, SLOT(read_audio_data(char*,qint64,qint64&)));
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
    TRACE(Logger::notice, "Output audio format: %d Hz, %d Bits, %s, %d c, %s, %d", fAudioFormat.sampleRate(), fAudioFormat.sampleSize()
          , QAudioParameter::getStringFromSampleType(fAudioFormat.sampleType()).toStdString().c_str()
          , fAudioFormat.channelCount(), fAudioFormat.codec().toStdString().c_str(), fAudioFormat.byteOrder());

    mAudioOutput = new QAudioOutput(fInfo, fAudioFormat);

    bool fOpened = mAudioOutputDevice->open(QIODevice::ReadOnly);
    if (fOpened)
    {
        mLastError = "Audio Output Device Initialized";
        mCommand   = eRun;
        mStarting  = false;
    }
}

void QAudioOutStream::runQtAudio()
{
    QAudio::Error fError = mAudioOutput->error();
    if (fError == QAudio::NoError)
    {
        while ( mCommand != eStopThread )
        {
            mCommandSeamphore.acquire();
            fError = mAudioOutput->error();
            switch (fError)
            {
            case QAudio::NoError:       break;
            case QAudio::OpenError:     mLastError = "Could not open Audio Input Device"; break;
            case QAudio::IOError:       mLastError = "IO-Error occurred in Audio Input Device"; break;
            case QAudio::UnderrunError: mLastError = "Underrun-Error occurred in Audio Input Device"; break;
            case QAudio::FatalError:    mLastError = "Fatal-Error occurred in Audio Input Device"; break;
            default:                    mLastError = "Unknown-Error occurred in Audio Input Device"; break;
            }
        };

        mAudioOutput->stop();

        mAudioOutputDevice->close();
        mAudioOutputDevice.clear();

        mAudioOutput.clear();
        mLastError = "Audio Input Device Closed properly";
    }
}

void QAudioOutStream::read_audio_dataFloat(char* aData, qint64 aBytes, qint64& aBytesRead)
{
    if (mBufferCycles == StopBurst)
    {
        aBytes = 0;
        do_start_stream(false, Infinite);
    }

    std::uint32_t i, fFramesPerBuffer = aBytes / sizeof(float) / mChannels;
    float *fOut = reinterpret_cast<float*>(aData);
    float *fEnd = reinterpret_cast<float*>(aData + aBytes);

    for( i=0; i<fFramesPerBuffer; i++ )
    {
        for (std::uint32_t j=0; j<mChannels; ++j)
        {
            *fOut++ = mBuffers[j][mBufferIndex[j]];  /* left */
            ++mBufferIndex[j];
            if (mBufferIndex[j] >= mBuffers[j].size())
            {
                mBufferIndex[j] = 0;
            }
        }
        if (mBufferCycles != Infinite)
        {
            if (--mBufferCycles == 0)
            {
                mBufferCycles = StopBurst;
                std::fill(fOut, fEnd, 0);
                break;
            }
        }
    }
    aBytesRead = aBytes;

    if (mAudioCallback)
    {
        mAudioCallback(reinterpret_cast<const void*>(aData), fFramesPerBuffer);
    }
}

void QAudioOutStream::read_audio_data(char* aData, qint64 aBytes, qint64& aBytesRead)
{
    if (mBufferCycles == StopBurst)
    {
        aBytes = 0;
        do_start_stream(false, Infinite);
    }

    bool fContinue = mConvert(mBufferIndex, mBuffers, mBufferCycles, aData, aBytes);
    aBytesRead = aBytes;
    if (!fContinue)
    {
        mBufferCycles = StopBurst;
    }

    if (mAudioCallback)
    {
        std::uint32_t fFramesPerBuffer = aBytes / sizeof(float) / mChannels;
        mAudioCallback(reinterpret_cast<const void*>(aData), fFramesPerBuffer);
    }
}

#else

void QAudioOutStream::run ()
{
    mCommand = eRun;
    PaStream *fAudioStream = 0;
    try
    {
        PaError fError = Pa_Initialize( );
        if (fError != paNoError) throw fError;

        std::uint32_t fDevices = Pa_GetHostApiCount ();
        if (mDeviceID >= fDevices)
        {
            mDeviceID = Pa_GetDefaultHostApi();
        }
        getAudioParameters();

        if (mSuggestedLatency == 0)
        {
            mSuggestedLatency = getAudioParameters().getDefaultLowOutputLatency();
        }

        PaStreamParameters fOutputParameters;
        fOutputParameters.device = mDeviceID;
        fOutputParameters.channelCount = mChannels;
        fOutputParameters.sampleFormat = mSampleFormat;
        fOutputParameters.hostApiSpecificStreamInfo = NULL;
        fOutputParameters.suggestedLatency = mSuggestedLatency;

        fError = Pa_OpenStream(
            &fAudioStream,
            NULL,               // no input
            &fOutputParameters,
            mSampleFrequency,
            mFramesPerBuffer,
            paClipOff,          // we won't output out of range samples so don't bother clipping them
            &QAudioOutStream::paCallback,
            this                // Using 'this' for userData so we can cast to Sine* in paCallback method
            );

        if (fError != paNoError) throw fError;

        fError = Pa_SetStreamFinishedCallback( fAudioStream, &QAudioOutStream::paStreamFinished );
        if (fError != paNoError) throw fError;

        mLastError = "Audio Output Device Initialized: ";
        mLastError += Pa_GetErrorText(fError);

        while ( mCommand != eStopThread )
        {
            mCommandSeamphore.acquire();
            switch (mCommand)
            {
            case eStartStream:
                if (Pa_IsStreamStopped(fAudioStream))
                {
                    fError = Pa_StartStream( fAudioStream );
                    if (fError != paNoError) throw fError;
                    TRACE(Logger::notice, "Audio output stream started\n");
                    send_stream_state(true);
                }
                mCommand = eRun;
            break;
            case eStopStream:
                if (Pa_IsStreamActive(fAudioStream))
                {
                    fError = Pa_StopStream( fAudioStream );
                    if (fError != paNoError) throw fError;
                    TRACE(Logger::notice, "Audio output stream stopped\n");
                    send_stream_state(false);
                }
                mCommand = eRun;
            break;
            default: break;
            }
        };
    }
    catch (PaError fError)
    {
        mCommand = eStopThread;
        mLastError = "Audio Device Error: ";
        mLastError += Pa_GetErrorText(fError);
    }
    if (fAudioStream)
    {
        if (Pa_IsStreamActive(fAudioStream))
        {
            Pa_StopStream( fAudioStream );
            TRACE(Logger::notice, "Audio output stopped\n");
        }
        Pa_CloseStream( fAudioStream );
        TRACE(Logger::notice, "Audio output stream closed\n");
    }
    Pa_Terminate();
    TRACE(Logger::notice, "Audio output device terminated\n");
}

/* The instance callback, where we have access to every method/variable in object of class Sine */
int QAudioOutStream::paCallbackMethod(const void *, void *aOutputBuffer,
    unsigned long aFramesPerBuffer, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags )
{
    int fReturn = paContinue;
    float *fOut = static_cast<float*>(aOutputBuffer);
    unsigned long i;

    for( i=0; i<aFramesPerBuffer; i++ )
    {
        for (std::uint32_t j=0; j<mChannels; ++j)
        {
            *fOut++ = mBuffers[j][mBufferIndex[j]];  /* left */
            ++mBufferIndex[j];
            if (mBufferIndex[j] >= mBuffers[j].size())
            {
                if (j == 0 && mBufferCycles != Infinite)
                {
                    if (--mBufferCycles == 0)
                    {
                        std::fill(fOut, fOut + (aFramesPerBuffer - i) * mChannels, 0);
                        do_start_stream(false, Infinite);
                        break;
                    }
                }
                mBufferIndex[j] = 0;
            }
        }
    }
    if (mAudioCallback)
    {
        mAudioCallback(0, aOutputBuffer, aFramesPerBuffer, 0, 0);
    }

    return fReturn;

}

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
int QAudioOutStream::paCallback( const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *userData )
{
    /* Here we cast userData to Sine* type so we can call the instance method paCallbackMethod, we can do that since
       we called Pa_OpenStream with 'this' for userData */
    return ((QAudioOutStream*)userData)->paCallbackMethod(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags);
}


void QAudioOutStream::paStreamFinishedMethod()
{
    send_stream_state(false);
}

/*
 * This routine is called by portaudio when playback is done.
 */
void QAudioOutStream::paStreamFinished(void* userData)
{
    return ((QAudioOutStream*)userData)->paStreamFinishedMethod();
}

void QAudioOutStream::read_audio_data(char* , qint64 , qint64& )
{
    // dummy
}
#endif

bool QAudioOutStream::isValid()
{
    return mCommand != eInvalid;
}

void QAudioOutStream::clearLastError()
{
    mLastError.clear();
}

void QAudioOutStream::do_start_stream(bool aStart, qint32 aBufferCycles)
{
    if (aStart)
    {
        std::fill(mBufferIndex.begin(), mBufferIndex.end(), 0);
        aStart = canStart();
    }
    mBufferCycles = aBufferCycles;
    mCommand      = aStart ? eStartStream : eStopStream;
#if (_USE_QT_AUDIO == 1)
    if (aStart)
    {
        mAudioOutput->start(mAudioOutputDevice.data());
    }
    else
    {
        mAudioOutput->stop();
    }

#endif
    mCommandSeamphore.release();
}

void QAudioOutStream::setChannels(std::uint32_t aChannels)
{
    mChannels = aChannels;
    mBuffers.resize(mChannels);
    mBufferIndex.resize(mChannels);
}

void QAudioOutStream::setSampleFrequency(double aSF)
{
    mSampleFrequency = aSF;
}

void QAudioOutStream::setSampleFormat(std::uint32_t aSF)
{
    mSampleFormat = aSF;
}

void QAudioOutStream::setFramesPerBuffer(std::uint32_t aFpB)
{
    mFramesPerBuffer = aFpB;
}

void QAudioOutStream::setSuggestedLatency(double aSL)
{
    mSuggestedLatency = aSL;
}

bool QAudioOutStream::canStart()
{
    bool fCanStart = true;
    for (std::uint32_t j=0; j<mChannels; ++j)
    {
         if (mBufferIndex[j] >= mBuffers[j].size())
         {
             fCanStart = false;
             break;
         }
    }
    return fCanStart;
}

std::vector<float>* QAudioOutStream::getBuffer(int aChannel)
{
    return &mBuffers[aChannel];
}

