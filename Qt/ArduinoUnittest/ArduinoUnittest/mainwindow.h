#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <sstream>

class QAbstractItemModel;
class QModelIndex;
class QLineEdit;
class QTime;
class LiquidCrystal;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum eFunction
    {
        ePinMode, eSetPinValue, eTone
    };
    typedef std::vector< std::vector< int > > tPinAccess;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static void pinMode(int aPin, int aType);

    static int  digitalRead(int aPin);
    static void digitalWrite(int aPin, int aValue);

    static int  analogRead(int aPin);
    static void analogWrite(int aPin, int aValue);

    static void tone(int aPin, int aFrequency, int aLength);
    static void noTone(int aPin);

    static int  pulseIn(int aPin, int aLevel, int aTimeout);

    static int  getFirstAnalogPin();
    static int  elapsed();


    void setLiquidCrystal(LiquidCrystal* aLCD);
    int  getPinValue(int aPin);
    void setPinValue(int aPin, int aValue);

private:
    void digitalWrite(const std::vector<int>& aPins, int aValue);

    std::vector<int>  getButtonPin(const QLineEdit& aLineEdit, int aNo, int& aValue) const;
    void updateLiquidCrystal();


public Q_SLOTS:
    void sendSerialText(const QString& aText);
    void printToSeriaDisplay(const QString& aText);

    void on_Setup();
    void on_Loop();

    void on_OneSecond();

private Q_SLOTS:
    void on_btnDoTest_clicked();
    void on_btnClearText_clicked();
    void on_btnSendText_clicked();

    void on_btnEscape_released();
    void on_btnEscape_pressed();
    void on_btnRight_pressed();
    void on_btnRight_released();
    void on_btnClear_pressed();
    void on_btnClear_released();
    void on_btnUp_pressed();
    void on_btnUp_released();
    void on_btnEnter_pressed();
    void on_btnEnter_released();
    void on_btnDown_pressed();
    void on_btnDown_released();
    void on_btnLeft_pressed();
    void on_btnLeft_released();

    void on_bntSendFile_clicked();

private:
    Ui::MainWindow *ui;
    QAbstractItemModel* mListModel;
    int  mFirstAnalogPin;
    QTime* mArduinoTime;
    LiquidCrystal* mLiquidCrystal;

    static MainWindow* gmThis;
    static tPinAccess gmStaticPinAccess;
};

#endif // MAINWINDOW_H
