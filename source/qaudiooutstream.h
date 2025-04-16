#ifndef QAUDIOOUTSTREAM_H
#define QAUDIOOUTSTREAM_H

#include <QThread>
#include <QSemaphore>
#include <vector>

#include "Channel.h"
#include "qaudioparameter.h"

#if (_USE_QT_AUDIO == 1)

#include <QAudioOutput>
#include <QPointer>
#include "DirectIoDevice.h"

#endif


#if (_USE_QT_AUDIO == 1)
   typedef bool (*convertOutputBufferFunc)(std::vector<std::uint32_t>& aIndex, const std::vector< std::vector<float> >& aBuffer, qint32& aCycles, char* aData, qint64 aBytes);
#else
   struct PaStreamCallbackTimeInfo;
   struct PaDeviceInfo;
   typedef unsigned long PaStreamCallbackFlags;
#endif

class QAudioOutStream : public QThread
{
    Q_OBJECT

public:
    enum eConst
    {
        Infinite  = -1,
        StopBurst = -2
    };
    enum eCmd {eInvalid=-1, eStopThread, eRun, eStartStream, eStopStream, eLast, eFirst = eStopThread };
    explicit QAudioOutStream(QObject *parent = 0);
    ~QAudioOutStream();

    void     setChannels(std::uint32_t aChannels);
    void     setSampleFrequency(double aSF);
    void     setSampleFormat(std::uint32_t aSF);
    void     setFramesPerBuffer(std::uint32_t aFpB);
    void     setSuggestedLatency(double aSL);
    void     setDeviceID(std::uint32_t aDI)   { mDeviceID = aDI; }
    void     stopDevice();
    void     setAudioCallback(const AudioCallback& aFunction);
    bool     isValid();

    const QAudioParameter& getAudioParameters();
    std::uint32_t   getChannels() const          { return mChannels; }
    double          getSampleFrequency() const   { return mSampleFrequency; }
    std::uint32_t   getSampleFormat() const      { return mSampleFormat; }
    std::uint32_t   getDeviceID() const          { return mDeviceID; }
    double          getSuggestedLatency() const  { return mSuggestedLatency; }
    const QString&  getLastError() const         { return mLastError; }
    void            clearLastError();

    std::vector<float>* getBuffer(int aChannel);

Q_SIGNALS:
    void send_stream_state(bool );

#if (_USE_QT_AUDIO == 1)
public Q_SLOTS:
    void do_start_stream(bool aStart, qint32 aBufferCycles=Infinite);
    void read_audio_dataFloat(char* aData, qint64 aBytes, qint64& aBytesWritten);
    void read_audio_data(char* aData, qint64 aBytes, qint64& aBytesWritten);
#else
public Q_SLOTS:
    void do_start_stream(bool aStart, qint32 aBufferCycles=Infinite);
    void read_audio_data(char* aData, qint64 aBytes, qint64& aBytesWritten);
#endif

public:
#if (_USE_QT_AUDIO == 1)
    void initQtAudio();
    void runQtAudio();
    void start(Priority = InheritPriority);
private:
    convertOutputBufferFunc mConvert;

#endif

private:
    virtual void run ();

    bool canStart();

#if (_USE_QT_AUDIO == 1)

    QPointer<QAudioOutput>    mAudioOutput;
    QPointer<DirectIODevice>  mAudioOutputDevice;
    bool                      mStarting;

#else

    static int  paCallback( const void *, void *, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void *);
    int         paCallbackMethod(const void *, void *, unsigned long, const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags );
    static void paStreamFinished(void* userData);
    void        paStreamFinishedMethod();

#endif

    std::uint32_t   mChannels;
    double          mSampleFrequency;
    QString         mLastError;
    volatile eCmd   mCommand;
    QSemaphore      mCommandSeamphore;

    std::vector< std::vector<float> >  mBuffers;
    std::vector< std::uint32_t >       mBufferIndex;
    qint32                             mBufferCycles;
    // PortAudio Parameters
    std::uint32_t   mDeviceID;
    std::uint32_t   mSampleFormat;
    QAudioParameter mAudioParameter;
    double          mSuggestedLatency;
    std::uint32_t   mFramesPerBuffer;

    AudioCallback   mAudioCallback;
};

#endif // QAUDIOOUTSTREAM_H
