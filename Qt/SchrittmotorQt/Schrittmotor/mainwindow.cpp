#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calibrateplotter.h"
#include "xml_functions.h"
#include "lptporttestdlg.h"
#include "gcodecontroldlg.h"
#include "plotterdriverconnection.h"

#include <boost/bind.hpp>
#include <QFileDialog>
#include <QProgressDialog>

#define XML 1

#if XML==2
    #include <boost/archive/xml_oarchive.hpp>
    #include <boost/archive/xml_iarchive.hpp>
#endif

#include <iostream>
#include <fstream>
#include <sstream>


static const char* sSettings            = "Settings";
static const char* sDriver              = "Driver";
static const char* sUseDriver           = "use_driver";
static const char* sPortNumber          = "port_number";
static const char* sPortPath            = "port_path";
static const char* sDelays              = "Delays";
static const char* sX                   = "X";
static const char* sY                   = "Y";
static const char* sZ                   = "Z";
static const char* sMotorDirection      = "MotorDirection";
static const char* sMiscelaneouus       = "Miscelaneouus";
static const char* sPenWidth            = "pen_width";
static const char* sCalibrationDistance = "CalibrationDistance";
static const char* sCalibrationSteps    = "CalibrationSteps";
static const char* sOffset              = "Offset";
static const char* sToPaper             = "to_paper";
static const char* sToPlate             = "to_plate";
static const char* sToMovingPos         = "to_moving_pos";
static const char* sPlotterDrawCommands = "PlotterDrawCommands";
static const char* sTransformation      = "sTransformation";
static const char* sTestText            = "TestText";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , mCodeTestDlg(0)
  , mpProgressDlg(0)
  , mSavePosition(false)
{
    ui->setupUi(this);
    ui->statusBar->insertPermanentWidget(0, new QLabel("x"), 0);
    ui->statusBar->insertPermanentWidget(1, new QLabel("y"), 0);
    ui->statusBar->insertPermanentWidget(2, new QLabel("z"), 0);

    QGraphicsScene* scene = new QGraphicsScene;
    ui->graphicsView->setScene(scene);

    ui->graphicsView->mSendPos = boost::bind(&MainWindow::statusPosition, this, _1, _2);

    mPS_Interpreter.setPlotterDC(ui->graphicsView);
    mHPGL_Interpreter.setPlotterDC(ui->graphicsView);
    mGCodeInterpreter.setPlotterDC(ui->graphicsView);
    mGbrInterpreter.setPlotterDC(ui->graphicsView);

    readConfig();

    connect(mMotorDriver.mConnection.get(), SIGNAL(updateParam(float,int)), this, SLOT(updatePlotterParam(float,int)));
    connect(mMotorDriver.mConnection.get(), SIGNAL(updatePosition(int,int,int)), this, SLOT(updatePlotterPosition(int,int,int)));

    mMotorDriver.start(QThread::TimeCriticalPriority);

    ui->menuBar->connect(ui->menuBar, SIGNAL(triggered(QAction*)), this, SLOT(on_menu_action_triggered(QAction*)));
    on_checkAutoDrillpoints_clicked(ui->checkAutoDrillpoints->isChecked());
    Logger::setLogFunction(boost::bind(&MainWindow::logFunction, this, _1));
}

MainWindow::~MainWindow()
{
    mMotorDriver.PostCommand(PlotterDriver::Command::Quit);
    mMotorDriver.exit();
    delete ui;
    delete mpProgressDlg;
    delete mCodeTestDlg;
}

void MainWindow::logFunction(const char *aText)
{
    if (strlen(aText) == 0)
    {
        ui->labelCommandInfo->setText("");
    }
    else
    {
        ui->labelCommandInfo->append(aText);
    }
}

QString MainWindow::getXmlName()
{
    return windowTitle()+".xml";
}

void MainWindow::updateElements(bool aSave)
{
    if (aSave)
    {
        std::vector<float> fDelays = mMotorDriver.getDelays();
        fDelays[PlotterDriver::horizonal_delay]       = ui->spinHorizontalDelay->text().toFloat();
        fDelays[PlotterDriver::horizonal_start_delay] = ui->spinHorizontalStartDelay->text().toFloat();
        fDelays[PlotterDriver::drill_delay]           = ui->spinDrillDelay->text().toFloat();
        fDelays[PlotterDriver::pen_delay]             = ui->spinVerticalDelay->text().toFloat();
        fDelays[PlotterDriver::pen_start_delay]       = ui->spinVerticalStartDelay->text().toFloat();
        fDelays[PlotterDriver::manual_delay_horizonal]= ui->spinManualDelayHorizontal->text().toFloat();
        fDelays[PlotterDriver::manual_delay_vertical] = ui->spinManualDelayVertical->text().toFloat();
        mMotorDriver.setDelayIn_ms(fDelays);

        mPS_Interpreter.usePath(ui->checkUsePath->checkState() == Qt::Checked);
    }
    else
    {
        const std::vector<float>& fDelays = mMotorDriver.getDelays();
        ui->spinHorizontalDelay->setValue(fDelays[PlotterDriver::horizonal_delay]);
        ui->spinHorizontalStartDelay->setValue(fDelays[PlotterDriver::horizonal_start_delay]);
        ui->spinDrillDelay->setValue(fDelays[PlotterDriver::drill_delay]);
        ui->spinVerticalDelay->setValue(fDelays[PlotterDriver::pen_delay]);
        ui->spinVerticalStartDelay->setValue(fDelays[PlotterDriver::pen_start_delay]);
        ui->spinManualDelayHorizontal->setValue(fDelays[PlotterDriver::manual_delay_horizonal]);
        ui->spinManualDelayVertical->setValue(fDelays[PlotterDriver::manual_delay_vertical]);
    }
}

void MainWindow::saveConfig()
{
    QDomDocument doc(sSettings);
    QDomElement root = doc.createElement(sSettings);
    doc.appendChild(root);

    {
        QDomElement fDriver= doc.createElement(sDriver);
        root.appendChild(fDriver);
        addElement( doc, fDriver, sUseDriver, mMotorDriver.mUseLptDriver);
        addElement( doc, fDriver, sPortNumber, mMotorDriver.getPortAddress());
        if (mMotorDriver.getLPT().getPortType() == LPTPort::parport)
        {
            addElementS( doc, fDriver, sPortPath, QString(mMotorDriver.getLPT().getPortPath(0)));
        }
    }
    {
        QDomElement fDelayElem = doc.createElement(sDelays);
        root.appendChild(fDelayElem);
        const std::vector<float>& fDelays = mMotorDriver.getDelays();
        for (int fDelay=0; fDelay<PlotterDriver::delays; ++fDelay)
        {
            addElement( doc, fDelayElem, PlotterDriver::getNameOfEnum(static_cast<PlotterDriver::delay>(fDelay)), fDelays[fDelay]);
        }
    }
    {
        QDomElement fElement= doc.createElement(sMotorDirection);
        root.appendChild(fElement);
        addElement( doc, fElement, sX, mMotorDriver.getMotor(PlotterDriver::xdir));
        addElement( doc, fElement, sY, mMotorDriver.getMotor(PlotterDriver::ydir));
        addElement( doc, fElement, sZ, mMotorDriver.getMotor(PlotterDriver::zdir));
    }
    {
        QDomElement fElement= doc.createElement(sMiscelaneouus);
        root.appendChild(fElement);
        addElement( doc, fElement, sPenWidth, ui->graphicsView->getPenWidth());
        if (mCodeTestDlg)
        {
            addElementS( doc, fElement, sTestText, mCodeTestDlg->getText());
        }
    }
    {
        QDomElement fElement= doc.createElement(sCalibrationDistance);
        root.appendChild(fElement);
        const std::vector<float>& fCalibrationDistance = mMotorDriver.getCalibrationDistance();
        addElement( doc, fElement, sX, fCalibrationDistance[PlotterDriver::xdir]);
        addElement( doc, fElement, sY, fCalibrationDistance[PlotterDriver::ydir]);
        addElement( doc, fElement, sZ, fCalibrationDistance[PlotterDriver::zdir]);
    }
    {
        QDomElement fElement= doc.createElement(sCalibrationSteps);
        root.appendChild(fElement);
        const std::vector<int>& fCalibrationSteps = mMotorDriver.getCalibrationSteps();
        addElement( doc, fElement, sX, fCalibrationSteps[PlotterDriver::xdir]);
        addElement( doc, fElement, sY, fCalibrationSteps[PlotterDriver::ydir]);
        addElement( doc, fElement, sZ, fCalibrationSteps[PlotterDriver::zdir]);
    }
    {
        QDomElement fElement= doc.createElement(sOffset);
        root.appendChild(fElement);
        const std::vector<FLOAT>& Offsets = mMotorDriver.getPenOffsets();
        addElement( doc, fElement, sToPaper, Offsets[PlotterDriver::to_paper]);
        addElement( doc, fElement, sToPlate, Offsets[PlotterDriver::to_plate]);
        addElement( doc, fElement, sToMovingPos, Offsets[PlotterDriver::to_moving_pos]);
    }

    {
        QString xml = doc.toString();
        QFile file;
        QDir::setCurrent(QDir::homePath());
        file.setFileName(getXmlName());
        file.open(QFile::WriteOnly|QFile::Text);
        file.write(xml.toStdString().c_str());
        file.close();
    }
}

void MainWindow::readConfig()
{
    QDomDocument fDoc(getXmlName());

    QFile file(getXmlName());
    QDir::setCurrent(QDir::homePath());
    if (file.open(QIODevice::ReadOnly))
    {
        fDoc.setContent(&file);
        file.close();
    }
    {
        QDomNode fDriver = findElement(fDoc.firstChild(), sDriver);
        mMotorDriver.mUseLptDriver = getValue(findElement(fDriver, sUseDriver), mMotorDriver.mUseLptDriver);
        QDomNode fPath = findElement(fDriver, sPortPath);
        bool fSuccess = false;
        if (!fPath.isNull())
        {
            fSuccess = mMotorDriver.setPortPath(fPath.toElement().text().toStdString().c_str());
        }
        if (!fSuccess)
        {
            mMotorDriver.setPortAddress(getValue(findElement(fDriver, sPortNumber), mMotorDriver.getPortAddress()));
        }
    }
    {
        QDomNode fElement = findElement(fDoc.firstChild(), sDelays);
        std::vector<float> fDelays(PlotterDriver::delays, 1);
        for (int fDelay=0; fDelay<PlotterDriver::delays; ++fDelay)
        {
            fDelays[fDelay] = getValue(findElement(fElement, PlotterDriver::getNameOfEnum(static_cast<PlotterDriver::delay>(fDelay))), 1.0);
        }
        mMotorDriver.setDelayIn_ms(fDelays);
    }
    {
        QDomNode fElement= findElement(fDoc.firstChild(), sMotorDirection);
        mMotorDriver.setMotorDirection(static_cast<PlotterDriver::motor>(getValue(findElement(fElement, sX), mMotorDriver.getMotor(PlotterDriver::xdir))), PlotterDriver::xdir);
        mMotorDriver.setMotorDirection(static_cast<PlotterDriver::motor>(getValue(findElement(fElement, sY), mMotorDriver.getMotor(PlotterDriver::ydir))), PlotterDriver::ydir);
        mMotorDriver.setMotorDirection(static_cast<PlotterDriver::motor>(getValue(findElement(fElement, sZ), mMotorDriver.getMotor(PlotterDriver::zdir))), PlotterDriver::zdir);
    }
    {
        QDomNode fElement= findElement(fDoc.firstChild(), sMiscelaneouus);
        ui->graphicsView->setPenWidth(getValue(findElement(fElement, sPenWidth), ui->graphicsView->getPenWidth()));
        mCodeTestDlg = new CodeTestDlg(this);
        mCodeTestDlg->setText(getValue(findElement(fElement, sTestText), QString("")));
        connect(mCodeTestDlg, SIGNAL(testParser(int,QString)), this, SLOT(testParser(int,QString)));
    }
    {
        QDomNode fElement= findElement(fDoc.firstChild(), sCalibrationDistance);
        std::vector<float> fCalibrationDistance(PlotterDriver::directions, 0);
        fCalibrationDistance[PlotterDriver::xdir] = getValue(findElement(fElement, sX), fCalibrationDistance[PlotterDriver::xdir]);
        fCalibrationDistance[PlotterDriver::ydir] = getValue(findElement(fElement, sY), fCalibrationDistance[PlotterDriver::ydir]);
        fCalibrationDistance[PlotterDriver::zdir] = getValue(findElement(fElement, sZ), fCalibrationDistance[PlotterDriver::zdir]);
        mMotorDriver.setCalibrationDistance(fCalibrationDistance);
    }
    {
        QDomNode fElement= findElement(fDoc.firstChild(), sCalibrationSteps);
        std::vector<int> fCalibrationSteps(PlotterDriver::directions, 0);
        fCalibrationSteps[PlotterDriver::xdir] = getValue(findElement(fElement, sX), fCalibrationSteps[PlotterDriver::xdir]);
        fCalibrationSteps[PlotterDriver::ydir] = getValue(findElement(fElement, sY), fCalibrationSteps[PlotterDriver::ydir]);
        fCalibrationSteps[PlotterDriver::zdir] = getValue(findElement(fElement, sZ), fCalibrationSteps[PlotterDriver::zdir]);
        mMotorDriver.setCalibrationSteps(fCalibrationSteps);
    }

    {
        QDomNode fElement= findElement(fDoc.firstChild(), sOffset);
        std::vector<FLOAT> fOffsets(PlotterDriver::offsets, 10);
        fOffsets[PlotterDriver::to_paper]      = getValue(findElement(fElement, sToPaper), fOffsets[PlotterDriver::to_paper]);
        fOffsets[PlotterDriver::to_plate]      = getValue(findElement(fElement, sToPlate), fOffsets[PlotterDriver::to_plate]);
        fOffsets[PlotterDriver::to_moving_pos] = getValue(findElement(fElement, sToMovingPos), fOffsets[PlotterDriver::to_moving_pos]);
        mMotorDriver.setPenOffsets(fOffsets);
    }

    updateElements(false);
}


void  MainWindow::statusPosition(float aX, float aY)
{
    QString s;
    s.sprintf("x: %.2f, y: %.2f", aX, aY);
    ui->statusBar->showMessage(s);
}

void  MainWindow::updatePlotterParam(float aParam, int aType)
{
    TRACE(Logger::debug, "MainWindow::updatePlotterParam(%f, %d)", aParam, aType);
    if (aType == CPlotterDC::Progress)
    {
        if (mpProgressDlg)
        {
            mpProgressDlg->setValue(static_cast<int>(aParam));
            if (aParam >= 99.9)
            {
                delete mpProgressDlg;
                mpProgressDlg = 0;
                if (ui->checkSimulate->isChecked())
                {
                    TRACE(Logger::to_info, "Used time for process: %f s", mMotorDriver.getDurationMS()*0.001);
                }
            }
        }
        else
        {
            ui->statusBar->showMessage(QString("Plotfortschritt %1 %").arg(aParam));
        }
    }
    else if (aType == CPlotterDC::CmdIndex)
    {
        ui->graphicsView->selectItem(static_cast<int>(aParam));
        ui->graphicsView->redraw();
    }
    else if (aType == CPlotterDC::SelectCmdIndex)
    {
        int fIndex = static_cast<int>(aParam);
        const auto& fDrawCmds = ui->graphicsView->getDrawCommands();
        if (fIndex >= 0 && fIndex < static_cast<int>(fDrawCmds.size()))
        {
            ui->checkIsDrillPoint->setCheckState(fDrawCmds.at(fIndex).isDrillPoint() ? Qt::Checked : Qt::Unchecked);
            ui->graphicsView->selectItem(fIndex);
            ui->labelCommandInfo->setText(QString::number(fIndex) + ": " + fDrawCmds[fIndex].info());
            ui->graphicsView->redraw();
        }
    }
}


void  MainWindow::updatePlotterPosition(int aX, int aY, int aZ)
{
    TRACE(Logger::debug, "MainWindow::updatePlotterPosition(%d, %d, %d)", aX, aY, aZ);
    QList<QLabel*> fLables = ui->statusBar->findChildren<QLabel*>();
    if (fLables.count()>=3)
    {
        fLables[0]->setNum(aX);
        fLables[1]->setNum(aY);
        fLables[2]->setNum(aZ);
    }
}

void MainWindow::testParser(int aParser, const QString& aText)
{
    switch (static_cast<Format>(aParser))
    {
    case Format::Postscript: mPS_Interpreter.parseText(aText); break;
    case Format::GCode:      mGCodeInterpreter.parseText(aText); break;
    case Format::Gerber:     mGbrInterpreter.parseText(aText); break;
    case Format::HPGL:       mHPGL_Interpreter.parseText(aText); break;
    }
}

void MainWindow::keyPressEvent ( QKeyEvent * event )
{
    QMainWindow::keyPressEvent(event);
    //printf("keypressed: %d\n", event->nativeVirtualKey());
}

void MainWindow::keyReleaseEvent ( QKeyEvent * event )
{
    QMainWindow::keyReleaseEvent(event);
    //printf("keyreleased: %d\n", event->nativeVirtualKey());
}


void MainWindow::on_menu_action_triggered(QAction*anAction)
{
    QString s = anAction->objectName();
    if (s.compare("actionLoadFile") == 0)
    {
        bool ok;
        ui->graphicsView->setRotation(ui->lineEditRotation->text().toFloat(&ok));
        onLoadFile();
    }
    else if (s.compare("actionSaveFile") == 0)
    {
        onSaveFile();
    }
    else if (s.compare("actionOptionsSettings") == 0)
    {
        CalibratePlotter fDlg(&mMotorDriver, ui->graphicsView, this);
        fDlg.exec();
    }
    else if (s.compare("actionPortTest") == 0)
    {
        LPTPortTestDlg fDlg(&mMotorDriver, this);
        fDlg.exec();
    }
    else if (s.compare("actionG_Code_Dialog") == 0)
    {
        mCodeTestDlg->show();
    }
    else if (s.compare("actionFileExit") == 0)
    {
        exit(0);
    }

}

void MainWindow::on_buttonOK_clicked()
{
    saveConfig();
    exit(0);
}

void MainWindow::onLoadFile()
{
    QFileDialog dialog(this, Qt::Dialog|Qt::SubWindow);
    QStringList filters;
    filters << "Gerber File (*.gbr)"
            << "Plotter File (*.plt)"
            << "G-Code File (*.gc)"
            << "HPGL File (*.hpgl)"
            << "Postscript File (*.ps)";
    dialog.setNameFilters(filters);
    QStringList fileNames;
    if (dialog.exec())
    {
        updateElements();
        fileNames = dialog.selectedFiles();
        if (fileNames.size())
        {
            if (fileNames.at(0).endsWith(".ps"))
            {
                mPS_Interpreter.readFile(fileNames.at(0));
            }
            else if (fileNames.at(0).endsWith(".hpgl"))
            {
                mHPGL_Interpreter.readFile(fileNames.at(0));
            }
            else if (fileNames.at(0).endsWith(".gc"))
            {
                mGCodeInterpreter.readFile(fileNames.at(0));
            }
            else if (fileNames.at(0).endsWith(".gbr"))
            {
                mGbrInterpreter.readFile(fileNames.at(0));
            }
            else if (fileNames.at(0).endsWith(".plt"))
            {
                QDomDocument fDoc(fileNames[0]);

                QFile file(fileNames[0]);
                if (file.open(QIODevice::ReadOnly))
                {
                    fDoc.setContent(&file);
                    ui->graphicsView->clearDrawCommands();

                    QDomNode fTransformation = findElement(fDoc.firstChild(), sTransformation);
                    std::istringstream fStream(fTransformation.toElement().text().toStdString().c_str());
                    rke::matrix<FLOAT> fTransformationMatrix;
                    fStream >> fTransformationMatrix;
                    if (fTransformationMatrix.size() == 9)
                    {
                        ui->graphicsView->setTransformation(fTransformationMatrix);
                    }

                    QDomNode fDrawCommands = findElement(fDoc.firstChild(), "Item");
                    for(QDomNode n = fDrawCommands; !n.isNull(); n = n.nextSibling())
                    {
                        CPlotterDC::DrawCommand fCmd;
                        fCmd.readXml(n);
                        ui->graphicsView->addDrawCommand(fCmd);
                    }

                    ui->graphicsView->updateWindowMapping();
                    ui->graphicsView->redraw();

                    file.close();
                }
            }
        }
    }
}

void MainWindow::onSaveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                 QDir::homePath()+"/untitled.plt",
                                 tr("Plotter File (*.plt)"));
    if (fileName.size())
    {
        QDomDocument fDoc(sPlotterDrawCommands);
        QDomElement fRoot = fDoc.createElement(sPlotterDrawCommands);
        fDoc.appendChild(fRoot);

        std::ostringstream fTransformation;
        fTransformation << ui->graphicsView->getTransformation();
        addElementS(fDoc, fRoot, sTransformation, QString(fTransformation.str().c_str()));

        for (std::vector<CPlotterDC::DrawCommand>::const_iterator it = ui->graphicsView->getDrawCommands().begin();
             it != ui->graphicsView->getDrawCommands().end(); ++it)
        {
            it->writeXml(fDoc, fRoot);
        }

        QString xml = fDoc.toString();
        QFile file;
        file.setFileName(fileName);
        file.open(QFile::WriteOnly|QFile::Text);
        file.write(xml.toStdString().c_str());
        file.close();
    }
}


void MainWindow::on_buttonPlot_clicked()
{
    updateElements();
    mMotorDriver.setPlotCommands(ui->graphicsView->getDrawCommands());
    mMotorDriver.setCountMS(ui->checkSimulate->isChecked());
    mMotorDriver.PostCommand(PlotterDriver::Command::Plot);

    mpProgressDlg = new QProgressDialog(tr("Plottausgabe"), tr("Abbrechen"), 0, 100, this);
    mpProgressDlg->setWindowModality(Qt::WindowModal);
    connect(mpProgressDlg, SIGNAL(canceled()), this, SLOT(on_buttonStop_clicked()));
    mpProgressDlg->setValue(0.1);
    mpProgressDlg->show();
}

void MainWindow::on_buttonDrill_clicked()
{
    updateElements();
    mMotorDriver.setPlotCommands(ui->graphicsView->getDrawCommands());
    mMotorDriver.setCountMS(ui->checkSimulate->isChecked());
    mMotorDriver.PostCommand(PlotterDriver::Command::Drill);

    mpProgressDlg = new QProgressDialog(tr("Bohren"), tr("Abbrechen"), 0, 100, this);
    mpProgressDlg->setWindowModality(Qt::WindowModal);
    connect(mpProgressDlg, SIGNAL(canceled()), this, SLOT(on_buttonStop_clicked()));
    mpProgressDlg->setValue(0.1);
    mpProgressDlg->show();
}

void MainWindow::on_buttonForward_pressed()
{
    updateElements();
    mMotorDriver.PostCommand(PlotterDriver::Command::TestTurn, 100000, PlotterDriver::forward);
}

void MainWindow::on_buttonForward_released()
{
    mMotorDriver.stopRun();
}

void MainWindow::on_buttonLeft_pressed()
{
    updateElements();
    mMotorDriver.PostCommand(PlotterDriver::Command::TestTurn, 100000, PlotterDriver::left);
}

void MainWindow::on_buttonLeft_released()
{
    mMotorDriver.stopRun();
}

void MainWindow::on_buttonBackward_pressed()
{
    updateElements();
    mMotorDriver.PostCommand(PlotterDriver::Command::TestTurn, 100000, PlotterDriver::backward);
}

void MainWindow::on_buttonBackward_released()
{
    mMotorDriver.stopRun();
}

void MainWindow::on_buttonRight_pressed()
{
    updateElements();
    mMotorDriver.PostCommand(PlotterDriver::Command::TestTurn, 100000, PlotterDriver::right);
}

void MainWindow::on_buttonRight_released()
{
    mMotorDriver.stopRun();
}

void MainWindow::on_buttonPenUp_pressed()
{
    updateElements();
    mMotorDriver.PostCommand(PlotterDriver::Command::TestTurn, 100000, PlotterDriver::up);
}

void MainWindow::on_buttonPenUp_released()
{
    mMotorDriver.stopRun();
}

void MainWindow::on_buttonPenDown_pressed()
{
    updateElements();
    mMotorDriver.PostCommand(PlotterDriver::Command::TestTurn, 100000, PlotterDriver::down);
}

void MainWindow::on_buttonPenDown_released()
{
    mMotorDriver.stopRun();
}

void MainWindow::on_buttonStop_clicked()
{
    mMotorDriver.stopRun();
    if (ui->checkSimulate->isChecked())
    {
        mMotorDriver.setCountMS(false);
    }
    delete mpProgressDlg;
    mpProgressDlg = 0;
}

void MainWindow::on_buttonSetUpEndPos_clicked()
{
    if (mSavePosition)
    {
        mMotorDriver.setUpEndPosition();
        ui->statusBar->showMessage(tr("Ruheposition des Stiftes gespeichert"));
    }
    else
    {
        mMotorDriver.PostCommand(PlotterDriver::Command::TestPen, 0);
        ui->statusBar->showMessage(tr("Ruheposition des Stiftes anfahren"));
    }
}

void MainWindow::on_buttonSetDownEndPos_clicked()
{
    if (mSavePosition)
    {
        mMotorDriver.setDownEndPosition();
        ui->statusBar->showMessage(tr("Schreibposition des Stiftes gespeichert"));
    }
    else
    {
        mMotorDriver.PostCommand(PlotterDriver::Command::TestPen, 2);
        ui->statusBar->showMessage(tr("Schreibposition des Stiftes anfahren"));
    }
}

void MainWindow::on_buttonSetDrillEndPos_clicked()
{
    if (mSavePosition)
    {
        mMotorDriver.setDrillEndPosition();
        ui->statusBar->showMessage(tr("Endposition des Bohrers gespeichert"));
    }
    else
    {
        mMotorDriver.PostCommand(PlotterDriver::Command::TestPen, 1);
        ui->statusBar->showMessage(tr("Endposition des Bohrers anfahren"));
    }
}

void MainWindow::on_buttonSetPosition_clicked()
{
    FLOAT x(0), y(0);
    if (ui->graphicsView->getStartPosition(x, y))
    {
        mMotorDriver.setPenPosition(x, y);
        ui->statusBar->showMessage(tr("Anfangsposition fuer Plottvorgang gesetzt (rotes Quadrat)"));
    }
}

void MainWindow::on_buttonCalibratePen_clicked()
{
    mMotorDriver.PostCommand(PlotterDriver::Command::CalibratePen);
}

void MainWindow::on_buttonSort_clicked()
{
    ui->graphicsView->sortDrawCommands();
    ui->graphicsView->redraw();
}


void MainWindow::on_zoomIn_clicked()
{
    ui->graphicsView->zoomIn();
}

void MainWindow::on_zoomOut_clicked()
{
    ui->graphicsView->zoomOut();
}

void MainWindow::on_checkSavePosition_clicked(bool checked)
{
    mSavePosition = checked;
    if (mSavePosition)
    {
        ui->statusBar->showMessage(tr("Speichern der Positionen aktiviert"));
    }
    else
    {
        ui->statusBar->showMessage(tr("Anfahren der Positionen aktiviert"));
    }
}

void MainWindow::on_buttonSetAsStartPosition_clicked()
{
    ui->graphicsView->setSelectedItemAsFirst();
    ui->graphicsView->redraw();
}

void MainWindow::on_buttonClearAllDrillPoints_clicked()
{
    ui->graphicsView->toggleAllDrillPointStates();
    ui->graphicsView->redraw();
}

void MainWindow::on_checkIsDrillPoint_clicked()
{
    ui->graphicsView->setSelectedtDrillPointState(ui->checkIsDrillPoint->checkState()==Qt::Checked);
    ui->graphicsView->redraw();
}

void MainWindow::on_buttonDisplayAllDrawCommands_clicked()
{
    ui->graphicsView->setClipRect(0);
    ui->graphicsView->redraw();
}

void MainWindow::on_buttonSelectPreviousCmd_clicked()
{
    int fSelected = ui->graphicsView->getSelectedItem()-1;
    updatePlotterParam(fSelected, CPlotterDC::SelectCmdIndex);
}

void MainWindow::on_buttonSelectNextCmd_clicked()
{
    int fSelected = ui->graphicsView->getSelectedItem()+1;
    updatePlotterParam(fSelected, CPlotterDC::SelectCmdIndex);
}

void MainWindow::on_checkAutoDrillpoints_clicked(bool checked)
{
    mGbrInterpreter.setAutoDetermineDrillpoints(checked);
    ui->graphicsView->setAutoDetermineDrillpoints(checked);
}
