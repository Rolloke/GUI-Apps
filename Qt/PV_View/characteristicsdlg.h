#ifndef CHARACTERISTICSDLG_H
#define CHARACTERISTICSDLG_H

#include <QDialog>
#include <QStandardItemModel>

#include "yaml_structs.h"

namespace Ui {
class CharacteristicsDlg;
}

class CharacteristicsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CharacteristicsDlg(QString &characteristic, const QString& name, parameters &parameter, QWidget *parent = nullptr);
    ~CharacteristicsDlg();

    QString& m_characteristic;

private slots:
    void on_btnCancel_clicked();
    void on_btnApply_clicked();
    void on_ckShowHoverValues_clicked(bool checked);
    void on_ckLegend_clicked(bool checked);
    void on_ckTicks_clicked(bool checked);
    void on_ckAxis_clicked(bool checked);
    void on_ckGrid_clicked(bool checked);

private:
    QStandardItemModel*  mListModel = nullptr;

    Ui::CharacteristicsDlg *ui;
};

#endif // CHARACTERISTICSDLG_H
