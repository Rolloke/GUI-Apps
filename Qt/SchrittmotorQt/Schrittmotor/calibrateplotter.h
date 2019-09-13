#ifndef CALIBRATEPLOTTER_H
#define CALIBRATEPLOTTER_H

#include "defines.h"
#include <QDialog>

namespace Ui {
class CalibratePlotter;
}

class PlotterDriver;
class CPlotterDC;

class CalibratePlotter : public QDialog
{
    Q_OBJECT
    
public:
    explicit CalibratePlotter(PlotterDriver* apPD, CPlotterDC*apPDC, QWidget *parent = 0);
    ~CalibratePlotter();

    const std::vector<float>& getCalibrationDistance() { return mCalibrationDistance; }
    const std::vector<int>& getCalibrationSteps() { return mCalibrationSteps; }

private:
    void updateElements(bool aSave=true);

private slots:

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_StopMoving_clicked();
    void on_DrawCalibrationRectangle_clicked();
    void on_moveLeft_pressed();
    void on_moveRight_pressed();
    void on_moveBackward_pressed();
    void on_moveForward_pressed();
    void on_moveUp_pressed();
    void on_moveDown_pressed();
    void on_moveLeft_released();
    void on_moveRight_released();
    void on_moveBackward_released();
    void on_moveForward_released();
    void on_moveUp_released();
    void on_moveDown_released();

private:
    Ui::CalibratePlotter *ui;
    PlotterDriver* mpMotorDriver;
    CPlotterDC*    mpPlotterDC;
    std::vector<float> mCalibrationDistance;
    std::vector<int>   mCalibrationSteps;
    std::vector<FLOAT> mOffsets;
    FLOAT              mPenWidth;
    std::vector<int>   mMotor;
};

#endif // CALIBRATEPLOTTER_H
