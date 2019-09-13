#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "PlotterDriver.h"
#include "PostscriptInterpreter.h"
#include "hpglinterpreter.h"
#include "gcodeinterpreter.h"
#include "gbr_interpreter.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QProgressDialog;
class CodeTestDlg;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void updateElements(bool aSave=true);
    QString getXmlName();
    void readConfig();
    void saveConfig();

    void onLoadFile();
    void onSaveFile();
    void logFunction(const char* aText);

private:
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void keyReleaseEvent ( QKeyEvent * event );

signals:
    void PlotterPositionChanged(int, int, int);
    void PlotterParamChanged(float, int );

private slots:

    void updatePlotterPosition(int aX, int aY, int aZ);
    void updatePlotterParam(float aParam, int aType);
    void testParser(int aParser, const QString &aText);
    void on_buttonOK_clicked();
    void on_buttonPlot_clicked();
    void on_buttonDrill_clicked();
    void on_buttonSetUpEndPos_clicked();
    void on_buttonSetDownEndPos_clicked();
    void on_buttonSetDrillEndPos_clicked();
    void on_buttonForward_pressed();
    void on_buttonForward_released();
    void on_buttonLeft_pressed();
    void on_buttonLeft_released();
    void on_buttonBackward_pressed();
    void on_buttonBackward_released();
    void on_buttonRight_pressed();
    void on_buttonRight_released();
    void on_buttonPenUp_pressed();
    void on_buttonPenUp_released();
    void on_buttonPenDown_pressed();
    void on_buttonPenDown_released();
    void on_buttonSort_clicked();
    void on_menu_action_triggered(QAction*);
    void on_zoomIn_clicked();
    void on_zoomOut_clicked();
    void on_buttonStop_clicked();
    void on_buttonSetPosition_clicked();
    void on_buttonCalibratePen_clicked();
    void on_checkSavePosition_clicked(bool checked);
    void on_buttonSetAsStartPosition_clicked();
    void on_buttonClearAllDrillPoints_clicked();
    void on_checkIsDrillPoint_clicked();
    void on_buttonDisplayAllDrawCommands_clicked();
    void on_buttonSelectPreviousCmd_clicked();
    void on_buttonSelectNextCmd_clicked();
    void on_checkAutoDrillpoints_clicked(bool checked);

private:
    enum class Format
    {
        Gerber,
        Postscript,
        HPGL,
        GCode
    };

    void statusPosition(float aX, float aY);

    Ui::MainWindow *ui;
    PlotterDriver mMotorDriver;
    PostscriptInterpreter mPS_Interpreter;
    HPGLInterpreter mHPGL_Interpreter;
    GCodeInterpreter mGCodeInterpreter;
    GbrInterpreter   mGbrInterpreter;
    CodeTestDlg* mCodeTestDlg;
    QProgressDialog* mpProgressDlg;
    bool mSavePosition;
};

#endif // MAINWINDOW_H
