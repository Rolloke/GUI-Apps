#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <vector>

#include "pintype.h"
#include "inputpin.h"
#include "outputpin.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QtSerialPort/QSerialPort>
#else
    #include "serialport.h"
#endif

class QComboBox;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void on_btnOpen_clicked(bool checked);
    void on_btnSend_clicked();
    void on_btnReceive_clicked();
    void on_btnClear_clicked();
    void on_btnSendFile_clicked(bool checked);
    void on_ckReceive_clicked(bool checked);
    void on_btnBrowse_clicked();
    void on_ckTriggerText_clicked(bool checked);
    void on_btnStartSimulation_clicked(bool checked);
    void on_radioStep_meter_clicked(bool checked);
    void on_radioStep_percent_clicked(bool checked);
    void on_lineMinDepth_m_editingFinished();
    void on_lineMaxDepth_m_editingFinished();
    void on_lineStep_m_editingFinished();
    void on_lineStepPercent_editingFinished();
    void on_ckPingBasedSim_clicked(bool checked);
    void on_btnSendCurrentDepth_clicked();
    void on_btnDeleteCommand_clicked();
    void on_btnStoreCommand_clicked();

    void on_comboInputPinNo_currentIndexChanged(int index);
    void on_comboInputNo_currentIndexChanged(int index);
    void on_comboInputEdge_currentIndexChanged(const QString &arg1);
    void on_comboInputType_currentIndexChanged(const QString &arg1);
    void on_spinAnalogThreshold_valueChanged(int arg1);
    void on_spinSwitchDeBounce_valueChanged(int arg1);
    void on_spinSwitchDelay_valueChanged(int arg1);
    void on_spinSwitchRepeat_valueChanged(int arg1);
    void on_checkReportValue_clicked(bool checked);

    void on_comboOutputNo_currentIndexChanged(int index);
    void on_comboOutputPinNo_currentIndexChanged(int index);
    void on_comboInputConnectionToOutput_currentIndexChanged(int index);
    void on_comboOutputType_currentIndexChanged(const QString &arg1);
    void on_comboOutputEdge_currentIndexChanged(const QString &arg1);
    void on_comboOutputConnectionToOutput_currentIndexChanged(int index);
    void on_spinBinaryDelay_valueChanged(int arg1);
    void on_spinBinaryPulse_valueChanged(int arg1);
    void on_spinBinaryPulseRepeat_valueChanged(int arg1);
    void on_spinOutputValue_valueChanged(int arg1);
    void on_checkOutputInvert_clicked(bool checked);
    void on_checkOutputToggle_clicked(bool checked);

    void on_btnSendConfig_clicked();
    void on_btnRequesConfig_clicked();
    void on_btnStoreConfig_clicked();
    void on_btnClearConfig_clicked();
    void on_btnHelp_clicked();

private:

    void sendText();
    void sendCurrentDepth();
    void enableSendButtons(bool aEnable);
    void changeDepth();
    void calculateSteps(bool aMeter);
    void initializeConfigurationPage();
    int  getPinIndex(int aPinNo);
    int  getPinIndexFromItemData(const QComboBox&, int aPinNo);
    void updateOutputConnectionNames();
    void updateInputControls();
    void updateOutputControls();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QSerialPort mPort;
#else
    SerialPort  mPort;
#endif
    QFile       mFile;
    int         mReceiveTimer;
    int         mSendTimer;
    int         mDepthSimulationTimer;
    bool        mDepthUp;
    int         mMaxPinNumbers;
    int         mSelectedInputPin;
    int         mSelectedOutputPin;
    bool        mCanUpdateComboBox;

    std::vector<PinType>   mPins;
    std::vector<InputPin>  mInputPins;
    std::vector<OutputPin> mOutputPins;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
