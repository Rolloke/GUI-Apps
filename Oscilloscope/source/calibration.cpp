#include "calibration.h"
#include "ui_calibration.h"
#include "Channel.h"
#include "main.h"

#include <cmath>
#include <QShowEvent>

Calibration::Calibration(QWidget *parent, std::vector<Channel>& aChannelParam) :
    QDialog(parent)
, ui(new Ui::Calibration)
, mChannelParameter(aChannelParam)
, mChannel(0)
{
    ui->setupUi(this);
    ui->textLevelMin->setUnit("V");
    ui->textLevelMax->setUnit("V");
    ui->textLevelOffset->setUnit("V");
    ui->textLevelTotal->setUnit("V");
    ui->textLevelTotal_RMS->setUnit("V");
    ui->textLevelMin->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->textLevelMax->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->textLevelOffset->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->textLevelTotal->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->textLevelTotal_RMS->getPrefix().setPrefixChange(UnitPrefix::auto_change);
    ui->groupOffset->setPrefixChange(UnitPrefix::auto_change);
    ui->groupMeasuredRMS->setTextFlags(Qt::AlignTop|Qt::AlignLeft);
}

Calibration::~Calibration()
{
    delete ui;
}

void Calibration::on_comboChannel_currentIndexChanged(int index)
{
    if (mChannel != static_cast<unsigned int>(index))
    {
        mChannel = index;
        ui->groupScale->value(mChannelParameter[mChannel].getAudioInputScale());
        ui->groupOffset->value(mChannelParameter[mChannel].getAudioInputOffset());
    }
}

void Calibration::updateParameters()
{
    const double fRMS_Factor = 1 / sqrt(8.0);
    if (ui->comboChannel->count() < static_cast<int>(mChannelParameter.size()-1))
    {
        for (unsigned int i=0; i<mChannelParameter.size()-1; ++i)
        {
            ui->comboChannel->addItem(QString::number(i+1));
        }
        ui->comboChannel->setCurrentIndex(0);
        ui->groupScale->value(mChannelParameter[mChannel].getAudioInputScale());
        ui->groupOffset->value(mChannelParameter[mChannel].getAudioInputOffset());
    }
    if (mChannel < mChannelParameter.size())
    {
        double fAvgMinVal = mChannelParameter[mChannel].getAvgMinValue();
        double fAvgMaxVal = mChannelParameter[mChannel].getAvgMaxValue();
        ui->textLevelMin->value(fAvgMinVal);
        ui->textLevelMax->value(fAvgMaxVal);
        double fTotalVal = fAvgMaxVal-fAvgMinVal;
        ui->textLevelTotal->value(fTotalVal);
        ui->textLevelTotal_RMS->value(fTotalVal * fRMS_Factor);
        double fOffsetVal = (fAvgMaxVal+fAvgMinVal)*0.5;
        ui->textLevelOffset->value(fOffsetVal / mChannelParameter[mChannel].getAudioInputScale());
    }
}

void Calibration::on_btnApply_clicked()
{
    mChannelParameter[mChannel].setAudioInputScale()  = ui->groupScale->value();
    mChannelParameter[mChannel].setAudioInputOffset() = ui->groupOffset->value();
}

void Calibration::showEvent(QShowEvent *event)
{
    if (event->type()==QEvent::Show)
    {
        updateParameters();
    }
}

void Calibration::on_btnCalcScalingFactor_clicked()
{
    ui->groupScale->value(ui->groupMeasuredRMS->value()/ui->textLevelTotal_RMS->value());
}


void Calibration::on_btnSetScalingOffset_clicked()
{
    double fOffset = ui->textLevelOffset->value() / mChannelParameter[mChannel].getAudioScale();
    ui->groupOffset->value( -fOffset );
}

void Calibration::on_btnResetScalingAndOffset_clicked()
{
    ui->groupScale->value(1.0);
    ui->groupOffset->value(0.0);
}
