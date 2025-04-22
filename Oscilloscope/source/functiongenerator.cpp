#include "main.h"
#include "functiongenerator.h"
#include "ui_functiongenerator.h"
#include "generatefunction.h"
#include "data_exchange.h"
#include "xml_functions.h"

#include <QThread>
#include <QShowEvent>
#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

#include "settings.h"

using namespace DataExchange;
namespace
{
    double invert(double aValue)
    {
        if (aValue != 0.0)
        {
            return 1.0 / aValue;
        }
        return 0.0;
    }
}

FunctionGenerator::FunctionGenerator(QWidget *parent) : QDialog(parent)
  , ui(new Ui::FunctionGenerator)
  , mChannel(0)
  , mMasterChannel(0)
  , mFunction(0)
  , mBurstType(Burst::Off)
  , mBurstCycles(-1)
{
    ui->setupUi(this);

    boost::function<double (double ) > fInvert = &invert;
    ui->groupFrequencyRectangle->setValueModifier(fInvert);
    QObject::connect(ui->groupFrequencyRectangle, SIGNAL(send_value(double)), ui->groupPeriodRectangle, SLOT(value(double)));

    ui->groupPeriodRectangle->setValueModifier(fInvert);
    QObject::connect(ui->groupPeriodRectangle, SIGNAL(send_value(double)), ui->groupFrequencyRectangle, SLOT(value(double)));

    ui->groupFrequencyTriangle->setValueModifier(fInvert);
    QObject::connect(ui->groupFrequencyTriangle, SIGNAL(send_value(double)), ui->groupPeriodTriangle, SLOT(value(double)));

    ui->groupPeriodTriangle->setValueModifier(fInvert);
    QObject::connect(ui->groupPeriodTriangle, SIGNAL(send_value(double)), ui->groupFrequencyTriangle, SLOT(value(double)));

    ui->groupWidthPercentRectangle->setPrefix(UnitPrefix::Percent);
    ui->groupWidthPercentTriangle->setPrefix(UnitPrefix::Percent);
    ui->groupAmplitudeSine->setPrefix(UnitPrefix::Percent);
    ui->groupHighLevelRectangle->setPrefix(UnitPrefix::Percent);
    ui->groupLowLevelRectangle->setPrefix(UnitPrefix::Percent);
    ui->groupHighLevelTriangle->setPrefix(UnitPrefix::Percent);
    ui->groupLowLevelTriangle->setPrefix(UnitPrefix::Percent);

    for (int fItem = static_cast<int>(Burst::Off); fItem < static_cast<int>(Burst::LastItem); ++fItem)
    {
        ui->comboBurstLengthType->addItem(getName(static_cast<Burst>(fItem)));
    }
    ui->groupBurst->setTextFlags(Qt::AlignLeft|Qt::AlignTop);

    for (int i=FilterFunctions::FirstFilter; i<FilterFunctions::LastFilter; ++i)
    {
        ui->comboHighPassType->addItem(FilterFunctions::getNameOfType(static_cast<FilterFunctions::eType>(i)));
        ui->comboLowPassType->addItem(FilterFunctions::getNameOfType(static_cast<FilterFunctions::eType>(i)));
    }
    for (int i=FilterFunctions::FirstOrder; i<=FilterFunctions::LastOrder; ++i)
    {
        ui->comboHighPassOrder->addItem(QString::number(i));
        ui->comboLowPassOrder->addItem(QString::number(i));
    }
}

FunctionGenerator::~FunctionGenerator()
{
    delete ui;
}

void FunctionGenerator::setChannels(int aChannels)
{
    mFunction.clear();
    mFunction.resize(aChannels);
    ui->comboChannel->clear();
    int i = 0;
    for (auto& function : mFunction)
    {
        function = std::make_shared<GenerateFunction>();
        ui->comboChannel->addItem(QString::number(++i));
    }
    ui->comboChannel->addItem(tr("All"));
}

void FunctionGenerator::setBuffer(int aChannel, std::vector<float>* aBuffer)
{
    if (!mFunction.empty())
    {
        mFunction[aChannel]->setBuffer(aBuffer);
    }
}

void FunctionGenerator::get_stream_state(bool aRun)
{
    ui->btnStartAudio->setChecked(aRun);
}

void FunctionGenerator::showEvent (QShowEvent * event)
{
    if (event->type() == QEvent::Show)
    {
        double fMaxFrequency = GenerateFunction::getSampleFrequency() / 2;
        double fMinFrequency = 0.1;
        ui->groupFrequencySine->setRange(fMinFrequency, fMaxFrequency);
        ui->groupStartPhaseSine->setRange(-360, 360);
        ui->groupSweepInterval->setRange(0.1, 10.0);
        ui->groupFrequencySine2->setRange(fMinFrequency, fMaxFrequency);
        ui->groupFrequencyTriangle->setRange(fMinFrequency, fMaxFrequency);
        ui->groupFrequencyRectangle->setRange(fMinFrequency, fMaxFrequency);
        ui->groupHighPassCutOff->setRange(fMinFrequency, fMaxFrequency);
        ui->groupLowPassCutOff->setRange(fMinFrequency, fMaxFrequency);

        ui->groupHighPassQ->setRange(fMinFrequency, fMaxFrequency);
        ui->groupLowPassQ->setRange(fMinFrequency, fMaxFrequency);

        ui->groupLowLevelRectangle->setRange(-1.0, 0.0);
        ui->groupLowLevelTriangle->setRange(-1.0, 0.0);
        ui->groupNoiseInterval->setRange(1, 10.0);

        updateChannelParameter();
    }
}

void FunctionGenerator::updateBurstParameters()
{
    ui->comboBurstLengthType->setCurrentIndex(static_cast<int>(mBurstType));
    // TODO calculate value depending on bursttype
    ui->groupBurst->value(mBurstCycles);
}

quint32 FunctionGenerator::getActiveChannel() const
{
    return mChannel == mFunction.size() ? mMasterChannel: mChannel;
}

void FunctionGenerator::getActiveChannelRange(quint32 &start, quint32 &end)
{
    if (mChannel == mFunction.size())
    {
        start=0;
        end = mChannel;
    }
    else
    {
        start = mChannel;
        end   = mChannel+1;
    }
}

void FunctionGenerator::updateNoiseFilterParameters()
{
    if (!mFunction.empty())
    {
        const auto &activeFunction = mFunction[getActiveChannel()];
        if (activeFunction->useNoiseFilter())
        {
            if (activeFunction->getNoiseFilter().getLowPassType() == FilterFunctions::Pink)
            {
                ui->groupHighPass->hide();
                ui->groupLowPassCutOff->show();
                ui->groupLowPassQ->hide();
                ui->comboLowPassOrder->hide();
                ui->comboLowPassType->hide();
            }
            else
            {
                ui->groupHighPass->show();
                if (activeFunction->getNoiseFilter().getHighPassType() == FilterFunctions::ButterworthVariableQ)
                {
                    ui->groupHighPassQ->show();
                }
                else
                {
                    ui->groupHighPassQ->hide();
                }
                ui->groupLowPass->show();
                ui->groupLowPassQ->show();
                ui->comboLowPassOrder->show();
                ui->comboLowPassType->show();
                if (activeFunction->getNoiseFilter().getLowPassType() == FilterFunctions::ButterworthVariableQ)
                {
                    ui->groupLowPassQ->show();
                }
                else
                {
                    ui->groupLowPassQ->hide();
                }
            }
        }
        else
        {
            ui->groupHighPass->hide();
            ui->groupLowPass->hide();
        }
    }
}

void FunctionGenerator::updateChannelParameter()
{
    if (!mFunction.empty())
    {
        const auto &activeFunction = mFunction[getActiveChannel()];
        ui->groupFrequencySine->value(activeFunction->getFrequency1());
        ui->groupStartPhaseSine->value(activeFunction->getStartPhase());
        ui->groupFrequencySine2->value(activeFunction->getFrequency2());
        ui->groupSweepInterval->value(activeFunction->getSweepInterval());
        ui->groupAmplitudeSine->value(activeFunction->getAmplitudeSine());

        ui->groupFrequencyRectangle->value(activeFunction->getFrequencyRectangle());
        ui->groupFrequencyRectangle->on_editingFinished();
        ui->groupWidthPercentRectangle->value(activeFunction->getPulsewidthFactorRectangle());
        ui->groupHighLevelRectangle->value(activeFunction->getHighLevelRectangle());
        ui->groupLowLevelRectangle->value(activeFunction->getLowLevelRectangle());

        ui->groupFrequencyTriangle->value(activeFunction->getFrequencyTriangle());
        ui->groupFrequencyTriangle->on_editingFinished();
        ui->groupWidthPercentTriangle->value(activeFunction->getPulsewidthFactorTriangle());
        ui->groupHighLevelTriangle->value(activeFunction->getHighLevelTriangle());
        ui->groupLowLevelTriangle->value(activeFunction->getLowLevelTriangle());

        ui->groupNoiseInterval->value(activeFunction->getNoiseInterval());
        ui->checkNoiseFitered->setChecked(activeFunction->useNoiseFilter());

        ui->comboHighPassOrder->setCurrentIndex(activeFunction->getNoiseFilter().getHighPassOrder());
        ui->comboHighPassType->setCurrentIndex(activeFunction->getNoiseFilter().getHighPassType());
        ui->groupHighPassCutOff->value(activeFunction->getNoiseFilter().getHighPassCutOff());
        ui->groupHighPassQ->value(activeFunction->getNoiseFilter().getHighPassQ());

        ui->checkPinkNoise->setChecked(activeFunction->getNoiseFilter().getLowPassType() == FilterFunctions::Pink);
        ui->comboLowPassType->setCurrentIndex(activeFunction->getNoiseFilter().getLowPassType());
        ui->comboLowPassOrder->setCurrentIndex(activeFunction->getNoiseFilter().getLowPassOrder());
        ui->groupLowPassCutOff->value(activeFunction->getNoiseFilter().getLowPassCutOff());
        ui->groupLowPassQ->value(activeFunction->getNoiseFilter().getLowPassQ());

        updateFunctionType();
        updateNoiseFilterParameters();
    }
}

void FunctionGenerator::updateFunctionType()
{
    if (!mFunction.empty())
    {
        const auto &activeFunction = mFunction[getActiveChannel()];
        ui->groupWaveform->setCurrentIndex(activeFunction->getType());
        bool fSine(false);
        bool fSweep(false);
        bool fRectangle(false);
        bool fTriangle(false);
        bool fNoise(false);
        bool fBurst(false);
        switch (activeFunction->getType())
        {
        case GenerateFunction::silent:                           break;
        case GenerateFunction::sine:      fSine      = fBurst = true; break;
        case GenerateFunction::sine_sweep:fSine      = fBurst = fSweep = true; break;
        case GenerateFunction::rectangle: fRectangle = fBurst = true; break;
        case GenerateFunction::triangle:  fTriangle  = fBurst = true; break;
        case GenerateFunction::noise:     fNoise              = true; break;
        default:                                                 break;
        }
        ui->checkSineActive->setChecked(fSine);
        ui->checkSineSweepActive->setChecked(fSweep);
        ui->checkRectangleActive->setChecked(fRectangle);
        ui->checkTriangleActive->setChecked(fTriangle);
        ui->checkNoiseActive->setChecked(fNoise);
        ui->comboBurstLengthType->setCurrentIndex(static_cast<int>(fBurst ? mBurstType : Burst::Off));
    }
}

void FunctionGenerator::getChannelParameter()
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            const auto &activeFunction = mFunction[i];

            activeFunction->setFrequency1(ui->groupFrequencySine->value());
            activeFunction->setFrequency2(ui->groupFrequencySine2->value());
            activeFunction->setStartPhase(ui->groupStartPhaseSine->value());
            activeFunction->setSweepInterval(ui->groupSweepInterval->value());
            activeFunction->setAmplitudeSine(ui->groupAmplitudeSine->value());

            activeFunction->setFrequencyRectangle(ui->groupFrequencyRectangle->value());
            activeFunction->setPulsewidthFactorRectangle(ui->groupWidthPercentRectangle->value());
            activeFunction->setHighLevelRectangle(ui->groupHighLevelRectangle->value());
            activeFunction->setLowLevelRectangle(ui->groupLowLevelRectangle->value());

            activeFunction->setFrequencyTriangle(ui->groupFrequencyTriangle->value());
            activeFunction->setPulsewidthFactorTriangle(ui->groupWidthPercentTriangle->value());
            activeFunction->setHighLevelTriangle(ui->groupHighLevelTriangle->value());
            activeFunction->setLowLevelTriangle(ui->groupLowLevelTriangle->value());

            activeFunction->setNoiseInterval(ui->groupNoiseInterval->value());
            activeFunction->useNoiseFilter(ui->checkNoiseFitered->isChecked());

            activeFunction->getNoiseFilter().setHighPassType(static_cast<FilterFunctions::eType>(ui->comboHighPassType->currentIndex()));
            activeFunction->getNoiseFilter().setHighPassOrder(static_cast<FilterFunctions::eOrder>(ui->comboHighPassOrder->currentIndex()));
            activeFunction->getNoiseFilter().setHighPassCutOff(ui->groupHighPassCutOff->value());
            activeFunction->getNoiseFilter().setHighPassQ(ui->groupHighPassQ->value());

            if (ui->checkPinkNoise->isChecked())
            {
                activeFunction->getNoiseFilter().setLowPassType(FilterFunctions::Pink);
            }
            else
            {
                activeFunction->getNoiseFilter().setLowPassType(static_cast<FilterFunctions::eType>(ui->comboLowPassType->currentIndex()));
            }

            activeFunction->getNoiseFilter().setLowPassOrder(static_cast<FilterFunctions::eOrder>(ui->comboLowPassOrder->currentIndex()));
            activeFunction->getNoiseFilter().setLowPassCutOff(ui->groupLowPassCutOff->value());
            activeFunction->getNoiseFilter().setLowPassQ(ui->groupHighPassQ->value());
        }
    }
}

void FunctionGenerator::saveParameter(QDomDocument& aDoc, QDomElement&aDE)
{
    for (quint32 fC=0; fC<mFunction.size(); ++fC)
    {
        QDomElement fChannel = aDoc.createElement(s::sChannel+QString::number(fC+1));
        aDE.appendChild(fChannel);
        saveChannel(aDoc, fChannel, *mFunction[fC]);
    }
}


void  FunctionGenerator::saveChannel(QDomDocument& aDoc, QDomElement& aDE, GenerateFunction& aChannel)
{
    addElement(aDoc, aDE, s::sType, aChannel.getType());

    QDomElement fSine = aDoc.createElement(s::sSine);
    aDE.appendChild(fSine);

    addElement(aDoc, fSine, s::sFrequency1, aChannel.getFrequency1());
    addElement(aDoc, fSine, s::sFrequency2, aChannel.getFrequency2());
    addElement(aDoc, fSine, s::sStartPhase, aChannel.getStartPhase());
    addElement(aDoc, fSine, s::sAmplitude , aChannel.getAmplitudeSine());
    addElement(aDoc, fSine, s::sInterval  , aChannel.getSweepInterval());

    QDomElement fRectangle = aDoc.createElement(s::sRectangle);
    aDE.appendChild(fRectangle);
    addElement(aDoc, fRectangle, s::sFrequency1, aChannel.getFrequencyRectangle());
    addElement(aDoc, fRectangle, s::sPulseWidthFactor, aChannel.getPulsewidthFactorRectangle());
    addElement(aDoc, fRectangle, s::sHighLevel, aChannel.getHighLevelRectangle());
    addElement(aDoc, fRectangle, s::sLowLevel, aChannel.getLowLevelRectangle());

    QDomElement fTriangle = aDoc.createElement(s::sTriangle);
    aDE.appendChild(fTriangle);
    addElement(aDoc, fTriangle, s::sFrequency1, aChannel.getFrequencyTriangle());
    addElement(aDoc, fTriangle, s::sPulseWidthFactor, aChannel.getPulsewidthFactorTriangle());
    addElement(aDoc, fTriangle, s::sHighLevel, aChannel.getHighLevelTriangle());
    addElement(aDoc, fTriangle, s::sLowLevel, aChannel.getLowLevelTriangle());

    QDomElement fNoise = aDoc.createElement(s::sNoise);
    aDE.appendChild(fNoise);
    addElement(aDoc, fNoise, s::sInterval, aChannel.getNoiseInterval());
    addElement(aDoc, fNoise, s::sUseNoiseFilter, aChannel.useNoiseFilter());
    QDomElement fFilter = aDoc.createElement(s::sHighPassFilter);
    fNoise.appendChild(fFilter);
    addElement(aDoc, fFilter, s::sCutOff, aChannel.getNoiseFilter().getHighPassCutOff());
    addElement(aDoc, fFilter, s::sOrder, aChannel.getNoiseFilter().getHighPassOrder());
    addElement(aDoc, fFilter, s::sQFactor, aChannel.getNoiseFilter().getHighPassQ());
    addElement(aDoc, fFilter, s::sType, aChannel.getNoiseFilter().getHighPassType());
    fFilter = aDoc.createElement(s::sLowPassFilter);
    fNoise.appendChild(fFilter);
    addElement(aDoc, fFilter, s::sCutOff, aChannel.getNoiseFilter().getLowPassCutOff());
    addElement(aDoc, fFilter, s::sOrder, aChannel.getNoiseFilter().getLowPassOrder());
    addElement(aDoc, fFilter, s::sQFactor, aChannel.getNoiseFilter().getLowPassQ());
    addElement(aDoc, fFilter, s::sType, aChannel.getNoiseFilter().getLowPassType());

}

void FunctionGenerator::loadParameter(const QDomNode& aDE)
{
    for (quint32 fC=0; fC<mFunction.size(); ++fC)
    {
        loadChannel(findElement(aDE, s::sChannel+QString::number(fC+1)), *mFunction[fC]);
    }
}

void  FunctionGenerator::loadChannel(const QDomNode& aDE, GenerateFunction& aChannel)
{
    aChannel.setType(getValue(findElement(aDE, s::sType), aChannel.getType()));
    QDomNode fSine = findElement(aDE, s::sSine);
    aChannel.setFrequency1(getValue(findElement(fSine, s::sFrequency1), aChannel.getFrequency1()));
    aChannel.setFrequency2(getValue(findElement(fSine, s::sFrequency2), aChannel.getFrequency2()));
    aChannel.setStartPhase(getValue(findElement(fSine, s::sStartPhase), aChannel.getStartPhase()));
    aChannel.setAmplitudeSine(getValue(findElement(fSine, s::sAmplitude), aChannel.getAmplitudeSine()));
    aChannel.setSweepInterval(getValue(findElement(fSine, s::sInterval), aChannel.getSweepInterval()));

    QDomNode fRectangle = findElement(aDE, s::sRectangle);
    aChannel.setFrequencyRectangle(getValue(findElement(fRectangle, s::sFrequency1), aChannel.getFrequencyRectangle()));
    aChannel.setPulsewidthFactorRectangle(getValue(findElement(fRectangle, s::sPulseWidthFactor), aChannel.getPulsewidthFactorRectangle()));
    aChannel.setHighLevelRectangle(getValue(findElement(fRectangle, s::sHighLevel), aChannel.getHighLevelRectangle()));
    aChannel.setLowLevelRectangle(getValue(findElement(fRectangle, s::sLowLevel), aChannel.getLowLevelRectangle()));

    QDomNode fTriangle = findElement(aDE, s::sTriangle);
    aChannel.setFrequencyRectangle(getValue(findElement(fTriangle, s::sFrequency1), aChannel.getFrequencyRectangle()));
    aChannel.setPulsewidthFactorRectangle(getValue(findElement(fTriangle, s::sPulseWidthFactor), aChannel.getPulsewidthFactorRectangle()));
    aChannel.setHighLevelRectangle(getValue(findElement(fTriangle, s::sHighLevel), aChannel.getHighLevelRectangle()));
    aChannel.setLowLevelRectangle(getValue(findElement(fTriangle, s::sLowLevel), aChannel.getLowLevelRectangle()));

    QDomNode fNoise = findElement(aDE, s::sNoise);
    aChannel.setNoiseInterval(getValue(findElement(fNoise, s::sInterval), aChannel.getNoiseInterval()));
    aChannel.useNoiseFilter(getValue(findElement(fNoise, s::sInterval), aChannel.useNoiseFilter()));

    QDomNode fFilter = findElement(fNoise, s::sHighPassFilter);
    aChannel.getNoiseFilter().setHighPassCutOff(getValue(findElement(fFilter, s::sCutOff), aChannel.getNoiseFilter().getHighPassCutOff()));
    aChannel.getNoiseFilter().setHighPassOrder(static_cast<FilterFunctions::eOrder>(getValue(findElement(fFilter, s::sOrder), static_cast<int>(aChannel.getNoiseFilter().getHighPassOrder()))));
    aChannel.getNoiseFilter().setHighPassQ(getValue(findElement(fFilter, s::sQFactor), aChannel.getNoiseFilter().getHighPassQ()));
    aChannel.getNoiseFilter().setHighPassType(static_cast<FilterFunctions::eType>(getValue(findElement(fFilter, s::sType), static_cast<int>(aChannel.getNoiseFilter().getHighPassType()))));

    fFilter = findElement(fNoise, s::sLowPassFilter);
    aChannel.getNoiseFilter().setLowPassCutOff(getValue(findElement(fFilter, s::sCutOff), aChannel.getNoiseFilter().getLowPassCutOff()));
    aChannel.getNoiseFilter().setLowPassOrder(static_cast<FilterFunctions::eOrder>(getValue(findElement(fFilter, s::sOrder), static_cast<int>(aChannel.getNoiseFilter().getLowPassOrder()))));
    aChannel.getNoiseFilter().setLowPassQ(getValue(findElement(fFilter, s::sQFactor), aChannel.getNoiseFilter().getLowPassQ()));
    aChannel.getNoiseFilter().setLowPassType(static_cast<FilterFunctions::eType>(getValue(findElement(fFilter, s::sType), static_cast<int>(aChannel.getNoiseFilter().getLowPassType()))));
}

double FunctionGenerator::calcPulseWidthRectangleFromPercent(double aPercent)
{
    return ui->groupPeriodRectangle->value() * aPercent;
}

void FunctionGenerator::on_comboChannel_currentIndexChanged(int index)
{
    if (index != -1 && mChannel != static_cast<quint32>(index))
    {
        if (static_cast<quint32>(index) < mFunction.size())
        {
            getChannelParameter();
            mMasterChannel = mChannel = index;
            updateChannelParameter();
        }
        else
        {
            mChannel = index;
            getChannelParameter();
        }
    }
}

void FunctionGenerator::on_btnStartAudio_clicked(bool checked)
{
    if (checked)
    {
        getChannelParameter();
        for (auto& function : mFunction)
        {
            function->calculate();
        }
    }
    Q_EMIT start_stream(checked, mBurstCycles);
#if (_USE_QT_AUDIO == 1)
    QThread::msleep(100);
#else
    usleep(100000);
#endif
}

void FunctionGenerator::on_btnApply_clicked()
{
    if (ui->btnStartAudio->isChecked())
    {
        on_btnStartAudio_clicked(false);
        on_btnStartAudio_clicked(true);
        ui->btnStartAudio->setChecked(true);
    }
    else
    {
        getChannelParameter();
    }
}

quint32 FunctionGenerator::getPeriodSamples()
{
    if (!mFunction.empty())
    {
        double fFrequency = 0;
        const auto &activeFunction = mFunction[getActiveChannel()];
        switch (activeFunction->getType())
        {
        case GenerateFunction::noise: default: case GenerateFunction::silent:
            break;
        case GenerateFunction::sine:       fFrequency = activeFunction->getFrequency1(); break;
        case GenerateFunction::rectangle:  fFrequency = activeFunction->getFrequencyRectangle(); break;
        case GenerateFunction::triangle:   fFrequency = activeFunction->getFrequencyTriangle(); break;
        case GenerateFunction::sine_sweep: return activeFunction->getSweepInterval() * GenerateFunction::getSampleFrequency();
        }
        if (fFrequency > 0)
        {
            return GenerateFunction::getSampleFrequency() / fFrequency;
        }

    }
    return 0;
}


const char* FunctionGenerator::getName(Burst aBurstType)
{
    switch (aBurstType)
    {
    case Burst::Off:     return "Off";
    case Burst::Samples: return "Samples";
    case Burst::Period:  return "Periods";
    case Burst::Time:    return "Time";
    default:             return "Unknown";
    }
}

void FunctionGenerator::on_comboBurstLengthType_currentIndexChanged(int index)
{
    mBurstType = static_cast<Burst>(index);
    ui->groupBurst->setUnit(mBurstType == Burst::Time ? "s" : " ");
    on_editBurstLength_textChanged("");
}

void FunctionGenerator::on_editBurstLength_textChanged(const QString &)
{
    switch (mBurstType)
    {
    case Burst::Off: default:
        mBurstCycles = NoBursts;
        break;
    case Burst::Samples:
        mBurstCycles = static_cast<qint32>(ui->groupBurst->value());
        break;
    case Burst::Period:
    {
        double fPeriodLength = ui->groupBurst->value();
        mBurstCycles = static_cast<qint32>(fPeriodLength * getPeriodSamples() + 0.5);
    }   break;
    case Burst::Time:
    {
        double fTime_s = ui->groupBurst->value();
        mBurstCycles = static_cast<qint32>(fTime_s * GenerateFunction::getSampleFrequency() + 0.5);
    }   break;
    }
}

void FunctionGenerator::currentTabPagesChanged(int index)
{
    switch (index)
    {
        case SinePage: break;
    }

    TRACE(Logger::debug, "on_TabPages_currentChanged: %d\n", index);
}

void FunctionGenerator::on_groupWaveform_clicked(int index)
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            mFunction[i]->setType(index);
        }
        updateFunctionType();
    }
}

void FunctionGenerator::on_checkSineActive_clicked(bool checked)
{
    if (!mFunction.empty())
    {
        GenerateFunction::eType fType = ui->checkSineSweepActive->isChecked() ? GenerateFunction::sine_sweep : GenerateFunction::sine;
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            mFunction[i]->setType(checked ? fType : GenerateFunction::silent);
        }
        updateFunctionType();
    }
}

void FunctionGenerator::on_checkRectangleActive_clicked(bool checked)
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            mFunction[i]->setType(checked ? GenerateFunction::rectangle : GenerateFunction::silent);
        }
        updateFunctionType();
    }
}

void FunctionGenerator::on_checkTriangleActive_clicked(bool checked)
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            mFunction[i]->setType(checked ? GenerateFunction::triangle : GenerateFunction::silent);
        }
        updateFunctionType();
    }
}

void FunctionGenerator::on_checkNoiseActive_clicked(bool checked)
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            mFunction[i]->setType(checked ? GenerateFunction::noise: GenerateFunction::silent);
        }
        updateFunctionType();
    }
}

void FunctionGenerator::on_checkSineSweepActive_clicked(bool checked)
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            mFunction[i]->setType(checked ? GenerateFunction::sine_sweep: GenerateFunction::silent);
        }
        if (checked)
        {
            ui->checkSineActive->setChecked(true);
        }
        ui->groupWaveform->setCurrentIndex(mFunction[getActiveChannel()]->getType());
    }
}

void FunctionGenerator::on_checkNoiseFitered_clicked(bool checked)
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            mFunction[i]->useNoiseFilter(checked);
        }
        updateNoiseFilterParameters();
    }
}


void FunctionGenerator::on_comboLowPassType_currentIndexChanged(int index)
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            mFunction[i]->getNoiseFilter().setLowPassType(static_cast<FilterFunctions::eType>(index));
        }
        updateNoiseFilterParameters();
    }
}

void FunctionGenerator::on_comboHighPassType_currentIndexChanged(int index)
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            mFunction[i]->getNoiseFilter().setHighPassType(static_cast<FilterFunctions::eType>(index));
        }
        updateNoiseFilterParameters();
    }
}

void FunctionGenerator::on_checkPinkNoise_clicked(bool checked)
{
    if (!mFunction.empty())
    {
        quint32 start {0};
        quint32 end   {0};
        getActiveChannelRange(start, end);
        for (quint32 i=start; i<end; ++i)
        {
            if (checked)
            {
                mFunction[i]->getNoiseFilter().setLowPassType(FilterFunctions::Pink);
            }
            else
            {
                mFunction[i]->getNoiseFilter().setLowPassType(static_cast<FilterFunctions::eType>(ui->comboLowPassType->currentIndex()));
            }
        }
        updateNoiseFilterParameters();
    }
}



