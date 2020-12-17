#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qaudiorecord.h"
#include "qaudiooutstream.h"
#include "Channel.h"
#include "calibration.h"
#include "scopesettings.h"
#include "functiongenerator.h"
#include "Average.h"
#include "TriggerParameter.h"
#include "combinecurves.h"
#include "workerthreadconnector.h"

#include <QMainWindow>
#include <QAtomicInt>
#include <QMutex>
#include <QPointer>


namespace Ui
{
    class MainWindow;
}

class QDoubleLableWithUnit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class ScopeSettings;
public:
    explicit MainWindow(QApplication& aApp, QWidget *parent = 0);
    ~MainWindow();

    void getProbeStrings(QStringList& fProbes);
    void setProbeValues(const QList<double>& fProbes);
    bool doRestart() { return mRestart; }

private:
    struct action
    {
        enum eAction
        {
            AudioInputSettings,
            AudioCalibrationSettings,
            OscilloscopeSettings,
            FunctionsGenerator,
            CopyToClipboard,
            About
        };
        enum eChanging
        {
            None,
            ComboVerticalChannel,
            ComboVerticalChannelAmplification,
            vslideLevel
        };
    };
#if TRIGGER_PROCESSING == 2
    struct TriggerPt
    {
        TriggerPt(int aPos=0, double aOffset=0, bool fValid=true): mPosition(aPos), mOffset(aOffset), mValid(fValid) {}
        int    mPosition;
        double mOffset;
        bool   mValid;
    };

    typedef std::vector<TriggerPt> IntToDoubleVector;
#endif

Q_SIGNALS:
    void send_value(double, int);
    void doWork(int);

private Q_SLOTS:

    void on_vslideOffset_valueChanged(int value);
    void on_vslideLevel_valueChanged(int value);
    void on_hslideTime_valueChanged(int value);
    void on_hslideTimePosition_valueChanged(int value);
    void on_vslideTriggerLevel_valueChanged(int value);
    void on_pushAutoLevel_clicked(bool checked);
    void on_pushAutoTime_clicked(bool checked);
    void on_pushTrigger50Percent_clicked();
    void click_radio_trigger(int index);
    void on_comboTriggerChannel_currentIndexChanged(int index);
    void on_comboVerticalChannel_currentIndexChanged(int index);
    void on_pushTriggerEdge_clicked(bool checked);
    void on_checkChannelVisible_clicked(bool checked);
    void trigger_menu_action(QAction*);
    void on_comboFFT_currentIndexChanged(int index);
    void on_checkFFT_Log_clicked(bool checked);
    void on_checkShowFFT_clicked(bool checked);
    void on_checkShowZoom_clicked(bool checked);
    void on_pushTriggerManual_clicked();
    void on_clickedMathCombine(int index);
    void on_updateViewByTimer();
    void on_updateStatusbarByTimer();
    void update_scope_view(Cursors::eCursor aCursor=Cursors::noCursor, Cursors::eView aID=Cursors::unknown_view);
    void on_comboVerticalChannelAmplification_currentIndexChanged(int index);
    void receive_subview_event(const QEvent* aEvent);

private:

    void timerEvent(QTimerEvent *event);
    void resizeEvent(QResizeEvent *event);

    void postBufferChanged(int aStartPosition);
    void onRedrawScopeView(bool aNewBuffer);
    bool searchForEdgeCrossingTriggerLevel(int aChannel, int aStartPoint, int &aIndexEdge, bool aTriggerEdgeRising, void (*aModify)(Trigger &), double *aOffset=NULL, int* aBuffersize=0);
    bool isDisplayTriggered();
    void determineAutomaticLevel(int aC);
    void onAudioInputSettings();
    void handleBufferChanged(int aStartPosition);

#if TRIGGER_PROCESSING == 1
    void determineAutomaticTime(int aSearchStart, double aTriggerTimeOffset);

    void initHoldOffTime(int aSearchStart);
    int  checkHoldOffTime();

    void initHoldOffEvents(int aStart);
    bool checkHoldOffEvents(int& aStart);
#endif
#if TRIGGER_PROCESSING == 2
    bool determineTrigger();
    void determineAutomaticTime();
    bool checkHoldOff(int aTriggerPosition);
#endif

    void updateChannelColors();
    void setGridColor(QColor aColor) ;
    void setBackgroundColor(QColor aColor) ;

    QString getXmlName() const;
    QString getConfigPath() const;
    void saveSettings();

    QColor getGridColor() const;
    QColor getBackgroundColor() const;
    void setLabel(const QPoint&, const QVariant& );
    void hideLabels();

private:
    Ui::MainWindow*      ui;
    QAudioRecord         mAudioInput;
    QAudioOutStream      mAudioOutput;
    bool                 mAudioSourceFG;
    int                  mAudioInputType;

    std::vector<QColor>  mCurveColors;
    std::vector<double>  mLevelPerDivision;
    bool                 mAutoLevelDetermination;

    std::vector<double>  mTimePerDivision;
    bool                 mAutoTimeDetermination;
    Average<double>     mAvgTime;

    std::vector<Channel> mChannel;
    int                  mActiveVerticalChannel;

    TriggerParameter     mTrigger;
    int                  mTimerTriggered;
    int                  mTimerTriggerArmed;

    int                  mViewUpdateRate;
    QAtomicInt           mScopeViewInvalid;
    QMutex               mUpdateMutex;

#if TRIGGER_PROCESSING == 1
    std::vector<int>     mBufferUpdate;
    int                  mLastBufferPosition;
    double               mLastTriggerOffset;
#endif

#if TRIGGER_PROCESSING == 2
    IntToDoubleVector    mTriggerPointsList;
    TriggerPt            mTriggerPoint;
    int                  mLatestStartPosition;
#endif

    WorkerThreadConnector mWorker;
    bool                 mCopyToClipboard;
    action::eChanging    mChanging;
    CombineCurves        mCombineCurves;

    Calibration          mCalibrationDlg;
    ScopeSettings        mScopeSettings;
    FunctionGenerator    mFunctionGenerator;
    QApplication&        mApp;
    bool                 mRestart;

    QPointer<QDoubleLableWithUnit> mStatusX_Axis;
    QPointer<QDoubleLableWithUnit> mStatusLevel;
    std::vector<QPointer<QDoubleLableWithUnit>> mLabels;
    unsigned int                                mCurrentLabel;

    const static int mOffsetFactor = 100;
    const static int mTriggerLevelFactor = 100;
    const static int mTimePositionFactor = 100;
};

#endif // MAINWINDOW_H
