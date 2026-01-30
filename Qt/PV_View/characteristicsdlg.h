#ifndef CHARACTERISTICSDLG_H
#define CHARACTERISTICSDLG_H

#include <QDialog>
#include <QStandardItemModel>

#include "yaml_structs.h"
#include "plot_view.h"

namespace Ui {
class CharacteristicsDlg;
}

class CharacteristicsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CharacteristicsDlg(QString &characteristic, const QString& name, parameters &parameter, const PlotView::settings &plot_settings, QWidget *parent = nullptr);
    ~CharacteristicsDlg();

    const PlotView::settings &get_plot_settings() const;

private slots:
    void on_btnCancel_clicked();
    void on_btnApply_clicked();
    void on_ckShowHoverValues_clicked(bool checked);
    void on_ckLegend_clicked(bool checked);
    void on_ckTicks_clicked(bool checked);
    void on_ckAxis_clicked(bool checked);
    void on_ckGrid_clicked(bool checked);

private:
    QString&            m_characteristic;
    QStandardItemModel*  mListModel = nullptr;

    Ui::CharacteristicsDlg *ui;
};

#endif // CHARACTERISTICSDLG_H
