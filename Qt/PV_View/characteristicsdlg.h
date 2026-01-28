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


private:
    QStandardItemModel*  mListModel = nullptr;

    Ui::CharacteristicsDlg *ui;
};

#endif // CHARACTERISTICSDLG_H
