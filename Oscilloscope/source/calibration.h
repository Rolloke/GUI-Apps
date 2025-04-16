#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QDialog>
#include <vector>
class Channel;

namespace Ui {
class Calibration;
}

class Calibration : public QDialog
{
    Q_OBJECT

public:
    explicit Calibration(QWidget *parent, std::vector<Channel>& aChannelParam);
    ~Calibration();

    void updateParameters();

private Q_SLOTS:
    void on_comboChannel_currentIndexChanged(int index);
    void on_btnApply_clicked();
    void on_btnCalcScalingFactor_clicked();
    void on_btnSetScalingOffset_clicked();

    void on_btnResetScalingAndOffset_clicked();

private:
    void showEvent (QShowEvent * event );
    Ui::Calibration *ui;
    std::vector<Channel>& mChannelParameter;
    unsigned int mChannel;
};

#endif // CALIBRATION_H
