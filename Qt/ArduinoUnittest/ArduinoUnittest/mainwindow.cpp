#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <serialinterface.h>

#include <Arduino.h>
#include <LiquidCrystal.h>

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

#define STORE_PTR(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)       SETTING.setValue(getSettingsName(#ITEM), ITEM)

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT) ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT) ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)           ITEM=SETTING.value(getSettingsName(#ITEM), ITEM).CONVERT()

QString getSettingsName(const QString& aItemName);

enum eTable
{
    ePinNo, ePinType, eValue, eLast
};

SerialInterface Serial;

MainWindow* MainWindow::gmThis = NULL;
MainWindow::tPinAccess MainWindow::gmStaticPinAccess;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mFirstAnalogPin(0)
    , mLiquidCrystal(0)
{
    ui->setupUi(this);

    QSettings fSettings("config.ini", QSettings::NativeFormat);

    mArduinoTime = new QTime;

    QString fArduinoName = "AtMega328";
    LOAD_PTR(fSettings, ui->editEscUp, setText, text, toString);
    LOAD_PTR(fSettings, ui->editRightEnterLeft, setText, text, toString);
    LOAD_PTR(fSettings, ui->editClearDown, setText, text, toString);

    LOAD_PTR(fSettings, ui->btnEscape, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnUp, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnRight, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnEnter, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnLeft, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnClear, setText, text, toString);
    LOAD_PTR(fSettings, ui->btnDown, setText, text, toString);
    LOAD_STR(fSettings, fArduinoName, toString);


    mListModel = new QStandardItemModel(0, eLast, this);
    mListModel->setHeaderData(ePinNo  , Qt::Horizontal, tr("Pin"));
    mListModel->setHeaderData(ePinType, Qt::Horizontal, tr("Type"));
    mListModel->setHeaderData(eValue  , Qt::Horizontal, tr("Value"));
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
        mListModel->setData(mListModel->index(fRow, ePinNo, QModelIndex()), fPin);
        mListModel->setData(mListModel->index(fRow, ePinType, QModelIndex()), "");
        mListModel->setData(mListModel->index(fRow, eValue, QModelIndex()), "0");
        ++fRow;
    }

    ui->tableView->setColumnWidth(ePinNo  ,  90);
    ui->tableView->setColumnWidth(ePinType, 100);
    ui->tableView->setColumnWidth(eValue  ,  68);

    QObject::connect(&Serial, SIGNAL(sendText(QString)), this, SLOT(printToSeriaDisplay(QString)));

    QTimer::singleShot(5, this, SLOT(on_Setup()));
    gmThis = this;
}

MainWindow::~MainWindow()
{
    QSettings fSettings("config.ini", QSettings::NativeFormat);
    STORE_PTR(fSettings, ui->editEscUp, text);
    STORE_PTR(fSettings, ui->editRightEnterLeft, text);
    STORE_PTR(fSettings, ui->editClearDown, text);

    STORE_PTR(fSettings, ui->btnEscape, text);
    STORE_PTR(fSettings, ui->btnUp, text);
    STORE_PTR(fSettings, ui->btnRight, text);
    STORE_PTR(fSettings, ui->btnEnter, text);
    STORE_PTR(fSettings, ui->btnLeft, text);
    STORE_PTR(fSettings, ui->btnClear, text);
    STORE_PTR(fSettings, ui->btnDown, text);
    QString fArduinoName = windowTitle();
    STORE_STR(fSettings, fArduinoName);

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

void MainWindow::on_Setup()
{
    for (auto fPA : gmStaticPinAccess)
    {
        switch (fPA[0])
        {
        case ePinMode:     pinMode(fPA[1], fPA[2]); break;
        case eSetPinValue: setPinValue(fPA[1], fPA[2]); break;
        case eTone:        tone(fPA[1], fPA[2], fPA[3]); break;
        }
    }

    mArduinoTime->start();
    setup();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_Loop()));
    timer->start(10);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_OneSecond()));
    timer->start(1000);
}

void MainWindow::on_Loop()
{
    loop();
    updateLiquidCrystal();
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

int  MainWindow::getPinValue(int aPin)
{
    if (aPin >= 0 && aPin < mListModel->rowCount())
    {
        return mListModel->data(mListModel->index(aPin, eValue, QModelIndex())).toInt();
    }
    else
    {
        printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
    }

    return -1;
}

void MainWindow::setPinValue(int aPin, int aValue)
{
    if (aPin >= 0 && aPin < mListModel->rowCount())
    {
        mListModel->setData(mListModel->index(aPin, eValue, QModelIndex()), QString::number(aValue));
    }
    else
    {
        printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
    }
}

void MainWindow::pinMode(int aPin, int aType)
{
    if (gmThis)
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
    else
    {
        std::vector<int> fFunction = { ePinMode, aPin, aType };
        gmStaticPinAccess.push_back(fFunction);
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
        std::vector<int> fFunction = { eSetPinValue, aPin, aValue };
        gmStaticPinAccess.push_back(fFunction);
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
        return gmThis->getPinValue(aPin);
    }
    return 0;
}

void MainWindow::analogWrite(int aPin, int aValue)
{
    if (gmThis)
    {
        gmThis->setPinValue(aPin, aValue);
    }
    else
    {
        std::vector<int> fFunction = { eSetPinValue, aPin, aValue };
        gmStaticPinAccess.push_back(fFunction);
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
        std::vector<int> fFunction = { eTone, aPin, aFrequency, aLength };
        gmStaticPinAccess.push_back(fFunction);
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
            } while (fOK);
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
    return 15;
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
    std::vector<int> fPins = getButtonPin(*ui->editEscUp, 0, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnEscape_released()
{
    int fValue = 0;
    std::vector<int>fPins = getButtonPin(*ui->editEscUp, 0, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnUp_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editEscUp, 1, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnUp_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editEscUp, 1, fValue);
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
    std::vector<int> fPins = getButtonPin(*ui->editClearDown, 0, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnClear_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editClearDown, 0, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnDown_pressed()
{
    int fValue = 1;
    std::vector<int> fPins = getButtonPin(*ui->editClearDown, 1, fValue);
    digitalWrite(fPins, fValue);
}
void MainWindow::on_btnDown_released()
{
    int fValue = 0;
    std::vector<int> fPins = getButtonPin(*ui->editClearDown, 1, fValue);
    digitalWrite(fPins, fValue);
}

void MainWindow::setLiquidCrystal(LiquidCrystal *aLCD)
{
    mLiquidCrystal = aLCD;
}

QString getSettingsName(const QString& aItemName)
{
    QRegExp fRegEx("([A-Z][A-Za-z]+)");
    int fPos = fRegEx.indexIn(aItemName);
    if (fPos != -1 && fRegEx.captureCount())
    {
        return fRegEx.capturedTexts()[0];
    }
    else return aItemName;
}

