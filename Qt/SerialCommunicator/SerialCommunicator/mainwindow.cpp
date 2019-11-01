#include "mainwindow.h"
#include "ui_mainwindow.h"

#if QT_VERSION >= 0x050000
#include <QSerialPortInfo>
#endif

#include <QSettings>
#include <QFileDialog>
#include <QFileInfo>
#include <QThread>

#include <cmath>
#include <sstream>
#include <iostream>

namespace config
{
    const QString  sFilename("/.config/SerialCommunicator.ini");

    const QString  sGroupFilter("Serial");
    const QVariant sSerialBaudDefault("38400");

    const QString  sGroupFile("File");

    const QString  sGroupMisc("Miscelaneous");
    const QString  sMiscCommand("command");
    const QString  sMiscPin("pin");

    const QString  sGroupDeptSim("DepthSimulation");

}

#define STORE_IP(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_IR(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_VAR(SETTING, VALUE) SETTING.setValue(getSettingsName(#VALUE), VALUE)

#define LOAD_IP(SETTING, ITEM, SET, GET, CONVERT) ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_IR(SETTING, ITEM, SET, GET, CONVERT) ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_VAR(SETTING, VALUE, CONVERT) VALUE = SETTING.value(getSettingsName(#VALUE), VALUE).CONVERT()

QString getSettingsName(const QString& aItemName);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mReceiveTimer(0)
    , mSendTimer(0)
    , mDepthSimulationTimer(0)
    , mDepthUp(true)
    , mMaxPinNumbers(10)
    , mSelectedInputPin(0)
    , mSelectedOutputPin(0)
    , mCanUpdateComboBox(true)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings fSettings(QDir::homePath() + config::sFilename, QSettings::IniFormat);
    fSettings.beginGroup(config::sGroupFilter);

#if QT_VERSION >= 0x050000
    QString fName;
    QList<QSerialPortInfo> fPorts = QSerialPortInfo::availablePorts();
    foreach (auto fInfo, fPorts)
    {
        fName = fInfo.portName();
        if (fInfo.manufacturer().size())
        {
            fName += " (" + fInfo.manufacturer() + ")";
        }
        else if (fInfo.description().size())
        {
            fName += " (" + fInfo.description() + ")";
        }
        ui->comboPort->addItem(fName, QVariant(fInfo.systemLocation()));
    }

    QString fPort;
    LOAD_VAR(fSettings, fPort, toString);
    int fPortindex = ui->comboPort->findData(QVariant(fPort));
    if (fPortindex != -1)
    {
        ui->comboPort->setCurrentIndex(fPortindex);
    }
    else
    {
        ui->comboPort->addItem(fPort, QVariant(fPort));
        fPortindex = ui->comboPort->count()-1;
        ui->comboPort->setCurrentIndex(fPortindex);
    }

    QList<qint32> fBaudRates = QSerialPortInfo::standardBaudRates();
    foreach (auto fBaudrate, fBaudRates)
    {
        ui->comboBaud->addItem(QString::number(fBaudrate));
    }
#endif
    int fBaudindex = LOAD_IP(fSettings, ui->comboBaud, findText, currentText, toString);
    ui->comboBaud->setCurrentIndex(fBaudindex);

    LOAD_IP(fSettings, ui->ckReceive, setChecked, isChecked, toBool);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupFile);
    LOAD_IP(fSettings, ui->lineFileName, setText, text, toString);
    LOAD_IP(fSettings, ui->ckRepeatFile, setChecked, isChecked, toBool);
    LOAD_IP(fSettings, ui->lineRate_ms, setText, text, toString);
    LOAD_IP(fSettings, ui->ckLineByLine, setChecked, isChecked, toBool);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupMisc);
    LOAD_IP(fSettings, ui->ckReceive, setChecked, isChecked, toBool);
    on_ckReceive_clicked(ui->ckReceive->isChecked());
    LOAD_IP(fSettings, ui->ckReportSent, setChecked, isChecked, toBool);
    LOAD_IP(fSettings, ui->ckTriggerText, setChecked, isChecked, toBool);
    LOAD_IP(fSettings, ui->ckHexBinary, setChecked, isChecked, toBool);

    int size = fSettings.beginReadArray(config::sMiscCommand);
    for (int i = 0; i < size; ++i)
    {
        fSettings.setArrayIndex(i);
        ui->comboSendCmd->addItem(fSettings.value(config::sMiscCommand).toString());
    }
    fSettings.endArray();

    size = fSettings.beginReadArray(config::sMiscPin);
    for (int i = 0; i < size; ++i)
    {
        fSettings.setArrayIndex(i);
        QString fPinText = fSettings.value(config::sMiscPin).toString();
        std::stringstream fPinStream(fPinText.toStdString());
        PinType fPin;
        fPinStream >> fPin;
        mPins.push_back(fPin);
    }
    fSettings.endArray();

    LOAD_VAR(fSettings, mMaxPinNumbers, toInt);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupDeptSim);
    LOAD_IP(fSettings, ui->lineMinDepth_m, setText, text, toString);
    LOAD_IP(fSettings, ui->lineMaxDepth_m, setText, text, toString);
    LOAD_IP(fSettings, ui->lineSoundVelocity_m_p_s, setText, text, toString);
    LOAD_IP(fSettings, ui->linePrefix, setText, text, toString);
    LOAD_IP(fSettings, ui->lineStep_m, setText, text, toString);
    LOAD_IP(fSettings, ui->lineStepPercent, setText, text, toString);
    LOAD_IP(fSettings, ui->lineSimInterval_ms, setText, text, toString);

    LOAD_IP(fSettings, ui->radioStep_meter, setChecked, isChecked, toBool);
    bool fMeter = ui->radioStep_meter->isChecked();
    ui->radioStep_meter->setChecked(fMeter);
    on_radioStep_meter_clicked(fMeter);
    ui->radioStep_percent->setChecked(!fMeter);

    LOAD_IP(fSettings, ui->ckPingBasedSim, setChecked, isChecked, toBool);
    on_ckPingBasedSim_clicked(ui->ckPingBasedSim->isChecked());

    fSettings.endGroup();

    ui->lineCurrentDepth_m->setText(ui->lineMinDepth_m->text());
    calculateSteps(fMeter);

    enableSendButtons(false);
    ui->verticalSliderDepth->setMinimum(ui->lineMinDepth_m->text().toInt());
    ui->verticalSliderDepth->setMaximum(ui->lineMaxDepth_m->text().toInt());

    initializeConfigurationPage();
}

MainWindow::~MainWindow()
{
    QSettings fSettings(QDir::homePath() + config::sFilename, QSettings::IniFormat);
    fSettings.beginGroup(config::sGroupFilter);

    STORE_IP(fSettings, ui->comboPort, currentData().toString);
    STORE_IP(fSettings, ui->comboBaud, currentText);
    STORE_IP(fSettings, ui->ckReceive, isChecked);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupFile);

    STORE_IP(fSettings, ui->lineFileName, text);
    STORE_IP(fSettings, ui->ckRepeatFile, isChecked);
    STORE_IP(fSettings, ui->lineRate_ms, text);
    STORE_IP(fSettings, ui->ckLineByLine, isChecked);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupMisc);
    STORE_IP(fSettings, ui->ckReceive, isChecked);
    STORE_IP(fSettings, ui->ckReportSent, isChecked);
    STORE_IP(fSettings, ui->ckTriggerText, isChecked);
    STORE_IP(fSettings, ui->ckHexBinary, isChecked);

    fSettings.beginWriteArray(config::sMiscCommand);
    int fCount = ui->comboSendCmd->count();
    for (int i = 0; i < fCount; ++i)
    {
        fSettings.setArrayIndex(i);
        fSettings.setValue(config::sMiscCommand,ui->comboSendCmd->itemText(i));
    }
    fSettings.endArray();

    fSettings.beginWriteArray(config::sMiscPin);
    fCount = mPins.size();
    for (int i = 0; i < fCount; ++i)
    {
        fSettings.setArrayIndex(i);
        std::stringstream fPin;
        fPin << mPins[i];
        fSettings.setValue(config::sMiscPin, fPin.str().c_str());
    }

    fSettings.endArray();

    STORE_VAR(fSettings, mMaxPinNumbers);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupDeptSim);
    STORE_IP(fSettings, ui->lineMinDepth_m, text);
    STORE_IP(fSettings, ui->lineMaxDepth_m, text);
    STORE_IP(fSettings, ui->lineSoundVelocity_m_p_s, text);
    STORE_IP(fSettings, ui->linePrefix, text);
    STORE_IP(fSettings, ui->lineStep_m, text);
    STORE_IP(fSettings, ui->lineStepPercent, text);
    STORE_IP(fSettings, ui->lineSimInterval_ms, text);
    STORE_IP(fSettings, ui->ckPingBasedSim, isChecked);
    STORE_IP(fSettings, ui->radioStep_meter, isChecked);
    fSettings.endGroup();

    delete ui;
}

void MainWindow::on_btnOpen_clicked(bool checked)
{
    if (checked)
    {
#if QT_VERSION >= 0x050000
        mPort.setPortName(ui->comboPort->currentData().toString());
#else
        mPort.setPortName(ui->comboPort->itemData(ui->comboPort->currentIndex()).toString());
#endif
        mPort.setBaudRate(ui->comboBaud->currentText().toInt());
        if (mPort.open(QIODevice::ReadWrite))
        {
            ui->statusBar->showMessage(tr("Opened ") + ui->comboPort->currentText());
            enableSendButtons(true);
        }
        else
        {
            ui->statusBar->showMessage(mPort.errorString());
            ui->btnOpen->setChecked(false);
        }
    }
    else
    {
        enableSendButtons(false);
        if (mPort.isOpen())
        {
            mPort.close();
            ui->statusBar->showMessage(tr("Closed ") + ui->comboPort->currentText());
        }
    }
}

void MainWindow::enableSendButtons(bool aEnable)
{
    ui->btnReceive->setEnabled(aEnable);
    ui->btnSend->setEnabled(aEnable);
    ui->btnSendFile->setEnabled(aEnable);
    ui->btnSendCurrentDepth->setEnabled(aEnable);
    ui->btnStartSimulation->setEnabled(aEnable);
    ui->btnSendConfig->setEnabled(aEnable);
    ui->btnRequesConfig->setEnabled(aEnable);
    ui->btnStoreConfig->setEnabled(aEnable);
    ui->btnClearConfig->setEnabled(aEnable);
    ui->btnHelp->setEnabled(aEnable);
}

void MainWindow::on_btnSendFile_clicked(bool checked)
{
    if (checked)
    {
        mFile.setFileName(ui->lineFileName->text());
        if (mFile.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QFileInfo fi(ui->lineFileName->text());
            ui->statusBar->showMessage(tr("Open File: ") + fi.fileName());
            if (ui->ckTriggerText->isChecked())
            {
                sendText();
            }
            else
            {
                mSendTimer = startTimer(ui->lineRate_ms->text().toInt());
            }
        }
        else
        {
            ui->statusBar->showMessage(mFile.errorString());
            ui->btnSendFile->setChecked(false);
        }
    }
    else
    {
        killTimer(mSendTimer);
        mSendTimer = 0;
        if (mFile.isOpen())
        {
            mFile.close();
        }
    }
}


void MainWindow::on_btnSend_clicked()
{
    if (mPort.isOpen())
    {
        mPort.write(ui->comboSendCmd->currentText().toStdString().c_str());
    }
}


void MainWindow::on_btnClear_clicked()
{
    ui->lineReceive->setText("");
}


void MainWindow::on_btnReceive_clicked()
{
    if (mPort.isOpen())
    {
        QByteArray fArray = mPort.read(256);
        if (fArray.size() > 0)
        {
            if (ui->ckHexBinary->isChecked())
            {
                QString fString;
                for (auto &fByte : fArray)
                {
                    fString += QString("%1").arg(static_cast<ushort>(fByte), static_cast<int>(2), static_cast<int>(16));
                }
                ui->lineReceive->append(fString);
            }
            else
            {
                ui->lineReceive->append(QString(fArray.data()));
            }
            if (ui->btnSendFile->isChecked() && ui->ckTriggerText->isChecked())
            {
                sendText();
            }
            if (ui->btnStartSimulation->isChecked() && ui->ckPingBasedSim->isChecked())
            {
                changeDepth();
            }
        }
    }
}


void MainWindow::on_ckReceive_clicked(bool checked)
{
    if (checked)
    {
        mReceiveTimer = startTimer(100);
    }
    else
    {
        killTimer(mReceiveTimer);
        mReceiveTimer = 0;
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == mReceiveTimer)
    {
        on_btnReceive_clicked();
    }
    else if (event->timerId() == mSendTimer)
    {
        sendText();
    }
    else if (event->timerId() == mDepthSimulationTimer)
    {
        changeDepth();
    }
}

void MainWindow::changeDepth()
{
    double fDepth = ui->lineCurrentDepth_m->text().toDouble();
    if (ui->radioStep_meter->isChecked())
    {
        if (mDepthUp)
        {
            if (fDepth < ui->lineMaxDepth_m->text().toDouble())
            {
                fDepth += ui->lineStep_m->text().toDouble();
            }
            else
            {
                fDepth = ui->lineMaxDepth_m->text().toDouble();
                mDepthUp = false;
            }
        }
        else
        {
            if (fDepth > ui->lineMinDepth_m->text().toDouble())
            {
                fDepth -= ui->lineStep_m->text().toDouble();
            }
            else
            {
                fDepth = ui->lineMinDepth_m->text().toDouble();
                mDepthUp = true;
            }
        }
    }
    else
    {
        if (mDepthUp)
        {
            if (fDepth < ui->lineMaxDepth_m->text().toDouble())
            {
                fDepth += (fDepth * ui->lineStepPercent->text().toDouble() / 100);
            }
            else
            {
                fDepth = ui->lineMaxDepth_m->text().toDouble();
                mDepthUp = false;
            }
        }
        else
        {
            if (fDepth > ui->lineMinDepth_m->text().toDouble())
            {
                fDepth -= (fDepth * ui->lineStepPercent->text().toDouble() / 100);
            }
            else
            {
                fDepth = ui->lineMinDepth_m->text().toDouble();
                mDepthUp = true;
            }
        }
    }

    ui->lineCurrentDepth_m->setText(QString::number(fDepth));
    ui->verticalSliderDepth->setValue(static_cast<int>(fDepth + 0.5));

    sendCurrentDepth();
}

void  MainWindow::sendCurrentDepth()
{
    if (mPort.isOpen())
    {
        QString fLine = ui->linePrefix->text();
        double fTime_s = ui->lineCurrentDepth_m->text().toDouble() * 2 / ui->lineSoundVelocity_m_p_s->text().toDouble();
        fLine += QString::number(static_cast<int>(0.5 + 1000 * fTime_s));
#if QT_VERSION >= 0x050000
        mPort.write(fLine.toStdString().c_str(), fLine.size());
#else
        mPort.write(fLine);
#endif
    }
}


void MainWindow::sendText()
{
    if (mPort.isOpen())
    {
        if (ui->ckLineByLine->isChecked())
        {
            QByteArray fLine = mFile.readLine();
            if (fLine.size() > 0)
            {
                mPort.write(fLine);
                if (ui->ckReportSent->checkState() == Qt::Checked)
                {
                    ui->lineReceive->append("sent: " + fLine);
                }
            }
        }
        else
        {
            QByteArray fText = mFile.readAll();
            if (fText.size() > 0)
            {
                mPort.write(fText);
            }
        }
    }
    if (mFile.atEnd())
    {
        if (ui->ckRepeatFile->isChecked())
        {
            ui->statusBar->showMessage(tr("Repeating File"));
            mFile.seek(0);
        }
        else
        {
            ui->statusBar->showMessage(tr("Stopped File"));
            ui->btnSendFile->setChecked(false);
            mFile.close();
            killTimer(mSendTimer);
            mSendTimer = 0;
        }
    }
}

void MainWindow::on_btnBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), ui->lineFileName->text(), tr("Textfile (*.txt *.*)"));
    if (fileName.size())
    {
        ui->lineFileName->setText(fileName);
    }
}

void MainWindow::on_ckTriggerText_clicked(bool checked)
{
    ui->lineRate_ms->setEnabled(!checked);
    ui->labelRate->setEnabled(!checked);
}

void MainWindow::on_btnStartSimulation_clicked(bool checked)
{
    if (checked)
    {
        ui->lineCurrentDepth_m->setText(ui->lineMinDepth_m->text());
        if (ui->ckPingBasedSim->isChecked())
        {
            changeDepth();
        }
        else
        {
            mDepthSimulationTimer = startTimer(ui->lineSimInterval_ms->text().toInt());
        }
    }
    else
    {
        killTimer(mDepthSimulationTimer);
        mDepthSimulationTimer = 0;
    }
}

void MainWindow::on_radioStep_meter_clicked(bool checked)
{
    ui->lineStep_m->setEnabled(checked);
    ui->lineStepPercent->setEnabled(!checked);
    calculateSteps(checked);
}

void MainWindow::on_radioStep_percent_clicked(bool checked)
{
    ui->lineStep_m->setEnabled(!checked);
    ui->lineStepPercent->setEnabled(checked);
    calculateSteps(!checked);
}

void  MainWindow::calculateSteps(bool aMeter)
{
    if (aMeter)
    {
        int fSteps = (ui->lineMaxDepth_m->text().toDouble() - ui->lineMinDepth_m->text().toDouble()) / ui->lineStep_m->text().toDouble();
        ui->lineCalculatedSteps->setText(QString::number(fSteps));
    }
    else
    {
        int fSteps = log(ui->lineMaxDepth_m->text().toDouble() / ui->lineMinDepth_m->text().toDouble()) / log(1 + ui->lineStepPercent->text().toDouble()/100.0);
        ui->lineCalculatedSteps->setText(QString::number(fSteps));
    }
}


void MainWindow::on_lineMinDepth_m_editingFinished()
{
    calculateSteps(ui->radioStep_meter->isChecked());
    ui->verticalSliderDepth->setMinimum(ui->lineMinDepth_m->text().toInt());
}

void MainWindow::on_lineMaxDepth_m_editingFinished()
{
    calculateSteps(ui->radioStep_meter->isChecked());
    ui->verticalSliderDepth->setMaximum(ui->lineMaxDepth_m->text().toInt());
}

void MainWindow::on_lineStep_m_editingFinished()
{
    calculateSteps(ui->radioStep_meter->isChecked());
}

void MainWindow::on_lineStepPercent_editingFinished()
{
    calculateSteps(ui->radioStep_meter->isChecked());
}

void MainWindow::on_ckPingBasedSim_clicked(bool checked)
{
    ui->lineSimInterval_ms->setEnabled(!checked);
}


void MainWindow::on_btnSendCurrentDepth_clicked()
{
    sendCurrentDepth();
}

void MainWindow::on_btnDeleteCommand_clicked()
{
    QString fCmd = ui->comboSendCmd->currentText();
    int fItem = ui->comboSendCmd->findText(fCmd);
    if (fItem != -1)
    {
        ui->statusBar->showMessage("Removed: " + fCmd);
        ui->comboSendCmd->removeItem(fItem);
    }
}

void MainWindow::on_btnStoreCommand_clicked()
{
    QString fCmd = ui->comboSendCmd->currentText();
    if (ui->comboSendCmd->findText(fCmd) == -1)
    {
        ui->statusBar->showMessage("Stored: " + fCmd);
        ui->comboSendCmd->addItem(fCmd);
    }
}

QString getSettingsName(const QString& aItemName)
{
    QRegExp fRegEx("([A-Z][A-Za-z_]+)");
    int fPos = fRegEx.indexIn(aItemName);
    if (fPos != -1 && fRegEx.captureCount())
    {
        return fRegEx.capturedTexts()[0];
    }
    else return aItemName;
}

void MainWindow::initializeConfigurationPage()
{
    if (mPins.size() == 0)
    {
        //                       Name       , No, Input, Output, Analog, PWM,   Tone, Binary, Interrup
#       if 0
        mPins.push_back(PinType("BNC Port 1", 14, true,  true,   true,   false, false, true,  false));
        mPins.push_back(PinType("BNC Port 2", 15, true,  true,   true,   false, false, true,  false));
        mPins.push_back(PinType("BNC Port 3", 16, true,  true,   true,   false, false, true,  false));

        mPins.push_back(PinType("BNC Port 4",  3, true,  true,   false,  true,  true,  true,  true ));
        mPins.push_back(PinType("BNC Port 5",  5, true,  true,   false,  true,  true,  true,  false));
        mPins.push_back(PinType("BNC Port 6",  6, true,  true,   false,  true,  true,  true,  false));

        mPins.push_back(PinType("Switch 1"  ,  4, true,  false,  false,  false, false, true,  false));
        mPins.push_back(PinType("Switch 2"  ,  7, true,  false,  false,  false, false, true,  false));
        mPins.push_back(PinType("Switch 3"  ,  8, true,  false,  false,  false, false, true,  false));

        mPins.push_back(PinType("LED 1"     ,  9, false, true,   false,  true,  false, true,  false));
        mPins.push_back(PinType("LED 2"     , 10, false, true,   false,  true,  false, true,  false));
        mPins.push_back(PinType("LED 3"     , 11, false, true,   false,  true,  false, true,  false));
#       else
        //                       Name          , No, Input, Output, Analog, PWM,   Tone, Binary, Interrup
        mPins.push_back(PinType("A0 with Poti" , 14, true,  true,   true,   false, false, true,  false));
        mPins.push_back(PinType("A1 with R 35k", 15, true,  true,   true,   false, false, true,  false));
        mPins.push_back(PinType("A2 direct"    , 16, true,  true,   true,   false, false, true,  false));
        mPins.push_back(PinType("A3 direct"    , 17, true,  true,   true,   false, false, true,  false));
        mPins.push_back(PinType("A4 direct"    , 18, true,  true,   true,   false, false, true,  false));
        mPins.push_back(PinType("A5 direct"    , 19, true,  true,   true,   false, false, true,  false));

        mPins.push_back(PinType("IO0 PWM"     , 11, true,  true,   false,  true,  true,  true,  false ));
        mPins.push_back(PinType("IO1 PWM"     , 10, true,  true,   false,  true,  true,  true,  false));
        mPins.push_back(PinType("IO2 PWM"     ,  9, true,  true,   false,  true,  true,  true,  false));
        mPins.push_back(PinType("IO3 PWM"     ,  6, true,  true,   false,  true,  true,  true,  false));
        mPins.push_back(PinType("IO4 PWM"     ,  5, true,  true,   false,  true,  true,  true,  false));

        mPins.push_back(PinType("Switch 1"     ,  4, true,  false,  false,  false, false, true,  false));
        mPins.push_back(PinType("Switch 2"     ,  2, true,  false,  false,  false, false, true,  true));
        mPins.push_back(PinType("Switch 3"     ,  7, true,  false,  false,  false, false, true,  false));

        mPins.push_back(PinType("LED Red IO5~I",  3, true,  true,   false,  true,  false, true,  true ));
        mPins.push_back(PinType("LED Yellow"   ,  8, false, true,   false,  false, false, true,  false));
        mPins.push_back(PinType("LED Green"    , 12, false, true,   false,  false, false, true,  false));
        mPins.push_back(PinType("LED Blue"     , 13, false, true,   false,  false, false, true,  false));
#       endif
    }
    mInputPins.resize(mMaxPinNumbers);
    mOutputPins.resize(mMaxPinNumbers);

    mCanUpdateComboBox = false;
    const QString fNotConnected = "No";
    ui->comboInputConnectionToOutput->addItem(fNotConnected);
    ui->comboOutputConnectionToOutput->addItem(fNotConnected);
    for (int fNumber = 1; fNumber <= mMaxPinNumbers; ++fNumber)
    {
        ui->comboInputNo->addItem(QString::number(fNumber));
        ui->comboOutputNo->addItem(QString::number(fNumber));
        ui->comboInputConnectionToOutput->addItem(QString::number(fNumber));
        ui->comboOutputConnectionToOutput->addItem(QString::number(fNumber));
        mInputPins[fNumber-1].mIndex = fNumber;
        mOutputPins[fNumber-1].mIndex = fNumber;
    }

    int fPinIndex = -1;
    const QString fNotDefined = "Not defined";
    ui->comboInputPinNo-> addItem(fNotDefined, QVariant(fPinIndex));
    ui->comboOutputPinNo->addItem(fNotDefined, QVariant(fPinIndex));

    for (const auto& fPin : mPins)
    {
        ++fPinIndex;
        if (fPin.isInput())
        {
            ui->comboInputPinNo->addItem(fPin.getName(), QVariant(fPinIndex));
        }
        if (fPin.isOutput())
        {
            ui->comboOutputPinNo->addItem(fPin.getName(), QVariant(fPinIndex));
        }
    }

    for (int fEdge = InputPin::Edge::Unknown; fEdge < InputPin::Edge::Last; ++fEdge)
    {
        ui->comboInputEdge->addItem(PinCommon::name_of(static_cast<PinCommon::Edge::eEdge>(fEdge)));
    }
    for (int fEdge = InputPin::Edge::Unknown; fEdge < InputPin::Edge::High; ++fEdge)
    {
        ui->comboOutputEdge->addItem(PinCommon::name_of(static_cast<PinCommon::Edge::eEdge>(fEdge)));
    }

    const QString fSpecialValue = "default";
    ui->spinSwitchDeBounce->setRange(-1, 32767);
    ui->spinSwitchDeBounce->setSpecialValueText(fSpecialValue);
    ui->spinSwitchDelay->setRange(-1, 32767);
    ui->spinSwitchDelay->setSpecialValueText(fSpecialValue);
    ui->spinSwitchRepeat->setRange(-1, 32767);
    ui->spinSwitchRepeat->setSpecialValueText(fSpecialValue);
    ui->spinAnalogThreshold->setRange(-1, InputPin::analog_value_range);
    ui->spinAnalogThreshold->setSpecialValueText(fSpecialValue);

    ui->spinBinaryDelay->setRange(-1, 32767);
    ui->spinBinaryDelay->setSpecialValueText(fSpecialValue);
    ui->spinBinaryPulse->setRange(-1, 32767);
    ui->spinBinaryPulse->setSpecialValueText(fSpecialValue);
    ui->spinBinaryPulseRepeat->setRange(-2, 32767);
    ui->spinBinaryPulseRepeat->setSpecialValueText(fSpecialValue);
    ui->spinOutputValue->setRange(-1, 1);
    ui->spinOutputValue->setSpecialValueText(fSpecialValue);

    mCanUpdateComboBox = true;
}

int  MainWindow::getPinIndex(int aPinNo)
{
    for (int fIndex = 0; fIndex < static_cast<int>(mPins.size()); ++fIndex)
    {
        if (mPins[fIndex].getPinNo() == aPinNo)
        {
            return fIndex;
        }
    }
    return -1;
}

int MainWindow::getPinIndexFromItemData(const QComboBox& aCB, int aPinNo)
{
    int fIndex = getPinIndex(aPinNo);
    for (int i=0; i < aCB.count(); ++i)
    {
        if (aCB.itemData(i).toInt() == fIndex)
        {
            return i;
        }
    }
    return -1;
}

void MainWindow::updateOutputConnectionNames()
{
    bool fValueSet = false;
    if (ui->comboInputConnectionToOutput->currentIndex())
    {
        int fIndex = getPinIndex(mOutputPins[ui->comboInputConnectionToOutput->currentIndex()-1].mPinNumber);
        if (fIndex != -1)
        {
            fValueSet = true;
            ui->labelInputConnectionToOutputPin->setText(mPins[fIndex].getName());
        }
    }
    if (! fValueSet)
    {
        ui->labelInputConnectionToOutputPin->setText("");
    }

    fValueSet = false;
    if (ui->comboOutputConnectionToOutput->currentIndex())
    {
        int fIndex = getPinIndex(mOutputPins[ui->comboOutputConnectionToOutput->currentIndex()-1].mPinNumber);
        if (fIndex != -1)
        {
            fValueSet = true;
            ui->labelOutputConnectionToOutputPin->setText(mPins[fIndex].getName());
        }
    }
    if (! fValueSet)
    {
        ui->labelOutputConnectionToOutputPin->setText("");
    }
}

void MainWindow::updateInputControls()
{
    PinCommon::Type::eType fType = mInputPins[mSelectedInputPin].mPinType;
    ui->spinAnalogThreshold->setEnabled(fType == PinCommon::Type::Analog);
    ui->labelSwitchDeBounce->setText(fType == PinCommon::Type::Analog ? "Hysteresis" : "Debounce [ms]");
    ui->spinSwitchDelay->setEnabled(fType != PinCommon::Type::Analog);
    ui->spinSwitchRepeat->setEnabled(fType != PinCommon::Type::Analog);
}


void MainWindow::on_comboInputNo_currentIndexChanged(int index)
{
    if (mCanUpdateComboBox)
    {
        mSelectedInputPin = index;
        const InputPin& fSelected = mInputPins[index];

        int fPinNumber = fSelected.mPinNumber;
        if (fPinNumber)
        {
            ui->comboInputPinNo->setCurrentIndex(getPinIndexFromItemData(*ui->comboInputPinNo, fPinNumber));
            ui->comboInputType->setCurrentIndex(ui->comboInputType->findText(PinCommon::name_of(fSelected.mPinType)));
            ui->comboInputEdge->setCurrentIndex(ui->comboInputEdge->findText(PinCommon::name_of(fSelected.mEdge)));
            ui->comboInputConnectionToOutput->setCurrentIndex(fSelected.mConnection);
            ui->spinAnalogThreshold->setValue(fSelected.mThreshold);
            ui->spinSwitchDeBounce->setValue(fSelected.mPulse);
            ui->spinSwitchDelay->setValue(fSelected.mDelay);
            ui->spinSwitchRepeat->setValue(fSelected.mRepeat);
        }
        else
        {
            ui->comboInputPinNo->setCurrentIndex(0);
            ui->comboInputType->setCurrentIndex(-1);
            ui->comboInputEdge->setCurrentIndex(0);
            ui->comboInputConnectionToOutput->setCurrentIndex(0);
            ui->spinAnalogThreshold->setValue(-1);
            ui->spinSwitchDeBounce->setValue(-1);
            ui->spinSwitchDelay->setValue(-1);
            ui->spinSwitchRepeat->setValue(-1);
        }
        updateOutputConnectionNames();
    }
}

void MainWindow::on_comboInputPinNo_currentIndexChanged(int index)
{
    if (mCanUpdateComboBox)
    {
        bool fOK = false;
        int fPin = ui->comboInputPinNo->itemData(index).toInt(&fOK);
        if (fOK && fPin != -1 && fPin < static_cast<int>(mPins.size()))
        {
            mCanUpdateComboBox = false;
            ui->comboInputType->clear();
            ui->comboInputType->addItems(mPins[fPin].getTypes(false));
            mCanUpdateComboBox = true;

            mInputPins[mSelectedInputPin].mPinNumber = mPins[fPin].getPinNo();
            on_comboInputNo_currentIndexChanged(mSelectedInputPin);
            updateOutputConnectionNames();
        }
    }
}

void MainWindow::on_comboInputType_currentIndexChanged(const QString &aPinType)
{
    if (mCanUpdateComboBox)
    {
        mInputPins[mSelectedInputPin].mPinType = PinCommon::type_name(aPinType.toStdString().c_str());
        updateInputControls();
    }
}

void MainWindow::on_comboInputEdge_currentIndexChanged(const QString &aEdge)
{
    if (mCanUpdateComboBox)
    {
        mInputPins[mSelectedInputPin].mEdge = PinCommon::edge_name(aEdge.toStdString().c_str());
    }
}

void MainWindow::on_comboInputConnectionToOutput_currentIndexChanged(int index)
{
    if (mCanUpdateComboBox)
    {
        mInputPins[mSelectedInputPin].mConnection = index;
        updateOutputConnectionNames();
    }
}

void MainWindow::on_spinAnalogThreshold_valueChanged(int aThreshold)
{
    mInputPins[mSelectedInputPin].mThreshold = aThreshold;
}

void MainWindow::on_spinSwitchDeBounce_valueChanged(int aPulse)
{
    mInputPins[mSelectedInputPin].mPulse = aPulse;
}

void MainWindow::on_spinSwitchDelay_valueChanged(int aDelay)
{
    mInputPins[mSelectedInputPin].mDelay = aDelay;
}

void MainWindow::on_spinSwitchRepeat_valueChanged(int aRepeat)
{
    mInputPins[mSelectedInputPin].mRepeat = aRepeat;
}

void MainWindow::on_checkReportValue_clicked(bool aReport)
{
    mInputPins[mSelectedInputPin].mReport = aReport;
}

void MainWindow::updateOutputControls()
{
    PinCommon::Type::eType fType = mOutputPins[mSelectedInputPin].mPinType;
    ui->spinOutputValue->setRange(-1, fType == PinCommon::Type::Binary ? 1 : 255);
}

void MainWindow::on_comboOutputNo_currentIndexChanged(int index)
{
    if (mCanUpdateComboBox)
    {
        mSelectedOutputPin = index;
        const OutputPin& fSelected = mOutputPins[index];
        int fPinNumber = fSelected.mPinNumber;
        if (fPinNumber)
        {
            ui->comboOutputPinNo->setCurrentIndex(getPinIndexFromItemData(*ui->comboOutputPinNo, fPinNumber));
            ui->comboOutputType->setCurrentIndex(ui->comboOutputType->findText(PinCommon::name_of(fSelected.mPinType)));
            ui->comboOutputEdge->setCurrentIndex(ui->comboOutputEdge->findText(PinCommon::name_of(fSelected.mEdge)));
            ui->comboOutputConnectionToOutput->setCurrentIndex(fSelected.mConnection);
            ui->spinBinaryDelay->setValue(fSelected.mDelay);
            ui->spinBinaryPulse->setValue(fSelected.mPulse);
            ui->spinBinaryPulseRepeat->setValue(fSelected.mRepeat);
            ui->spinOutputValue->setValue(fSelected.mValue);
            ui->checkOutputInvert->setChecked(fSelected.mInvert == 1);
            ui->checkOutputToggle->setChecked(fSelected.mToggle == 1);
        }
        else
        {
            ui->comboOutputPinNo->setCurrentIndex(0);
            ui->comboOutputType->setCurrentIndex(-1);
            ui->comboOutputEdge->setCurrentIndex(0);
            ui->comboOutputConnectionToOutput->setCurrentIndex(0);
            ui->spinBinaryDelay->setValue(-1);
            ui->spinBinaryPulse->setValue(-1);
            ui->spinBinaryPulseRepeat->setValue(-2);
            ui->spinOutputValue->setValue(-1);
            ui->checkOutputInvert->setChecked(false);
            ui->checkOutputToggle->setChecked(false);
        }
        updateOutputConnectionNames();
    }
}

void MainWindow::on_comboOutputPinNo_currentIndexChanged(int index)
{
    if (mCanUpdateComboBox)
    {
        bool fOK = false;
        int fPin = ui->comboOutputPinNo->itemData(index).toInt(&fOK);
        if (fOK && fPin != -1 && fPin < static_cast<int>(mPins.size()))
        {
            mCanUpdateComboBox = false;
            ui->comboOutputType->clear();
            ui->comboOutputType->addItems(mPins[fPin].getTypes(true));
            mCanUpdateComboBox = true;
            mOutputPins[mSelectedOutputPin].mPinNumber = mPins[fPin].getPinNo();
            on_comboOutputNo_currentIndexChanged(mSelectedOutputPin);
            updateOutputConnectionNames();
        }
    }
}

void MainWindow::on_comboOutputType_currentIndexChanged(const QString &aPinType)
{
    if (mCanUpdateComboBox)
    {
        mOutputPins[mSelectedOutputPin].mPinType = PinCommon::type_name(aPinType.toStdString().c_str());
        updateOutputControls();
    }
}

void MainWindow::on_comboOutputEdge_currentIndexChanged(const QString &aEdge)
{
    if (mCanUpdateComboBox)
    {
        mOutputPins[mSelectedOutputPin].mEdge = PinCommon::edge_name(aEdge.toStdString().c_str());
    }
}

void MainWindow::on_comboOutputConnectionToOutput_currentIndexChanged(int index)
{
    if (mCanUpdateComboBox)
    {
        mOutputPins[mSelectedOutputPin].mConnection = index;
        updateOutputConnectionNames();
    }
}

void MainWindow::on_spinBinaryDelay_valueChanged(int aDelay)
{
    mOutputPins[mSelectedOutputPin].mDelay = aDelay;
    ui->spinBinaryPulse->setEnabled(aDelay > 0);
    ui->spinBinaryPulseRepeat->setEnabled(aDelay > 0);
}

void MainWindow::on_spinBinaryPulse_valueChanged(int aPulse)
{
    mOutputPins[mSelectedOutputPin].mPulse = aPulse;
}

void MainWindow::on_spinBinaryPulseRepeat_valueChanged(int aRepeat)
{
    mOutputPins[mSelectedOutputPin].mRepeat = aRepeat;
}

void MainWindow::on_spinOutputValue_valueChanged(int aValue)
{
    mOutputPins[mSelectedOutputPin].mValue = aValue;
}

void MainWindow::on_checkOutputInvert_clicked(bool aInvert)
{
    mOutputPins[mSelectedOutputPin].mInvert = aInvert ? 1 : 0;
}

void MainWindow::on_checkOutputToggle_clicked(bool aToggle)
{
    mOutputPins[mSelectedOutputPin].mToggle = aToggle ? 1 : 0;
}

void MainWindow::on_btnSendConfig_clicked()
{
    if (mPort.isOpen())
    {
        QString fConfig;
        for (const auto& fPin : mInputPins)
        {
            if (fPin.mPinNumber)
            {
                fConfig += fPin.getPinDefinitions();
            }
        }
        for (const auto& fPin : mOutputPins)
        {
            if (fPin.mPinNumber)
            {
                fConfig += fPin.getPinDefinitions();
            }
        }
        QStringList fConfigLines = fConfig.split('\n');
        for (auto fLine : fConfigLines)
        {
            fLine += "\n";
            mPort.write(fLine.toStdString().c_str(), fLine.size());
            QThread::msleep(100);
        }
        ui->lineReceive->append("sent:\n" + fConfig);
    }
}

void MainWindow::on_btnRequesConfig_clicked()
{
    if (mPort.isOpen())
    {
        const QString fRequest = "CONFIG?";
        mPort.write(fRequest.toStdString().c_str(), fRequest.size());
        ui->lineReceive->append("sent:\n" + fRequest);
    }
}

void MainWindow::on_btnStoreConfig_clicked()
{
    if (mPort.isOpen())
    {
        const QString fCommand = "CONFIG:STORE";
        mPort.write(fCommand.toStdString().c_str(), fCommand.size());
        ui->lineReceive->append("sent:\n" + fCommand);
    }
}

void MainWindow::on_btnClearConfig_clicked()
{
    if (mPort.isOpen())
    {
        const QString fCommand = "CONFIG:CLEAR";
        mPort.write(fCommand.toStdString().c_str(), fCommand.size());
        ui->lineReceive->append("sent:\n" + fCommand);
    }
}

void MainWindow::on_btnHelp_clicked()
{
    if (mPort.isOpen())
    {
        QString fRequest = "HELP?";
        mPort.write(fRequest.toStdString().c_str(), fRequest.size());
        ui->lineReceive->append("sent:\n" + fRequest);
    }
}

