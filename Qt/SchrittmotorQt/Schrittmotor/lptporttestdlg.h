#ifndef LPTPORTTESTDLG_H
#define LPTPORTTESTDLG_H

#include <QDialog>

namespace Ui {
class LPTPortTestDlg;
}
class LPTPort;
class PlotterDriver;

class LPTPortTestDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LPTPortTestDlg(PlotterDriver*apPD, QWidget *parent = 0);
    ~LPTPortTestDlg();

private slots:
    void on_buttonSetData_clicked();
    void on_buttonReadStatus_clicked();
    void on_buttonReadControl_clicked();
    void on_buttonSetControl_clicked();
    void on_buttonReadData_clicked();

private:
    Ui::LPTPortTestDlg *ui;
    const LPTPort *mPort;
};

#endif // LPTPORTTESTDLG_H
