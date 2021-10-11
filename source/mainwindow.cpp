#include "main.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xml_functions.h"
#include "audiosettings.h"
#include "generatefunction.h"
#include "settings.h"
#include "events.h"
#include "aboutdlg.h"

#include <QFile>
#include <QDir>
#include <QSettings>
#include <QTimer>
#include <QMutexLocker>
#include <QClipboard>

#include <cmath>
#include <algorithm>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

// NOTE _MSVC_STL_VERSION >= 141 has certain security functions regarding iterators compared to end()

using namespace std;
#define _USE_WORKERTHREAD_FOR_AUDIOBUFFER

#if (_USE_QT_AUDIO != 1)
QAudioInit gAudio;
#endif


MainWindow::MainWindow(QApplication& aApp, QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , mAudioSourceFG(false)
  , mAudioInputType(0)
  , mAutoLevelDetermination(false)
  , mAutoTimeDetermination(false)
  , mActiveVerticalChannel(InvalidIndex)
  , mViewUpdateRate(10)
  , mScopeViewInvalid(0)
#if TRIGGER_PROCESSING == 1
  , mLastBufferPosition(0)
  , mLastTriggerOffset(0)
#endif
#if TRIGGER_PROCESSING == 2
  , mLatestStartPosition(0)
#endif
  , mWorker(this)
  , mCopyToClipboard(false)
  , mChanging(action::None)
  , mCombineCurves(this)
  , mCalibrationDlg(this, mChannel)
  , mScopeSettings(this, mChannel, mTrigger, mCombineCurves)
  , mFunctionGenerator(this)
  , mApp(aApp)
  , mRestart(false)
  , mCurrentLabel(0)
{
    ui->setupUi(this);

    mScopeSettings.setFFT_FilterIndex(ui->graphicsViewFFT->getFilterFunction());
    QObject::connect(&mScopeSettings, SIGNAL(send_radio_filter(int)), ui->graphicsViewFFT, SLOT(setFilterFunction(int)));
    mScopeSettings.setFFT_OutputIndex(ui->graphicsViewFFT->getFFT_ValueFunction());
    QObject::connect(&mScopeSettings, SIGNAL(send_radio_output(int)), ui->graphicsViewFFT, SLOT(setFFT_ValueFunction(int)));
    QObject::connect(&mScopeSettings, SIGNAL(send_floating_avg_values(int)), ui->graphicsViewFFT, SLOT(setFFT_FloatingAvgValues(int)));

    QObject::connect(&mFunctionGenerator, SIGNAL(start_stream(bool, qint32)), &mAudioOutput, SLOT(do_start_stream(bool, qint32)));
    QObject::connect(&mAudioOutput, SIGNAL(send_stream_state(bool)), &mFunctionGenerator, SLOT(get_stream_state(bool)));

    QObject::connect(ui->groupTrigger, SIGNAL(clicked(int)), this, SLOT(click_radio_trigger(int)));

    mScopeSettings.addCursors(ui->graphicsViewScope->getCursors());
    QObject::connect(&ui->graphicsViewScope->getCursors(), SIGNAL(update(Cursors::eCursor,Cursors::eView)), this, SLOT(update_scope_view(Cursors::eCursor,Cursors::eView)));
    QObject::connect(ui->graphicsViewScope, SIGNAL(sendEvent(const QEvent*)), this, SLOT(receive_subview_event(const QEvent*)));
    ui->graphicsViewScope->setDragMode(QGraphicsView::RubberBandDrag);
    QObject::connect(ui->graphicsViewScope, SIGNAL(sendSelectedRange(const QRectF&)), ui->graphicsViewZoom, SLOT(onSelectRange(const QRectF&)));

    mScopeSettings.addCursors(ui->graphicsViewFFT->getCursors());
    QObject::connect(&ui->graphicsViewFFT->getCursors(), SIGNAL(update(Cursors::eCursor,Cursors::eView)), this, SLOT(update_scope_view(Cursors::eCursor,Cursors::eView)));
    QObject::connect(ui->graphicsViewFFT, SIGNAL(sendEvent(const QEvent*)), this, SLOT(receive_subview_event(const QEvent*)));
    ui->graphicsViewFFT->setFont(ui->labelLevel->font());

    ui->graphicsViewZoom->getCursors().setID(Cursors::zoom_view);
    mScopeSettings.addCursors(ui->graphicsViewZoom->getCursors());
    QObject::connect(&ui->graphicsViewZoom->getCursors(), SIGNAL(update(Cursors::eCursor,Cursors::eView)), this, SLOT(update_scope_view(Cursors::eCursor,Cursors::eView)));
    QObject::connect(ui->graphicsViewZoom, SIGNAL(sendEvent(const QEvent*)), this, SLOT(receive_subview_event(const QEvent*)));

    ui->labelLevelValue->initUnit();
    ui->labelLevelValue->getPrefix().setPrefixChange(UnitPrefix::auto_change);

    ui->labelTriggerLevelValue->initUnit();
    ui->labelTriggerLevelValue->getPrefix().setPrefixChange(UnitPrefix::auto_change);

    ui->labelTimeValue->initUnit();
    ui->labelTimeValue->getPrefix().setPrefixChange(UnitPrefix::auto_change);

    ui->labelTimePositionValue->initUnit();
    ui->labelTimePositionValue->getPrefix().setPrefixChange(UnitPrefix::auto_change);

    mStatusX_Axis= new QDoubleLableWithUnit(ui->statusBar);
    mStatusX_Axis->setUnit("s");
    mStatusX_Axis->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    mStatusX_Axis->value(0);
    ui->statusBar->insertPermanentWidget(0, mStatusX_Axis.data());

    mStatusLevel = new QDoubleLableWithUnit(ui->statusBar);
    mStatusLevel->setUnit("V");
    mStatusLevel->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    mStatusLevel->value(0);
    ui->statusBar->insertPermanentWidget(1, mStatusLevel.data());

    QDomDocument fDoc(getXmlName());
    QFile file(getXmlName());
    QDir::setCurrent(getConfigPath());
    if (file.open(QIODevice::ReadOnly))
    {
        fDoc.setContent(&file);
        file.close();
    }

    {
        QDomNode fDebugInfo = findElement(fDoc.firstChild(), s::sDebugInfo);
        Logger::setSeverity(Logger::trace     , getValue(findElement(fDebugInfo, s::sTrace    ), Logger::isSeverityActive(Logger::trace)      ));
        Logger::setSeverity(Logger::debug     , getValue(findElement(fDebugInfo, s::sDebug    ), Logger::isSeverityActive(Logger::debug)      ));
        Logger::setSeverity(Logger::info      , getValue(findElement(fDebugInfo, s::sInfo     ), Logger::isSeverityActive(Logger::info)       ));
        Logger::setSeverity(Logger::notice    , getValue(findElement(fDebugInfo, s::sNotice   ), Logger::isSeverityActive(Logger::notice)     ));
        Logger::setSeverity(Logger::warning   , getValue(findElement(fDebugInfo, s::sWarning  ), Logger::isSeverityActive(Logger::warning)    ));
        Logger::setSeverity(Logger::error     , getValue(findElement(fDebugInfo, s::sError    ), Logger::isSeverityActive(Logger::error)      ));
        Logger::setSeverity(Logger::critical  , getValue(findElement(fDebugInfo, s::sCritical ), Logger::isSeverityActive(Logger::critical)   ));
        Logger::setSeverity(Logger::alert     , getValue(findElement(fDebugInfo, s::sAlert    ), Logger::isSeverityActive(Logger::alert)      ));
        Logger::setSeverity(Logger::emergency , getValue(findElement(fDebugInfo, s::sEmergency), Logger::isSeverityActive(Logger::emergency)  ));
        Logger::setSeverity(Logger::to_console, getValue(findElement(fDebugInfo, s::sToConsole), Logger::isSeverityActive(Logger::to_console) ));
        Logger::setSeverity(Logger::to_syslog , getValue(findElement(fDebugInfo, s::sToSyslog ), Logger::isSeverityActive(Logger::to_syslog)  ));
    }

    {
        QDomNode fAudioInput = findElement(fDoc.firstChild(), s::sAudioInput);
        mAudioInput.setType(static_cast<QAudioRecord::eType>(getValue(findElement(fAudioInput, s::sType), mAudioInput.getType())));
        mAudioInput.setDeviceID(        getValue(findElement(fAudioInput, s::sDeviceID        ), mAudioInput.getDeviceID()          ));
        mAudioInput.setChannels(        getValue(findElement(fAudioInput, s::sChannels        ), mAudioInput.getChannels()          ));
        mAudioInput.setSampleFrequency( getValue(findElement(fAudioInput, s::sSampleFrequency ), mAudioInput.getSampleFrequency()   ));
        mAudioInput.setSampleFormat(    getValue(findElement(fAudioInput, s::sSampleFormat    ),(int) mAudioInput.getSampleFormat() ));
        mAudioInput.setBuffers(         getValue(findElement(fAudioInput, s::sBuffers         ), mAudioInput.getBuffers()           ));
        mAudioInput.setBufferSize(      getValue(findElement(fAudioInput, s::sBufferSize      ), mAudioInput.getBufferSize()        ));
        mAudioInput.setSuggestedLatency(getValue(findElement(fAudioInput, s::sSuggestedLatency), mAudioInput.getSuggestedLatency()  ));
        mAudioInput.setBufferPoolSize(  getValue(findElement(fAudioInput, s::sBufferPoolSize  ), 0));
        mViewUpdateRate = getValue(findElement(fAudioInput, s::sUpdateRate), mViewUpdateRate);
        mAudioSourceFG  = getValue(findElement(fAudioInput, s::sShowFGinScope), mAudioSourceFG);
        mAudioInputType = mAudioInput.getType();
    }

    {
        QDomNode fAudioOutput = findElement(fDoc.firstChild(), s::sAudioOutput);
        mAudioOutput.setDeviceID(        getValue(findElement(fAudioOutput, s::sDeviceID        ), (int)mAudioOutput.getDeviceID()        ));
        mAudioOutput.setChannels(        getValue(findElement(fAudioOutput, s::sChannels        ), (int)mAudioOutput.getChannels()        ));
        mAudioOutput.setSampleFrequency( getValue(findElement(fAudioOutput, s::sSampleFrequency ), mAudioOutput.getSampleFrequency() ));
        mAudioOutput.setSampleFormat(    getValue(findElement(fAudioOutput, s::sSampleFormat    ), (int) mAudioOutput.getSampleFormat()    ));
        mAudioOutput.setSuggestedLatency(getValue(findElement(fAudioOutput, s::sSuggestedLatency), mAudioOutput.getSuggestedLatency()));
        GenerateFunction::setSampleFrequency(mAudioOutput.getSampleFrequency());
        mFunctionGenerator.setChannels(mAudioOutput.getChannels());
        for (std::uint32_t i=0; i < mAudioOutput.getChannels(); ++i)
        {
            mFunctionGenerator.setBuffer(i, mAudioOutput.getBuffer(i));
        }
    }

    int fInputChannels = mAudioInput.getChannels();
    if (mAudioSourceFG)
    {
        mAudioInput.setCallbackChannels(mAudioOutput.getChannels());
        mAudioOutput.setAudioCallback(mAudioInput.getAudioCallback());
        mAudioOutput.setFramesPerBuffer(mAudioInput.getBufferSize());
        fInputChannels = mAudioInput.getChannels() + mAudioOutput.getChannels();
    }
    else if (mAudioInput.getType() == QAudioRecord::eSimulation)
    {
        mAudioOutput.setAudioCallback(mAudioInput.getAudioCallback());
        mAudioOutput.setFramesPerBuffer(mAudioInput.getBufferSize());
        fInputChannels = mAudioOutput.getChannels();
    }

    {
        QDomNode fColors = findElement(fDoc.firstChild(), s::sColors);
        setGridColor(getValue(findElement(fColors, s::sGrid), ui->graphicsViewScope->getGridColor()));
        setBackgroundColor(getValue(findElement(fColors, s::sBackground), ui->graphicsViewScope->backgroundBrush().color()));
        int fNumber=1;
        mCurveColors.push_back(getValue(findElement(fColors, s::sChannel + QString::number(fNumber++)), QColor(Qt::red)));
        mCurveColors.push_back(getValue(findElement(fColors, s::sChannel + QString::number(fNumber++)), QColor(Qt::green)));
        mCurveColors.push_back(getValue(findElement(fColors, s::sChannel + QString::number(fNumber++)), QColor(Qt::cyan)));
        mCurveColors.push_back(getValue(findElement(fColors, s::sChannel + QString::number(fNumber++)), QColor(Qt::magenta)));
        mCurveColors.push_back(getValue(findElement(fColors, s::sChannel + QString::number(fNumber++)), QColor(Qt::darkRed)));
        mCurveColors.push_back(getValue(findElement(fColors, s::sChannel + QString::number(fNumber++)), QColor(Qt::darkBlue)));
        mCurveColors.push_back(getValue(findElement(fColors, s::sChannel + QString::number(fNumber++)), QColor(Qt::darkCyan)));
        mCurveColors.push_back(getValue(findElement(fColors, s::sChannel + QString::number(fNumber++)), QColor(Qt::darkMagenta)));
    }

    const double fScale[3]   = { 5, 2, 1 };
    const double fMinLevel   = 1.0 / pow(2, min(mAudioInput.getBitsPerSample(), 16));
    double       fFactor     = 1.0;

    // vertical parameter
    while (fMinLevel < fFactor)
    {
        mLevelPerDivision.push_back(fScale[0] * fFactor);
        mLevelPerDivision.push_back(fScale[1] * fFactor);
        mLevelPerDivision.push_back(fScale[2] * fFactor);
        fFactor *= 0.1;
    }
    ui->vslideLevel->setRange(0, mLevelPerDivision.size()-1);
    ui->vslideOffset->setRange(-mOffsetFactor, mOffsetFactor);

    int fFloatingAvgCount = 10;
    {
        QDomNode fParameters = findElement(fDoc.firstChild(), s::sParameters);
        fFloatingAvgCount = getValue(findElement(fParameters, s::sFloatingAvgCount), fFloatingAvgCount);

        QString s;
        int fFFTminOrder = getValue(findElement(fParameters, s::sMinFFTOrder), 5);
        int fFFTmaxOrder = getValue(findElement(fParameters, s::sMaxFFTOrder), 15);
        int fCurrentFFTOrder = getValue(findElement(fParameters, s::sFFTOrder), 256);
        for (int fFFTorder=fFFTminOrder; fFFTorder<=fFFTmaxOrder; ++fFFTorder)
        {
            s.setNum(static_cast<ulong>(1<<fFFTorder));
            ui->comboFFT->addItem(s, qVariantFromValue(fFFTorder));
        }
        ui->comboFFT->setCurrentIndex(ui->comboFFT->findText(QString::number(fCurrentFFTOrder)));
        ui->checkFFT_Log->setChecked(getValue(findElement(fParameters, ui->checkFFT_Log->text()), 1));
    }

    mChannel.resize(fInputChannels+1);
    mCombineCurves.setMathChannelIndex(fInputChannels);
    mAudioInput.setChannelParameter(&mChannel[0]);

    {
        QDomNode fAmplifier = findElement(fDoc.firstChild(), s::sAmplifier);
        QDomNode fValueNode = findElement(fAmplifier, s::sValue);
        QList<double> fValues;

        while (!fValueNode.isNull())
        {
            double fValue = getValue(fValueNode, 1.0);
            fValues.append(fValue);
            fValueNode = fValueNode.nextSibling();
        }
        setProbeValues(fValues);
    }

    for (size_t i=0; i<mChannel.size(); ++i)
    {
        QString s;
        Channel& c =  mChannel[i];
        s.setNum(static_cast<ulong>(i+1));
        QDomNode fDNchannel = findElement(fDoc.firstChild(), s::sChannel + s );
        c.mPen.setColor(mCurveColors[i]);
        c.mPen.setWidth(0);
        c.setLevelPerDivision() = getValue(findElement(fDNchannel, s::sLevelPerDivision ), mLevelPerDivision[0]);
        c.setOffsetIndex()      = getValue(findElement(fDNchannel, s::sOffsetIndex ), 0);
        c.setOffset()           = getValue(findElement(fDNchannel, s::sOffset ), 0.0);
        c.setTriggerLevel()     = getValue(findElement(fDNchannel, s::sTriggerLevel), 0.0);
        c.mVisible          = getValue(findElement(fDNchannel, s::sVisible), c.mVisible);
        c.setNoOfAvgValues(fFloatingAvgCount);

        QDomNode fInputChannel  = findElement(fDNchannel, s::sAudioInput );
        c.setAudioInputScale()  = getValue(findElement(fInputChannel, s::sScaleFactor), c.getAudioInputScale());
        c.setAudioInputOffset() = getValue(findElement(fInputChannel, s::sOffset), c.getAudioInputOffset());
        c.setAudioScale()       = getValue(findElement(fInputChannel, s::sAmplifier), c.getAudioScale());
        c.mName                 = getValue(findElement(fInputChannel, s::sName), c.mName);

        ui->comboVerticalChannel->addItem(s);
        ui->comboTriggerChannel->addItem(s);
    }
    // remove the math calculation channel afterwards
    ui->comboVerticalChannel->removeItem(mCombineCurves.getMathChannelIndex());
    ui->comboTriggerChannel->removeItem(mCombineCurves.getMathChannelIndex());


    {
        QDomNode fVertical = findElement(fDoc.firstChild(), s::sVertical);
        int fCurrentIndex  = ui->comboVerticalChannel->currentIndex();
        int fDesiredIndex  = getValue(findElement(fVertical, s::sActiveChannel), 0);
        if (fDesiredIndex != fCurrentIndex)
        {
            ui->comboVerticalChannel->setCurrentIndex(fDesiredIndex);
        }
        mAutoLevelDetermination = getValue(findElement(fVertical, s::sAutomatic), 0);
    }

    // horizontal parameter
    const double fSampleTime = 0.2 / mAudioInput.getSampleFrequency();
    fFactor = 0.1;
    while (fSampleTime < fFactor)
    {
        mTimePerDivision.push_back(fScale[0] * fFactor);
        mTimePerDivision.push_back(fScale[1] * fFactor);
        mTimePerDivision.push_back(fScale[2] * fFactor);
        fFactor *= 0.1;
    }

    ui->hslideTime->setRange(0, mTimePerDivision.size()-1);
    ui->hslideTimePosition->setRange(-mTimePositionFactor, mTimePositionFactor);
    {
        QDomNode fTime = findElement(fDoc.firstChild(), s::sTime);
        ui->hslideTime->setValue(getValue(findElement(fTime, s::sBaseIndex), static_cast<int>(mTimePerDivision.size()/2)));
        ui->hslideTimePosition->setValue(getValue(findElement(fTime, s::sPositionIndex), ui->hslideTimePosition->value()));
        mAutoTimeDetermination = getValue(findElement(fTime, s::sAutomatic), 0);
        mAvgTime.setNoOfAvgValues(fFloatingAvgCount);
    }

    // Trigger parameter
    ui->vslideTriggerLevel->setRange(-mTriggerLevelFactor / 2,  mTriggerLevelFactor / 2);
    {
        QDomNode fTrigger  = findElement(fDoc.firstChild(), s::sTrigger);
        mTrigger.mType       = static_cast<TriggerType::eType>(getValue(findElement(fTrigger, s::sType), static_cast<int>(TriggerType::Auto)));
        mTrigger.mEdgeRising = getValue(findElement(fTrigger, s::sEdge), 0);
        int fCurrentIndex  = ui->comboTriggerChannel->currentIndex();
        int fDesiredIndex  = getValue(findElement(fTrigger, s::sActiveChannel), 0);
        if (fDesiredIndex != fCurrentIndex)
        {
            ui->comboTriggerChannel->setCurrentIndex(fDesiredIndex);
        }
        mTrigger.mHoldOffTime   = getValue(findElement(fTrigger, s::sHoldOff), 0);
        mTrigger.mDelay         = getValue(findElement(fTrigger, s::sDelay), 0);
        mTrigger.mHoldOffType   = static_cast<TriggerParameter::HoldOff::eType>(getValue(findElement(fTrigger, QString(s::sHoldOff)+s::sActive), TriggerParameter::HoldOff::Off));
        mTrigger.mDelayActive   = getValue(findElement(fTrigger, QString(s::sDelay)  +s::sActive), 0);
    }
    {
        QDomNode fFunctionGenerator  = findElement(fDoc.firstChild(), s::sFunctionGenerator);
        mFunctionGenerator.loadParameter(fFunctionGenerator);
    }

    ui->groupTrigger->setCurrentIndex(mTrigger.mType);
    ui->pushTriggerEdge->setChecked(mTrigger.mEdgeRising);

    ui->pushAutoLevel->setChecked(mAutoLevelDetermination);
    ui->pushAutoTime->setChecked(mAutoTimeDetermination);

    QGraphicsScene* scene = new QGraphicsScene;
    ui->graphicsViewScope->setScene(scene);
    ui->graphicsViewScope->setTimeStep(1.0 / static_cast<double>(mAudioInput.getSampleFrequency()));
    ui->graphicsViewScope->setTimePerDivision(mTimePerDivision[ui->hslideTime->value()]);

    scene = new QGraphicsScene;
    ui->graphicsViewFFT->setScene(scene);
    ui->graphicsViewFFT->setFFT_Parameter(mAudioInput.getSampleFrequency(), ui->comboFFT->currentText().toInt());
    ui->graphicsViewFFT->setLogarithmic(ui->checkFFT_Log->isChecked());
    ui->graphicsViewFFT->hide();

    scene = new QGraphicsScene;
    ui->graphicsViewZoom->setScene(scene);
    ui->graphicsViewZoom->setTimeStep(1.0 / static_cast<double>(mAudioInput.getSampleFrequency()));
    ui->graphicsViewZoom->setTimePerDivision(mTimePerDivision[ui->hslideTime->value()]);
    ui->graphicsViewZoom->hide();

#ifdef _USE_WORKERTHREAD_FOR_AUDIOBUFFER
    mAudioInput.mPostBufferChanged = boost::bind(&MainWindow::postBufferChanged, this, _1);
    mWorker.setWorkerFunction(boost::bind(&MainWindow::handleBufferChanged, this, _1));
    QObject::connect(this, SIGNAL(doWork(int)), &mWorker, SLOT(doWork(int)));
#else
    mAudioInput.mPostBufferChanged = boost::bind(&MainWindow::handleBufferChanged, this, _1);
#endif

#if TRIGGER_PROCESSING == 1
    mBufferUpdate.resize(mAudioInput.getBuffers(), -1);
#endif

    ui->menuBar->connect(ui->menuBar, SIGNAL(triggered(QAction*)), this, SLOT(trigger_menu_action(QAction*)));
    on_hslideTimePosition_valueChanged(ui->hslideTimePosition->value());

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_updateStatusbarByTimer()));
//    timer->start(15000); // for calgrind analysis
    timer->start(500);

    QTimer *timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(on_updateViewByTimer()));
    timer1->start(mViewUpdateRate);
}

MainWindow::~MainWindow()
{
    saveSettings();

    mAudioOutput.stopDevice();
    mAudioInput.stopDevice();


    delete ui;
}

void MainWindow::getProbeStrings(QStringList& fProbes)
{
    for (int i = 0; i < ui->comboVerticalChannelAmplification->count(); ++i)
    {
        fProbes.append(ui->comboVerticalChannelAmplification->itemText(i));
    }
}

void MainWindow::setProbeValues(const QList<double> &fProbes)
{
    mChanging = action::ComboVerticalChannelAmplification;
    ui->comboVerticalChannelAmplification->clear();
    int index = 0;
    for (double fValue : fProbes)
    {
        QString sProbe;
        if (fValue >= 1.0) sProbe.sprintf("%.0f : 1", fValue);
        else               sProbe.sprintf("1 : %.0f", 1 / fValue);
        ui->comboVerticalChannelAmplification->addItem(sProbe);
        ui->comboVerticalChannelAmplification->setItemData(index++, QVariant(fValue));
    }
    if (ui->comboVerticalChannelAmplification->count() == 0)
    {
        ui->comboVerticalChannelAmplification->addItem("1 : 1");
        ui->comboVerticalChannelAmplification->setItemData(index++, QVariant(1.0));
    }
    mChanging = action::None;
}

void MainWindow::trigger_menu_action(QAction*anAction)
{
    static map<string, action::eAction> fMap =
    {
        {ui->actionAudioInputSettings->objectName().toStdString()      , action::AudioInputSettings},
        {ui->actionAudioCalibrationSettings->objectName().toStdString(), action::AudioCalibrationSettings},
        {ui->actionOscilloscopeSettings->objectName().toStdString()    , action::OscilloscopeSettings},
        {ui->actionFunctions_Generator->objectName().toStdString()     , action::FunctionsGenerator},
        {ui->actionCopyToClipboard->objectName().toStdString()         , action::CopyToClipboard},
        {ui->actionAbout->objectName().toStdString()                   , action::About}
    };

    map<string, action::eAction>::iterator fFound = fMap.find(anAction->objectName().toStdString());
    if (fFound != fMap.end())
    {
        switch (fFound->second)
        {
        case action::AudioInputSettings:        onAudioInputSettings();    break;
        case action::AudioCalibrationSettings:  mCalibrationDlg.show();    break;
        case action::OscilloscopeSettings:      mScopeSettings.show();     break;
        case action::FunctionsGenerator:        mFunctionGenerator.show(); break;
        case action::CopyToClipboard:           mCopyToClipboard = true;   break;
        case action::About: { AboutDlg dlg(this); dlg.exec(); }; break;

        default:
            break;
        }
    }
}

void MainWindow::onAudioInputSettings()
{
    AudioSettings fDlg(this, mAudioInput, mAudioOutput);
    fDlg.mShowFGinScope  = mAudioSourceFG;
    fDlg.mAudioInputType = mAudioInputType;
    int fReturn = fDlg.exec();
    if (   fDlg.mShowFGinScope  != mAudioSourceFG
        || fDlg.mAudioInputType != mAudioInputType)
    {
        mAudioSourceFG  = fDlg.mShowFGinScope;
        mAudioInputType = fDlg.mAudioInputType;
        fReturn =  AudioSettings::Restart;
    }
    if (fReturn == AudioSettings::Restart)
    {
        mRestart = true;
        close();
    }
}


void MainWindow::saveSettings()
{
    QDomDocument doc(s::sSettings);
    QDomElement root = doc.createElement(s::sSettings);
    QString     fName;
    doc.appendChild(root);

    QDomElement fAudioInput = doc.createElement(s::sAudioInput);
    root.appendChild(fAudioInput);
    addElementS(doc, fAudioInput, s::sType, QAudioRecord::getStringFromType(static_cast<QAudioRecord::eType>(mAudioInputType)));
    fName = "valid types: ";
    for (int fT = QAudioRecord::eFirst; fT < QAudioRecord::eLast; ++fT)
    {
        fName += QAudioRecord::getStringFromType(static_cast<QAudioRecord::eType>(fT));
        fName += ", ";
    }
    fAudioInput.appendChild(doc.createComment(fName));

    addElement( doc, fAudioInput, s::sBufferSize     , mAudioInput.getBufferSize());
    addElement( doc, fAudioInput, s::sBuffers        , mAudioInput.getBuffers());
    addElement( doc, fAudioInput, s::sBufferPoolSize , mAudioInput.getBufferPoolSize());
    addElement( doc, fAudioInput, s::sChannels       , mAudioInput.getChannels());

    fName = "valid number of input channels: ";
    fName += boost::lexical_cast<string, int>(mAudioInput.getAudioParameters().getMaxInputChannels()).c_str();
    fAudioInput.appendChild(doc.createComment(fName));

    addElement( doc, fAudioInput, s::sSampleFrequency, mAudioInput.getSampleFrequency());
    fName = "valid sample frequencies: ";
    for (double f : mAudioInput.getAudioParameters().getSupportedSampleFrequencies())
    {
        fName += boost::lexical_cast<string>(f).c_str();
        fName += ", ";
    }
    fAudioInput.appendChild(doc.createComment(fName));

    addElement( doc, fAudioInput, s::sShowFGinScope, mAudioSourceFG);

    addElement( doc, fAudioInput, s::sUpdateRate, mViewUpdateRate);
    addElement( doc, fAudioInput, s::sDeviceID  , mAudioInput.getDeviceID());
    fName = "input id range: 0 - ";
    fName += boost::lexical_cast<string, int>(mAudioInput.getAudioParameters().getNumberOfDevices()).c_str();
    fAudioInput.appendChild(doc.createComment(fName));

    addElement( doc, fAudioInput, s::sSampleFormat , mAudioInput.getSampleFormat());
    fName = "valid sample formats: "+ mAudioInput.getAudioParameters().getSupportedSampleFormatStrings();
    fAudioInput.appendChild(doc.createComment(fName));

    addElement( doc, fAudioInput, s::sSuggestedLatency, mAudioInput.getSuggestedLatency());
    addElementS(doc, fAudioInput, s::sName            , mAudioInput.getAudioParameters().getName());

    QDomElement fAudioOutput = doc.createElement(s::sAudioOutput);
    root.appendChild(fAudioOutput);
    addElement( doc, fAudioOutput, s::sChannels        , mAudioOutput.getChannels());
    addElement( doc, fAudioOutput, s::sDeviceID        , mAudioOutput.getDeviceID());
    addElement( doc, fAudioOutput, s::sSampleFrequency , mAudioOutput.getSampleFrequency());
    addElement( doc, fAudioOutput, s::sSampleFormat    , mAudioOutput.getSampleFormat());
    addElement( doc, fAudioOutput, s::sSuggestedLatency, mAudioOutput.getSuggestedLatency());
    addElementS(doc, fAudioOutput, s::sName            , mAudioOutput.getAudioParameters().getName());

    QDomElement fParameters = doc.createElement(s::sParameters);
    root.appendChild(fParameters);
    addElement(doc, fParameters, s::sFloatingAvgCount, mAvgTime.getNoOfAvgValues());
    if (ui->comboFFT->count())
    {
        addElement(doc, fParameters, s::sMinFFTOrder, ui->comboFFT->itemData(0).toInt());
        addElement(doc, fParameters, s::sMaxFFTOrder, ui->comboFFT->itemData(ui->comboFFT->count()-1).toInt());
        addElement(doc, fParameters, s::sFFTOrder   , ui->comboFFT->currentText().toInt());
        addElement(doc, fParameters, ui->checkFFT_Log->text(), ui->checkFFT_Log->isChecked());
    }

    QDomElement fVertical = doc.createElement(s::sVertical);
    root.appendChild(fVertical);
    addElement(doc, fVertical, s::sActiveChannel, mActiveVerticalChannel);
    addElement(doc, fVertical, s::sAutomatic    , mAutoLevelDetermination);

    for (size_t i=0; i<mChannel.size(); ++i)
    {
        fName.setNum(i+1);
        QDomElement channel = doc.createElement(s::sChannel+fName);
        root.appendChild(channel);
        Channel& c =  mChannel[i];
        addElement( doc, channel, s::sLevelPerDivision, c.getLevelPerDivision(false));
        addElement( doc, channel, s::sOffsetIndex     , c.getOffsetIndex());
        addElement( doc, channel, s::sOffset          , c.getOffset());
        addElement( doc, channel, s::sTriggerLevel    , c.getTriggerLevel());
        addElement( doc, channel, s::sVisible         , c.mVisible);
        QDomElement fAudioInput = doc.createElement(s::sAudioInput);
        channel.appendChild(fAudioInput);
        addElement( doc, fAudioInput, s::sScaleFactor , c.getAudioInputScale());
        addElement( doc, fAudioInput, s::sOffset      , c.getAudioInputOffset());
        addElement( doc, fAudioInput, s::sAmplifier   , c.getAudioScale());
        addElementS(doc, fAudioInput, s::sName        , c.mName);
    }

    {
        QDomElement fAmplifier = doc.createElement(s::sAmplifier);
        root.appendChild(fAmplifier);
        const int count = ui->comboVerticalChannelAmplification->count();
        bool ok;
        for (int i=0; i<count; ++i)
        {
            double fValue = ui->comboVerticalChannelAmplification->itemData(i).toDouble(&ok);
            if (ok)
            {
                addElement( doc, fAmplifier, s::sValue, fValue);
            }
        }
    }


    QDomElement fTime = doc.createElement(s::sTime);
    root.appendChild(fTime);
    addElement(doc, fTime, s::sBaseIndex    , ui->hslideTime->value());
    addElement(doc, fTime, s::sPositionIndex, ui->hslideTimePosition->value());
    addElement(doc, fTime, s::sAutomatic    , mAutoTimeDetermination);

    QDomElement fTrigger = doc.createElement(s::sTrigger);
    root.appendChild(fTrigger);
    addElement(doc, fTrigger, s::sType         , mTrigger.mType);
    addElement(doc, fTrigger, s::sActiveChannel, mTrigger.mActiveChannel);
    addElement(doc, fTrigger, s::sEdge         , mTrigger.mEdgeRising);
    addElement(doc, fTrigger, s::sHoldOff      , mTrigger.mHoldOffTime);
    addElement(doc, fTrigger, s::sDelay        , mTrigger.mDelay);
    addElement(doc, fTrigger, QString(s::sHoldOff)+s::sActive, mTrigger.mHoldOffType);
    addElement(doc, fTrigger, QString(s::sDelay)  +s::sActive, mTrigger.mDelayActive);

    QDomElement fFunctionGenerator = doc.createElement(s::sFunctionGenerator);
    root.appendChild(fFunctionGenerator);
    mFunctionGenerator.saveParameter(doc, fFunctionGenerator);

    QDomElement fColors = doc.createElement(s::sColors);
    root.appendChild(fColors);
    addElementS(doc, fColors, s::sGrid, ui->graphicsViewScope->getGridColor().name());
    addElementS(doc, fColors, s::sBackground, ui->graphicsViewScope->backgroundBrush().color().name());
    for (unsigned int i=0; i<mCurveColors.size(); ++i)
    {
        addElementS(doc, fColors, s::sChannel+QString::number(i+1), mCurveColors[i].name());
    }

    QDomElement fDebugInfo = doc.createElement(s::sDebugInfo);
    root.appendChild(fDebugInfo);
    addElement(doc, fDebugInfo, s::sTrace    , Logger::isSeverityActive(Logger::trace));
    addElement(doc, fDebugInfo, s::sDebug    , Logger::isSeverityActive(Logger::debug));
    addElement(doc, fDebugInfo, s::sInfo     , Logger::isSeverityActive(Logger::info));
    addElement(doc, fDebugInfo, s::sNotice   , Logger::isSeverityActive(Logger::notice));
    addElement(doc, fDebugInfo, s::sWarning  , Logger::isSeverityActive(Logger::warning));
    addElement(doc, fDebugInfo, s::sError    , Logger::isSeverityActive(Logger::error));
    addElement(doc, fDebugInfo, s::sCritical , Logger::isSeverityActive(Logger::critical));
    addElement(doc, fDebugInfo, s::sAlert    , Logger::isSeverityActive(Logger::alert    ));
    addElement(doc, fDebugInfo, s::sEmergency, Logger::isSeverityActive(Logger::emergency));
    addElement(doc, fDebugInfo, s::sToConsole, Logger::isSeverityActive(Logger::to_console));
    addElement(doc, fDebugInfo, s::sToSyslog , Logger::isSeverityActive(Logger::to_syslog));

    QString xml = doc.toString();
    QFile file;
    QDir fdir;
    QString fConfigPath = getConfigPath();
    bool fOk = fdir.mkpath(fConfigPath);
    if (fOk)
    {
        fOk = QDir::setCurrent(fConfigPath);
    }
    if (fOk)
    {
        file.setFileName(getXmlName());
        file.open(QFile::WriteOnly|QFile::Text);
        file.write(xml.toStdString().c_str());
        file.close();
    }
}

QString MainWindow::getConfigPath() const
{
    QString sTitle = windowTitle();
    sTitle.replace(" ", "");
    return QDir::homePath() + "/.config/" + sTitle;
}

QString MainWindow::getXmlName() const
{
#if (_USE_QT_AUDIO == 1)
    return "ConfigQAudio.xml";
#else
    return "ConfigPortAudio.xml";
#endif
}

void MainWindow::updateChannelColors()
{
    for (unsigned int i=0; i<mChannel.size(); ++i)
    {
        mChannel[i].mPen.setColor(mCurveColors[i]);
    }
    setWidgetColor(*ui->labelVertical, QPalette::WindowText, mChannel[mActiveVerticalChannel].mPen.color());
    setWidgetColor(*ui->labelTrigger , QPalette::WindowText, mChannel[mTrigger.mActiveChannel].mPen.color());
}

void MainWindow::setGridColor(QColor aColor)
{
    ui->graphicsViewFFT->setGridColor(aColor);
    ui->graphicsViewScope->setGridColor(aColor);
    ui->graphicsViewZoom->setGridColor(aColor);
}

void MainWindow::setBackgroundColor(QColor aColor)
{
    ui->graphicsViewScope->setBackgroundBrush(QBrush(aColor, Qt::SolidPattern));
    ui->graphicsViewZoom->setBackgroundBrush(QBrush(aColor, Qt::SolidPattern));
    ui->graphicsViewFFT->setBackgroundBrush(QBrush(aColor, Qt::SolidPattern));
}

QColor MainWindow::getGridColor() const
{
    return ui->graphicsViewScope->getGridColor();
}

QColor MainWindow::getBackgroundColor() const
{
    return ui->graphicsViewScope->backgroundBrush().color();
}

void MainWindow::on_updateViewByTimer()
{
    if (mScopeViewInvalid.testAndSetRelease(1, 0))
    {
        onRedrawScopeView(true);
    }
    if (mScopeViewInvalid.testAndSetRelease(2, 0))
    {
        onRedrawScopeView(false);
    }
}

void MainWindow::on_updateStatusbarByTimer()
{
    if (!mAudioInput.isRunning() && mAudioInput.getType() != QAudioRecord::eStopDevice)
    {
        TRACE(Logger::notice, "mAudioInput.start()\n");
        mAudioInput.start();
    }
    else if (!mAudioOutput.isRunning())
    {
        TRACE(Logger::notice, "mAudioOutput.start()\n");
        mAudioOutput.start();
    }
    if (mAudioInput.getLastError().size())
    {
        ui->statusBar->showMessage(mAudioInput.getLastError());

        TRACE(mAudioInput.isValid() ? Logger::notice : Logger::error, "%s\n", mAudioInput.getLastError().toStdString().c_str());
        mAudioInput.clearLastError();
    }
    else if (mAudioOutput.getLastError().size())
    {
        ui->statusBar->showMessage(mAudioOutput.getLastError());
        TRACE(mAudioOutput.isValid() ? Logger::notice : Logger::error, "%s\n", mAudioOutput.getLastError().toStdString().c_str());
        mAudioOutput.clearLastError();
    }
}

void MainWindow::timerEvent(QTimerEvent *aEvent)
{
    if (aEvent->timerId() == mTimerTriggered )
    {
        killTimer(mTimerTriggered);
        ui->ledTriggered->setChecked(false);
        if (isDisplayTriggered())
        {
            ui->ledTriggerArmed->setChecked(true);
        }
    }
    if (aEvent->timerId() == mTimerTriggerArmed )
    {
        killTimer(mTimerTriggerArmed);
        ui->ledTriggerArmed->setChecked(false);
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    update_scope_view();
}

void MainWindow::postBufferChanged(int aStartPosition)
{
    Q_EMIT doWork(aStartPosition);
}

void MainWindow::receive_subview_event(const QEvent* aEvent)
{
    if (aEvent->type() == MouseClickPosEvent::id())
    {
        const MouseClickPosEvent* fEvent = dynamic_cast<const MouseClickPosEvent*>(aEvent);
        mStatusX_Axis->setUnit(fEvent->getXunit());
        mStatusLevel->setUnit(fEvent->getYunit());
        if (fEvent->getXunit() == fEvent->getYunit())   // lissayous
        {
            Channel& aC1 = mChannel[mCombineCurves.getChannel1()];
            mStatusX_Axis->value(aC1.getLevelPerDivision() * ui->graphicsViewScope->getTimeDivisions() * (fEvent->getXvalue() - aC1.getOffset()));

            Channel& aC2 = mChannel[mCombineCurves.getChannel2()];
            mStatusLevel->value(aC2.getLevelPerDivision() * fEvent->getLevelDivFactor() * (fEvent->getYvalue() - aC2.getOffset()));
        }
        else
        {
            Channel& aC = mChannel[mActiveVerticalChannel];
            if (fEvent->isLeftButton())                 // level
            {
                mStatusX_Axis->value(fEvent->getXvalue());
                mStatusLevel->value(aC.getLevelPerDivision() * fEvent->getLevelDivFactor() * (fEvent->getYvalue() - aC.getOffset()));
            }
            else                                        // slope
            {
                mStatusX_Axis->value(fEvent->getXvalue());
                mStatusLevel->value(aC.getLevelPerDivision() * fEvent->getLevelDivFactor() * fEvent->getYvalue());
            }
        }
    }
    else if (aEvent->type() == SetLabelEvent::id())
    {
        const SetLabelEvent* fEvent = dynamic_cast<const SetLabelEvent*>(aEvent);
        if (fEvent->hideLabels())
        {
            hideLabels();
        }
        else if (fEvent->getView()->isVisible())
        {
            setLabel(fEvent->getPos(), fEvent->getVariant());
        }
    }
    else if (aEvent->type() == UpdateViewEvent::id())
    {
        if (mTrigger.mType == TriggerType::Stop)
        {
            update_scope_view();
        }
    }
}

void MainWindow::setLabel(const QPoint& aP, const QVariant& aValue)
{
    QDoubleLableWithUnit * pLabel = 0;
    if (mCurrentLabel < mLabels.size())
    {
        pLabel = mLabels[mCurrentLabel].data();
    }
    else
    {
        mLabels.push_back(QPointer<QDoubleLableWithUnit>(new QDoubleLableWithUnit(this)));
        pLabel = mLabels.rbegin()->data();
        pLabel->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    }
    ++mCurrentLabel;

    if (aValue.type() == QVariant::Double)
    {
        pLabel->value(aValue.toDouble());
    }
    else if (aValue.type() == QVariant::String)
    {
        pLabel->setText(aValue.toString());
    }
    QRect fRc(0, 0, 100, 30);
    fRc = pLabel->style()->itemTextRect(pLabel->fontMetrics(), fRc, 0, true, pLabel->text());

    pLabel->resize(fRc.width(), fRc.height());

    QPoint fP(aP.x() - fRc.width()/2, aP.y() + fRc.height() * 8 / 5);
    pLabel->move(fP);
    pLabel->setVisible(true);
}

void MainWindow::hideLabels()
{
    for (auto& fLabel : mLabels)
    {
        fLabel->setVisible(false);
    }
    mCurrentLabel = 0;
}


#if TRIGGER_PROCESSING == 1
void MainWindow::handleBufferChanged(int aStartPosition)
{
    int fOffsetTimeSteps  = ui->graphicsViewScope->getDrawnTimeSteps() * ui->hslideTimePosition->value() / mTimePositionFactor;
    if (mTrigger.mDelayActive)
    {
        fOffsetTimeSteps -= mAudioInput.getSampleFrequency() * mTrigger.mDelay;
    }

    int fBuffersDelay     = (ui->graphicsViewScope->getDrawnTimeSteps() + fOffsetTimeSteps) / mAudioInput.getBufferSize() + 1;
    if (mTrigger.mHoldOffType == TriggerParameter::HoldOff::Events)
    {
        ++fBuffersDelay;
    }
    int fBuffer           = aStartPosition / mAudioInput.getBufferSize();
    int fBufferbefore     = fBuffer > 0 ? fBuffer - 1 : mBufferUpdate.size() - 1;
    {
        int fLastBuffersDelay = mBufferUpdate[fBufferbefore];
        // TRACE(Logger::debug, "Start: %d -> %d, Buffer: %d, Delay: %d, last Delay: %d\n", aStartPosition, aStartPosition + mAudioInput.getBufferSize(), fBuffer, fBuffersDelay, fLastBuffersDelay);
        if (fLastBuffersDelay > 0 && fLastBuffersDelay <  fBuffersDelay)
        {
            int fDifference = fBuffersDelay - fLastBuffersDelay;
            transform(mBufferUpdate.begin(), mBufferUpdate.end(), mBufferUpdate.begin(), bind2nd(plus<int>(), fDifference));
            TRACE(Logger::debug, "Last: %d < %d, Difference: %d\n", fLastBuffersDelay, fBuffersDelay, fDifference);
        }
        else if (fLastBuffersDelay > fBuffersDelay)
        {
            int fDifference = fLastBuffersDelay - fBuffersDelay;
            transform(mBufferUpdate.begin(), mBufferUpdate.end(), mBufferUpdate.begin(), bind2nd(minus<int>(), fDifference));
            TRACE(Logger::debug, "Last: %d > %d, Difference: %d\n", fLastBuffersDelay, fBuffersDelay, fDifference);
        }

        mBufferUpdate[fBuffer] = fBuffersDelay+1;
        transform(mBufferUpdate.begin(), mBufferUpdate.end(), mBufferUpdate.begin(), bind2nd(minus<int>(), 1));
    }
    if (mTrigger.mType == TriggerType::Single)
    {
        if (mTrigger.mSingleTriggerDelayBuffers > 0)
        {
            if (--mTrigger.mSingleTriggerDelayBuffers == 0)
            {
                mScopeViewInvalid = 1;
            }
        }
        else if (searchForEdgeCrossingTriggerLevel(mTrigger.mActiveChannel, aStartPosition, mTrigger.mSingleTriggerPos, mTrigger.mEdgeRising, &Trigger::forward, &mTrigger.mSingleTriggerOffset))
        {
            mTrigger.mSingleTriggerDelayBuffers = fBuffersDelay+1;
            mTrigger.mSingleTriggerDelayBuffersFix = mTrigger.mSingleTriggerDelayBuffers;
            ui->ledTriggerArmed->setChecked(false);
            ui->ledTriggered->setChecked(true);

            initHoldOffTime(mTrigger.mSingleTriggerPos);
            initHoldOffEvents(mTrigger.mSingleTriggerPos);

            if (mTrigger.mDelayActive)
            {
                mTrigger.mSingleTriggerPos -= mAudioInput.getSampleFrequency() * mTrigger.mDelay;
            }
        }
    }
    else
    {
        ui->ledTriggerArmed->setChecked(true);
        mScopeViewInvalid = 1;
    }
}
void MainWindow::determineMinMaxLevel(int start_pos, int stop_pos)
{
    for (int c=0; c < mCombineCurves.getMathChannelIndex(); ++c)
    {
        Channel& fChannel = mChannel[c];
        const circlevector<double>& fValues = static_cast<const circlevector<double>&>(mAudioInput.getValues(c));
        auto start = fValues.begin(start_pos);
        auto stop  = fValues.begin(stop_pos);

#if _MSVC_STL_VERSION >= 141

        double fMin = *start;
        double fMax = fMin;
        for (;start != stop; ++start)
        {
            fMin = min(fMin, *start);
            fMax = max(fMax, *start);
        }
#elif __cplusplus >= 201103
        auto   element = std::minmax_element(start, stop);
        double fMin = *element.first;
        double fMax = *element.second;
#else
        double fMin = *min_element(start, stop);
        double fMax = *max_element(start, stop);
#endif
        fChannel.setMinMaxValue(fMin, fMax);
        if (mScopeSettings.isVisible() && c == mTrigger.mActiveChannel)
        {
            mScopeSettings.setValue(fMin, ScopeSettings::measured_min);
            mScopeSettings.setValue(fMax, ScopeSettings::measured_max);
        }
    }
}

void MainWindow::onRedrawScopeView(bool aNewBuffer)
{
    TRACE(Logger::trace, "onRedrawScopeView(%d)\n", aNewBuffer);
    if (mAudioInput.isRunning() && mAudioInput.isValid())
    {
        // TODO why double called
        // ui->graphicsViewScope->setTimePerDivision(mTimePerDivision[ui->hslideTime->value()]);
        if (ui->labelTimePositionValue->isInvalid())
        {
            on_hslideTimePosition_valueChanged(ui->hslideTimePosition->value());
        }
        vector<int>::iterator fZero = mBufferUpdate.end();
        if (aNewBuffer)
        {
            fZero = std::find(mBufferUpdate.begin(), mBufferUpdate.end(), 0);
        }

        int    fSearchStart       = 0;
        int    fStartPosition     = 0;
        double fTriggerTimeOffset = 0;
        if (!aNewBuffer) // just update display with new settings
        {
            fSearchStart       = mLastBufferPosition;
            fTriggerTimeOffset = mLastTriggerOffset;
        }
        else if (isDisplayTriggered() && fZero != mBufferUpdate.end())
        {
            int fPos       = fZero - mBufferUpdate.begin();
            int fBuffers   = count_if(mBufferUpdate.begin(), mBufferUpdate.end(), bind2nd(greater<int>(), 0)) + 1;
            fStartPosition = fPos * mAudioInput.getBufferSize();

            if (Logger::isSeverityActive(Logger::trace))
            {
                TRACE(Logger::trace, "Update start (%d, %d), > 0: %d, < 0: %d, = 0: %d\n", fPos, fPos*mAudioInput.getBufferSize(), fBuffers,
                      count_if(mBufferUpdate.begin(), mBufferUpdate.end(), bind2nd(less<int>(), 0)),
                      count_if(mBufferUpdate.begin(), mBufferUpdate.end(), bind2nd(equal_to<int>(), 0)));
            }

            determineMinMaxLevel(fStartPosition, fStartPosition + mAudioInput.getBufferSize() * fBuffers);
            if (mCalibrationDlg.isVisible())
            {
                mCalibrationDlg.updateParameters();
            }

            int fHoldOff = checkHoldOffTime();
            if (fHoldOff == -1)
            {
                return;
            }
            if (checkHoldOffEvents(fSearchStart))
            {
                return;
            }

            fSearchStart = fStartPosition + fHoldOff;

//            ui->ledTriggerArmed->setChecked(true);
//            mTimerTriggerArmed = startTimer( 100 );
            int  fFoundEdge = 0;
            if (searchForEdgeCrossingTriggerLevel(mTrigger.mActiveChannel, fSearchStart, fFoundEdge, mTrigger.mEdgeRising, &Trigger::forward, &fTriggerTimeOffset))
            {
                ui->ledTriggerArmed->setChecked(false);
                ui->ledTriggered->setChecked(true);
                mTimerTriggered = startTimer( 100 );

                fSearchStart = fFoundEdge;
                determineAutomaticTime(fSearchStart, fTriggerTimeOffset);

                initHoldOffTime(fSearchStart);
                initHoldOffEvents(fSearchStart);

                if (mTrigger.mDelayActive)
                {
                    fSearchStart -= mAudioInput.getSampleFrequency() * mTrigger.mDelay;
                }

                mLastBufferPosition = fSearchStart;
                mLastTriggerOffset  = fTriggerTimeOffset;
            }
            else
            {
                if (mTrigger.mType == TriggerType::Normal || mTrigger.mHoldOffType == TriggerParameter::HoldOff::Time)
                {
                    return;
                }
                mLastBufferPosition = fStartPosition;
                mLastTriggerOffset  = 0.0;
            }
        }
        else if (mTrigger.mType == TriggerType::Single && mTrigger.mSingleTriggerDelayBuffers == 0)
        {
            mAudioInput.suspend();
            fSearchStart       = mTrigger.mSingleTriggerPos;
            fTriggerTimeOffset = mTrigger.mSingleTriggerOffset;
            determineMinMaxLevel(fSearchStart, fSearchStart + mAudioInput.getBufferSize() * mTrigger.mSingleTriggerDelayBuffersFix);
            determineAutomaticTime(fSearchStart, fTriggerTimeOffset, false);

            mTrigger.mType = TriggerType::Stop;
            ui->groupTrigger->setCurrentIndex(mTrigger.mType);

            mLastBufferPosition = fSearchStart;
            mLastTriggerOffset  = fTriggerTimeOffset;
        }
        else if (mTrigger.mType == TriggerType::Manual)
        {
            mTrigger.mType = static_cast<TriggerType::eType>(ui->groupTrigger->getCurrentIndex());
            fSearchStart   = distance(mBufferUpdate.begin(), fZero) * mAudioInput.getBufferSize();
        }
        else if (mTrigger.mType == TriggerType::Off)
        {
            mLastBufferPosition = fSearchStart       = distance(mBufferUpdate.begin(), fZero) * mAudioInput.getBufferSize();
            mLastTriggerOffset  = fTriggerTimeOffset = 0;
        }
        else
        {
            return;
        }

        // Update ScopeWindows
        ui->graphicsViewScope->scene()->clear();
        ui->graphicsViewScope->setTimePerDivision(mTimePerDivision[ui->hslideTime->value()]);

        double fTimeOffset = static_cast<double>(ui->hslideTimePosition->value()) / static_cast<double>(mTimePositionFactor);
        fTimeOffset       *= ui->graphicsViewScope->getTotalTime();
        ui->graphicsViewScope->setTriggerStartTime(fTimeOffset);
        fSearchStart -= fTimeOffset / ui->graphicsViewScope->getTimeStep();

        const Channel& fActiveChannel = mChannel[mTrigger.mActiveChannel];

        ui->graphicsViewScope->setTriggerLevel(fActiveChannel.getTriggerLevel() / fActiveChannel.getLevelPerDivision() + fActiveChannel.getOffset() * ui->graphicsViewScope->getLevelDivisions() );
        ui->graphicsViewScope->drawDivision();

        if (!ui->graphicsViewFFT->isHidden())
        {
            ui->graphicsViewFFT->scene()->clear();
            ui->graphicsViewFFT->drawDivision();
        }

        if (!ui->graphicsViewZoom->isHidden())
        {
            ui->graphicsViewZoom->scene()->clear();
            ui->graphicsViewZoom->setTimePerDivision(mTimePerDivision[ui->hslideTime->value()]);
            ui->graphicsViewZoom->setTriggerStartTime(fTimeOffset);
            ui->graphicsViewZoom->setTriggerLevel(fActiveChannel.getTriggerLevel() / fActiveChannel.getLevelPerDivision() + fActiveChannel.getOffset() * ui->graphicsViewScope->getLevelDivisions() );
            ui->graphicsViewZoom->drawDivision();
        }

        TRACE(Logger::debug, "RedrawView: start: %d; trigger: %d; end: %d; offset0: %f; new: %d\n", fStartPosition, fSearchStart, fSearchStart + ui->graphicsViewScope->getDrawnTimeSteps(), fTriggerTimeOffset, aNewBuffer);
        for (int c=0; c<mCombineCurves.getMathChannelIndex(); ++c)
        {
            determineAutomaticLevel(c);
            const vector<double>& fValues = mAudioInput.getValues(c);
            if (!ui->graphicsViewScope->isLissayousView())
            {
                ui->graphicsViewScope->addPolygon(fValues, mChannel[c], fTriggerTimeOffset, fSearchStart);
            }
            if (!ui->graphicsViewZoom->isHidden())
            {
                ui->graphicsViewZoom->addPolygon(fValues, mChannel[c], fTriggerTimeOffset, fSearchStart);
            }
            if (!ui->graphicsViewFFT->isHidden())
            {
                ui->graphicsViewFFT->addPolygon(fValues, mChannel[c], fSearchStart);
            }
        }

        std::vector<double> fCalculatedCurve;
        if (mCombineCurves.combineCurves(
            mAudioInput.getValues(mCombineCurves.getChannel1()),
            mAudioInput.getValues(mCombineCurves.getChannel2()),
            fCalculatedCurve, fSearchStart, fSearchStart+ui->graphicsViewScope->getDrawnTimeSteps()+2))
        {
            ui->graphicsViewScope->addPolygon(fCalculatedCurve, mChannel[mCombineCurves.getMathChannelIndex()], fTriggerTimeOffset, 1);
        }

        if (mCopyToClipboard)
        {
            QString fClpTxt;
            int fSteps = ui->graphicsViewScope->getDrawnTimeSteps();
            int fSize  = mAudioInput.getValues(0).size();
            for (int i=0, j=fSearchStart; i<fSteps; ++i, j++)
            {
                if (j == fSize) j = 0;
                for (int c=0; c<mCombineCurves.getMathChannelIndex(); ++c)
                {
                    fClpTxt += QString::number(mAudioInput.getValues(c)[j]);
                    fClpTxt += "\t";
                }
                fClpTxt += "\n";
            }
            mApp.clipboard()->setText(fClpTxt);
            mCopyToClipboard = false;
        }

        if (ui->graphicsViewScope->isLissayousView())
        {
            ui->graphicsViewScope->addLissajous(
                mAudioInput.getValues(mCombineCurves.getChannel1()), mChannel[mCombineCurves.getChannel1()],
                mAudioInput.getValues(mCombineCurves.getChannel2()), mChannel[mCombineCurves.getChannel2()],
                fStartPosition, fStartPosition + mAudioInput.getBufferSize());
        }
    }
}

void MainWindow::determineAutomaticTime(int fSearchStart, double fTriggerTimeOffset, bool search_backward)
{
    if (mAutoTimeDetermination || mScopeSettings.isVisible())
    {
        double fTriggerTimeOffset2 = 0;
        int    fFoundEdge          = 0;
        const int startoffset = search_backward ? -1 : 1;
        auto*  trigger_search = search_backward ? &Trigger::backward : &Trigger::forward;
        if (searchForEdgeCrossingTriggerLevel(mTrigger.mActiveChannel, fSearchStart + startoffset, fFoundEdge,
                                              !mTrigger.mEdgeRising, trigger_search, &fTriggerTimeOffset2))
        {
            int fSteps = search_backward ? fSearchStart - fFoundEdge : fFoundEdge - fSearchStart;
            if (fSteps < 0)
            {
                fSteps = fSearchStart + (mAudioInput.getValues(0).size() - fFoundEdge);
            }

            double fTime = (fSteps + fTriggerTimeOffset - fTriggerTimeOffset2 - 1.0) * ui->graphicsViewScope->getTimeStep();

            mAvgTime.addValue(fTime);
            fTime = mAvgTime.getAverage();

            if (mScopeSettings.isVisible())
            {
                fSearchStart = fFoundEdge;
                fTriggerTimeOffset = fTriggerTimeOffset2;
                if (searchForEdgeCrossingTriggerLevel(mScopeSettings.getPhaseChannel(), fSearchStart + startoffset, fFoundEdge,
                                                      !mTrigger.mEdgeRising, trigger_search, &fTriggerTimeOffset2))
                {
                    int fPhaseOffset = search_backward ? fSearchStart - fFoundEdge : fFoundEdge - fSearchStart;
                    if (fPhaseOffset < fSteps)
                    {
                        double fPhaseOffsetTime = (fPhaseOffset + fTriggerTimeOffset - fTriggerTimeOffset2 - 1.0) * ui->graphicsViewScope->getTimeStep();
                        mScopeSettings.setValue(fPhaseOffsetTime, ScopeSettings::measured_time_offset);
                        double fPhase = fPhaseOffsetTime / fTime * 360;
                        mScopeSettings.setValue(fPhase, ScopeSettings::measured_phase);
                    }
                }
                // TODO: validate measured values for backward an forward search
                mScopeSettings.setValue(fTime, ScopeSettings::measured_period);

                const circlevector<double>& fValues = static_cast<const circlevector<double>&>(mAudioInput.getValues(mTrigger.mActiveChannel));
                double fSum = 0;
                double fSquareSum = 0;
                auto fValue = fValues.begin(fFoundEdge);
                auto fEnd   = fValues.begin(fFoundEdge+fSteps);

                for (; fValue != fEnd; ++fValue)
                {
                    fSum       += fabs(*fValue);
                    fSquareSum += (*fValue * *fValue);
                }

                if (fSteps > 0)
                {
                    fSum /= fSteps;
                    fSquareSum = sqrt(fSquareSum/fSteps);
                }
                mScopeSettings.setValue(fSum      , ScopeSettings::calculated_avg);
                mScopeSettings.setValue(fSquareSum, ScopeSettings::calculated_rms);
            }

            if (mAutoTimeDetermination)
            {
                fTime /= ui->graphicsViewScope->getTimeDivisions();
                unsigned int i;
                for ( i = 1; i < mTimePerDivision.size(); ++i)
                {
                    if (fTime > mTimePerDivision[i])
                    {
                        --i;
                        break;
                    }
                }

                unsigned int fOldValue = ui->hslideTime->value();
                if (fOldValue != i)
                {
                    double fDiff = fabs(mTimePerDivision[fOldValue] - fTime);
                    if (fDiff > 0.05*mTimePerDivision[fOldValue])
                    {
                        ui->hslideTime->setValue(i);
                    }
                }
            }
        }
    }
}

void MainWindow::initHoldOffTime(int aSearchStart)
{
    if (mTrigger.mHoldOffType == TriggerParameter::HoldOff::Time)
    {
        mTrigger.mHoldOffPosition = aSearchStart % mAudioInput.getBufferSize() + mAudioInput.getSampleFrequency() * mTrigger.mHoldOffTime - mAudioInput.getBufferSize();
        TRACE(Logger::trace, "HO start time: %f s : %d, %f \n", mTrigger.mHoldOffTime, mTrigger.mHoldOffPosition, mTrigger.mHoldOffPosition / mAudioInput.getSampleFrequency());
    }
}

int  MainWindow::checkHoldOffTime()
{
    int    fHoldOff = 0;
    if (mTrigger.mHoldOffType == TriggerParameter::HoldOff::Time && mTrigger.mHoldOffPosition != InvalidIndex)
    {
        if (mTrigger.mHoldOffPosition < mAudioInput.getBufferSize())
        {
            fHoldOff = mTrigger.mHoldOffPosition;
            mTrigger.mHoldOffPosition = InvalidIndex;
        }
        else
        {
            mTrigger.mHoldOffPosition -= mAudioInput.getBufferSize();
            fHoldOff = -1;
        }
    }
    return fHoldOff;
}

void MainWindow::initHoldOffEvents(int aStart)
{
    if (mTrigger.mHoldOffType == TriggerParameter::HoldOff::Events)
    {
        mTrigger.mHoldOffPosition = mTrigger.mHoldOffEvents;
        int fBuffersize = mAudioInput.getBufferSize();
        while (mTrigger.mHoldOffPosition > 0)
        {
            int    fFoundEdge = 0;
            if (searchForEdgeCrossingTriggerLevel(mTrigger.mActiveChannel, aStart, fFoundEdge, mTrigger.mEdgeRising, &Trigger::forward, 0, &fBuffersize))
            {
                TRACE(Logger::trace, "initHoldOffEvents(%d): %d, %d, %d\n", aStart, fFoundEdge, mTrigger.mHoldOffPosition, fBuffersize);
                --mTrigger.mHoldOffPosition;
                aStart = fFoundEdge;
            }
            else
            {
                break;
            }
        }
    }
}

bool MainWindow::checkHoldOffEvents(int& aStart)
{
    bool fReturn = false;
    if (mTrigger.mHoldOffType == TriggerParameter::HoldOff::Events)
    {
        int fBuffersize = mAudioInput.getBufferSize();
        while (mTrigger.mHoldOffPosition > 0)
        {
            int    fFoundEdge = 0;
            if (searchForEdgeCrossingTriggerLevel(mTrigger.mActiveChannel, aStart, fFoundEdge, mTrigger.mEdgeRising, &Trigger::forward, 0, &fBuffersize))
            {
                --mTrigger.mHoldOffPosition;
                aStart = fFoundEdge;
            }
            else
            {
                break;
            }
        }
        fReturn = mTrigger.mHoldOffPosition > 0;
    }
    return fReturn;
}

#endif

void MainWindow::on_vslideLevel_valueChanged(int value)
{
    if (mChanging != action::vslideLevel)
    {
        double fLevelPerDiv = mLevelPerDivision[value];
        ui->labelLevelValue->value(fLevelPerDiv * mChannel[mActiveVerticalChannel].getAudioScale());
        mChannel[mActiveVerticalChannel].setLevelPerDivision() = fLevelPerDiv;
        update_scope_view();
    }
}

void MainWindow::on_vslideOffset_valueChanged(int value)
{
    mChannel[mActiveVerticalChannel].setOffset() = value / static_cast<double>(mOffsetFactor);
    mChannel[mActiveVerticalChannel].setOffsetIndex() = value;
    ui->labelOffsetValue->setText(QString::number(mChannel[mActiveVerticalChannel].getOffset()));
    update_scope_view();
}

void MainWindow::on_comboVerticalChannel_currentIndexChanged(int index)
{
    if (index < 0 || index >= static_cast<int>(mChannel.size()))
    {
        index = 0;
    }
    if (   mActiveVerticalChannel != InitializationIndex
        && mActiveVerticalChannel != index
        && index < static_cast<int>(mChannel.size()))
    {
        mActiveVerticalChannel = index;
        mScopeSettings.setActiveChannel(index);
        Channel& fActiveChannel = mChannel[mActiveVerticalChannel];
        Cursors::getPen().setColor(fActiveChannel.mPen.color());
        Cursors::getPen().setWidth(0);
        ui->vslideOffset->setValue(fActiveChannel.getOffsetIndex());
        ui->labelOffsetValue->setText(QString::number(fActiveChannel.getOffset()));
        int flevelindex = -1;
        double fLevelPerDiv = fActiveChannel.getLevelPerDivision(false);
        for (double fLevel : mLevelPerDivision)
        {
            ++flevelindex;
            if (fabs(fLevel-fLevelPerDiv) < 1e-6)
            {
                mChanging = action::vslideLevel;
                ui->vslideLevel->setValue(flevelindex);
                mChanging = action::None;
                ui->labelLevelValue->value(fActiveChannel.getLevelPerDivision());
                break;
            }
        }

        ui->checkChannelVisible->setChecked(fActiveChannel.mVisible);
        setWidgetColor(*ui->labelVertical, QPalette::WindowText, fActiveChannel.mPen.color());
        const int count = ui->comboVerticalChannelAmplification->count();
        bool ok;
        for (int i=0; i<count; ++i)
        {
            double fValue = ui->comboVerticalChannelAmplification->itemData(i).toDouble(&ok);
            if (fValue == fActiveChannel.getAudioScale())
            {
                mChanging = action::ComboVerticalChannelAmplification;
                ui->comboVerticalChannelAmplification->setCurrentIndex(i);
                mChanging = action::None;
                break;
            }
        }
    }
}

void MainWindow::on_comboVerticalChannelAmplification_currentIndexChanged(int index)
{
    if (   mActiveVerticalChannel != InitializationIndex
        && mChanging != action::ComboVerticalChannelAmplification
        && index >= 0 && index < ui->comboVerticalChannelAmplification->count())
    {
        Channel& fActiveChannel = mChannel[mActiveVerticalChannel];
        fActiveChannel.setAudioScale() = ui->comboVerticalChannelAmplification->itemData(index).toDouble();
        ui->labelLevelValue->value(fActiveChannel.getLevelPerDivision());
    }

}

void MainWindow::on_hslideTime_valueChanged(int value)
{
    ui->labelTimeValue->value(mTimePerDivision[value]);
    ui->labelTimePositionValue->invalidate();
    update_scope_view();
    mScopeSettings.setTimePrefix(ui->labelTimeValue->getPrefix().getPrefix());
}

void MainWindow::on_hslideTimePosition_valueChanged(int value)
{
    double fOffset = ui->graphicsViewScope->getTotalTime() * static_cast<double>(value) / static_cast<double>(mTimePositionFactor);
    ui->labelTimePositionValue->value(fOffset);
    update_scope_view();
}

void MainWindow::on_pushAutoLevel_clicked(bool checked)
{
    mAutoLevelDetermination = checked;
}

void MainWindow::on_pushAutoTime_clicked(bool checked)
{
    mAutoTimeDetermination = checked;
}

void MainWindow::on_comboTriggerChannel_currentIndexChanged(int index)
{
    if (mTrigger.mActiveChannel != InitializationIndex && mTrigger.mActiveChannel != index)
    {
        const Channel& fActiveChannel = mChannel[index];
        mTrigger.mActiveChannel = index;
        int value = static_cast<int>(0.5 + fActiveChannel.getTriggerLevel() /
           (fActiveChannel.getLevelPerDivision() * ui->graphicsViewScope->getLevelDivisions()) *
                                     static_cast<double>(mTriggerLevelFactor) );
        ui->vslideTriggerLevel->setValue(value);
        ui->labelTriggerLevelValue->setText(QString::number(fActiveChannel.getTriggerLevel()));
        setWidgetColor(*ui->labelTrigger, QPalette::WindowText, fActiveChannel.mPen.color());
    }
}

void MainWindow::on_vslideTriggerLevel_valueChanged(int value)
{
    double fTriggerLevelNormalized = static_cast<double>(value) / static_cast<double>(mTriggerLevelFactor);
    Channel& fActiveChannel = mChannel[mTrigger.mActiveChannel];
    fActiveChannel.setTriggerLevel() = fTriggerLevelNormalized * fActiveChannel.getLevelPerDivision() * ui->graphicsViewScope->getLevelDivisions();
    ui->labelTriggerLevelValue->value(fActiveChannel.getTriggerLevel());
    update_scope_view();
}

void  MainWindow::update_scope_view(Cursors::eCursor, Cursors::eView )
{
    if (mScopeViewInvalid != 1) mScopeViewInvalid = 2;
}


void MainWindow::on_checkChannelVisible_clicked(bool checked)
{
    Channel& fActiveChannel = mChannel[mActiveVerticalChannel];
    fActiveChannel.mVisible = checked;
}

void MainWindow::on_pushTrigger50Percent_clicked()
{
    Channel& fActiveChannel = mChannel[mTrigger.mActiveChannel];

    double fMinVal = fActiveChannel.getAvgMinValue();
    double fMaxVal = fActiveChannel.getAvgMaxValue();
    double fLevel  = fMaxVal - fMinVal;
    double fCenter = 0.5 * (fMaxVal + fMinVal);
    fCenter /= (0.5*fLevel);
    int index = fCenter * mTriggerLevelFactor;
    ui->vslideTriggerLevel->setValue(index);
}

void MainWindow::click_radio_trigger(int index)
{
    mTrigger.mType = static_cast<TriggerType::eType>(index);
    TRACE(Logger::trace, "trigger: %d\n", index);
    switch (mTrigger.mType)
    {
    case TriggerType::Single:
        mTrigger.mSingleTriggerDelayBuffers = -1;
        ui->ledTriggerArmed->setChecked(true);
        ui->ledTriggered->setChecked(false);
        mAudioInput.resume();
        break;

    case TriggerType::Normal: case TriggerType::Auto:
        ui->ledTriggerArmed->setChecked(true);
        ui->ledTriggered->setChecked(false);
        mAudioInput.resume();
        break;

    case TriggerType::Off:
        mAudioInput.resume();
        break;
    case TriggerType::Stop:
        mAudioInput.suspend();
        break;
    case TriggerType::Manual: break;
    }
    ui->pushTriggerManual->setEnabled(mTrigger.mType == TriggerType::Single || mTrigger.mType == TriggerType::Normal);
}

void MainWindow::on_pushTriggerManual_clicked()
{
    mTrigger.mType = TriggerType::Manual;
}

void MainWindow::on_clickedMathCombine(int index)
{
    ui->graphicsViewScope->setLissajousView(index == CombineCurves::lissajous);
    if (index != CombineCurves::lissajous && index != CombineCurves::none)
    {
        if (ui->comboVerticalChannel->count() == mCombineCurves.getMathChannelIndex())
        {
            QString s;
            s.setNum(static_cast<ulong>(mCombineCurves.getMathChannelIndex()+1));
            ui->comboVerticalChannel->addItem(s);
        }
    }
    else if (ui->comboVerticalChannel->count() != mCombineCurves.getMathChannelIndex())
    {
        ui->comboVerticalChannel->removeItem(mCombineCurves.getMathChannelIndex());
    }
}

void MainWindow::on_pushTriggerEdge_clicked(bool checked)
{
    mTrigger.mEdgeRising = checked;
}

bool MainWindow::isDisplayTriggered()
{
    switch (mTrigger.mType)
    {
    case TriggerType::Auto:
    case TriggerType::Normal:
        return true;
    default:
        return false;
    }
}

void MainWindow::determineAutomaticLevel(int aC)
{
    if (mAutoLevelDetermination)
    {
        Channel& fChannel = mChannel[aC];
        double dLevel =  (fChannel.getAvgMaxValue() - fChannel.getAvgMinValue()) /
                (ui->graphicsViewScope->getLevelDivisions() * fChannel.getAudioScale());
        for (unsigned int i = 1; i < mLevelPerDivision.size(); ++i)
        {
            if (dLevel > mLevelPerDivision[i])
            {
                --i;
                fChannel.setLevelPerDivision() = mLevelPerDivision[i];
                if (aC == mTrigger.mActiveChannel)
                {
                    ui->vslideLevel->setValue(i);
                }
                break;
            }
        }
    }
}

bool MainWindow::searchForEdgeCrossingTriggerLevel(int aChannel, int aStartPoint, int &aIndexEdge, bool aTriggerEdgeRising, void (*aModify)(Trigger &), double *aOffset/*=NULL*/, int* aBuffersize/*=0*/)
{
    if (aChannel < 0 || aChannel >= mAudioInput.getChannels())
    {
        return false;
    }
    bool   fFound = false;
    const circlevector<double>& fValues = static_cast<const circlevector<double>&>(mAudioInput.getValues(aChannel));
    const Channel& fActiveChannel = mChannel[aChannel];
    int    fBufferSize = mAudioInput.getBufferSize();
    if (aBuffersize)
    {
        fBufferSize = *aBuffersize;
    }

    Trigger fTE(fActiveChannel.getTriggerLevel(), fValues.begin(aStartPoint));

    bool (*fFindEdge)( Trigger&) = aTriggerEdgeRising ? &Trigger::isRising : &Trigger::isFalling;

    if (mTrigger.mSlopeActive)
    {
        fTE.slope = aTriggerEdgeRising ? mTrigger.mSlope : - mTrigger.mSlope;
        fTE.inv_timestep =  mAudioInput.getSampleFrequency();
        fFindEdge = Trigger::isSlope;
    }

    int j;
    for ( j = 0; j < fBufferSize; ++j)
    {
        aModify(fTE);
        if (fFindEdge(fTE))
        {
            aIndexEdge = distance(fValues.begin(), fTE.current);
            if (j==0)
            {
                // TODO find bug: when buffer wraps, or offset is wrong
                TRACE(Logger::info, "j==%d: %d: %f, %f, %f\n", j, aIndexEdge, *fTE.current, fTE.edge, *(fTE.last));
            }
            if (aOffset)
            {
                if (mTrigger.mSlopeActive)
                {
                    *aOffset = fTE.offset;
                }
                else if (aTriggerEdgeRising)
                {
                    *aOffset = (fTE.edge     - *fTE.last) / (*fTE.current - *fTE.last);
                }
                else
                {
                    *aOffset = (*fTE.current - fTE.edge)  / (*fTE.current - *fTE.last);
                }
            }
            fFound = true;
            break;
        }
        fTE.updateTrigger();
    }
    if (aBuffersize)
    {
        *aBuffersize -= j;
    }
    return fFound;
}


void MainWindow::on_checkFFT_Log_clicked(bool aChecked)
{
    ui->graphicsViewFFT->setLogarithmic(aChecked);
}

void MainWindow::on_checkShowFFT_clicked(bool checked)
{
    if (checked)
    {
        ui->checkShowZoom->setChecked(false);
        on_checkShowZoom_clicked(false);
        ui->graphicsViewFFT->show();
        ui->graphicsViewFFT->updateFrequencyDivision();
        ui->comboFFT->setEnabled(true);
        ui->checkFFT_Log->setEnabled(true);
    }
    else
    {
        hideLabels();
        ui->graphicsViewFFT->hide();
        ui->comboFFT->setEnabled(false);
        ui->checkFFT_Log->setEnabled(false);
    }
    update_scope_view();
}

void MainWindow::on_checkShowZoom_clicked(bool checked)
{
    if (checked)
    {
        ui->checkShowFFT->setChecked(false);
        on_checkShowFFT_clicked(false);
        ui->graphicsViewZoom->show();
    }
    else
    {
        ui->graphicsViewZoom->hide();
    }
    update_scope_view();
}

void MainWindow::on_comboFFT_currentIndexChanged(int index)
{
    if (index >= 0)
    {
        ui->graphicsViewFFT->setFFT_Parameter(mAudioInput.getSampleFrequency(), ui->comboFFT->itemText(index).toInt());
    }
}


#if TRIGGER_PROCESSING == 2
void MainWindow::handleBufferChanged(int aStartPosition)
{
    // determine trigger points
    int       fInputBufferSize = mAudioInput.getBufferSize();
    const int fEndPosition     = aStartPosition + fInputBufferSize;
    int       fStartPosition   = aStartPosition;
    int       fFoundEdge       = 0;
    double    fTriggerTimeOffset = 0;

    if (fStartPosition > 0)
    {
        fStartPosition--;
    }


    QMutexLocker fLock(&mUpdateMutex);
    const unsigned int maxsize = 200;
    if (mTriggerPointsList.size() > maxsize)
    {
        int fToDelete = mTriggerPointsList.size() - maxsize;
        mTriggerPointsList.erase(mTriggerPointsList.begin(), mTriggerPointsList.begin()+fToDelete);
    }

    int fFoundTriggers = 0;

    while (isDisplayTriggered() && fInputBufferSize > 0
           && searchForEdgeCrossingTriggerLevel(mTrigger.mActiveChannel, fStartPosition, fFoundEdge,
              mTrigger.mEdgeRising, &Trigger::forward, &fTriggerTimeOffset, &fInputBufferSize))
    {
        mTriggerPointsList.push_back(TriggerPt(fFoundEdge, fTriggerTimeOffset, checkHoldOff(fFoundEdge)));
        ++fFoundTriggers;

        fInputBufferSize = fEndPosition - fFoundEdge;
        fStartPosition   = fFoundEdge + 1;
    }

    if (fFoundTriggers == 0 && mTrigger.mType == TriggerType::Auto)
    {
        mTriggerPointsList.push_back(TriggerPt(aStartPosition, 0));
    }

    mLatestStartPosition = aStartPosition;
    fLock.unlock();

    mScopeViewInvalid = 1;
}


bool MainWindow::determineTrigger()
{
    int aStartPosition = mLatestStartPosition;
    // determine average channel levels
    for (int c=0; c < mCombineCurves.getMathChannelIndex(); ++c)
    {
        Channel& fChannel = mChannel[c];
        const circlevector<double>& fValues = static_cast<const circlevector<double>&>(mAudioInput.getValues(c));
        circlevector<double>::const_circlevector_iterator start = fValues.begin(aStartPosition);
        circlevector<double>::const_circlevector_iterator stop  = fValues.begin(aStartPosition + mAudioInput.getBufferSize());
        double fMin = *min_element(start, stop);
        double fMax = *max_element(start, stop);
        fChannel.setMinMaxValue(fMin, fMax);

        if (mScopeSettings.isVisible() && c == mTrigger.mActiveChannel)
        {
            mScopeSettings.setValue(fMin, ScopeSettings::measured_min);
            mScopeSettings.setValue(fMax, ScopeSettings::measured_max);
        }
    }

    if (mCalibrationDlg.isVisible())
    {
        mCalibrationDlg.updateParameters();
    }


    int       fDrawnTimeSteps  = ui->graphicsViewScope->getDrawnTimeSteps();

    const int fEndPosition     = aStartPosition + mAudioInput.getBufferSize();
    double fTimeOffset = ui->graphicsViewScope->getTotalTime() * static_cast<double>(ui->hslideTimePosition->value()) / static_cast<double>(mTimePositionFactor);
    fDrawnTimeSteps -= round(fTimeOffset / ui->graphicsViewScope->getTimeStep());

    // Trigger evaluation
    bool      fUpdateView     = false;

    // limit size of triggerpoint list
    QMutexLocker fLock(&mUpdateMutex);

    if (mTrigger.mType == TriggerType::Manual || mTrigger.mType == TriggerType::Off)
    {
        mTrigger.mType = static_cast<TriggerType::eType>(ui->groupTrigger->getCurrentIndex());
        mTriggerPoint  = TriggerPt(fEndPosition - fDrawnTimeSteps);
        fUpdateView = true;
    }
    else if (isDisplayTriggered())
    {
        // determine first trigger point, that is early enough for the total display
        const int fTotalInputBuffersize = mAudioInput.getTotalBufferSize();
        IntToDoubleVector::reverse_iterator fIter;
        for (fIter = mTriggerPointsList.rbegin(); fIter != mTriggerPointsList.rend(); ++fIter)
        {
            if (!fIter->mValid) continue;
            int fSteps = fEndPosition - fIter->mPosition;
            if (fSteps < 0)
            {
                fSteps = fEndPosition + (fTotalInputBuffersize - fIter->mPosition);
            }
            if (fSteps > fDrawnTimeSteps)
            {
                break;
            }
        }

        if (fIter != mTriggerPointsList.rend())
        {
            fLock.unlock();
//            if (Logger::isSeverityActive(Logger::debug))
//            {
//                TRACE(Logger::debug, "detected trigger points: %d, Last %d, Position: %d, Steps: %d, drawn: %d", std::distance(mTriggerPointsList.rbegin(), fIter), fIter->mPosition, fEndPosition, fEndPosition - fIter->mPosition, fDrawnTimeSteps);
//            }

            mTriggerPoint = *fIter;
            fUpdateView = true;

            if (mTrigger.mType == TriggerType::Single)
            {
                mAudioInput.suspend();
                mTrigger.mType = TriggerType::Stop;
                ui->groupTrigger->setCurrentIndex(mTrigger.mType);
            }
            determineAutomaticTime();
        }
    }
    return fUpdateView;
}

void MainWindow::onRedrawScopeView(bool aNewBuffer)
{
    TRACE(Logger::trace, "onRedrawScopeView(%d)\n", aNewBuffer);
    if (   mAudioInput.isRunning() && mAudioInput.isValid()
        && (!aNewBuffer || determineTrigger()) )
    {
        ui->graphicsViewScope->setTimePerDivision(mTimePerDivision[ui->hslideTime->value()]);
        if (ui->labelTimePositionValue->isInvalid())
        {
            on_hslideTimePosition_valueChanged(ui->hslideTimePosition->value());
        }

        // Update ScopeWindows
        ui->graphicsViewScope->scene()->clear();
        ui->graphicsViewScope->setTimePerDivision(mTimePerDivision[ui->hslideTime->value()]);

        double fTimeOffset = static_cast<double>(ui->hslideTimePosition->value()) / static_cast<double>(mTimePositionFactor);
        fTimeOffset       *= ui->graphicsViewScope->getTotalTime();
        ui->graphicsViewScope->setTriggerStartTime(fTimeOffset);

        const Channel& fActiveChannel = mChannel[mTrigger.mActiveChannel];

        ui->graphicsViewScope->setTriggerLevel(fActiveChannel.getTriggerLevel() / fActiveChannel.getLevelPerDivision() + fActiveChannel.getOffset() * ui->graphicsViewScope->getLevelDivisions() );
        ui->graphicsViewScope->drawDivision();

        if (!ui->graphicsViewFFT->isHidden())
        {
            ui->graphicsViewFFT->scene()->clear();
            ui->graphicsViewFFT->drawDivision();
        }

        if (!ui->graphicsViewZoom->isHidden())
        {
            ui->graphicsViewZoom->scene()->clear();
            ui->graphicsViewZoom->setTimePerDivision(mTimePerDivision[ui->hslideTime->value()]);
            ui->graphicsViewZoom->setTriggerLevel(fActiveChannel.getTriggerLevel() / fActiveChannel.getLevelPerDivision() + fActiveChannel.getOffset() * ui->graphicsViewScope->getLevelDivisions() );
            ui->graphicsViewZoom->drawDivision();
        }
        int    fStartPosition     = mTriggerPoint.mPosition;
        double fTriggerTimeOffset = mTriggerPoint.mOffset;

        if (mTrigger.mDelayActive)
        {
            fStartPosition -= static_cast<int>(mTrigger.mDelay / ui->graphicsViewScope->getTimeStep());
        }

        fStartPosition -= round(fTimeOffset / ui->graphicsViewScope->getTimeStep());
        TRACE(Logger::trace, "RedrawView: trigger: %d; end: %d; offset0: %f; new: %d\n", fStartPosition, fStartPosition + ui->graphicsViewScope->getDrawnTimeSteps(), fTriggerTimeOffset, aNewBuffer);

        for (int c=0; c < mCombineCurves.getMathChannelIndex(); ++c)
        {
            determineAutomaticLevel(c);
            const vector<double>& fValues = mAudioInput.getValues(c);
            if (!ui->graphicsViewScope->isLissayousView())
            {
                ui->graphicsViewScope->addPolygon(fValues, mChannel[c], fTriggerTimeOffset, fStartPosition);
            }
            if (!ui->graphicsViewZoom->isHidden())
            {
                ui->graphicsViewZoom->addPolygon(fValues, mChannel[c], fTriggerTimeOffset, fStartPosition);
            }
            if (!ui->graphicsViewFFT->isHidden())
            {
                ui->graphicsViewFFT->addPolygon(fValues, mChannel[c], fStartPosition);
            }
        }
        std::vector<double> fCalculatedCurve;
        if (mCombineCurves.combineCurves(
            mAudioInput.getValues(mCombineCurves.getChannel1()),
            mAudioInput.getValues(mCombineCurves.getChannel2()),
            fCalculatedCurve, fStartPosition, fStartPosition+ui->graphicsViewScope->getDrawnTimeSteps()+2))
        {
            ui->graphicsViewScope->addPolygon(fCalculatedCurve, mChannel[mCombineCurves.getMathChannelIndex()], fTriggerTimeOffset, 1);
        }

        if (ui->graphicsViewScope->isLissayousView())
        {
            ui->graphicsViewScope->addLissajous(
                mAudioInput.getValues(mCombineCurves.getChannel1()), mChannel[mCombineCurves.getChannel1()],
                mAudioInput.getValues(mCombineCurves.getChannel2()), mChannel[mCombineCurves.getChannel2()],
                mLatestStartPosition, mLatestStartPosition + mAudioInput.getBufferSize());
        }
    }
}

void MainWindow::determineAutomaticTime()
{
    if (mTriggerPointsList.size() >= 2 && (mAutoTimeDetermination || mScopeSettings.isVisible()))
    {
        IntToDoubleVector::const_reverse_iterator fIter = mTriggerPointsList.rbegin();
        int    fPosition1          = fIter->mPosition;
        double fTriggerTimeOffset1 = fIter->mOffset;
        ++fIter;
        int    fPosition2          = fIter->mPosition;
        double fTriggerTimeOffset2 = fIter->mOffset;

        int fSteps = fPosition1 - fPosition2;
        if (fSteps < 0)
        {
            fSteps = fPosition1 + (mAudioInput.getValues(0).size() - fPosition2);
        }

        double fTime = (fSteps + fTriggerTimeOffset1 - fTriggerTimeOffset2 - 1.0) * ui->graphicsViewScope->getTimeStep();

        mAvgTime.addValue(fTime);
        fTime = mAvgTime.getAverage();

        if (mScopeSettings.isVisible())
        {
            mScopeSettings.setValue(fTime, ScopeSettings::measured_period);

            const circlevector<double>& fValues = static_cast<const circlevector<double>&>(mAudioInput.getValues(mTrigger.mActiveChannel));
            double fSum = 0;
            double fSquareSum = 0;
            circlevector<double>::const_circlevector_iterator fValue = fValues.begin(fPosition2);
            circlevector<double>::const_circlevector_iterator fEnd   = fValues.begin(fPosition2+fSteps);

            for (; fValue != fEnd; ++fValue)
            {
                fSum       += fabs(*fValue);
                fSquareSum += (*fValue * *fValue);
            }

            if (fSteps > 0)
            {
                fSum /= fSteps;
                fSquareSum = sqrt(fSquareSum/fSteps);
            }
            mScopeSettings.setValue(fSum      , ScopeSettings::calculated_avg);
            mScopeSettings.setValue(fSquareSum, ScopeSettings::calculated_rms);
#if 1
            fPosition1 = fPosition2;
            fTriggerTimeOffset1 = fTriggerTimeOffset2;
            if (searchForEdgeCrossingTriggerLevel(mScopeSettings.getPhaseChannel(), fPosition1-1, fPosition2, !mTrigger.mEdgeRising, &Trigger::backward, &fTriggerTimeOffset2))
            {
                int fPhaseOffset = fPosition1 - fPosition2;
                if (fPhaseOffset < fSteps)
                {
                    // TODO validate phase offset
                    double fPhaseOffsetTime = (fPhaseOffset + fTriggerTimeOffset1 - fTriggerTimeOffset2 - 1.0) * ui->graphicsViewScope->getTimeStep();
                    double fPhase = fPhaseOffsetTime / fTime * 360;
                    mScopeSettings.setValue(fPhase, ScopeSettings::measured_phase);
                }
            }
#endif
        }

        if (mAutoTimeDetermination)
        {
            fTime /= ui->graphicsViewScope->getTimeDivisions();
            unsigned int i;
            for ( i = 1; i < mTimePerDivision.size(); ++i)
            {
                if (fTime > mTimePerDivision[i])
                {
                    --i;
                    break;
                }
            }
            // TODO hysteresis
            unsigned int fOldValue = ui->hslideTime->value();
            if (fOldValue != i)
            {
                double fDiff = fabs(mTimePerDivision[fOldValue] - fTime);
                if (fDiff > 0.05*mTimePerDivision[fOldValue])
                {
                    ui->hslideTime->setValue(i);
                }
            }
        }
    }
}

bool MainWindow::checkHoldOff(int aTriggerPosition)
{
    bool fReturn = true;
    switch (mTrigger.mHoldOffType)
    {
    case TriggerParameter::HoldOff::Time:
    {
        uint64_t fTotalSize = mAudioInput.getBuffers() * mAudioInput.getBufferSize();
        if (mTrigger.mHoldOffPosition == InvalidIndex)
        {
            uint64_t fPostion = static_cast<uint64_t>(mAudioInput.getSampleFrequency() * mTrigger.mHoldOffTime + 0.5) + aTriggerPosition;
            mTrigger.mHoldOffPosition = fPostion % fTotalSize;
            mTrigger.mHoldOffWrapAround = fPostion / fTotalSize;
            mTrigger.mLastTriggerpoint = InvalidIndex;
        }
        else
        {
            if (mTrigger.mHoldOffWrapAround)
            {
                if (mTrigger.mLastTriggerpoint == InvalidIndex) mTrigger.mLastTriggerpoint = aTriggerPosition;
                if (aTriggerPosition < mTrigger.mLastTriggerpoint)
                {
                    --mTrigger.mHoldOffWrapAround;
                }
                mTrigger.mLastTriggerpoint = aTriggerPosition;
                fReturn = false;
            }
            else
            {
                if (aTriggerPosition < mTrigger.mHoldOffPosition)
                {
                    fReturn = false;
                }
                else
                {
                    mTrigger.mHoldOffPosition = InvalidIndex;
                    checkHoldOff(aTriggerPosition);
                }
            }
        }
    } break;
    case  TriggerParameter::HoldOff::Events:
        if (mTrigger.mHoldOffPosition == InvalidIndex)
        {
            mTrigger.mHoldOffPosition = mTrigger.mHoldOffEvents;
        }
        else
        {
            fReturn =  mTrigger.mHoldOffPosition-- <= 0;
            if (fReturn)
            {
                mTrigger.mHoldOffPosition = InvalidIndex;
                checkHoldOff(aTriggerPosition);
            }

        }
        break;
    default: break;
    }
    return fReturn;
}

#endif

