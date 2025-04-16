#ifndef QAUDIORECORD_H
#define QAUDIORECORD_H

#include <QThread>
#include <QSemaphore>
#include <QMutex>

#include <vector>
#include <QString>

#include "main.h"

#include "Channel.h"
#include "qaudioparameter.h"
#include "memorypool.h"


#if (_USE_QT_AUDIO == 1)
#include <QAudioInput>
#include <QPointer>
#include "DirectIoDevice.h"
#endif

struct PaDeviceInfo;

#if (_USE_QT_AUDIO != 1)
const char* getStringFromPaType(unsigned long aType);
unsigned long getPaTypeFromString(QString& aName);
#endif

typedef void (*convertInputBufferFunc)(size_t aStartPos, size_t aSize, std::uint32_t aStartChannel, std::uint32_t aChannels, const Channel*, const void *aInputBuffer, std::vector< std::vector<double> >&aOutputBuffer);

class QAudioRecord : public QThread
{
    Q_OBJECT
public:
    enum eType {eInvalid=-1, eStopDevice,
#if (_USE_QT_AUDIO == 1)
                eQtAudio,
                eSimulation,
                eLast, eFirst = eQtAudio
#else
                ePortAudio,
                eSimulation,
                eLast, eFirst = ePortAudio
#endif
               };

    explicit QAudioRecord(QObject *parent = 0);
    virtual ~QAudioRecord();
    void     setType(eType aType);
    void     setBufferSize(int aSize);
    void     setBuffers(int aBuffers);
    void     setBufferPoolSize(int aSize);
    void     setSampleParameter(double aSampleFrequency, double aBufferLength, double aSampleLength);
    void     setChannels(int aChannels);
    void     setSampleFrequency(double aSF);
    void     setSampleFormat(unsigned long aSF);
    void     setSuggestedLatency(double aSL);
    void     suspend();
    void     resume();
    void     stopDevice();

    const std::vector<double>& getValues(int aChannel) { return mBuffer[aChannel]; }
    size_t   getCurrentPosition() const { return mCurrentPosition; }
    size_t   getTotalBufferSize() const { return mBuffer[0].size(); }
    int      getBufferSize()      const { return mBufferSize; }
    int      getBuffers()         const { return mBuffers; }
    int      getBufferPoolSize()  const { return mBufferPoolSize; }
    int      getChannels()        const { return mChannels; }
    double   getSampleFrequency() const { return mSampleFrequency; }
    int      getBitsPerSample() const;
    eType    getType() const { return mType; }
    bool     isValid();

    AudioCallback   getAudioCallback();
    void            setCallbackChannels(int aChannels);

    bool     isStarting() { return mStarting; }

    const QString& getLastError() { return mLastError; }
    void     clearLastError();

    // portaudio
    const QAudioParameter& getAudioParameters();
    void            setDeviceID(int aDI)    { mDeviceID = aDI; }
    int             getDeviceID()           { return mDeviceID; }
    uint            getSampleFormat()       { return mSampleFormat; }
    double          getSuggestedLatency()   { return mSuggestedLatency; }
    void            setChannelParameter(const Channel* aCP)     { mChannelParameter = aCP; }

    static eType    getTypeFromString(QString& aName);
    static const char* getStringFromType(eType aType);

    boost::function< void (int) > mPostBufferChanged;

Q_SIGNALS:


public Q_SLOTS:
    void start(Priority = InheritPriority);
    void on_write_audio_data(const char* aData, qint64 aBytes, qint64& aBytesWritten);

private:
    virtual void run ();


    void runSimulation();
#if (_USE_QT_AUDIO == 1)
    void initQtAudio();
    void runQtAudio();
    int convertQAudioSamples(const void *aInputBuffer, unsigned long aFramesPerBuffer);
#else
    void runPortAudio();
    static int PortAudioCallback(  const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                                   const PaStreamCallbackTimeInfo* timeInfo, unsigned long statusFlags, void *userData );

    int PortAudioCallbackM( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo, unsigned long statusFlags);
#endif
    void hold();


    void processIntermediateBuffer();
    void convertIntermediateBuffer(const void *aInputBuffer, void *aOutputBuffer);

    int  getPoolBuffer();
    void releasePoolBuffer(int aPB);

    QString mLastError;
    volatile eType mType;
    unsigned long mSampleFormat;

#if (_USE_QT_AUDIO == 1)
    QPointer<QAudioInput>     mAudioInput;
    QPointer<DirectIODevice>  mAudioInputDevice;
    std::vector<uint8_t>      mTempBuffer;
    uint32_t                  mTempBufferPos;
#endif

    volatile bool mStarting;
    int     mChannels;
    double  mSampleFrequency;

    QAudioParameter mAudioParameter;

    // PortAudio Parameters
    int           mDeviceID;
    double        mSuggestedLatency;

    unsigned long mBufferSize;
    unsigned long mBuffers;
    size_t  mCurrentPosition;
    const Channel* mChannelParameter;
    std::vector< std::vector<double> > mBuffer;
    QMutex     mBufferMutex;

    QList<void *> mBufferListIM;
    MemoryPool    mBufferPoolIM;
    QSemaphore    mBufferSemaphoreIM;
    QMutex        mBufferMutexIM;
    int           mBufferPoolSize;

    convertInputBufferFunc mConvert;
    convertInputBufferFunc mCallbackConvert;
    int           mCallbackChannels;

    QSemaphore mHoldSeamphore;
    bool       mHold;

};

class QDomNode;
QAudioRecord::eType getValue(const QDomNode& aValue, QAudioRecord::eType aDefault);

#endif // QAUDIORECORD_H
