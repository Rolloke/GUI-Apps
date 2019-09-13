#include "calibrateplotter.h"
#include "ui_calibrateplotter.h"
#include "PlotterDriver.h"
//test
CalibratePlotter::CalibratePlotter(PlotterDriver*apPD, CPlotterDC*apPDC, QWidget *parent) :
    QDialog(parent)
  , ui(new Ui::CalibratePlotter)
  , mpMotorDriver(apPD)
  , mpPlotterDC(apPDC)

{
    mCalibrationDistance = mpMotorDriver->getCalibrationDistance();
    mCalibrationSteps    = mpMotorDriver->getCalibrationSteps();
    mOffsets             = mpMotorDriver->getPenOffsets();
    mPenWidth            = mpPlotterDC->getPenWidth();
    mMotor.resize(PlotterDriver::directions);
    mMotor[PlotterDriver::xdir] = mpMotorDriver->getMotor(PlotterDriver::xdir);
    mMotor[PlotterDriver::ydir] = mpMotorDriver->getMotor(PlotterDriver::ydir);
    mMotor[PlotterDriver::zdir] = mpMotorDriver->getMotor(PlotterDriver::zdir);

    ui->setupUi(this);
    const char* Motortypes[] = {
    "Motor Bits 1, 2 (bipol full step)",
    "Motor Bits 3, 4 (bipol full step)",
    "Motor Bits 5, 6 (bipol full step)",
    "Motor Bits 7, 8 (bipol full step)",
    "Motor Bits 1-4 (bipol half step)",
    "Motor Bits 5-8 (bipol half step)",
    "Motor Bits 1-4 (unipol half step)",
    "Motor Bits 5-8 (unipol half step)",
    "Motor Bits 1, 2 (bipol half step)",
    "Motor Bits 3, 4 (bipol half step)",
    "Motor Bits 5, 6 (bipol half step)",
    "Motor Bits 7, 8 (bipol half step)",
     NULL
    };
    int i=0;
    while (Motortypes[i])
    {
        ui->comboMotorX->addItem(Motortypes[i]);
        ui->comboMotorY->addItem(Motortypes[i]);
        ui->comboMotorZ->addItem(Motortypes[i]);
        ++i;
    }
    for (int i=0; i<mpMotorDriver->getLPT().getPorts(); ++i)
    {
        ui->comboPorts->addItem(mpMotorDriver->getLPT().getPortName(i));
    }
    if (mpMotorDriver->getLPT().getPortType() == LPTPort::parport)
    {
        ui->lineEditPortPath->setText(mpMotorDriver->getLPT().getPortPath(0));
    }
    updateElements(false);
}

CalibratePlotter::~CalibratePlotter()
{
    delete ui;
}

void CalibratePlotter::updateElements(bool aSave)
{
    if (aSave)
    {
        bool ok = false;
        mCalibrationDistance[PlotterDriver::xdir] = ui->leCalibrationDistanceX->text().toFloat(&ok);
        mCalibrationDistance[PlotterDriver::ydir] = ui->leCalibrationDistanceY->text().toFloat(&ok);
        mCalibrationDistance[PlotterDriver::zdir] = ui->leCalibrationDistanceZ->text().toFloat(&ok);

        mCalibrationSteps[PlotterDriver::xdir] = ui->leCalibrationStepsX->text().toInt(&ok);
        mCalibrationSteps[PlotterDriver::ydir] = ui->leCalibrationStepsY->text().toInt(&ok);
        mCalibrationSteps[PlotterDriver::zdir] = ui->leCalibrationStepsZ->text().toInt(&ok);

        mOffsets[PlotterDriver::to_paper]      = ui->leOffsetToPaper->text().toFloat(&ok);
        mOffsets[PlotterDriver::to_plate]      = ui->leOffsetToPlate->text().toFloat(&ok);
        mOffsets[PlotterDriver::to_moving_pos] = ui->leOffsetToMovingPos->text().toFloat(&ok);

        mPenWidth                              = ui->lePenWidth->text().toFloat(&ok);

        mMotor[PlotterDriver::xdir] = ui->comboMotorX->currentIndex();
        mMotor[PlotterDriver::ydir] = ui->comboMotorY->currentIndex();
        mMotor[PlotterDriver::zdir] = ui->comboMotorZ->currentIndex();

        mpMotorDriver->setPortAddress(ui->lineEditPortAddress->text().toInt(&ok, 16));

    }
    else
    {
        ui->leCalibrationDistanceX->setText(QString().setNum(mCalibrationDistance[PlotterDriver::xdir]));
        ui->leCalibrationDistanceY->setText(QString().setNum(mCalibrationDistance[PlotterDriver::ydir]));
        ui->leCalibrationDistanceZ->setText(QString().setNum(mCalibrationDistance[PlotterDriver::zdir]));

        ui->leCalibrationStepsX->setText(QString().setNum(mCalibrationSteps[PlotterDriver::xdir]));
        ui->leCalibrationStepsY->setText(QString().setNum(mCalibrationSteps[PlotterDriver::ydir]));
        ui->leCalibrationStepsZ->setText(QString().setNum(mCalibrationSteps[PlotterDriver::zdir]));

        ui->leOffsetToPaper->setText(QString().setNum(mOffsets[PlotterDriver::to_paper]));
        ui->leOffsetToPlate->setText(QString().setNum(mOffsets[PlotterDriver::to_plate]));
        ui->leOffsetToMovingPos->setText(QString().setNum(mOffsets[PlotterDriver::to_moving_pos]));

        ui->lePenWidth->setText(QString().setNum(mPenWidth));

        ui->comboMotorX->setCurrentIndex(mMotor[PlotterDriver::xdir]);
        ui->comboMotorY->setCurrentIndex(mMotor[PlotterDriver::ydir]);
        ui->comboMotorZ->setCurrentIndex(mMotor[PlotterDriver::zdir]);

        ui->lineEditPortAddress->setText(QString().setNum(mpMotorDriver->getPortAddress(), 16));
    }
    ui->labelAccuracyX->setText(QString().setNum(mCalibrationDistance[PlotterDriver::xdir]/mCalibrationSteps[PlotterDriver::xdir], 'g', 3));
    ui->labelAccuracyY->setText(QString().setNum(mCalibrationDistance[PlotterDriver::ydir]/mCalibrationSteps[PlotterDriver::ydir], 'g', 3));
    ui->labelAccuracyZ->setText(QString().setNum(mCalibrationDistance[PlotterDriver::zdir]/mCalibrationSteps[PlotterDriver::zdir], 'g', 3));
}

void CalibratePlotter::on_buttonBox_accepted()
{
    updateElements(true);
    mpMotorDriver->setCalibrationDistance(mCalibrationDistance);
    mpMotorDriver->setCalibrationSteps(mCalibrationSteps);
    mpMotorDriver->setPenOffsets(mOffsets);
    mpPlotterDC->setPenWidth(mPenWidth);
    mpMotorDriver->setMotorDirection(static_cast<PlotterDriver::motor>(mMotor[PlotterDriver::xdir]), PlotterDriver::xdir);
    mpMotorDriver->setMotorDirection(static_cast<PlotterDriver::motor>(mMotor[PlotterDriver::ydir]), PlotterDriver::ydir);
    mpMotorDriver->setMotorDirection(static_cast<PlotterDriver::motor>(mMotor[PlotterDriver::zdir]), PlotterDriver::zdir);

}

void CalibratePlotter::on_buttonBox_rejected()
{

}

void CalibratePlotter::on_DrawCalibrationRectangle_clicked()
{
    updateElements(true);
    mpMotorDriver->PostCommand(PlotterDriver::Command::CalibratePlotter, mCalibrationSteps[PlotterDriver::xdir], mCalibrationSteps[PlotterDriver::ydir]);
}

void CalibratePlotter::on_moveLeft_pressed()
{
    updateElements(true);
    mpMotorDriver->PostCommand(PlotterDriver::Command::TestTurn, mCalibrationSteps[PlotterDriver::xdir], PlotterDriver::left);
}

void CalibratePlotter::on_moveRight_pressed()
{
    updateElements(true);
    mpMotorDriver->PostCommand(PlotterDriver::Command::TestTurn, mCalibrationSteps[PlotterDriver::xdir], PlotterDriver::right);
}

void CalibratePlotter::on_moveBackward_pressed()
{
    updateElements(true);
    mpMotorDriver->PostCommand(PlotterDriver::Command::TestTurn, mCalibrationSteps[PlotterDriver::ydir], PlotterDriver::backward);
}

void CalibratePlotter::on_moveForward_pressed()
{
    updateElements(true);
    mpMotorDriver->PostCommand(PlotterDriver::Command::TestTurn, mCalibrationSteps[PlotterDriver::ydir], PlotterDriver::forward);
}

void CalibratePlotter::on_moveUp_pressed()
{
    updateElements(true);
    mpMotorDriver->PostCommand(PlotterDriver::Command::TestTurn, mCalibrationSteps[PlotterDriver::zdir], PlotterDriver::up);
}

void CalibratePlotter::on_moveDown_pressed()
{
    updateElements(true);
    mpMotorDriver->PostCommand(PlotterDriver::Command::TestTurn, mCalibrationSteps[PlotterDriver::zdir], PlotterDriver::down);
}


void CalibratePlotter::on_moveLeft_released()
{
    //mpMotorDriver->stopRun();
}

void CalibratePlotter::on_moveRight_released()
{
    //mpMotorDriver->stopRun();
}

void CalibratePlotter::on_moveBackward_released()
{
    //mpMotorDriver->stopRun();
}

void CalibratePlotter::on_moveForward_released()
{
    //mpMotorDriver->stopRun();
}

void CalibratePlotter::on_moveUp_released()
{
    //mpMotorDriver->stopRun();
}

void CalibratePlotter::on_moveDown_released()
{
    //mpMotorDriver->stopRun();
}

void CalibratePlotter::on_StopMoving_clicked()
{
    mpMotorDriver->stopRun();
}
