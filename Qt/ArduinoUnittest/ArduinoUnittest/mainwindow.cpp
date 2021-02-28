#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <serialinterface.h>

#include <Arduino.h>
#include <LiquidCrystal.h>

#include <iostream>

#include <QVector>
#include <QString>
#include <QSettings>
#include <QRegExp>
#include <QTimer>
#include <QTime>
#include <QThread>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QFileDialog>
#include <QFileInfo>
#include <QMutexLocker>

#define STORE_PTR(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC)  SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)       SETTING.setValue(getSettingsName(#ITEM), ITEM)

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT) ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT)  ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)           ITEM=SETTING.value(getSettingsName(#ITEM), ITEM).CONVERT()


QString getSettingsName(const QString& aItemName);

enum eTable
{
    ePinIndex, ePinNo, ePinType, eValue, eLast, eRange=eLast
};

SerialInterface Serial;

MainWindow* MainWindow::gmThis = NULL;
MainWindow::tPinAccess MainWindow::gmStaticPinAccess;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mFirstAnalogPin(0)
    , mLiquidCrystal(0)
    , mLogicAnalyser(this)
    , mUseWorkerThread(true)
{
    ui->setupUi(this);

    connect(&mLogicAnalyser, SIGNAL(record(bool)), this, SLOT(on_Record(bool)));
    connect(&mLogicAnalyser, SIGNAL(isshown(bool)), this, SLOT(on_ShowLogicAnalyser(bool)));
    mLogicAnalyser.millis = &MainWindow::elapsed;

    QSettings fSettings("config.ini", QSettings::NativeFormat);

    mArduinoTime = new QTime;

    QString fArduinoName = "AtMega328";
    LOAD_PTR(fSettings, ui->editEscUpF1, setText, text, toString);
    LOAD_PTR(fSettings, ui->editRightEnterLeft, setText, text, toString);
    LOAD_PTR(fSettings, ui->editClearDownF2, setText, text, toString);

    LOAD_PTR(fSettings, ui->btnEscape, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnUp, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnRight, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnEnter, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnLeft, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnClear, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnDown, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnF1, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnF2, setText, text, toString);
    LOAD_STR(fSettings, fArduinoName, toString);
    LOAD_STR(fSettings, mUseWorkerThread, toBool);


    mListModel = new QStandardItemModel(0, eLast, this);
    mListModel->setHeaderData(ePinNo   , Qt::Horizontal, tr("Pin"));
    mListModel->setHeaderData(ePinIndex, Qt::Horizontal, tr("No."));
    mListModel->setHeaderData(ePinType , Qt::Horizontal, tr("Type"));
    mListModel->setHeaderData(eValue   , Qt::Horizontal, tr("Value"));
    ui->tableView->setModel(mListModel);

    QStringList fPinsAtMega328  = { "D0 Rx", "D1 Tx", "D2 Int0", "D3~ Int1", "D4", "D5~", "D6~", "D7", "D8", "D9", "D10~ SS", "D11~ MOSI", "D12 MISO", "D13 SCK", "A0", "A1", "A2", "A3", "A4 SDA", "A5 SCL"};
    QStringList fPinsAtMega2560 = { "D0 Rx0", "D1 Tx0", "D2~ Int0", "D3~ Int1", "D4~", "D5~", "D6~", "D7~", "D8~", "D9~",
                                    "D10~", "D11~", "D12~", "D13~", "D14 Tx3", "D15 Rx3", "D16 Tx2", "D17 Rx2", "D18 Tx1 Int2", "D19 Rx1 Int3",
                                    "D20 SDA Int4", "D21 SCL Int5", "D22", "D23", "D24", "D25", "D26", "D27", "D28", "D29",
                                    "D30", "D31", "D32", "D33", "D34", "D35", "D36", "D37", "D38", "D39",
                                    "D40", "D41", "D42", "D43", "D44", "D45", "D46", "D47", "D48", "D49",
                                    "D50", "D51", "D52", "D53",
                                    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "A10", "A11", "A12", "A13", "A14", "A15"};

    QStringList* fPins = &fPinsAtMega328;
    if (fArduinoName == "AtMega2560")
    {
        fPins = &fPinsAtMega2560;
    }
    setWindowTitle(fArduinoName);

    mFirstAnalogPin = fPins->indexOf("A0");
    int fRow = 0;
    for (auto &fPin : *fPins)
    {
        mListModel->insertRows(fRow, 1, QModelIndex());
        mListModel->setData(mListModel->index(fRow, ePinIndex, QModelIndex()), QString::number(fRow));
        mListModel->setData(mListModel->index(fRow, ePinNo, QModelIndex()), fPin);
        mListModel->setData(mListModel->index(fRow, ePinType, QModelIndex()), "");
        mListModel->setData(mListModel->index(fRow, eValue, QModelIndex()), "0");
        mListModel->setData(mListModel->index(fRow, eRange, QModelIndex()), "1");
        ++fRow;
    }

    ui->tableView->setColumnWidth(ePinIndex,  30);
    ui->tableView->setColumnWidth(ePinNo   ,  85);
    ui->tableView->setColumnWidth(ePinType , 100);
    ui->tableView->setColumnWidth(eValue   ,  68);

    QObject::connect(&Serial, SIGNAL(sendText(QString)), this, SLOT(printToSeriaDisplay(QString)));

    QTimer::singleShot(5, this, SLOT(on_Setup()));
    gmThis = this;
}

MainWindow::~MainWindow()
{
    {
        QMutexLocker fLock(&mPinMutex);
        gmStaticPinAccess.clear();
    }
    if (mWorker)
    {
        mWorker->stop();
        mWorker->wait();
    }
    QSettings fSettings("config.ini", QSettings::NativeFormat);
    STORE_PTR(fSettings, ui->editEscUpF1, text);
    STORE_PTR(fSettings, ui->editRightEnterLeft, text);
    STORE_PTR(fSettings, ui->editClearDownF2, text);

    STORE_PTR(fSettings, ui->btnEscape, text);
    STORE_PTR(fSettings, ui->btnUp, text);
    STORE_PTR(fSettings, ui->btnRight, text);
    STORE_PTR(fSettings, ui->btnEnter, text);
    STORE_PTR(fSettings, ui->btnLeft, text);
    STORE_PTR(fSettings, ui->btnClear, text);
    STORE_PTR(fSettings, ui->btnDown, text);
    STORE_PTR(fSettings, ui->btnF1, text);
    STORE_PTR(fSettings, ui->btnF2, text);
    QString fArduinoName = windowTitle();
    STORE_STR(fSettings, fArduinoName);
    STORE_STR(fSettings, mUseWorkerThread);

    delete mArduinoTime;
    delete ui;
}

void MainWindow::sendSerialText(const QString& aText)
{
    Serial.pushText(aText + "\n");
    loop();
}

void MainWindow::printToSeriaDisplay(const QString &aText)
{
    ui->textReceivedFromSerialDevice->insertPlainText(aText);
}

int  MainWindow::elapsed()
{
    if (gmThis)
    {
        return gmThis->mArduinoTime->elapsed();
    }
    return 0;
}

void MainWindow::setPins()
{
    QMutexLocker fLock(&mPinMutex);
    for (auto fPA : gmStaticPinAccess)
    {
        setPin(fPA);
    }
    gmStaticPinAccess.clear();
}


void MainWindow::on_Setup()
{
    setPins();
    mArduinoTime->start();
    setup();

    if (mUseWorkerThread)
    {
        mWorker.reset(new ArduinoWorker(this));
    }

    QTimer *timer;
    if (mWorker)
    {
        connect(mWorker.get(), SIGNAL(updateLiquidCrystal()), this, SLOT(updateLiquidCrystal()));
        mWorker->setLoopFunction(boost::bind(&MainWindow::on_Loop, this));
        mWorker->start();
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(setPins()));
        timer->start(1);
    }
    else
    {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(on_Loop()));
        timer->start(10);
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_OneSecond()));
    timer->start(1000);
}

void MainWindow::on_Loop()
{
    loop();
    if (!mWorker)
    {
        updateLiquidCrystal();
    }
}

void MainWindow::on_OneSecond()
{
    if (mLiquidCrystal)
    {
        mLiquidCrystal->updateBlinkState();
    }
}

void MainWindow::updateLiquidCrystal()
{
    if (mLiquidCrystal && mLiquidCrystal->isChanged())
    {
        QString fText;
        for (int fLine = 0; fLine < mLiquidCrystal->getLines(); ++fLine)
        {
            fText += QString("[") + mLiquidCrystal->getDisplayTextLine(fLine).c_str() + QString("]\n");
        }
        ui->textLCD_Sim->setText(fText);
        uint8_t fRow, fCol;
        mLiquidCrystal->getCursor(fRow, fCol);
        mLiquidCrystal->setUnchanged();
    }
}

int  MainWindow::getPinValue(int aPin, int aRange)
{
    if (aPin >= 0 && aPin < mListModel->rowCount())
    {
        if (aRange != 1)
        {
            mListModel->setData(mListModel->index(aPin, eRange, QModelIndex()), QString::number(aRange));
        }
        int fValue = mListModel->data(mListModel->index(aPin, eValue, QModelIndex())).toInt();
        if (mLogicAnalyser.isRecording() && isPinSelected(aPin))
        {
            QString fPinName = mListModel->data(mListModel->index(aPin, ePinNo, QModelIndex())).toString();
            mLogicAnalyser.setValue(fPinName.toStdString(), millis(),
                                    aRange != 1 ? static_cast<float>(fValue) / static_cast<float>(aRange) : fValue);
        }
        return fValue;
    }
    else
    {
        printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
    }

    return -1;
}

void MainWindow::setPinValue(int aPin, int aValue, int aRange)
{
    if (aPin >= 0 && aPin < mListModel->rowCount())
    {
        if (mWorker && mWorker->runningInCurrentThread())
        {
            QMutexLocker fLock(&mPinMutex);
            gmStaticPinAccess.push_back({ eSetPinValue, aPin, aValue, aRange });
        }
        else
        {
            if (aRange != 1)
            {
                mListModel->setData(mListModel->index(aPin, eRange, QModelIndex()), QString::number(aRange));
            }
            mListModel->setData(mListModel->index(aPin, eValue, QModelIndex()), QString::number(aValue));

            if (mLogicAnalyser.isRecording() && isPinSelected(aPin))
            {
                QString fPinName = mListModel->data(mListModel->index(aPin, ePinNo, QModelIndex())).toString();
                mLogicAnalyser.setValue(fPinName.toStdString(), millis(),
                                        aRange != 1 ? static_cast<float>(aValue) / static_cast<float>(aRange) : aValue);
            }
        }
    }
    else
    {
        printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
    }
}

void MainWindow::setPin(const std::vector< int >& aSet )
{
    switch (aSet[0])
    {
    case ePinMode:     pinMode(aSet[1], aSet[2]); break;
    case eSetPinValue: setPinValue(aSet[1], aSet[2], aSet[3]); break;
    case eTone:        tone(aSet[1], aSet[2], aSet[3]); break;
    }

}


void MainWindow::on_Record(bool aRecord)
{
    if (aRecord)
    {
        QList<int> fPinList;
        QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
        if (indexes.count())
        {
            for (auto &findex : indexes)
            {
                fPinList.append(findex.row());
            }
        }
        else
        {
            for (int i=0; i<mListModel->rowCount(); ++i)
            {
                fPinList.append(i);
            }
        }

        for (auto &fPin : fPinList)
        {
            QString fPinName = mListModel->data(mListModel->index(fPin, ePinNo, QModelIndex())).toString();
            int fValue = mListModel->data(mListModel->index(fPin, eValue, QModelIndex())).toInt();
            int fRange  = mListModel->data(mListModel->index(fPin, eRange, QModelIndex())).toInt();
            mLogicAnalyser.setValue(fPinName.toStdString(), millis(),
                                    fRange != 1 ? static_cast<float>(fValue) / static_cast<float>(fRange) : fValue);
        }
    }
}

void MainWindow::on_ShowLogicAnalyser(bool aShow)
{
    ui->btnLogicAnalyzer->setChecked(aShow);
}



bool MainWindow::isPinSelected(int aPin)
{
    QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
    if (indexes.count())
    {
        if (std::find_if(indexes.begin(), indexes.end(), [aPin] (QModelIndexList::const_reference aR) { return aR.row() == aPin; } ) != indexes.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

// TODO: use vector for thread delegation
// std::vector<int> fFunction = { ePinMode, aPin, aType };
void MainWindow::pinMode(int aPin, int aType)
{
    if (gmThis)
    {
        if (gmThis->mWorker && gmThis->mWorker->runningInCurrentThread())
        {
            QMutexLocker fLock(&gmThis->mPinMutex);
            gmStaticPinAccess.push_back({ ePinMode, aPin, aType });
        }
        else
        {
            if (aPin >= 0 && aPin < gmThis->mListModel->rowCount())
            {
                gmThis->mListModel->setData(gmThis->mListModel->index(aPin, ePinType, QModelIndex()), nameof_pintype(aType));
            }
            else
            {
                gmThis->printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
            }
        }
    }
    else
    {
        gmStaticPinAccess.push_back({ ePinMode, aPin, aType });
    }
}

int  MainWindow::digitalRead(int aPin)
{
    if (gmThis)
    {
       return gmThis->getPinValue(aPin);
    }
    return 0;
}

void MainWindow::digitalWrite(int aPin, int aValue)
{
    if (gmThis)
    {
        gmThis->setPinValue(aPin, aValue);
    }
    else
    {
        gmStaticPinAccess.push_back({ eSetPinValue, aPin, aValue, 1});
    }
}

void MainWindow::digitalWrite(const std::vector<int>& aPins, int aValue)
{
    for (auto fPin : aPins)
    {
        digitalWrite(fPin, aValue);
    }
}

int  MainWindow::analogRead(int aPin)
{
    if (gmThis)
    {
        return gmThis->getPinValue(aPin, 1024);
    }
    return 0;
}

void MainWindow::analogWrite(int aPin, int aValue)
{
    if (gmThis)
    {
        gmThis->setPinValue(aPin, aValue, 256);
    }
    else
    {
        gmStaticPinAccess.push_back({ eSetPinValue, aPin, aValue, 256 });
    }
}

class Beeper : public QThread
{
public:
    typedef std::map<int, Beeper*> tThreadMap;
    static void beep(int aFrequency, int aLength_ms, int aPin)
    {
        new Beeper(aFrequency, aLength_ms, aPin);
    }

    static void nobeep(int aPin)
    {
        auto fThread = mThreadMap.find(aPin);
        if (fThread != mThreadMap.end())
        {
//            fThread->second->terminate();
        }
    }

private:
    Beeper(int aFrequency, int aLength, int aPin) : mFrequency(aFrequency), mLength(aLength), mPin(aPin)
    {
        connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
        if (aLength == 0)
        {
            mThreadMap[aPin] = this;
            aLength = 100000;
        }
        start();
    }
    ~Beeper()
    {
        auto fThread = mThreadMap.find(mPin);
        if (fThread != mThreadMap.end())
        {
            mThreadMap.erase(fThread);
        }
    }

    void run()
    {
        do_beep(mFrequency, mLength);
    }

    void do_beep(int aFrequency, int aLength_ms)
    {
        std::stringstream fCmd;
        fCmd << "( speaker-test -t sine -f" << aFrequency << " )& pid=$! ; sleep " << static_cast<double>(aLength_ms * 0.001) << "s ; kill -9 $pid";
        system(fCmd.str().c_str());
    }
    int mFrequency;
    int mLength;
    int mPin;
    static tThreadMap mThreadMap;
};

Beeper::tThreadMap Beeper::mThreadMap;

void MainWindow::tone(int aPin, int aFrequency, int aLength)
{
    if (gmThis)
    {
        Beeper::beep(aFrequency, aLength, aPin);
        gmThis->setPinValue(aPin, aFrequency);
    }
    else
    {
        gmStaticPinAccess.push_back( { eTone, aPin, aFrequency, aLength });
    }
}

void MainWindow::noTone(int aPin)
{
    if (gmThis)
    {
        if (aPin >= 0 && aPin < gmThis->mListModel->rowCount())
        {
            gmThis->mListModel->setData(gmThis->mListModel->index(aPin, eValue, QModelIndex()), "");
            Beeper::nobeep(aPin);
        }
        else
        {
            gmThis->printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
        }
    }
}

int  MainWindow::pulseIn(int /* aPin */, int /* aLevel */, int /* aTimeout */)
{
    if (gmThis)
    {
        gmThis->printToSeriaDisplay("pulseIn not implemented\n");
    }
    return 0;
}


void MainWindow::on_btnSendText_clicked()
{
    sendSerialText(ui->textToSendToSerialDevice->text());
}

void MainWindow::on_bntSendFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), tr(""), tr("Textfile (*.txt *.*)"));
    QFile fFile;
    if (fileName.length())
    {
        fFile.setFileName(fileName);
        if (fFile.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            bool fOK = true;
            do
            {
                QByteArray fLine = fFile.readLine();
                fOK = fLine.length() > 0;
                if (fOK)
                {
                    sendSerialText(fLine);
                }
            }
            while (fOK);
        }
    }
}

void MainWindow::on_btnClearText_clicked()
{
    ui->textReceivedFromSerialDevice->setText("");
}

void MainWindow::on_btnDoTest_clicked()
{
    doTest();
}

int MainWindow::getFirstAnalogPin()
{
    if (gmThis)
    {
        return gmThis->mFirstAnalogPin;
    }
    return 14;
}


std::vector<int>  MainWindow::getButtonPin(const QLineEdit& aLineEdit, int aNo, int& aValue) const
{
    std::vector<int> fPins;
    QStringList fList = aLineEdit.text().split(':');
    int fSize = fList.size();
    bool bNegative = false;
    if (aNo < fSize && fList[aNo].length() > 0)
    {
        QStringList fPinList = fList[aNo].split(',');
        for (auto fPinVal : fPinList)
        {
            fPins.push_back(fPinVal.toInt());
            if (*fPins.rbegin() < 0)
            {
                *fPins.rbegin() = -(*fPins.rbegin());
                bNegative = true;
            }
        }
    }
    if (bNegative)
    {
        aValue = !aValue;
    }

    return fPins;
}


void MainWindow::on_btnEscape_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editEscUpF1, 0, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnEscape_released()
{
    int fValue = 0;
    std::vector<int>fPins = getButtonPin(*ui->editEscUpF1, 0, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnUp_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editEscUpF1, 1, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnUp_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editEscUpF1, 1, fValue);
    digitalWrite(fPins, fValue);
}

void MainWindow::on_btnF1_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editEscUpF1, 2, fValue);
    digitalWrite(fPins, fValue);
}

void MainWindow::on_btnF1_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editEscUpF1, 2, fValue);
    digitalWrite(fPins, fValue);
}

void MainWindow::on_btnRight_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editRightEnterLeft, 0, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnRight_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editRightEnterLeft, 0, fValue);
    digitalWrite(fPins, fValue);
}

void MainWindow::on_btnEnter_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editRightEnterLeft, 1, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnEnter_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editRightEnterLeft, 1, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnLeft_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editRightEnterLeft, 2, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnLeft_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editRightEnterLeft, 2, fValue);
    digitalWrite(fPins, fValue);
}

void MainWindow::on_btnClear_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editClearDownF2, 0, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnClear_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editClearDownF2, 0, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnDown_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editClearDownF2, 1, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnDown_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editClearDownF2, 1, fValue);
    digitalWrite(fPins, fValue);
}

void MainWindow::on_btnF2_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editClearDownF2, 2, fValue);
    digitalWrite(fPins, fValue);
}

void MainWindow::on_btnF2_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editClearDownF2, 2, fValue);
    digitalWrite(fPins, fValue);
}

void MainWindow::setLiquidCrystal(LiquidCrystal *aLCD)
{
    mLiquidCrystal = aLCD;
}

QString getSettingsName(const QString& aItemName)
{
    QRegExp fRegEx("([A-Z][A-Za-z0-9]+)");
    int fPos = fRegEx.indexIn(aItemName);
    if (fPos != -1 && fRegEx.captureCount())
    {
        return fRegEx.capturedTexts()[0];
    }
    else return aItemName;
}

void MainWindow::on_btnLogicAnalyzer_clicked(bool checked)
{
    if (checked)
    {
        mLogicAnalyser.show();
    }
    else
    {
        mLogicAnalyser.hide();
    }
}
