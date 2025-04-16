#include "main.h"
#include "audiosettings.h"
#include "ui_audiosettings.h"
#include "qaudiorecord.h"
#include "qaudiooutstream.h"
#include <QShowEvent>



AudioSettings::AudioSettings(QWidget *parent, QAudioRecord& aAudioInput, QAudioOutStream& aAudioOutput) :
  QDialog(parent)
, mShowFGinScope(false)
, ui(new Ui::Settings)
, mAudioInput(aAudioInput)
, mAudioOutput(aAudioOutput)
, mInitializingComboDeviceID(false)
{
    ui->setupUi(this);
}

AudioSettings::~AudioSettings()
{
    delete ui;
}

void AudioSettings::showEvent (QShowEvent * event)
{
    if (event->type() == QEvent::Show)
    {
        updateParameters();
    }
}


void AudioSettings::initComboDeviceID(QComboBox& aCombo, const QAudioParameter& aParam, int aID)
{
    mInitializingComboDeviceID = true;
    aCombo.clear();

#if (_USE_QT_AUDIO == 1)
    for (int id=0; id<aParam.getNumberOfDevices(); ++id)
    {
        aCombo.addItem(aParam.getDeviceInfo(id).deviceName(), QVariant(id));
    }
    int fIndex = aCombo.findData(QVariant(aID));
#else
    for (int id=0; id<aParam.getNumberOfDevices(); ++id)
    {
        aCombo.addItem(QString::number(id));
    }
    int fIndex = aCombo.findText(QString::number(aID));
#endif

    aCombo.setCurrentIndex(fIndex);

    mInitializingComboDeviceID = false;

}

void AudioSettings::initComboFrequencies(QComboBox& aCombo, const QAudioParameter& aParam, const QString& aCurrentText)
{
    aCombo.clear();
    std::vector<double> fFrequencies = aParam.getSupportedSampleFrequencies();
    for (double fFrequency : fFrequencies)
    {
        aCombo.addItem(QString::number(fFrequency));
    }
    int fIndex = aCombo.findText(aCurrentText);
    aCombo.setCurrentIndex(fIndex);
}

void AudioSettings::initComboFormats(QComboBox& aCombo, const QAudioParameter&
                                     #if (_USE_QT_AUDIO != 1)
                                     aParam
                                     #endif
                                     , int aFormat
                                     )
{
    aCombo.clear();
#if (_USE_QT_AUDIO == 1)
#define COMBO_ADD(X) aCombo.addItem(QAudioParameter::getStringFromSampleType(X), QVariant(X))
    COMBO_ADD(QAudioFormat::Float);
    COMBO_ADD(QAudioFormat::SignedInt);
    COMBO_ADD(QAudioParameter::SignedInt16);
    COMBO_ADD(QAudioParameter::SignedInt32);
    COMBO_ADD(QAudioFormat::UnSignedInt);
    COMBO_ADD(QAudioParameter::UnSignedInt16);
    COMBO_ADD(QAudioParameter::UnSignedInt32);
#undef COMBO_ADD
#else
    unsigned long fFormats = aParam.getSupportedSampleFormats();
    if (fFormats & QAudioParameter::isFormatFloat())  aCombo.addItem("Float" , QVariant(QAudioParameter::isFormatFloat()));
    if (fFormats & QAudioParameter::isFormatInt32())  aCombo.addItem("Int32" , QVariant(QAudioParameter::isFormatInt32()));
    if (fFormats & QAudioParameter::isFormatInt24())  aCombo.addItem("Int24" , QVariant(QAudioParameter::isFormatInt24()));
    if (fFormats & QAudioParameter::isFormatInt16())  aCombo.addItem("Int16" , QVariant(QAudioParameter::isFormatInt16()));
    if (fFormats & QAudioParameter::isFormatInt8())   aCombo.addItem("Int8"  , QVariant(QAudioParameter::isFormatInt8()));
    if (fFormats & QAudioParameter::isFormatUInt8())  aCombo.addItem("UInt8" , QVariant(QAudioParameter::isFormatUInt8()));
    if (fFormats & QAudioParameter::isFormatCustom()) aCombo.addItem("Custom", QVariant(QAudioParameter::isFormatCustom()));
#endif
    int fIndex = aCombo.findData(QVariant(aFormat));
    aCombo.setCurrentIndex(fIndex);
}

void AudioSettings::initComboChannels(QComboBox& aCombo, const QAudioParameter& aParam, const QString& aCurrentText)
{
    aCombo.clear();
    for (int channel=1; channel <= aParam.getMaxChannels(); ++channel)
    {
        aCombo.addItem(QString::number(channel));
    }
    int fIndex = aCombo.findText(aCurrentText);
    aCombo.setCurrentIndex(fIndex);
}

void AudioSettings::updateParameters()
{
    ui->radioModeSimulation->setChecked(mAudioInput.getType()==QAudioRecord::eSimulation);
    ui->radioModeAudioInput->setChecked(mAudioInput.getType()!=QAudioRecord::eSimulation);

    ui->labelInputDeviceName->setText(mAudioInput.getAudioParameters().getName());
    ui->labelOutputDeviceName->setText(mAudioOutput.getAudioParameters().getName());
    ui->checkShowFGinScope->setChecked(mShowFGinScope);

    initComboDeviceID(*ui->comboDeviceID, mAudioInput.getAudioParameters(), mAudioInput.getDeviceID());
    initComboDeviceID(*ui->comboDeviceID_2, mAudioOutput.getAudioParameters(), mAudioOutput.getDeviceID());

    initComboFrequencies(*ui->comboSampleFrequency, mAudioInput.getAudioParameters(), QString::number(mAudioInput.getSampleFrequency()));
    initComboFrequencies(*ui->comboSampleFrequency_2, mAudioOutput.getAudioParameters(), QString::number(mAudioOutput.getSampleFrequency()));

    initComboFormats(*ui->comboFormats, mAudioInput.getAudioParameters(), mAudioInput.getSampleFormat());
    initComboFormats(*ui->comboFormats_2, mAudioOutput.getAudioParameters(), mAudioOutput.getSampleFormat());

    initComboChannels(*ui->comboChannels, mAudioInput.getAudioParameters(), QString::number(mAudioInput.getChannels()));
    initComboChannels(*ui->comboChannels_2, mAudioOutput.getAudioParameters(), QString::number(mAudioOutput.getChannels()));

    ui->edtBufferSize->setVisible(true);
    ui->labelBufferSize->setVisible(true);
    double fBuffersize = (mAudioInput.getBufferSize() * mAudioInput.getBuffers() ) / mAudioInput.getSampleFrequency();
    ui->edtBufferSize->setText(QString::number(fBuffersize));

    ui->edtSampleSize->setVisible(true);
    ui->labelSampleSize->setVisible(true);
    double fSampleSize = mAudioInput.getBufferSize() / mAudioInput.getSampleFrequency() ;
    ui->edtSampleSize->setText(QString::number(fSampleSize*1000));
    ui->labelLastErrorText->setText( mAudioInput.getLastError());

    ui->edtLatency->setText(QString::number(mAudioInput.getSuggestedLatency()));
    ui->edtLatency_2->setText(QString::number(mAudioOutput.getSuggestedLatency()));
    ui->labelLastErrorText->setText( mAudioOutput.getLastError());
}


void AudioSettings::on_btnApply_clicked()
{
    if (   ui->comboChannels->count()
        && ui->comboFormats->count()
        && ui->comboSampleFrequency->count())
    {
        if (ui->tabWidget->currentIndex() == 0)
        {
            mAudioInput.stopDevice();
            int fChannels = mAudioInput.getChannels();
            if (ui->radioModeSimulation->isChecked())
            {
                mAudioInputType = QAudioRecord::eSimulation;
            }
            if (ui->radioModeAudioInput->isChecked())
            {
#if (_USE_QT_AUDIO == 1)
                mAudioInputType = QAudioRecord::eQtAudio;
#else
                mAudioInputType = QAudioRecord::ePortAudio;
#endif
            }

            mAudioInput.setSampleParameter(
                        ui->comboSampleFrequency->currentText().toDouble(),
                        ui->edtBufferSize->text().toDouble(),
                        ui->edtSampleSize->text().toDouble()*0.001);
#if (_USE_QT_AUDIO == 1)
            mAudioInput.setDeviceID(ui->comboDeviceID->currentData().toInt());
            mAudioInput.setSampleFormat(ui->comboFormats->currentData().toInt());
#else
            mAudioInput.setDeviceID(ui->comboDeviceID->currentText().toInt());
            mAudioInput.setSampleFormat(ui->comboFormats->itemData(ui->comboFormats->currentIndex()).toInt());
#endif
            mAudioInput.setChannels(ui->comboChannels->currentText().toInt());
            mAudioInput.setSuggestedLatency(ui->edtLatency->text().toDouble());

            mAudioInput.setType(static_cast<QAudioRecord::eType>(mAudioInputType));
            if (fChannels != mAudioInput.getChannels())
            {
                done(Restart);
                return;
            }
            if (mAudioInput.getType() == QAudioRecord::eStopDevice)
            {
#if (_USE_QT_AUDIO == 1)
                mAudioInput.setType(QAudioRecord::eQtAudio);
#endif
            }
            mAudioInput.start();
            while (mAudioInput.isStarting() && mAudioInput.getType() != QAudioRecord::eStopDevice)
            {
#if (_USE_QT_AUDIO == 1)
                QThread::msleep(100);
#else
                usleep(100000);
#endif
            }
        }
        else
        {
            mAudioOutput.stopDevice();
#if (_USE_QT_AUDIO == 1)
            mAudioOutput.setDeviceID(ui->comboDeviceID_2->currentData().toInt());
            mAudioOutput.setSampleFormat(ui->comboFormats_2->currentData().toInt());
#else
            mAudioOutput.setDeviceID(ui->comboDeviceID_2->currentText().toInt());
            mAudioOutput.setSampleFormat(ui->comboFormats_2->itemData(ui->comboFormats_2->currentIndex()).toInt());
#endif
            mAudioOutput.setChannels(ui->comboChannels_2->currentText().toInt());
            mAudioOutput.setSuggestedLatency(ui->edtLatency_2->text().toDouble());
            mAudioOutput.setSampleFrequency(ui->comboSampleFrequency_2->currentText().toDouble());

            mAudioOutput.start();
        }
        updateParameters();
    }
    else
    {
        ui->labelLastErrorText->setText("wrong parameters for this device ID");
    }
}

void AudioSettings::on_btnOk_clicked()
{
    on_btnApply_clicked();
    close();
}


void AudioSettings::on_tabWidget_currentChanged(int )
{

}

void AudioSettings::on_checkShowFGinScope_clicked(bool checked)
{
    mShowFGinScope = checked;
}

void AudioSettings::on_comboDeviceID_currentIndexChanged(int index)
{
    if (!mInitializingComboDeviceID && index != -1)
    {
#if (_USE_QT_AUDIO == 1)
        QAudioParameter fParamInput = mAudioInput.getAudioParameters();
#else
        QAudioParameter fParamInput;
#endif
        fParamInput.init(index, 1);
        ui->labelInputDeviceName->setText(fParamInput.getName());
        initComboChannels(*ui->comboChannels, fParamInput, QString::number(mAudioInput.getChannels()));
        initComboFormats(*ui->comboFormats, fParamInput, mAudioInput.getSampleFormat());
        initComboFrequencies(*ui->comboSampleFrequency, fParamInput, QString::number(mAudioInput.getSampleFrequency()));
    }
}

void AudioSettings::on_comboDeviceID_2_currentIndexChanged(int index)
{
    if (!mInitializingComboDeviceID && index != -1)
    {
#if (_USE_QT_AUDIO == 1)
        QAudioParameter fParam = mAudioOutput.getAudioParameters();
#else
        QAudioParameter fParam;
#endif
        fParam.init(index, 1);
        ui->labelOutputDeviceName->setText(fParam.getName());
        initComboChannels(*ui->comboChannels_2, fParam, QString::number(mAudioOutput.getChannels()));
        initComboFormats(*ui->comboFormats_2, fParam, mAudioOutput.getSampleFormat());
        initComboFrequencies(*ui->comboSampleFrequency_2, fParam, QString::number(mAudioOutput.getSampleFrequency()));
    }
}

void AudioSettings::on_comboDeviceID_highlighted(int index)
{
#if (_USE_QT_AUDIO == 1)
    if (ui->tabWidget->currentIndex() == 0)
    {
        QAudioParameter fParam = mAudioInput.getAudioParameters();
        fParam.init(index, 1);
        ui->labelInputDeviceName->setText(fParam.getName());
    }
    else
    {
        QAudioParameter fParam = mAudioOutput.getAudioParameters();
        fParam.init(index, 1);
        ui->labelOutputDeviceName->setText(fParam.getName());
    }
#endif
    index = index;
}
