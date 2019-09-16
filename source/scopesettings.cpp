#include "scopesettings.h"
#include "ui_scopesettings.h"
#include "main.h"
#include "Channel.h"
#include "TriggerParameter.h"
#include "data_exchange.h"
#include "qcomboboxdelegate.h"
#include "qwidgetitemdelegate.h"
#include "mainwindow.h"

#include <QShowEvent>
#include <QColorDialog>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <cmath>
#include <cstdio>

ScopeSettings::ScopeSettings(QWidget *parent, std::vector<Channel>& aChannelParam, TriggerParameter& aTriggerpParam, CombineCurves& aCombineCurves) : QDialog(parent)
  , ui(new Ui::ScopeSettings)
  , mChannelParameter(aChannelParam)
  , mTriggerParameter(aTriggerpParam)
  , mCombineCurves(aCombineCurves)
  , mActiveChannel(0)
  , mActiveViewCursor(Cursors::scope_view)
  , mPhaseChannel(0)
  , mChannelTableListModel(0)
  , mProbeListModel(0)
  , mProbeCombo(0)
  , mInitialize(false)
  , mProbesChanged(false)
{
    ui->setupUi(this);
    ui->rdTime->setChecked(true);

    QObject::connect(ui->groupFilter    , SIGNAL(clicked(int)), this, SLOT(click_radio_filter(int)));
    QObject::connect(ui->groupOutput    , SIGNAL(clicked(int)), this, SLOT(click_radio_output(int)));
    QObject::connect(ui->groupCursorView, SIGNAL(clicked(int)), this, SLOT(click_radio_cursor_view(int)));

    ui->groupVerticalCursor1->setPrefixChange(UnitPrefix::auto_change);
    ui->groupVerticalCursor2->setPrefixChange(UnitPrefix::auto_change);
    ui->groupVerticalDifference->setPrefixChange(UnitPrefix::auto_change);
    ui->groupVerticalInverseDifference->setPrefixChange(UnitPrefix::no_change);

    ui->groupHorizontalCursor1->setPrefixChange(UnitPrefix::auto_change);
    ui->groupHorizontalCursor2->setPrefixChange(UnitPrefix::auto_change);
    ui->groupHorizontalDifference->setPrefixChange(UnitPrefix::auto_change);
    ui->groupHorizontalInverseDifference->setPrefixChange(UnitPrefix::auto_change);

    ui->groupHoldOffTime->setRange(0, 10);
    ui->groupHoldOffTime->setPrefixChange(UnitPrefix::auto_change);
    ui->groupTriggerDelay->setRange(-10, 10);
    ui->groupTriggerDelay->setPrefixChange(UnitPrefix::auto_change);

    QObject::connect(ui->groupColorButtons, SIGNAL(clicked(int)), this, SLOT(on_clickedColorButtons(int)));
    ui->groupColorButtons->setClickSignal(QRadioButtonGroup::click_always);

    ui->valueMeasuredFrequency->initUnit();
    ui->valueMeasuredFrequency->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->valueMeasuredFrequency->value(0);
    ui->valueCalculatedRMS->initUnit();
    ui->valueCalculatedRMS->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->valueCalculatedRMS->value(0);
    ui->valueCalculatedAVG->initUnit();
    ui->valueCalculatedAVG->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->valueCalculatedAVG->value(0);
    ui->valueMeasuredFrequency_avg->initUnit();
    ui->valueMeasuredFrequency_avg->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->valueMeasuredFrequency_avg->value(0);
    ui->valueCalculatedRMS_avg->initUnit();
    ui->valueCalculatedRMS_avg->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->valueCalculatedRMS_avg->value(0);
    ui->valueCalculatedAVG_avg->initUnit();
    ui->valueCalculatedAVG_avg->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->valueCalculatedAVG_avg->value(0);
    mAverageFrequency.setNoOfAvgValues(50);
    mAverageRMSLevel.setNoOfAvgValues(50);
    mAverageAVGLevel.setNoOfAvgValues(50);
    ui->valueMeasuredMin->initUnit();
    ui->valueMeasuredMin->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->valueMeasuredMin->value(0);
    ui->valueMeasuredMax->initUnit();
    ui->valueMeasuredMax->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->valueMeasuredMax->value(0);
    ui->valueMeasuredPhase->initUnit();
    ui->valueMeasuredPhase->getPrefix().setPrefixChange(UnitPrefix::no_change);
    ui->valueMeasuredPhase->value(0);

    MainWindow* fMain = dynamic_cast<MainWindow*>(parentWidget());
    if (fMain)
    {
        QObject::connect(ui->groupMathCombine, SIGNAL(clicked(int)), this, SLOT(on_clickedMathCombine(int)));
        QObject::connect(ui->groupMathCombine, SIGNAL(clicked(int)), fMain, SLOT(on_clickedMathCombine(int)));
    }
    ui->groupMathCombine->setCurrentIndex(mCombineCurves.getType());

#if (_USE_QT_AUDIO != 1)
    ui->groupSlopeTrigger->setEnabled(false);
#endif

}

ScopeSettings::~ScopeSettings()
{
    delete ui;
}

void  ScopeSettings::setTimePrefix(int aPrefix)
{
    ui->groupHoldOffTime->setPrefix(static_cast<UnitPrefix::ePrefix>(aPrefix));
    ui->groupTriggerDelay->setPrefix(static_cast<UnitPrefix::ePrefix>(aPrefix));
}

void ScopeSettings::addCursors(Cursors& aCursors)
{
    mCursors.push_back(&aCursors);
    QObject::connect(&aCursors, SIGNAL(update(Cursors::eCursor, Cursors::eView)), this, SLOT(update_cursor(Cursors::eCursor, Cursors::eView)));
}

void ScopeSettings::showEvent(QShowEvent *event)
{
    if (event->type() == QEvent::Show)
    {
        updateParameters();
    }
}

void ScopeSettings::timerEvent(QTimerEvent *)
{
    ui->groupHoldOffTime->checkPrefixChanged();
}

void ScopeSettings::updateParameters()
{
    mInitialize = true;
    update_cursor(Cursors::vertical_1, mActiveViewCursor);
    update_cursor(Cursors::vertical_2, mActiveViewCursor);
    update_cursor(Cursors::horizontal_1, mActiveViewCursor);
    update_cursor(Cursors::horizontal_2, mActiveViewCursor);

    ui->ckHorizontal->setChecked(mCursors[mActiveViewCursor]->isHorizontalVisible());
    ui->ckVertical->setChecked(mCursors[mActiveViewCursor]->isVerticalVisible());

    ui->groupHoldOffTime->value(mTriggerParameter.mHoldOffTime);
    ui->groupHoldOffTime->setPrefixChange(UnitPrefix::wheel_change);
    ui->checkHoldOffTimeActive->setChecked(mTriggerParameter.mHoldOffType == TriggerParameter::HoldOff::Time);

    ui->editHoldOffEvents->setText(QString::number(mTriggerParameter.mHoldOffEvents));
    ui->checkHoldOffEventActive->setChecked(mTriggerParameter.mHoldOffType == TriggerParameter::HoldOff::Time);

    ui->groupTriggerDelay->value(mTriggerParameter.mDelay);
    ui->groupTriggerDelay->setPrefixChange(UnitPrefix::wheel_change);
    ui->checkTriggerDelayActive->setChecked(mTriggerParameter.mDelayActive);

    ui->groupSlopeTrigger->value(mTriggerParameter.mDelay);
    ui->groupSlopeTrigger->setPrefixChange(UnitPrefix::wheel_change);
    ui->checkSlopeTriggerActive->setChecked(mTriggerParameter.mSlopeActive);

    ui->spinChannel1->setRange(1, mCombineCurves.getMathChannelIndex());
    ui->spinChannel2->setRange(1, mCombineCurves.getMathChannelIndex());
    ui->spinChannel1->setValue(mCombineCurves.getChannel1()+1);
    ui->spinChannel2->setValue(mCombineCurves.getChannel2()+1);

    QStringList fProbes;
    MainWindow* fMain = dynamic_cast<MainWindow*>(parentWidget());
    if (fMain)
    {
        fMain->getProbeStrings(fProbes);
    }

    if (mChannelTableListModel == 0)
    {
        mChannelTableListModel = new QStandardItemModel(mChannelParameter.size(), eListSize, this);
        mChannelTableListModel->setHeaderData(eName, Qt::Horizontal, tr("Name"));
        mChannelTableListModel->setHeaderData(eProbe, Qt::Horizontal, tr("Probe"), Qt::DisplayRole);
        mChannelTableListModel->setHeaderData(eScale, Qt::Horizontal, tr("Scale"), Qt::DisplayRole);
        mChannelTableListModel->setHeaderData(eOffset, Qt::Horizontal, tr("Offset"), Qt::DisplayRole);
        ui->channelTableView->setModel(mChannelTableListModel);
        connect(mChannelTableListModel,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(on_ChannelTableListItemChanged(QStandardItem*)));
        mChannelTableListModel->insertRows(0, mChannelParameter.size());
        mProbeCombo = new QComboBoxDelegate(fProbes);
        ui->channelTableView->setItemDelegateForColumn(eProbe, mProbeCombo);
    }

    unsigned int i;
    QString fChannel;
    for (i = 0; i < mChannelParameter.size(); ++i)
    {
        fChannel.setNum(i+1);
        if (mChannelParameter[i].mName.length())
        {
            mChannelTableListModel->setData(mChannelTableListModel->index(i, eName), mChannelParameter[i].mName);
        }
        else
        {
            mChannelTableListModel->setData(mChannelTableListModel->index(i, eName), fChannel);
        }
        QString sProbe;
        if (mChannelParameter[i].getAudioScale() >= 1.0)
        {
            sProbe.setNum(mChannelParameter[i].getAudioScale());
            sProbe = sProbe + " : 1";
        }
        else
        {
            sProbe.setNum(1.0 / mChannelParameter[i].getAudioScale());
            sProbe =  "1 : " + sProbe;
        }
        mChannelTableListModel->setData(mChannelTableListModel->index(i, eProbe), sProbe);
#if (_USE_QT_AUDIO == 1)
        mChannelTableListModel->setData(mChannelTableListModel->index(i, eScale), QString::asprintf("%.3f", mChannelParameter[i].getAudioInputScale()));
        mChannelTableListModel->setData(mChannelTableListModel->index(i, eOffset), QString::asprintf("%.3f mV", mChannelParameter[i].getAudioInputOffset()*1000));
#else
        mChannelTableListModel->setData(mChannelTableListModel->index(i, eScale), QString::number(mChannelParameter[i].getAudioInputScale()));
        mChannelTableListModel->setData(mChannelTableListModel->index(i, eOffset), QString::number(mChannelParameter[i].getAudioInputOffset()*1000));
#endif
    }

    if (mProbeListModel == 0)
    {
        mProbeListModel = new QStandardItemModel(fProbes.size(), 1, this);
        ui->probesListView->setModel(mProbeListModel);
        connect(mProbeListModel, SIGNAL(itemChanged(QStandardItem*)),this, SLOT(on_ProbeListItemChanged(QStandardItem*)));
    }

    for (int i=0; i<fProbes.size(); ++i)
    {
        mProbeListModel->setData(mProbeListModel->index(i, 0), fProbes[i]);
    }
    mInitialize = false;
    updateColors();
}

void ScopeSettings::updateColors()
{
    MainWindow* fMain = dynamic_cast<MainWindow*>(parentWidget());
    QColor fBackground = fMain->getBackgroundColor();
    QColor *fBackgroundPtr = &fBackground;
    setWidgetStyleSheet(*ui->buttonColorOfCurve_1, &fMain->mCurveColors[0], fBackgroundPtr);
    setWidgetStyleSheet(*ui->buttonColorOfCurve_2, &fMain->mCurveColors[1], fBackgroundPtr);
    setWidgetStyleSheet(*ui->buttonColorOfCurve_3, &fMain->mCurveColors[2], fBackgroundPtr);
    setWidgetStyleSheet(*ui->buttonColorOfCurve_4, &fMain->mCurveColors[3], fBackgroundPtr);
    setWidgetStyleSheet(*ui->buttonColorOfCurve_5, &fMain->mCurveColors[4], fBackgroundPtr);
    setWidgetStyleSheet(*ui->buttonColorOfCurve_6, &fMain->mCurveColors[5], fBackgroundPtr);
    setWidgetStyleSheet(*ui->buttonColorOfCurve_7, &fMain->mCurveColors[6], fBackgroundPtr);
    setWidgetStyleSheet(*ui->buttonColorOfCurve_8, &fMain->mCurveColors[7], fBackgroundPtr);

    setWidgetStyleSheet(*ui->buttonBackgroundColor, &fBackground, fBackgroundPtr);
    QColor fGrid = fMain->getGridColor();
    setWidgetStyleSheet(*ui->buttonGridColor, &fGrid, fBackgroundPtr);
}

void ScopeSettings::setFFT_FilterIndex(int anIndex)
{
    ui->groupFilter->setCurrentIndex(anIndex);
}

void ScopeSettings::setFFT_OutputIndex(int anIndex)
{
    ui->groupOutput->setCurrentIndex(anIndex);
}

void  ScopeSettings::setActiveChannel(int aChannel)
{
    mActiveChannel = aChannel;
}

void ScopeSettings::on_ckVertical_clicked(bool checked)
{
    mCursors[mActiveViewCursor]->setVerticalVisible(checked);
    if (checked)
    {
        ui->ckHorizontal->setChecked(false);
        mCursors[mActiveViewCursor]->setHorizontalVisible(!checked);
    }
    Cursors::eView fView = getSynchedView(mActiveViewCursor);
    if (fView  != Cursors::unknown_view)
    {
        mCursors[fView]->setVerticalVisible(checked);
        mCursors[fView]->setHorizontalVisible(!checked);
    }
}

void ScopeSettings::on_ckHorizontal_clicked(bool checked)
{
    mCursors[mActiveViewCursor]->setHorizontalVisible(checked);
    if (checked)
    {
        ui->ckVertical->setChecked(false);
        mCursors[mActiveViewCursor]->setVerticalVisible(!checked);
    }
    Cursors::eView fView = getSynchedView(mActiveViewCursor);
    if (fView  != Cursors::unknown_view)
    {
        mCursors[fView]->setHorizontalVisible(checked);
        mCursors[fView]->setVerticalVisible(!checked);
    }
}

void ScopeSettings::on_ckSyncCursors_clicked(bool checked)
{
    if (checked)
    {
        Cursors::eView fView = getSynchedView(mActiveViewCursor);
        if (fView  != Cursors::unknown_view)
        {
            mCursors[fView]->setHorizontalVisible(mCursors[mActiveViewCursor]->isHorizontalVisible());
            mCursors[fView]->setVerticalVisible(mCursors[mActiveViewCursor]->isVerticalVisible());
        }
    }
}


void ScopeSettings::click_radio_filter(int index)
{
    send_radio_filter(index);
}

void ScopeSettings::click_radio_output(int index)
{
    send_radio_output(index);
}

void ScopeSettings::on_edtFloatingAverage_textEdited(const QString &values)
{
    send_floating_avg_values(ui->ckFloatingAverage->checkState()==Qt::Checked ? values.toInt() : 0);
}

void ScopeSettings::on_ckFloatingAverage_clicked(bool checked)
{
    send_floating_avg_values(checked ? ui->edtFloatingAverage->text().toInt() : 0);
}


void ScopeSettings::click_radio_cursor_view(int index)
{
    mActiveViewCursor = static_cast<Cursors::eView>(index);
    update_horizontal_cursor_unit();
    updateParameters();
}

void ScopeSettings::update_horizontal_cursor_unit()
{
    ui->groupHorizontalCursor1->setUnit(mActiveViewCursor == Cursors::fft_view ? "Hz" : "s");
    ui->groupHorizontalCursor2->setUnit(mActiveViewCursor == Cursors::fft_view ? "Hz" : "s");
    ui->groupHorizontalDifference->setUnit(mActiveViewCursor == Cursors::fft_view ? "Hz" : "s");
    ui->groupHorizontalInverseDifference->setUnit(mActiveViewCursor == Cursors::fft_view ? "s" : "Hz");
}

Cursors::eView ScopeSettings::getSynchedView(Cursors::eView aViewId)
{
    if (ui->ckSyncCursors->isChecked())
    {
        switch (aViewId)
        {
        case Cursors::scope_view: return Cursors::zoom_view;
        case Cursors::zoom_view:  return Cursors::scope_view;
        default: break;
        }
    }
    return Cursors::unknown_view;
}

void ScopeSettings::update_cursor(Cursors::eCursor aCursor, Cursors::eView aViewId)
{
    if (aViewId >= 0 && aViewId < static_cast<int>(mCursors.size()))
    {
        Cursors::eView fView = getSynchedView(aViewId);
        if (fView  != Cursors::unknown_view)
        {
            double fValue = mCursors[aViewId]->getCursor(aCursor);
            mCursors[fView]->setCursor(aCursor, fValue, false);
        }
        if (mActiveViewCursor != aViewId)
        {
            ui->groupCursorView->setCurrentIndex(aViewId);
            mActiveViewCursor = aViewId;
            update_horizontal_cursor_unit();
            ui->ckVertical->setChecked(mCursors[mActiveViewCursor]->isVerticalVisible());
            ui->ckHorizontal->setChecked(mCursors[mActiveViewCursor]->isHorizontalVisible());
        }
    }

    double fFactor = mChannelParameter[mActiveChannel].getLevelPerDivision() * 10;
    switch (aCursor)
    {
        case Cursors::vertical_1:     ui->groupVerticalCursor1->value(mCursors[mActiveViewCursor]->getCursor(aCursor) * fFactor); break;
        case Cursors::vertical_2:     ui->groupVerticalCursor2->value(mCursors[mActiveViewCursor]->getCursor(aCursor) * fFactor); break;
        case Cursors::horizontal_1:   ui->groupHorizontalCursor1->value(mCursors[mActiveViewCursor]->getCursor(aCursor)); break;
        case Cursors::horizontal_2:   ui->groupHorizontalCursor2->value(mCursors[mActiveViewCursor]->getCursor(aCursor)); break;
        default: break;
    }
    switch (aCursor)
    {
        case Cursors::vertical_1: case Cursors::vertical_2:
        {
            double fDifference = mCursors[mActiveViewCursor]->getCursor(Cursors::vertical_2)-mCursors[mActiveViewCursor]->getCursor(Cursors::vertical_1);
            fDifference *= fFactor;
            ui->groupVerticalDifference->value(fDifference);
            if (fDifference != 0.0)
            {
                ui->groupVerticalInverseDifference->value(1.0 / fabs(fDifference));
            }
            else
            {
                ui->editVerticalInverseDifference->setText("");
            }
            break;
        }
        case Cursors::horizontal_1: case Cursors::horizontal_2:
        {
            double fDifference = mCursors[mActiveViewCursor]->getCursor(Cursors::horizontal_2)-mCursors[mActiveViewCursor]->getCursor(Cursors::horizontal_1);
            ui->groupHorizontalDifference->value(fDifference);
            if (fDifference != 0.0)
            {
                ui->groupHorizontalInverseDifference->value(1.0 / fabs(fDifference));
            }
            else
            {
                ui->editHorizontalInverseDifference->setText("");
            }
            break;
        }
        default: break;
    }
}

void ScopeSettings::on_editVerticalCursor1_textEdited(const QString &)
{
    if (mActiveViewCursor < static_cast<int>(mCursors.size()))
    {
        double fValue  = ui->groupVerticalCursor1->value() / (mChannelParameter[mActiveChannel].getLevelPerDivision() * 10);
        mCursors[mActiveViewCursor]->setCursor(Cursors::vertical_1, fValue, false);
        Cursors::eView fView = getSynchedView(mActiveViewCursor);
        if (fView  != Cursors::unknown_view)
        {
            mCursors[fView]->setCursor(Cursors::vertical_1, fValue, false);
        }
    }
}

void ScopeSettings::on_editVerticalCursor2_textEdited(const QString &)
{
    if (mActiveViewCursor < static_cast<int>(mCursors.size()))
    {
        double fValue  = ui->groupVerticalCursor2->value() / (mChannelParameter[mActiveChannel].getLevelPerDivision() * 10);
        mCursors[mActiveViewCursor]->setCursor(Cursors::vertical_2, fValue, false);
        Cursors::eView fView = getSynchedView(mActiveViewCursor);
        if (fView  != Cursors::unknown_view)
        {
            mCursors[fView]->setCursor(Cursors::vertical_2, fValue, false);
        }
    }
}

void ScopeSettings::on_editHorizontalCursor1_textEdited(const QString &)
{
    if (mActiveViewCursor < static_cast<int>(mCursors.size()))
    {
        double fValue  = ui->groupHorizontalCursor1->value();
        mCursors[mActiveViewCursor]->setCursor(Cursors::horizontal_1, fValue, false);
        Cursors::eView fView = getSynchedView(mActiveViewCursor);
        if (fView  != Cursors::unknown_view)
        {
            mCursors[fView]->setCursor(Cursors::horizontal_1, fValue, false);
        }
    }
}

void ScopeSettings::on_editHorizontalCursor2_textEdited(const QString &)
{
    if (mActiveViewCursor < static_cast<int>(mCursors.size()))
    {
        double fValue  = ui->groupHorizontalCursor2->value();
        mCursors[mActiveViewCursor]->setCursor(Cursors::horizontal_2, fValue, false);
        Cursors::eView fView = getSynchedView(mActiveViewCursor);
        if (fView  != Cursors::unknown_view)
        {
            mCursors[fView]->setCursor(Cursors::horizontal_2, fValue, false);
        }
    }
}

void ScopeSettings::on_checkHoldOffTimeActive_clicked(bool checked)
{
    if (checked)
    {
        mTriggerParameter.mHoldOffPosition = InvalidIndex;
        ui->checkHoldOffEventActive->setChecked(false);
    }
    mTriggerParameter.mHoldOffType = checked ? TriggerParameter::HoldOff::Time : TriggerParameter::HoldOff::Off;
}

void ScopeSettings::on_editHoldOffTime_textEdited(const QString &)
{
    mTriggerParameter.mHoldOffTime = ui->groupHoldOffTime->value();
}

void ScopeSettings::on_checkHoldOffEventActive_clicked(bool checked)
{
    if (checked)
    {
        mTriggerParameter.mHoldOffPosition = InvalidIndex;
        ui->checkHoldOffTimeActive->setChecked(false);
    }
    mTriggerParameter.mHoldOffType = checked ? TriggerParameter::HoldOff::Events : TriggerParameter::HoldOff::Off;
}

void ScopeSettings::on_checkTriggerDelayActive_clicked(bool checked)
{
    mTriggerParameter.mDelayActive = checked;
    mTriggerParameter.mDelay       = ui->groupTriggerDelay->value();
}

void ScopeSettings::on_checkSlopeTriggerActive_clicked(bool checked)
{
    mTriggerParameter.mSlopeActive = checked;
}

void ScopeSettings::on_editHoldOffEvents_textEdited(const QString &)
{
   mTriggerParameter.mHoldOffEvents = DataExchange::getValue(ui->editHoldOffEvents, 0, 1000000, 1);
}

void ScopeSettings::on_editTriggerDelay_textEdited(const QString &)
{
    mTriggerParameter.mDelay = ui->groupTriggerDelay->value();
}

void ScopeSettings::on_editSlopeTrigger_textEdited(const QString &)
{
    mTriggerParameter.mSlope = ui->groupSlopeTrigger->value();
}



void ScopeSettings::on_editVerticalCursor1_editingFinished()
{
    mCursors[mActiveViewCursor]->setCursor(Cursors::vertical_1, ui->groupVerticalCursor1->value());
}

void ScopeSettings::on_editVerticalCursor2_editingFinished()
{
    mCursors[mActiveViewCursor]->setCursor(Cursors::vertical_2, ui->groupVerticalCursor2->value());
}

void ScopeSettings::on_editHorizontalCursor1_editingFinished()
{
    mCursors[mActiveViewCursor]->setCursor(Cursors::horizontal_1, ui->groupHorizontalCursor1->value());
}

void ScopeSettings::on_editHorizontalCursor2_editingFinished()
{
    mCursors[mActiveViewCursor]->setCursor(Cursors::horizontal_2, ui->groupHorizontalCursor2->value());
}

void ScopeSettings::on_clickedColorButtons(int index)
{
    QColor*     fColor       = 0;
    MainWindow* fMain        = dynamic_cast<MainWindow*>(parentWidget());
    QColor      fBackground  = fMain->getBackgroundColor();
    QColor      fGrid        = fMain->getGridColor();
    const int   fCurveColors = static_cast<int>(fMain->mCurveColors.size());
    QString fTitle = "Select color of ";
    if (index >= 0 && index < fCurveColors)
    {
        fColor = &fMain->mCurveColors[index];
        fTitle += "Channel" + QString::number(index+1);
    }
    else if (index == fCurveColors)
    {
        fColor = &fBackground;
        fTitle += "scope view background";
    }
    else if (index == fCurveColors+1)
    {
        fColor = &fGrid;
        fTitle += "scope view grid";
    }

    if (fColor)
    {
        QColor fNewColor = QColorDialog::getColor(*fColor, this, fTitle);
        if (fNewColor.isValid())
        {
            *fColor = fNewColor;

            fMain->updateChannelColors();
            fMain->setBackgroundColor(fBackground);
            fMain->setGridColor(fGrid);

            updateColors();
        }
    }

}

void ScopeSettings::on_clickedMathCombine(int index)
{
    mCombineCurves.setType(static_cast<CombineCurves::eType>(index));
}

void ScopeSettings::on_spinChannel1_valueChanged(int arg1)
{
    mCombineCurves.setChannel1(arg1-1);
}

void ScopeSettings::on_spinChannel2_valueChanged(int arg1)
{
    mCombineCurves.setChannel2(arg1-1);
}

void ScopeSettings::on_spinChannelPhase_valueChanged(int arg1)
{
    mPhaseChannel = arg1-1;
}

void ScopeSettings::setValue(double aValue, int aReceiveType)
{
    switch (aReceiveType) {
    case measured_period:
        if (aValue != 0.0)
        {
            ui->valueMeasuredFrequency->value(1 / aValue);
            mAverageFrequency.addValue(ui->valueMeasuredFrequency->value());
            ui->valueMeasuredFrequency_avg->value(mAverageFrequency.getAverage());
        }
        if (mChannelParameter[mTriggerParameter.mActiveChannel].mName.size())
        {
            ui->valueChannel->setText(mChannelParameter[mTriggerParameter.mActiveChannel].mName);
        }
        else
        {
            ui->valueChannel->setText(QString::number(mTriggerParameter.mActiveChannel+1));
        }
        break;
    case calculated_rms:
        ui->valueCalculatedRMS->value(aValue);
        mAverageRMSLevel.addValue(aValue);
        ui->valueCalculatedRMS_avg->value(mAverageRMSLevel.getAverage());
        break;
    case calculated_avg:
        ui->valueCalculatedAVG->value(aValue);
        mAverageAVGLevel.addValue(aValue);
        ui->valueCalculatedAVG_avg->value(mAverageAVGLevel.getAverage());
        break;
    case measured_min:
        ui->valueMeasuredMin->value(aValue);
        break;
    case measured_max:
        ui->valueMeasuredMax->value(aValue);
        break;
    case measured_phase:
        ui->valueMeasuredPhase->value(aValue);
        break;
    default:
        break;
    }
}

int ScopeSettings::getPhaseChannel() const
{
    return mPhaseChannel;
}

void ScopeSettings::on_Tabulator_currentChanged(int index)
{
    switch (index)
    {
    case 0: case 1:
        ui->groupCursorView->show();
        break;
    default:
        ui->groupCursorView->hide();
        break;
    }
    if (mProbesChanged)
    {
        on_buttonBox_accepted();
        updateParameters();
    }
}

void ScopeSettings::on_ChannelTableListItemChanged ( QStandardItem * item )
{
    if (!mInitialize)
    {
        switch (item->column())
        {
        case eName:
            if (item->row() < static_cast<int>(mChannelParameter.size()))
            {
                mChannelParameter[item->row()].mName = item->text();
            }
            break;
        case eProbe:
        {
            double fD1, fD2;
            sscanf(item->text().toStdString().c_str(), "%lf : %lf", &fD1, &fD2);
            mChannelParameter[item->row()].setAudioScale() = fD1 / fD2;
        }   break;
        }
    }
}

void ScopeSettings::on_ProbeListItemChanged ( QStandardItem * /* item */ )
{
    if (!mInitialize)
    {
        mProbesChanged = true;
    }
}


void ScopeSettings::on_buttonAddProbe_clicked()
{
    mProbeListModel->appendRow(new QStandardItem("1 : 1"));
    mProbesChanged = true;
}

void ScopeSettings::on_buttonRemoveProbe_clicked()
{
    if ( mProbeListModel->rowCount() > 1)
    {
        mProbeListModel->removeRow(ui->probesListView->selectionModel()->currentIndex().row());
        mProbesChanged = true;
    }
}

void ScopeSettings::on_buttonBox_accepted()
{
    MainWindow* fMain = dynamic_cast<MainWindow*>(parentWidget());
    if (mProbesChanged && fMain)
    {
        int fRows = mProbeListModel->rowCount();
        QList<double> fValues;
        for (int i=0; i<fRows; ++i)
        {
            double fD1, fD2;
            sscanf(mProbeListModel->data(mProbeListModel->index(i, 0)).toString().toStdString().c_str(), "%lf : %lf", &fD1, &fD2);
            fValues.append(fD1 / fD2);
        }
        fMain->setProbeValues(fValues);
        ui->channelTableView->setModel(0);
        delete mChannelTableListModel;
        delete mProbeCombo;
        mChannelTableListModel = 0;

    }
    mProbesChanged = false;
}

